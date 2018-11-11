/*
   Project: esp32-oled
   Version: 0.1
   Author: Antoine de Chassey
   Date: 09/11/2018
 */
#include "configuration.hpp"
#include "deviceData.hpp"
// JSON
#include "ArduinoJson.h"
// OLED
#include "SH1106.h"
// Include custom fonts & images
#include "images.h"
// WiFi
#include "WiFi.h"
// HTTP client
#include "HTTPClient.h"

//========== Functions ==========//
// HTTP
void getHttpData();
// Display
void newTextFlow(String text, OLEDDISPLAY_TEXT_ALIGNMENT textAlignment);
void appendTextFlow(String text);
void displayDataUp(DeviceData deviceData);
void displayDataMiddle(DeviceData deviceData);
void displayDataDown(DeviceData deviceData);

//========== Objects ==========//
// App configuration
Configuration app;
// Device data
DeviceData deviceData;
// Initialize the OLED display using Wire library
SH1106 display(0x3c, 5, 4);
// HTTP client
HTTPClient http;

void setup()
{
	Serial.begin(115200);
	Serial.println("-------------------------\n");

	// Initialising the UI will init the display too
	display.init();
	display.clear();
	display.display();
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_CENTER);
	display.setContrast(255);
	//display.flipScreenVertically();

	// Read config.json stored in SPIFFS memory
	app.load();

	// sprintf(clientID, "%lu", (long uint) ESP.getEfuseMac());

	Serial.print(F("Trying to connect to access point..."));

	WiFi.mode(WIFI_STA);
	WiFi.begin(app.configData.wifi_ssid.c_str(), app.configData.wifi_password.c_str());
	// Trying to connect to default AP
	int counter = 0;
	while ((WiFi.status() != WL_CONNECTED))
	{
		delay(500);
		Serial.print(".");
		// UI
		display.clear();
		display.drawString(64, 10, "Connecting to WiFi");
		display.drawXbm(46, 30, 8, 8, counter % 3 == 0 ? activeSymbole : inactiveSymbole);
		display.drawXbm(60, 30, 8, 8, counter % 3 == 1 ? activeSymbole : inactiveSymbole);
		display.drawXbm(74, 30, 8, 8, counter % 3 == 2 ? activeSymbole : inactiveSymbole);
		display.display();
		counter++;
	}

	Serial.println("");
	Serial.print("WiFi SSID: ");
	Serial.println(WiFi.SSID());
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	Serial.printf("\nConnection status: %d\n", WiFi.status());
	WiFi.setAutoReconnect(true);

	newTextFlow("Connected to WiFi", TEXT_ALIGN_CENTER);
	delay(1000);
}

void loop()
{
	display.clear();
	/*
   // draw the current demo method
   demos[demoMode]();
   display.setTextAlignment(TEXT_ALIGN_RIGHT);
   display.drawString(10, 128, String(millis()));
   // write the buffer to the display
   display.display();
   if (millis() - timeSinceLastModeSwitch > DEMO_DURATION) {
    demoMode = (demoMode + 1)  % demoLength;
    timeSinceLastModeSwitch = millis();
   }
   counter++;
   delay(10);
  */
	getHttpData();
	WiFi.disconnect();
	ESP.deepSleep(120000000); // 10 minutes
}

/* ================================================================
                          HTTP functions
================================================================ */
void getHttpData()
{
	http.begin(app.configData.apiUrl);
	int httpCode = http.GET();
	if (httpCode == 200)
	{
		const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(11) + JSON_OBJECT_SIZE(1) + 6 * JSON_OBJECT_SIZE(3) + 5 * JSON_OBJECT_SIZE(4) + 480;
		DynamicJsonBuffer jsonBuffer(bufferSize);

		const int size = http.getSize() + 1;
		char json[size];
		http.getString().toCharArray(json, size);
		// Serial.println(json);

		JsonArray &root = jsonBuffer.parseArray(json);
		if (!root.success())
		{
			Serial.println("parseArray() failed");
			return;
		}
		JsonArray &data_parsed = root[0]["data_parsed"];

		display.clear();

		JsonObject &obj1 = data_parsed[2];
		deviceData.key = obj1["key"];
		deviceData.value = obj1["value"];
		deviceData.type = obj1["type"];
		deviceData.unit = obj1["unit"];
		displayDataUp(deviceData);

		JsonObject &obj2 = data_parsed[3];
		deviceData.key = obj2["key"];
		deviceData.value = obj2["value"];
		deviceData.type = obj2["type"];
		deviceData.unit = obj2["unit"];
		displayDataMiddle(deviceData);

		JsonObject &obj3 = data_parsed[10];
		deviceData.key = obj3["key"];
		deviceData.value = obj3["value"];
		deviceData.type = obj3["type"];
		deviceData.unit = obj3["unit"];
		displayDataDown(deviceData);
	}
	else
	{
		Serial.println("Error on HTTP request");
	}
	http.end();
}

