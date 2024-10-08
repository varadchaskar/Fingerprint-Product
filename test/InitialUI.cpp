#include <Arduino.h>
#include <FS.h>
#include <SPI.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

// Pins for Fingerprint Sensor and LVGL Display
#define RX_PIN 25
#define TX_PIN 33
#define TOUCH_CS 21

// TFT and Fingerprint configurations
TFT_eSPI tft = TFT_eSPI(); // TFT instance
HardwareSerial mySerial(2); // Fingerprint Serial on Serial2
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

/* Screen resolution */
static const uint32_t screenWidth = 320;
static const uint32_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

// Global objects
lv_obj_t *fingerLabel;
lv_obj_t *inputTextArea;
lv_obj_t *keyboard;
lv_obj_t *idLabel;
lv_obj_t *returnButton;
lv_obj_t *dropdown;
lv_obj_t *scanResultLabel;
lv_timer_t *password_timer = NULL;

uint8_t id = 0;  // Fingerprint ID to be enrolled
bool enrollingMode = false;
bool scanningMode = false;
bool idInputMode = true;  // Start by asking for ID

/* Touch calibration */
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

// Function to handle dropdown selection
void dropdown_event_handler(lv_event_t *e) {
  lv_obj_t *dropdown = lv_event_get_target(e);
  uint16_t selected = lv_dropdown_get_selected(dropdown);
  const char *selected_option = lv_dropdown_get_text(dropdown);

  // Process based on the selected option
  if (strcmp(selected_option, "Enroll") == 0) {
    Serial.println("Enroll selected");
    // Enroll functionality code here
  } else if (strcmp(selected_option, "Scan") == 0) {
    Serial.println("Scan selected");
    // Scan functionality code here
  } else if (strcmp(selected_option, "Delete") == 0) {
    Serial.println("Delete selected");
    // Delete functionality code here
  } else if (strcmp(selected_option, "Password Remove") == 0) {
    Serial.println("Password Remove selected");
    // Password remove functionality code here
  }
}

// Function to create the dropdown menu
void create_dropdown() {
  dropdown = lv_dropdown_create(lv_scr_act());
  lv_dropdown_set_options(dropdown, "Enroll\nScan\nDelete\nPassword");
  lv_obj_set_width(dropdown, 150);
  lv_obj_align(dropdown, LV_ALIGN_CENTER, 0, -40);

  // Event handler for dropdown selection
  lv_obj_add_event_cb(dropdown, dropdown_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/* Touchpad input handler for LVGL */
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

  // LVGL init
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

  // Create dropdown menu
  create_dropdown();
}

void loop() {
  lv_timer_handler(); // let LVGL handle the tasks
  delay(5);
}
