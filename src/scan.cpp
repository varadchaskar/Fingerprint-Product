#include "Scan.h"
#include "Globals.h"

void show_scan_screen() {
    lv_label_set_text(statusLabel, "Scanning...");
    lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_MID, 0, -10);
    // Add more logic here for scanning the fingerprint
}
