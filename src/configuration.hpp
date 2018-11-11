#include "ArduinoJson.h"

class Configuration
{
  public:
    struct ConfigData
    {
        String apiUrl;
        String wifi_ssid;
        String wifi_password;
    } configData;
    void load();

  protected:
    bool getConfig();
};
