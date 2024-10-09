#include "Delete.h"
#include "Globals.h" // Include Globals.h to access statusLabel

void show_delete_screen() {
    lv_label_set_text(statusLabel, "Deleting...");
    lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_MID, 0, -10);
}
