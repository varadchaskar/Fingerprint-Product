#include "UserDataManager.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>

const char* filename = "/user_data.json";

void initializeFileSystem() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount file system.");
    }
}

bool saveData(JsonDocument& doc) {
    File file = SPIFFS.open(filename, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }
    serializeJson(doc, file);
    file.close();
    return true;
}

bool loadData(JsonDocument& doc) {
    File file = SPIFFS.open(filename, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return false;
    }
    deserializeJson(doc, file);
    file.close();
    return true;
}

bool addUser(UserData user) {
    StaticJsonDocument<1024> doc;
    loadData(doc);
    JsonArray users = doc["users"].to<JsonArray>();

    JsonObject newUser = users.createNestedObject();
    newUser["id"] = user.id;
    newUser["name"] = user.name;

    return saveData(doc);
}

bool deleteUser(int id) {
    StaticJsonDocument<1024> doc;
    loadData(doc);
    JsonArray users = doc["users"].to<JsonArray>();

    for (JsonObject user : users) {
        if (user["id"] == id) {
            users.remove(user);
            return saveData(doc);
        }
    }
    return false;
}

JsonArray getUserList() {
    StaticJsonDocument<1024> doc;
    loadData(doc);
    return doc["users"].as<JsonArray>();
}

JsonDocument get_users_from_json() {
    File file = SPIFFS.open("/users.json", "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return JsonDocument(0);  // Return an empty JSON document on failure
    }

    size_t size = file.size();
    std::unique_ptr<char[]> buf(new char[size]);
    file.readBytes(buf.get(), size);
    file.close();

    // Parse the JSON file
    StaticJsonDocument<1024> doc;  // Adjust size as needed
    DeserializationError error = deserializeJson(doc, buf.get());
    if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return JsonDocument(0);  // Handle parsing error
    }

    return doc;
}
