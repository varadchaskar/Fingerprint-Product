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

/* Function to handle the Return button event */
void ReturnButtonEventHandler(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    Serial.println("Return button clicked.");

    // Show the main menu dropdown
    lv_obj_clear_flag(dropdown_menu, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(return_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(finger_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(status_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(input_text_area, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(password_area, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(password_keyboard, LV_OBJ_FLAG_HIDDEN);
    if (delete_button != NULL) lv_obj_add_flag(delete_button, LV_OBJ_FLAG_HIDDEN);
    if (user_dropdown != NULL) lv_obj_add_flag(user_dropdown, LV_OBJ_FLAG_HIDDEN);

    // Stop enrolling and scanning modes
    enrolling_mode = false;
    scanning_mode = false;

    // Reset ID and Name for future enrollments
    id = 0;
    user_name = "";

    // Reset status label message
    lv_label_set_text(status_label, "Welcome! Please select an option.");
    lv_obj_align(status_label, LV_ALIGN_CENTER, 0, -40);
    lv_obj_clear_flag(status_label, LV_OBJ_FLAG_HIDDEN);
  }
}

/* Event handler for the dropdown menu */
void DropdownEventHandler(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t* dropdown = lv_event_get_target(e);

  if (code == LV_EVENT_VALUE_CHANGED) {
    char buf[32];
    lv_dropdown_get_selected_str(dropdown, buf, sizeof(buf));

    if (strcmp(buf, "Enroll") == 0) {
      // Call enroll function
      EnrollAction();
    } else if (strcmp(buf, "Scan") == 0) {
      // Call scan function
      ScanAction();
    } else if (strcmp(buf, "Delete") == 0) {
      // Call delete function
      DeleteAction();
    } else if (strcmp(buf, "Password") == 0) {
      // Call password function
      PasswordAction();
    }
  }
}

/* Function for Enroll action */
void EnrollAction() {
  lv_obj_clear_flag(finger_label, LV_OBJ_FLAG_HIDDEN);
  lv_label_set_text(finger_label, "Enrolling, please enter the ID:");

  // Hide the dropdown menu and show input area
  lv_obj_add_flag(dropdown_menu, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(input_text_area, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

  // Show the Return button
  lv_obj_clear_flag(return_button, LV_OBJ_FLAG_HIDDEN);

  // Reposition label above keyboard
  RepositionLabelAboveKeyboard();

  // Move the Return button to top-left corner
  lv_obj_align(return_button, LV_ALIGN_TOP_LEFT, 10, 10);

  // Hide the status label
  lv_obj_add_flag(status_label, LV_OBJ_FLAG_HIDDEN);
}

/* Function for Scan action */
void ScanAction() {
  scanning_mode = true;
  lv_label_set_text(finger_label, "Scanning...");

  // Hide the dropdown menu and show the Return button
  lv_obj_add_flag(dropdown_menu, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(return_button, LV_OBJ_FLAG_HIDDEN);

  // Start continuous scanning
  lv_obj_clear_flag(finger_label, LV_OBJ_FLAG_HIDDEN);

  // Move the Return button to top-left corner
  lv_obj_align(return_button, LV_ALIGN_TOP_LEFT, 10, 10);

  // Hide the status label
  lv_obj_add_flag(status_label, LV_OBJ_FLAG_HIDDEN);
}

/* Function for Delete action */
void DeleteAction() {
  // Hide the dropdown menu
  lv_obj_add_flag(dropdown_menu, LV_OBJ_FLAG_HIDDEN);

  // Hide the status label
  lv_obj_add_flag(status_label, LV_OBJ_FLAG_HIDDEN);

  // Show the Return button
  lv_obj_clear_flag(return_button, LV_OBJ_FLAG_HIDDEN);
  lv_obj_align(return_button, LV_ALIGN_TOP_LEFT, 10, 10);

  // Create and show the user dropdown
  if (user_dropdown == NULL) {
    user_dropdown = lv_dropdown_create(lv_scr_act());
    lv_obj_set_width(user_dropdown, 200);
    lv_obj_align(user_dropdown, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_add_event_cb(user_dropdown, UserDropdownEventHandler, LV_EVENT_VALUE_CHANGED, NULL);
  } else {
    lv_obj_clear_flag(user_dropdown, LV_OBJ_FLAG_HIDDEN);
  }

  // Populate the user dropdown with users from JSON
  String user_list = GetUserListForDropdown();
  if (user_list.length() > 0) {
    lv_dropdown_set_options(user_dropdown, user_list.c_str());
  } else {
    lv_dropdown_set_options(user_dropdown, "No users found.");
  }

  // Hide the Delete button initially
  if (delete_button != NULL) {
    lv_obj_add_flag(delete_button, LV_OBJ_FLAG_HIDDEN);
  }
}

/* Event handler for the user dropdown in delete action */
void UserDropdownEventHandler(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t* dropdown = lv_event_get_target(e);

  if (code == LV_EVENT_VALUE_CHANGED) {
    // Get selected user
    char selected_user[64];
    lv_dropdown_get_selected_str(dropdown, selected_user, sizeof(selected_user));

    // Parse the selected user to extract ID
    sscanf(selected_user, "ID: %hhu", &id);  // Extract the ID

    // Show the Delete button
    lv_obj_clear_flag(delete_button, LV_OBJ_FLAG_HIDDEN);
  }
}

/* Event handler for the Delete button */
void DeleteButtonEventHandler(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    // Delete the user from the fingerprint sensor
    DeleteFingerprint(id);

    // Delete the user from JSON
    DeleteUserFromJSON(id);

    // Display confirmation message
    lv_label_set_text_fmt(finger_label, "User ID #%d deleted.", id);
    lv_obj_clear_flag(finger_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_align(finger_label, LV_ALIGN_CENTER, 0, -40);

    // Hide the Delete button and user dropdown
    lv_obj_add_flag(delete_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(user_dropdown, LV_OBJ_FLAG_HIDDEN);

    // Reset ID
    id = 0;
  }
}

/* Function for Password action */
void PasswordAction() {
  // Hide the dropdown menu and show the password screen
  lv_obj_add_flag(dropdown_menu, LV_OBJ_FLAG_HIDDEN);
  ShowPasswordScreen();

  // Hide other labels if necessary
  lv_obj_add_flag(finger_label, LV_OBJ_FLAG_HIDDEN);

  // Ensure status_label is visible
  lv_obj_clear_flag(status_label, LV_OBJ_FLAG_HIDDEN);

  // Show the Return button
  lv_obj_clear_flag(return_button, LV_OBJ_FLAG_HIDDEN);

  // Move the Return button to top-left corner
  lv_obj_align(return_button, LV_ALIGN_TOP_LEFT, 10, 10);
}

/* Function to show the password input screen */
void ShowPasswordScreen() {
  lv_label_set_text(status_label, "Enter Password:");
  lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_clear_flag(status_label, LV_OBJ_FLAG_HIDDEN);

  // Show the password area and keyboard
  lv_obj_clear_flag(password_area, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(password_keyboard, LV_OBJ_FLAG_HIDDEN);
}

/* Event handler for the password keyboard input */
void PassKeyboardEventHandler(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t* kb = lv_event_get_target(e);

  if (code == LV_EVENT_READY) {  // When the "Enter" button is pressed
    const char* input = lv_textarea_get_text(password_area);

    // Make sure the status_label is visible
    lv_obj_clear_flag(status_label, LV_OBJ_FLAG_HIDDEN);

    if (strcmp(input, "0000") == 0) {
      lv_label_set_text(status_label, "Welcome Varad!");
      lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -10);

      // Hide the keyboard and password text area
      lv_obj_add_flag(password_area, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(password_keyboard, LV_OBJ_FLAG_HIDDEN);

      // After 2 seconds, go back to the initial screen
      lv_timer_t* timer = lv_timer_create([](lv_timer_t* t) {
        ReturnToMainMenu();
        lv_timer_del(t);  // Delete the timer after execution
      }, 2000, NULL);  // 2000 milliseconds = 2 seconds
    } else {
      // Hide the keyboard and password text area
      lv_obj_add_flag(password_area, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(password_keyboard, LV_OBJ_FLAG_HIDDEN);

      lv_label_set_text(status_label, "Wrong Password!");
      lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -10);

      // After 2 seconds, go back to the initial screen
      lv_timer_t* timer = lv_timer_create([](lv_timer_t* t) {
        ReturnToMainMenu();
        lv_timer_del(t);  // Delete the timer after execution
      }, 2000, NULL);  // 2000 milliseconds = 2 seconds
    }
    lv_textarea_set_text(password_area, "");  // Clear the text area for another input
  }
}

/* Event handler for the keyboard input */
void KeyboardEventHandler(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_READY) {
    const char* input = lv_textarea_get_text(input_text_area);

    if (id == 0) {  // Capture ID first
      id = atoi(input);  // Convert input to integer ID
      if (id > 0 && id <= 127) {
        lv_label_set_text_fmt(finger_label, "ID #%d entered. Now, enter your Name:", id);
        lv_textarea_set_text(input_text_area, "");  // Clear text area for Name input
        RepositionLabelAboveKeyboard();  // Adjust label position
      } else {
        lv_label_set_text(finger_label, "Invalid ID, please try again.");
        id = 0;  // Reset ID for re-entry
      }
    } else {  // After ID, capture the Name
      user_name = String(input);  // Store the entered Name
      lv_label_set_text_fmt(finger_label, "Enrolling ID #%d, Name: %s", id, user_name.c_str());
      lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);  // Hide the keyboard
      lv_obj_add_flag(input_text_area, LV_OBJ_FLAG_HIDDEN);

      RepositionLabelAboveKeyboard();  // Adjust label position back to normal

      // Now save to JSON file
      SaveUserToJSON(id, user_name.c_str());

      lv_textarea_set_text(input_text_area, "");  // Clear text area

      enrolling_mode = true;
    }
  }
}

/* Function to reposition the label when keyboard is shown */
void RepositionLabelAboveKeyboard() {
  if (lv_obj_has_flag(keyboard, LV_OBJ_FLAG_HIDDEN)) {
    // Keyboard is hidden, restore the label's default position
    lv_obj_align(finger_label, LV_ALIGN_CENTER, 0, -40);  // Original position
  } else {
    // Keyboard is visible, move the label higher
    lv_obj_align(finger_label, LV_ALIGN_CENTER, 0, -80);  // Move it higher
  }
}

/* Touchpad input handler for LVGL */
void LVGLPortTPRead(lv_indev_drv_t* indev, lv_indev_data_t* data) {
  uint16_t touch_x, touch_y;
  bool touched = tft.getTouch(&touch_x, &touch_y);

  if (!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touch_x;
    data->point.y = touch_y;
  }
}

/* Display flushing function for LVGL */
void MyDispFlush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t*)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/* Function to handle fingerprint enrollment */
void HandleFingerprintEnrollment() {
  if (!enrolling_mode || id == 0) return;

  // Delete the existing fingerprint template for the ID before enrolling
  Serial.print("Deleting fingerprint for ID #");
  Serial.println(id);
  int delete_status = finger.deleteModel(id);
  if (delete_status == FINGERPRINT_OK) {
    Serial.println("Existing fingerprint deleted.");
  } else {
    Serial.println("No existing fingerprint to delete.");
  }

  lv_label_set_text_fmt(finger_label, "Place finger to enroll as ID #%d", id);
  lv_timer_handler();  // Ensure display updates after setting the message
  delay(200);

  int p = finger.getImage();
  if (p == FINGERPRINT_NOFINGER) {
    Serial.println("No finger detected.");
    return;
  }

  if (p == FINGERPRINT_OK) {
    Serial.println("Image taken");
    lv_label_set_text(finger_label, "Image taken, processing...");
    lv_timer_handler();  // Refresh the display immediately
    delay(200);

    p = finger.image2Tz(1);
    if (p == FINGERPRINT_OK) {
      Serial.println("Remove finger and place it again.");
      lv_label_set_text(finger_label, "Remove finger and place it again.");
      lv_timer_handler();
      delay(2000);

      while (finger.getImage() != FINGERPRINT_NOFINGER) {
        delay(100);
      }

      lv_label_set_text(finger_label, "Place the same finger again.");
      lv_timer_handler();
      delay(500);

      while (finger.getImage() != FINGERPRINT_OK) {
        delay(100);
      }

      p = finger.image2Tz(2);
      if (p == FINGERPRINT_OK) {
        p = finger.createModel();
        if (p == FINGERPRINT_OK) {
          p = finger.storeModel(id);
          if (p == FINGERPRINT_OK) {
            Serial.println("Fingerprint enrolled successfully.");
            lv_label_set_text_fmt(finger_label, "Fingerprint enrolled successfully as ID #%d", id);
            lv_timer_handler();
            delay(2000);
            ReturnToMainMenu();
          } else {
            lv_label_set_text(finger_label, "Failed to store fingerprint.");
            lv_timer_handler();
          }
        } else {
          lv_label_set_text(finger_label, "Fingerprints did not match.");
          lv_timer_handler();
        }
      } else {
        lv_label_set_text(finger_label, "Failed to capture second image.");
        lv_timer_handler();
      }
    } else {
      lv_label_set_text(finger_label, "Failed to process image.");
      lv_timer_handler();
    }
  } else {
    lv_label_set_text(finger_label, "Error capturing image.");
    lv_timer_handler();
  }
}

/* Function to scan for fingerprints */
void ScanFingerprint() {
  uint8_t fingerprint_id = GetFingerprintID();
  switch (fingerprint_id) {
    case FINGERPRINT_NOFINGER:
      lv_label_set_text(finger_label, "No Finger Detected");
      Serial.println("No Finger Detected");
      break;
    case FINGERPRINT_NOTFOUND:
      lv_label_set_text(finger_label, "No Match Found");
      Serial.println("No Match Found");
      break;
    default:
      if (fingerprint_id >= 0) {
        // Get the user's name based on the fingerprint ID from the JSON file
        const char* user_name = GetUserNameByID(fingerprint_id);

        // Display fingerprint ID and user name on the label
        String msg = "ID: " + String(fingerprint_id) + ", Name: " + String(user_name);
        lv_label_set_text(finger_label, msg.c_str());
        Serial.println(msg);
      }
      break;
  }
}

/* Function to return to the main menu */
void ReturnToMainMenu() {
  // Show the main menu (dropdown_menu)
  lv_obj_clear_flag(dropdown_menu, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(return_button, LV_OBJ_FLAG_HIDDEN);

  // Hide text labels and input fields
  lv_obj_add_flag(finger_label, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(status_label, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(input_text_area, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(password_area, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(password_keyboard, LV_OBJ_FLAG_HIDDEN);
  if (delete_button != NULL) lv_obj_add_flag(delete_button, LV_OBJ_FLAG_HIDDEN);
  if (user_dropdown != NULL) lv_obj_add_flag(user_dropdown, LV_OBJ_FLAG_HIDDEN);

  // Stop enrolling and scanning modes
  enrolling_mode = false;
  scanning_mode = false;

  // Reset ID and Name for future enrollments
  id = 0;
  user_name = "";

  // Reset status label message
  lv_label_set_text(status_label, "Welcome! Please select an option.");
  lv_obj_align(status_label, LV_ALIGN_CENTER, 0, -40);
  lv_obj_clear_flag(status_label, LV_OBJ_FLAG_HIDDEN);
}
