#include <Arduino.h>
#include <FS.h>
#include <SPI.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

// Include the newly created header files
#include "Enroll.h"
#include "Delete.h"
#include "Scan.h"
#include "Password.h"
#include "Globals.h" 

// Pins and TFT setup
#define RX_PIN 25
#define TX_PIN 33
#define TOUCH_CS 21

TFT_eSPI tft = TFT_eSPI(); 
HardwareSerial mySerial(2); 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

static const uint32_t screenWidth = 320;
static const uint32_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

lv_obj_t *dropdown;
lv_obj_t *statusLabel;

void touch_calibrate() {
      uint16_t calData[5];
  uint8_t calDataOK = 0;

  if (!SPIFFS.begin()) {
    Serial.println("Formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  if (SPIFFS.exists("/TouchCalData3")) {
    File f = SPIFFS.open("/TouchCalData3", "r");
    if (f) {
      if (f.readBytes((char *)calData, 14) == 14) calDataOK = 1;
      f.close();
    }
  }

  if (calDataOK) {
    tft.setTouch(calData);
  } else {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.println("Touch corners as indicated");

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
    File f = SPIFFS.open("/TouchCalData3", "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

void dropdown_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        char selected_str[32];
        lv_dropdown_get_selected_str(dropdown, selected_str, sizeof(selected_str));

        if (strcmp(selected_str, "Enroll") == 0) {
            show_enroll_screen();
        } else if (strcmp(selected_str, "Scan") == 0) {
            show_scan_screen();
        } else if (strcmp(selected_str, "Delete") == 0) {
            show_delete_screen();
        } else if (strcmp(selected_str, "Password") == 0) {
            show_password_screen();
        }
    }
}

void create_dropdown() {
    dropdown = lv_dropdown_create(lv_scr_act());
    lv_dropdown_set_options(dropdown, "Enroll\nScan\nDelete\nPassword");
    lv_obj_set_width(dropdown, 100);
    lv_obj_align(dropdown, LV_ALIGN_TOP_RIGHT, -10, 10);

    lv_obj_add_event_cb(dropdown, dropdown_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    statusLabel = lv_label_create(lv_scr_act());
    lv_label_set_text(statusLabel, "Select an option...");
    lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_MID, 0, -10);
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void lvgl_port_tp_read(lv_indev_drv_t *indev, lv_indev_data_t *data) {
    uint16_t touchX, touchY;
    bool touched = tft.getTouch(&touchX, &touchY);

    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

void setup() {
    Serial.begin(115200);
    mySerial.begin(57600, SERIAL_8N1, RX_PIN, TX_PIN);

    lv_init();
    tft.begin();
    tft.setRotation(1);

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lvgl_port_tp_read;
    lv_indev_drv_register(&indev_drv);

    touch_calibrate();
    create_dropdown();
}

void loop() {
    lv_timer_handler();
    delay(5);
}
