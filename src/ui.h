// ui.h

#ifndef UI_H_
#define UI_H_

// Include LVGL library and hardware definitions
#include <lvgl.h>
#include "hardware.h"

// Extern declarations for UI objects
extern lv_obj_t* finger_label;        // Label to display fingerprint messages
extern lv_obj_t* dropdown_menu;       // Dropdown menu for main options
extern lv_obj_t* input_text_area;     // Text area for input
extern lv_obj_t* keyboard;            // On-screen keyboard
extern lv_obj_t* return_button;       // Return (Back) button
extern lv_obj_t* user_dropdown;       // Dropdown menu for user selection (delete action)
extern lv_obj_t* delete_button;       // Delete button in delete action
extern lv_obj_t* password_area;       // Text area for password input
extern lv_obj_t* password_keyboard;   // On-screen keyboard for password input
extern lv_obj_t* status_label;        // Label to display status messages

// Global variables
extern uint8_t id;         // Fingerprint ID to be enrolled or deleted
extern bool enrolling_mode;  // Flag indicating if enrolling mode is active
extern bool scanning_mode;   // Flag indicating if scanning mode is active
extern String user_name;     // Variable to store the user's name

// LVGL display buffer
extern lv_disp_draw_buf_t draw_buf;  // LVGL display buffer
extern lv_color_t buf[];             // Buffer for LVGL display

// Function declarations for UI-related functions
void SetupUI();                      // Function to initialize the LVGL UI
void ReturnToMainMenu();             // Function to return to the main menu
void EnrollAction();                 // Function for Enroll action
void ScanAction();                   // Function for Scan action
void DeleteAction();                 // Function for Delete action
void PasswordAction();               // Function for Password action
void ShowPasswordScreen();           // Function to show the password input screen
void KeyboardEventHandler(lv_event_t* e);      // Event handler for keyboard input
void PassKeyboardEventHandler(lv_event_t* e);  // Event handler for password keyboard input
void DropdownEventHandler(lv_event_t* e);      // Event handler for dropdown menu
void ReturnButtonEventHandler(lv_event_t* e);  // Event handler for Return button
void DeleteButtonEventHandler(lv_event_t* e);  // Event handler for Delete button
void UserDropdownEventHandler(lv_event_t* e);  // Event handler for user dropdown in delete action
void RepositionLabelAboveKeyboard();           // Function to reposition label when keyboard is shown
void LVGLPortTPRead(lv_indev_drv_t* indev, lv_indev_data_t* data);  // Touchpad input handler
void MyDispFlush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p);  // Display flushing
void HandleFingerprintEnrollment();            // Function to handle fingerprint enrollment
void ScanFingerprint();                        // Function to scan for fingerprints

#endif  // UI_H_
