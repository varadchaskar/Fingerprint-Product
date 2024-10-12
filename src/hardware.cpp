#include "hardware.h"

// Hardware instances
TFT_eSPI tft = TFT_eSPI();        // Create TFT display instance
HardwareSerial mySerial(2);       // Create hardware serial on UART2 for fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);  // Create fingerprint sensor instance

/* Touch screen calibration function */
void TouchCalibrate() {
  uint16_t cal_data[5];  // Array to store calibration data
  uint8_t cal_data_ok = 0;  // Flag to indicate if calibration data is valid

  // Initialize SPIFFS (SPI Flash File System)
  if (!SPIFFS.begin()) {
    Serial.println("Formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // Check if calibration data file exists
  if (SPIFFS.exists("/TouchCalData3")) {
    File f = SPIFFS.open("/TouchCalData3", "r");
    if (f) {
      // Read calibration data from file
      if (f.readBytes((char*)cal_data, 14) == 14) cal_data_ok = 1;
      f.close();
    }
  }

  if (cal_data_ok) {
    // Set touch calibration data
    tft.setTouch(cal_data);
  } else {
    // Perform touch calibration
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.println("Touch corners as indicated");

    // Calibrate touch and save calibration data
    tft.calibrateTouch(cal_data, TFT_MAGENTA, TFT_BLACK, 15);
    File f = SPIFFS.open("/TouchCalData3", "w");
    if (f) {
      f.write((const unsigned char*)cal_data, 14);
      f.close();
    }
  }
}

/* Initialize hardware components */
void InitializeHardware() {
  // Initialize Serial communication
  Serial.begin(115200);

  // Initialize hardware serial for fingerprint sensor
  mySerial.begin(57600, SERIAL_8N1, RX_PIN, TX_PIN);

  // Initialize TFT display
  tft.begin();
  tft.setRotation(1);  // Set display rotation

  // Perform touch screen calibration
  TouchCalibrate();

  // Initialize SPIFFS (SPI Flash File System)
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Initialize the fingerprint sensor
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }  // Halt execution
  }
}

/* Function to handle fingerprint detection and matching */
uint8_t GetFingerprintID() {
  uint8_t p = finger.getImage();

  // No finger detected
  if (p == FINGERPRINT_NOFINGER) return FINGERPRINT_NOFINGER;

  // Check if the image can be converted to features
  if (p != FINGERPRINT_OK) return p;
  p = finger.image2Tz();

  // Search for a matching fingerprint
  if (p != FINGERPRINT_OK) return p;
  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) return p;

  // Return the found fingerprint ID
  return finger.fingerID;
}

/* Save user data to JSON file */
void SaveUserToJSON(uint8_t id, const char* name) {
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Load existing JSON file
  File file = SPIFFS.open("/users.json", "r");
  StaticJsonDocument<512> doc;  // JSON document to hold user data

  if (file) {
    // Deserialize JSON data from file
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
      Serial.println(F("Failed to read file, using empty JSON"));
    }
    file.close();
  }

  // Remove existing entry for the same ID if exists
  String id_str = String(id);
  if (doc.containsKey(id_str)) {
    doc.remove(id_str);
    Serial.println("Old user data for this ID has been removed.");
  }

  // Add or update the user in the JSON object
  JsonObject user_obj = doc.createNestedObject(id_str);
  user_obj["id"] = id;
  user_obj["name"] = name;

  // Save the updated JSON to the file
  file = SPIFFS.open("/users.json", "w");
  if (!file) {
    Serial.println(F("Failed to open file for writing"));
    return;
  }

  // Serialize JSON data to file
  serializeJson(doc, file);
  file.close();

  Serial.println("User data saved successfully.");
}

/* Helper function to get the user name by fingerprint ID */
const char* GetUserNameByID(uint8_t id) {
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Failed to mount SPIFFS");
    return "Unknown User";
  }

  // Open the JSON file
  File file = SPIFFS.open("/users.json", "r");
  if (!file) {
    Serial.println("Failed to open users.json");
    return "Unknown User";
  }

  // Parse the JSON file
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("Failed to parse JSON");
    file.close();
    return "Unknown User";
  }

  // Close the file
  file.close();

  // Search for the user by ID
  String id_str = String(id);
  if (doc.containsKey(id_str)) {
    const char* name = doc[id_str]["name"];
    return name;
  }
  // If no user is found, return "Unknown User"
  return "Unknown User";
}

/* Read users from JSON and return as formatted string */
String ReadUsersFromJSON() {
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return "Failed to mount SPIFFS";
  }

  // Open the JSON file
  File file = SPIFFS.open("/users.json", "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return "No users found.";
  }

  // Load the JSON data
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println(F("Failed to read file, using empty JSON"));
    return "Error reading user data.";
  }

  // Format the users list
  String user_list = "";
  for (JsonPair kv : doc.as<JsonObject>()) {
    uint8_t id = kv.value()["id"];
    const char* name = kv.value()["name"];
    user_list += "ID: " + String(id) + ", Name: " + String(name) + "\n";
  }

  return user_list.length() > 0 ? user_list : "No users found.";
}

/* Get user list in dropdown format */
String GetUserListForDropdown() {
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return "";
  }

  // Open the JSON file
  File file = SPIFFS.open("/users.json", "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return "";
  }

  // Load the JSON data
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.println(F("Failed to read file, using empty JSON"));
    return "";
  }

  // Format the users list for dropdown options
  String user_list = "";
  for (JsonPair kv : doc.as<JsonObject>()) {
    uint8_t id = kv.value()["id"];
    const char* name = kv.value()["name"];
    user_list += "ID: " + String(id) + ", Name: " + String(name) + "\n";
  }

  return user_list;
}

/* Delete user data from JSON */
void DeleteUserFromJSON(uint8_t id) {
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Load existing JSON file
  File file = SPIFFS.open("/users.json", "r");
  StaticJsonDocument<512> doc;

  if (file) {
    // Deserialize JSON data from file
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
      Serial.println(F("Failed to read file, using empty JSON"));
    }
    file.close();
  }

  // Remove the user from the JSON object
  String id_str = String(id);
  if (doc.containsKey(id_str)) {
    doc.remove(id_str);

    // Save the updated JSON to the file
    file = SPIFFS.open("/users.json", "w");
    if (!file) {
      Serial.println(F("Failed to open file for writing"));
      return;
    }

    // Serialize JSON data to file
    serializeJson(doc, file);
    file.close();

    Serial.println("User data deleted successfully.");
  } else {
    Serial.println("User ID not found in JSON.");
  }
}

/* Delete fingerprint template from sensor */
void DeleteFingerprint(uint8_t id) {
  int delete_status = finger.deleteModel(id);
  if (delete_status == FINGERPRINT_OK) {
    Serial.println("Fingerprint deleted from sensor.");
  } else {
    Serial.println("Failed to delete fingerprint from sensor.");
  }
}
