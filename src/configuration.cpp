#include "configuration.hpp"
#include "SPIFFS.h"
#include "FS.h"
#include "ArduinoJson.h"

/* ================================================================
                  Load configuration from SPIFFS
================================================================ */

bool Configuration::getConfig()
{
	File configFile = SPIFFS.open("/config.json", "r");
	if (!configFile)
	{
		Serial.println("Failed to open config file");
		return false;
	}

	size_t size = configFile.size();
	if (size > 4096)
	{
		Serial.println("Config file size is too large");
		return false;
	}

	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);

	// We don't use * here because ArduinoJson library requires the input
	// buffer to be mutable. If you don't use ArduinoJson, you may as well
	// use configFile.readBytes instead.
	configFile.readBytes(buf.get(), size);

	// See this helper assistant to calculate the bufferSize - https://bblanchon.github.io/ArduinoJson/assistant/
	const size_t bufferSize = 2 * JSON_OBJECT_SIZE(2) + 410;
	DynamicJsonBuffer jsonBuffer(bufferSize);
	JsonObject &root = jsonBuffer.parseObject(buf.get());

	if (!root.success())
	{
		Serial.println("Failed to parse config file");
		return false;
	}

	this->configData.apiUrl = root["apiUrl"].as<String>();
	JsonObject &wifi = root["wifi"];
	this->configData.wifi_ssid = wifi["ssid"].as<String>();
	this->configData.wifi_password = wifi["password"].as<String>();

	return true;
}

void Configuration::load()
{
	if (!SPIFFS.begin())
	{
		Serial.println("Failed to mount file system");
		return;
	}
	if (!getConfig())
	{
		Serial.println("Failed to load config");
		return;
	}
	else
	{
		Serial.println("Config loaded");
	}
}