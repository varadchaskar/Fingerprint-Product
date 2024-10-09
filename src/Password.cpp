#include "Password.h"
#include "Globals.h" // Include Globals.h to access statusLabel

void show_password_screen() {
    lv_label_set_text(statusLabel, "Enter Password:");
    lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_MID, 0, -10);
}
