#include "Password.h"
#include "Globals.h" // For statusLabel
#include <lvgl.h>

lv_obj_t *keyboard = NULL;      // Define the keyboard here
lv_obj_t *password_area = NULL; // Define the password_area here

// Callback function for keyboard events
static void keyboard_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *kb = lv_event_get_target(e);

    if (code == LV_EVENT_READY) {  // When the "Enter" button is pressed
        const char *input = lv_textarea_get_text(password_area);
        if (strcmp(input, "0000") == 0) {
            lv_label_set_text(statusLabel, "Welcome Varad!");
            lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_MID, 0, -10);

            // Hide the keyboard and password text area instead of deleting them
            lv_obj_add_flag(password_area, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

            // After 2 seconds, go back to the initial screen
            lv_timer_t *timer = lv_timer_create([](lv_timer_t *t) {
                lv_label_set_text(statusLabel, "Select an option...");
                lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_MID, 0, -10);
                lv_timer_del(t);  // Delete the timer after execution
            }, 2000, NULL);  // 2000 milliseconds = 2 seconds
        } else {
             // Hide the keyboard and password text area instead of deleting them
            lv_obj_add_flag(password_area, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(statusLabel, "Wrong Password!");
            lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_MID, 0, -10);
            // After 2 seconds, go back to the initial screen
            lv_timer_t *timer = lv_timer_create([](lv_timer_t *t) {
                lv_label_set_text(statusLabel, "Select an option...");
                lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_MID, 0, -10);
                lv_timer_del(t);  // Delete the timer after execution
            }, 2000, NULL);  // 2000 milliseconds = 2 seconds
        }
        lv_textarea_set_text(password_area, "");  // Clear the text area for another input
    }
}

// Function to show password screen with keyboard
void show_password_screen() {
    // Show a label to ask for the password
    lv_label_set_text(statusLabel, "Enter Password:");
    lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_MID, 0, -10);

    // Create a text area for password input if it doesn't exist
    if (password_area == NULL) {
        password_area = lv_textarea_create(lv_scr_act());
        lv_textarea_set_password_mode(password_area, true);  // Enable password mode (hide input)
        lv_obj_set_width(password_area, 150);
        lv_obj_align(password_area, LV_ALIGN_CENTER, 0, -40);  // Position the text area
    } else {
        lv_obj_clear_flag(password_area, LV_OBJ_FLAG_HIDDEN);  // Show it if it's hidden
    }

    // Create a keyboard if it doesn't exist
    if (keyboard == NULL) {
        keyboard = lv_keyboard_create(lv_scr_act());
        lv_obj_set_size(keyboard, LV_HOR_RES, 100);
        lv_keyboard_set_textarea(keyboard, password_area);  // Link the keyboard to the text area
        lv_obj_add_event_cb(keyboard, keyboard_event_handler, LV_EVENT_READY, NULL);
    } else {
        lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);  // Show it if it's hidden
    }
}