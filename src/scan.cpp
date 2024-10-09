#include "Globals.h"  // For statusLabel
#include <Adafruit_Fingerprint.h>
#include <lvgl.h>
#include "Scan.h"

// Declare the serial and fingerprint sensor as extern
extern HardwareSerial mySerial;
extern Adafruit_Fingerprint finger;

// Helper function to update the LVGL status label
void update_status_label(const char* message) {
    lv_label_set_text(statusLabel, message);
    lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_MID, 0, -10);  // Re-align the label
}

// Function prototype for the timer callback
void fingerprint_scan_task(lv_timer_t* timer);

void setup_scanner() {
    mySerial.begin(57600, SERIAL_8N1, RX_PIN, TX_PIN);

    // Initialize the fingerprint sensor
    finger.begin(57600);
    delay(5);
    if (finger.verifyPassword()) {
        update_status_label("Fingerprint sensor found!");
    } else {
        update_status_label("Sensor not found!");
        while (1) { delay(1); }
    }

    finger.getParameters();

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "Sensor contains %d templates", finger.templateCount);
    update_status_label(buffer);

    delay(2000);  // Delay to allow the user to read the message
    update_status_label("Place finger on sensor.");

    // Create a timer to trigger scanning every 1 second (1000 ms)
    lv_timer_t* scan_timer = lv_timer_create(fingerprint_scan_task, 1000, NULL);
}

uint8_t getFingerprintID() {
    uint8_t p = finger.getImage();
    switch (p) {
        case FINGERPRINT_OK:
            update_status_label("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            update_status_label("No finger detected");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            update_status_label("Communication error");
            return p;
        case FINGERPRINT_IMAGEFAIL:
            update_status_label("Imaging error");
            return p;
        default:
            update_status_label("Unknown error");
            return p;
    }

    p = finger.image2Tz();
    switch (p) {
        case FINGERPRINT_OK:
            update_status_label("Image converted");
            break;
        case FINGERPRINT_IMAGEMESS:
            update_status_label("Image too messy");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            update_status_label("Communication error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            update_status_label("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            update_status_label("Invalid fingerprint");
            return p;
        default:
            update_status_label("Unknown error");
            return p;
    }

    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Found ID #%d, Confidence: %d", finger.fingerID, finger.confidence);
        update_status_label(buffer);
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        update_status_label("Communication error");
        return p;
    } else if (p == FINGERPRINT_NOTFOUND) {
        update_status_label("No match found");
        return p;
    } else {
        update_status_label("Unknown error");
        return p;
    }

    return finger.fingerID;
}

void scan_fingerprint() {
    getFingerprintID();  // Scan for fingerprint ID
}

// Function to show scan screen
void show_scan_screen() {
    update_status_label("Scanning...");
    scan_fingerprint();
}