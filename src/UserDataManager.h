#include <ArduinoJson.h>
#include <SPIFFS.h>

struct UserData {
    int id;
    String name;
};

void initializeFileSystem();
bool addUser(UserData user);
bool deleteUser(int id);
JsonArray getUserList();
bool saveData(JsonDocument& doc);
bool loadData(JsonDocument& doc);
JsonDocument get_users_from_json();