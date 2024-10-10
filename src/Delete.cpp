#include "Delete.h"
#include "Globals.h" // Include Globals.h to access statusLabel
#include "UserDataManager.h"
#include <lvgl.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

lv_obj_t* delete_dropdown;
lv_obj_t* confirm_label;
lv_obj_t *no_users_label; 
lv_obj_t *return_button;   
int selected_id;

void delete_dropdown_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        selected_id = lv_dropdown_get_selected(delete_dropdown);
        lv_label_set_text(confirm_label, "Press delete to remove user");
    }
}

void delete_confirm_handler(lv_event_t* e) {
    if (deleteUser(selected_id)) {
        lv_label_set_text(confirm_label, "User deleted!");
    } else {
        lv_label_set_text(confirm_label, "Error deleting user");
    }
}

void return_event_handler(lv_event_t* e) {
    Serial.println("Return button pressed");
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        Serial.println("Hiding dropdown and return button");

        // Hide the dropdown and return button using global instances
        if (delete_dropdown) {
            lv_obj_add_flag(delete_dropdown, LV_OBJ_FLAG_HIDDEN);
        }
        if (return_button) {
            lv_obj_add_flag(return_button, LV_OBJ_FLAG_HIDDEN);
        }
        if (no_users_label) {
            lv_obj_add_flag(no_users_label, LV_OBJ_FLAG_HIDDEN);
        }

        // Show the "Select an option..." label again
        if (statusLabel) {
            lv_obj_clear_flag(statusLabel, LV_OBJ_FLAG_HIDDEN);  // Show the statusLabel
        }

        // Optionally, clear the screen or show the main menu
        // lv_obj_clean(lv_scr_act());  // Clear the screen if needed
    }
}

void show_delete_screen() {
    // Hide the "Select an option..." label
    if (statusLabel) {
        lv_obj_add_flag(statusLabel, LV_OBJ_FLAG_HIDDEN);  // Hide the statusLabel
    }

    // Retrieve the user list from the JSON
    JsonDocument doc = get_users_from_json();

    // Check if there are users in the JSON
    if (doc.isNull()) {
        Serial.println("No users found in JSON");

        // Display a message saying "No users found"
        no_users_label = lv_label_create(lv_scr_act());
        lv_label_set_text(no_users_label, "No users found");
        lv_obj_align(no_users_label, LV_ALIGN_CENTER, 0, 0);

        // Create the "Return" button using the global instance
        return_button = lv_btn_create(lv_scr_act());
        lv_obj_align(return_button, LV_ALIGN_BOTTOM_MID, 10, -10);
        lv_obj_set_size(return_button, 100, 50);
        lv_obj_add_event_cb(return_button, return_event_handler, LV_EVENT_CLICKED, NULL);

        lv_obj_t* label = lv_label_create(return_button);
        lv_label_set_text(label, "Return");
        lv_obj_center(label);

        return;  // Exit function if no users found
    }

    // Prepare user options for the dropdown
    String userOptions = "";
    JsonArray users = doc["users"].as<JsonArray>();
    for (JsonObject user : users) {
        String username = user["name"].as<String>();
        userOptions += username + "\n";  // Add each username to the dropdown options
    }

    // Create the dropdown menu using the global instance
    delete_dropdown = lv_dropdown_create(lv_scr_act());
    lv_dropdown_set_options(delete_dropdown, userOptions.c_str());  // Convert String to const char*
    lv_obj_align(delete_dropdown, LV_ALIGN_TOP_MID, 0, 50);  // Position it below the top of the screen

    // Create the return button using the global instance
    return_button = lv_btn_create(lv_scr_act());
    lv_obj_align(return_button, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_size(return_button, 100, 50);
    lv_obj_add_event_cb(return_button, return_event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t* label = lv_label_create(return_button);
    lv_label_set_text(label, "Return");
    lv_obj_center(label);
}
