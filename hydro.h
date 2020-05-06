#include "sysconfig.h"
#ifdef _HAS_THINGSPEAK
unsigned long channelId = 12345678;
const char *writeApiKey = "ABCDEF12345678";
#include <ThingSpeak.h>
#endif

#ifdef _HAS_ETHERNET
#include "Ethernet.h"
byte mac[] = { 0x02, 0x60, 0x8c, 0x01, 0x02, 0x05 };
IPAddress ip(192, 168, 1, 2);
IPAddress dns(8, 8, 8, 8);
#ifndef _HAS_WIRELESS
EthernetClient(client);
#endif
#endif

#ifdef _HAS_WIFI
#ifdef _HAS_WIFI_ESP
#include <WiFiEsp.h>
#ifndef _HAS_ETHERNET
WiFiEspClient client;
#endif
#endif
char ssid[] = "Emergent_Technology";
char pass[] = "3F14AAE191504E309182163153";
#endif

#ifdef _HAS_DHT
#include <DHT.h>
#include <DHT_U.h>
#define DHT_PIN   3
#define DHT_TYPE  DHT22
DHT dht(DHT_PIN, DHT_TYPE);
#endif

#ifdef _HAS_DALLAS
#include <DallasTemperature.h>
#include <OneWire.h>
#define ONE_WIRE_BUS  2
OneWire oneWireBus(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWireBus);
float fluidAlarmLow = 8.0;
float fluidAlarmHigh = 30.0;

#ifdef _HAS_EC
#define PIN_EC    A1
#define EC_POWER_PIN  6
#define EC_SINK_PIN   7

float readEc(float fluidTemp);
#endif
#endif

#ifdef _HAS_PUMP_RELAY
#define PIN_PUMP    4
int idlePump = 1;
#endif

#ifdef _HAS_LAMP_RELAY
#define PIN_LAMP  5
#endif

#ifdef _HAS_PH
#define PIN_PH    A0
#endif


#ifdef _HAS_VBATT
#define PIN_VBATT    A2
float batFactor = 11.69;
#endif

#ifdef _HAS_SSD1306
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
#endif

#define RLY_OFF HIGH
#define RLY_ON  LOW

// Internal defs
int stat = 1;
int secs = 0;
int mins = 0;
