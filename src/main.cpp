#include "hardware.h"
#include "ui.h"
#include <lvgl.h>

/* Main setup function */
void setup() {
  // Initialize hardware components
  InitializeHardware();

  // Initialize LVGL library
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, kScreenWidth * 10);

  // Set up the display driver
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = kScreenWidth;
  disp_drv.ver_res = kScreenHeight;
  disp_drv.flush_cb = MyDispFlush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // Set up the touch input device driver
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = LVGLPortTPRead;
  lv_indev_drv_register(&indev_drv);

  // Set up the UI components
  SetupUI();
}

/* Main loop function */
void loop() {
  // Refresh LVGL GUI
  lv_timer_handler();
  delay(5);

  // If in scanning mode, continuously check fingerprint
  if (scanning_mode) {
    ScanFingerprint();
  }

  // If in enrolling mode, handle fingerprint enrollment
  if (enrolling_mode) {
    HandleFingerprintEnrollment();
  }
}
