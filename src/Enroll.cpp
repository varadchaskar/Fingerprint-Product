#include "Globals.h"  // For statusLabel and keyboard
#include <Adafruit_Fingerprint.h>
#include <lvgl.h>

extern HardwareSerial mySerial; // Reuse the serial instance for consistency
extern Adafruit_Fingerprint finger;

// Global variables
uint8_t id;
lv_obj_t* kb;  // Reference to the keyboard object

// Function prototypes
void enroll_fingerprint();
uint8_t getFingerprintEnroll();
void get_fingerprint_id(lv_obj_t* event_obj, lv_event_t* e);

// Function to update the status label (this already exists)
extern void update_status_label(const char* text);

void keyboard_event_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_READY) {
        // Get the textarea that is linked to this keyboard
        lv_obj_t* ta = (lv_obj_t*)lv_event_get_user_data(e);
        const char* input = lv_textarea_get_text(ta);

        // Debug: Print raw input to serial monitor
        Serial.print("Raw input: ");
        Serial.println(input ? input : "null");

        // Hide the keyboard immediately after submission
        lv_obj_add_flag(lv_event_get_target(e), LV_OBJ_FLAG_HIDDEN);

        // Check if the input is not empty
        if (input == NULL || strlen(input) == 0) {
            update_status_label("No ID entered. Please try again.");
            return;
        }

        // Convert the input to an integer
        id = atoi(input);

        // Debug: Print converted ID to serial
        Serial.print("Converted ID: ");
        Serial.println(id);

        // Check if the ID is within the valid range
        if (id > 0 && id <= 127) {
            update_status_label("Starting enrollment...");
            enroll_fingerprint();  // Start the fingerprint enrollment process
        } else {
            update_status_label("Invalid ID entered. Please try again.");
        }
    }
}

// Function to show the enroll screen
void show_enroll_screen() {
    update_status_label("Enter ID to enroll (1 to 127):");

    // Create a text area for keyboard input (specific to this screen)
    lv_obj_t* ta = lv_textarea_create(lv_scr_act());
    lv_textarea_set_placeholder_text(ta, "Enter ID...");
    lv_textarea_set_max_length(ta, 3);  // Limit to 3 digits (IDs between 1 and 127)
    lv_textarea_set_text(ta, "");  // Ensure the textarea is empty
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 20);

    // Create a keyboard and link it to the text area
    lv_obj_t* kb = lv_keyboard_create(lv_scr_act());  // Local keyboard object
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUMBER);  // Use numeric mode for ID entry
    lv_keyboard_set_textarea(kb, ta);  // Link the keyboard to the textarea
    lv_obj_add_event_cb(kb, keyboard_event_cb, LV_EVENT_READY, ta);  // Pass the text area as user data
}


// Function to enroll fingerprint
void enroll_fingerprint() {
    update_status_label("Initializing sensor...");

    mySerial.begin(57600, SERIAL_8N1, 18, 19);  // TX=18, RX=19
    finger.begin(57600);

    if (finger.verifyPassword()) {
        update_status_label("Found fingerprint sensor!");
    } else {
        update_status_label("Did not find fingerprint sensor!");
        return;
    }

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "Sensor has %d templates", finger.templateCount);
    update_status_label(buffer);

    delay(2000);  // Show sensor data before starting enrollment
    getFingerprintEnroll();  // Begin enrollment process
}

uint8_t getFingerprintEnroll() {
    char buffer[128];
    int p = -1;
    snprintf(buffer, sizeof(buffer), "Waiting for valid finger for ID #%d", id);
    update_status_label(buffer);

    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        switch (p) {
        case FINGERPRINT_OK:
            update_status_label("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            update_status_label("No finger detected");
            delay(500);  // Small delay to avoid rapid message flooding
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            update_status_label("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            update_status_label("Imaging error");
            break;
        default:
            update_status_label("Unknown error");
            break;
        }
    }

    p = finger.image2Tz(1);
    if (p == FINGERPRINT_OK) {
        update_status_label("Image converted");
    } else {
        update_status_label("Error converting image");
        return p;
    }

    // Continue the enrollment process...

    // Let the user know when to remove and place the finger again
    update_status_label("Remove finger");
    delay(2000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER) {
        p = finger.getImage();
    }

    update_status_label("Place same finger again");
    p = -1;
    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        if (p == FINGERPRINT_OK) {
            update_status_label("Image taken");
        } else {
            update_status_label("No finger detected");
        }
    }

    // Convert second image and store the model
    p = finger.image2Tz(2);
    if (p == FINGERPRINT_OK) {
        update_status_label("Second image converted");
    } else {
        update_status_label("Error with second image");
        return p;
    }

    p = finger.createModel();
    if (p == FINGERPRINT_OK) {
        update_status_label("Fingerprints matched!");
    } else {
        update_status_label("Fingerprints did not match. Try again.");
        return p;
    }

    // Store the fingerprint model
    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK) {
        update_status_label("Fingerprint stored successfully!");
    } else {
        update_status_label("Error storing fingerprint");
    }

    return p;
}
