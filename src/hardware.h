#ifndef HARDWARE_H_
#define HARDWARE_H_

// Include necessary libraries for hardware functionality
#include <Arduino.h>                   // Core Arduino library
#include <FS.h>                        // Filesystem library
#include <SPI.h>                       // SPI communication library
#include <SPIFFS.h>                    // SPI Flash File System library
#include <Adafruit_Fingerprint.h>      // Library for fingerprint sensor
#include <TFT_eSPI.h>                  // TFT display library
#include <ArduinoJson.h>               // JSON library

// Hardware pin definitions
#define RX_PIN 25    // Fingerprint sensor RX pin
#define TX_PIN 33    // Fingerprint sensor TX pin
#define TOUCH_CS 21  // Touch screen chip select pin

// Extern declarations for hardware instances
extern TFT_eSPI tft;                 // TFT display instance
extern HardwareSerial mySerial;      // Hardware serial for fingerprint sensor
extern Adafruit_Fingerprint finger;  // Fingerprint sensor instance

// Screen resolution constants
const uint32_t kScreenWidth = 320;   // Screen width in pixels
const uint32_t kScreenHeight = 240;  // Screen height in pixels

// Function declarations for hardware-related functions
void TouchCalibrate();                // Function to calibrate touch screen
void InitializeHardware();            // Function to initialize hardware components
uint8_t GetFingerprintID();           // Function to get the fingerprint ID
void DeleteUserFromJSON(uint8_t id);  // Function to delete user data from JSON file
void SaveUserToJSON(uint8_t id, const char* name);  // Function to save user data to JSON file
const char* GetUserNameByID(uint8_t id);            // Function to get user name by ID
String ReadUsersFromJSON();           // Function to read users from JSON file
String GetUserListForDropdown();      // Function to get user list for dropdown menu
void DeleteFingerprint(uint8_t id);   // Function to delete fingerprint from sensor

#endif  // HARDWARE_H_
