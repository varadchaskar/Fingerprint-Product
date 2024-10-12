// ui.cpp

#include "ui.h"

// Global LVGL objects
lv_obj_t* finger_label;
lv_obj_t* dropdown_menu;
lv_obj_t* input_text_area;
lv_obj_t* keyboard;
lv_obj_t* return_button;
lv_obj_t* user_dropdown;
lv_obj_t* delete_button;
lv_obj_t* password_area;
lv_obj_t* password_keyboard;
lv_obj_t* status_label;

// Global variables
uint8_t id = 0;
bool enrolling_mode = false;
bool scanning_mode = false;
String user_name = "";

// LVGL display buffer
lv_disp_draw_buf_t draw_buf;
lv_color_t buf[kScreenWidth * 10];

/* Function to initialize the LVGL UI */
void SetupUI() {
  // Create the dropdown menu
  dropdown_menu = lv_dropdown_create(lv_scr_act());
  lv_dropdown_set_options(dropdown_menu, "Enroll\nScan\nDelete\nPassword");
  lv_obj_set_size(dropdown_menu, 100, 30);  // Adjust size as needed
  lv_obj_align(dropdown_menu, LV_ALIGN_TOP_RIGHT, -10, 10);  // Top right corner

  // Attach event handler for dropdown
  lv_obj_add_event_cb(dropdown_menu, DropdownEventHandler, LV_EVENT_VALUE_CHANGED, NULL);

  // Initial finger label setup (hidden by default)
  finger_label = lv_label_create(lv_scr_act());
  lv_label_set_text(finger_label, "");
  lv_obj_align(finger_label, LV_ALIGN_CENTER, 0, -40);
  lv_obj_add_flag(finger_label, LV_OBJ_FLAG_HIDDEN);  // Initially hidden

  // Initial status label setup
  status_label = lv_label_create(lv_scr_act());
  lv_label_set_text(status_label, "Welcome! Please select an option.");
  lv_obj_align(status_label, LV_ALIGN_CENTER, 0, -40);

  // Create Delete button (initially hidden)
  delete_button = lv_btn_create(lv_scr_act());
  lv_obj_set_size(delete_button, 80, 40);
  lv_obj_align(delete_button, LV_ALIGN_CENTER, 0, 80);
  lv_obj_t* delete_label = lv_label_create(delete_button);
  lv_label_set_text(delete_label, "Delete");
  lv_obj_center(delete_label);
  lv_obj_add_flag(delete_button, LV_OBJ_FLAG_HIDDEN);  // Initially hidden
  lv_obj_add_event_cb(delete_button, DeleteButtonEventHandler, LV_EVENT_CLICKED, NULL);

  // Initialize user_dropdown to NULL
  user_dropdown = NULL;

  // Create input text area (initially hidden)
  input_text_area = lv_textarea_create(lv_scr_act());
  lv_obj_align(input_text_area, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_flag(input_text_area, LV_OBJ_FLAG_HIDDEN);

  // Create keyboard (initially hidden)
  keyboard = lv_keyboard_create(lv_scr_act());
  lv_keyboard_set_textarea(keyboard, input_text_area);
  lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_event_cb(keyboard, KeyboardEventHandler, LV_EVENT_READY, NULL);

  // Create password text area (initially hidden)
  password_area = lv_textarea_create(lv_scr_act());
  lv_textarea_set_password_mode(password_area, true);  // Enable password mode
  lv_obj_set_width(password_area, 150);
  lv_obj_align(password_area, LV_ALIGN_CENTER, 0, -40);
  lv_obj_add_flag(password_area, LV_OBJ_FLAG_HIDDEN);

  // Create password keyboard (initially hidden)
  password_keyboard = lv_keyboard_create(lv_scr_act());
  lv_keyboard_set_textarea(password_keyboard, password_area);
  lv_obj_add_flag(password_keyboard, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_event_cb(password_keyboard, PassKeyboardEventHandler, LV_EVENT_READY, NULL);

  // Create Return button (initially hidden)
  return_button = lv_btn_create(lv_scr_act());
  lv_obj_set_size(return_button, 60, 40);  // Width 60px, Height 40px
  lv_obj_set_style_pad_all(return_button, 5, 0);  // Add padding
  lv_obj_align(return_button, LV_ALIGN_TOP_LEFT, 10, 10);
  lv_obj_t* return_label = lv_label_create(return_button);
  lv_label_set_text(return_label, "Back");
  lv_obj_add_event_cb(return_button, ReturnButtonEventHandler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_flag(return_button, LV_OBJ_FLAG_HIDDEN);  // Hide Return button initially
}