/* ================================================================
                          OLED functions
================================================================ */

void newTextFlow(String text, OLEDDISPLAY_TEXT_ALIGNMENT alignment)
{
    // Clear the display
    display.clear();
    display.setTextAlignment(alignment);
    display.drawStringMaxWidth(64, 5, 128, text); //display.drawString(64, 5, text);
    // Write the buffer to the display
    display.display();
}

void appendTextFlow(String text)
{
    display.drawString(64, 5, text);
    // Write the buffer to the display
    display.display();
}

void displayDataUp(DeviceData deviceData)
{
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(2, 10, strcat((char *)deviceData.key, " :"));
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 10, strcat(strcat((char *)deviceData.value, " "), deviceData.unit));
    display.display();
}

void displayDataMiddle(DeviceData deviceData)
{
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(2, 30, strcat((char *)deviceData.key, " :"));
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 30, strcat(strcat((char *)deviceData.value, " "), deviceData.unit));
    display.display();
}

void displayDataDown(DeviceData deviceData)
{
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(2, 50, strcat((char *)deviceData.key, " :"));
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(128, 50, strcat(strcat((char *)deviceData.value, " "), deviceData.unit));
    display.display();
}

// void drawFontFaceDemo() {
//         // Font Demo1
//         // create more fonts at http://oleddisplay.squix.ch/
//         display.setTextAlignment(TEXT_ALIGN_LEFT);
//         display.setFont(ArialMT_Plain_10);
//         display.drawString(0, 0, "Hello world");
//         display.setFont(ArialMT_Plain_16);
//         display.drawString(0, 10, "Hello world");
//         display.setFont(ArialMT_Plain_24);
//         display.drawString(0, 26, "Hello world");
// }
//

//
// void drawTextAlignmentDemo() {
//         // Text alignment demo
//         display.setFont(ArialMT_Plain_10);
//
//         // The coordinates define the left starting point of the text
//         display.setTextAlignment(TEXT_ALIGN_LEFT);
//         display.drawString(0, 10, "Left aligned (0,10)");
//
//         // The coordinates define the center of the text
//         display.setTextAlignment(TEXT_ALIGN_CENTER);
//         display.drawString(64, 22, "Center aligned (64,22)");
//
//         // The coordinates define the right end of the text
//         display.setTextAlignment(TEXT_ALIGN_RIGHT);
//         display.drawString(128, 33, "Right aligned (128,33)");
// }
//
// void drawRectDemo() {
//         // Draw a pixel at given position
//         for (int i = 0; i < 10; i++) {
//                 display.setPixel(i, i);
//                 display.setPixel(10 - i, i);
//         }
//         display.drawRect(12, 12, 20, 20);
//
//         // Fill the rectangle
//         display.fillRect(14, 14, 17, 17);
//
//         // Draw a line horizontally
//         display.drawHorizontalLine(0, 40, 20);
//
//         // Draw a line horizontally
//         display.drawVerticalLine(40, 0, 20);
// }
//
// void drawCircleDemo() {
//         for (int i=1; i < 8; i++) {
//                 display.setColor(WHITE);
//                 display.drawCircle(32, 32, i*3);
//                 if (i % 2 == 0) {
//                         display.setColor(BLACK);
//                 }
//                 display.fillCircle(96, 32, 32 - i* 3);
//         }
// }
//
// void drawProgressBarDemo() {
//         int progress = (counter / 5) % 100;
//         // draw the progress bar
//         display.drawProgressBar(0, 32, 120, 10, progress);
//
//         // draw the percentage as String
//         display.setTextAlignment(TEXT_ALIGN_CENTER);
//         display.drawString(64, 15, String(progress) + "%");
// }
//
// void drawImageDemo() {
//         // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
//         // on how to create xbm files
//         display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
// }