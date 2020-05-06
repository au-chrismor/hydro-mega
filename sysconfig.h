#define _DEBUG
#define _HAS_THINGSPEAK
//#define _HAS_ETHERNET
#define _HAS_WIFI
#ifdef _HAS_WIFI
#define _HAS_WIFI_ESP
//#define _HAS_WIFI_NINA
#endif
//#define _HAS_GSM
#define _HAS_DHT
#define _HAS_DALLAS
#define _HAS_PUMP_RELAY
#ifdef _HAS_PUMP_RELAY
#define _IDLE_MINUTES 15
#define _RUN_MINUTES  120
#endif
//#define _HAS_LAMP_RELAY
#define _HAS_PH
#define _HAS_EC
#define _HAS_VBATT
#define _HAS_SSD1306
