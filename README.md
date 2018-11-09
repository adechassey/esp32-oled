# ESP32 OLED - data display
A small WiFi enabled device that pulls JSON from a server to display data.

# Parts
* Wemos LoLin D32: buy it [here](https://fr.aliexpress.com/item/Lolin-ESP32-wemos-ESP32-WiFi-Modules-Bluetooth-Dual-ESP-32-ESP-32S-ESP8266-X-Battery-Shield/32810419853.html?spm=a2g0w.search0104.3.1.291b2edbiGsuLm&ws_ab_test=searchweb0_0,searchweb201602_4_10065_10068_319_10892_317_10696_5728811_453_10084_454_10083_10618_10304_10307_10820_10821_537_10302_536_5730115_5733215_5733315_10843_328_10059_10884_5731015_5733115_10887_5732715_100031_5733415_321_322_5730015_10103_10913_5729115_10912_5733515_5729015_5733615,searchweb201603_2,ppcSwitch_0&algo_expid=3b1ad4e7-cf29-441f-89cb-ce2ea198281e-0&algo_pvid=3b1ad4e7-cf29-441f-89cb-ce2ea198281e)

# Prerequisite
* Do not forget to create the `config.json` file in folder `data` (in the root project directory - same as src/)
* `config.json` must contain:
```json
{
	"apiUrl": "<YOUR_API_URL>",
	"wifi": {
		"ssid": "<YOUR_SSID>",
		"password": "<YOUR_PASSWORD>"
	}
}
```

# Build
* Uses [Platform.io](https://platform.io/) IDE
