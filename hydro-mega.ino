/*
 * Copyright (C), 2000 Christopher F. Moran
 * This code controls a simple hydroponic pump for Ebb * Flow operation
 * 
 * It is intended to be modular, and features can be added or removed by
 * defining conditions within sysconfig.h.  The logic behind this structure is
 * that the final code can get very close to maximum memory on some controllers
 * which may cause stability problems.
 */
 
#include "hydro.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
#ifdef _HAS_PUMP_RELAY
  pinMode(PIN_PUMP, OUTPUT);
#endif  
#ifdef _HAS_LAMP_RELAY
  pinMode(PIN_LAMP, OUTPUT);
#endif

  Serial.begin(115200);
  Serial.println();
#ifdef _DEBUG  
  Serial.println("Compiled modules:");
#ifdef _HAS_THINGSPEAK
  Serial.println("ThingSpeak");
#endif
#ifdef _HAS_ETHERNET
  Serial.println("Ethernet");
#endif
#ifdef _HAS_WIFI_ESP
  Serial.println("ESP WiFi");
#endif
#ifdef _HAS_DALLAS
  Serial.println("DS18B20");
#endif
#ifdef _HAS_DHT
  Serial.println("DHT");
#endif
#ifdef _HAS_PUMP_RELAY
  Serial.println("Pump Relay");
#endif
#ifdef _HAS_LAMP_RELAY
  Serial.println("Lamp Relay");
#endif
#ifdef _HAS_PH
  Serial.println("pH Sensor");
#endif
#ifdef _HAS_EC
  Serial.println("Ec Sensor");
#endif
#ifdef _HAS_VBATT
  Serial.println("Battery Voltage");
#endif
#endif
  Serial.println("Starting up");

#ifdef _HAS_ETHERNET
  Ethernet.init(10);
  if(Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if(Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet hardware not found");
      while(true);  // Can't continue
    }
    if(Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable not connected");
      while(true); // Can't continue
    }
    Ethernet.begin(mac, ip, dns);
  }
  // Allow the Ethernet hardware to start up fully
  delay(1500);
#endif

#ifdef _HAS_WIFI
  int wifiStatus = WL_IDLE_STATUS;
  Serial3.begin(115200);
  WiFi.init(&Serial3);
  if(WiFi.status() == WL_NO_SHIELD) {
    Serial.println("No WiFi Shield");
    while(true);  // Can't continue
  }
  while(wifiStatus != WL_CONNECTED) {
    wifiStatus = WiFi.begin(ssid, pass);
    delay(1000);
  }
#endif

#ifdef _HAS_DHT
  dht.begin();
#endif
#ifdef _HAS_DALLAS
  sensors.begin();
#endif
#ifdef _HAS_THINGSPEAK
  ThingSpeak.begin(client);
#endif
}

#ifdef _HAS_PUMP_RELAY
void setPump(void) {
  Serial.println("Checking pump " + String(mins) + " min (" + String(idlePump) + ")");
  if(idlePump == 0 && mins > 120) {
    idlePump = 1;
    Serial.println("Stop Pump");
    digitalWrite(PIN_PUMP, RLY_OFF);
#ifdef _HAS_THINGSPEAK
    ThingSpeak.setStatus("Pump Stopped");
#endif
    mins = 0;
  }
  if(idlePump == 1 && mins > 114) {
    idlePump = 1;
    Serial.println("Start Pump");
    digitalWrite(PIN_PUMP, RLY_ON);
#ifdef _HAS_THINGSPEAK
    ThingSpeak.setStatus("Pump Started");
#endif
    mins = 0;
  }
}
#endif

#ifdef _HAS_EC
// Reference
// https://hackaday.io/project/7008-fly-wars-a-hackers-solution-to-world-hunger/log/24646-three-dollar-ec-ppm-meter-arduino
float readEc(float fluidTemp) {
  int r1 = 1000;
  int rA = 25;
  float ppmConversion = 0.7;
  float temperatureCoef = 0.019;
  float k = 2.88;
  float raw = 0.0;
  float vIn = 5.0;
  float vDrop = 0.0;
  float rC = 0;
  float eC = 0.0;

  digitalWrite(EC_POWER_PIN, HIGH); // Power on
  digitalWrite(EC_SINK_PIN, LOW);
  raw= analogRead(PIN_EC);
  raw= analogRead(PIN_EC);  // Needs to be run twice, based on experimental results.  See source article.
  digitalWrite(EC_POWER_PIN,HIGH);  // Power off
  vDrop = (vIn * raw) / 1024.0;
  rC = (vDrop * r1) / (vIn - vDrop) - rA;
  eC = (1000 / (rC * k)) / (1+ temperatureCoef*(fluidTemp - 25.0));

  return eC;
}
#endif


void loop() {
#ifdef _HAS_DHT
  float airHumidity = 0.0;
  float airTemp = 0.0;
#endif
#ifdef _HAS_DALLAS  
  float fluidTemp = 0.0;
#endif
#ifdef _HAS_PH
  float pH = 7.0;
#endif
#ifdef _HAS_EC  
  float eC = 0.0;
#endif
#ifdef _HAS_VBATT  
  float vBatt = 12.0;
#endif  

  digitalWrite(LED_BUILTIN, stat);
  stat = !stat;

  if(secs == 59) {
    secs = 0;
#ifdef _HAS_PUMP_RELAY
    setPump();
#endif
#ifdef _HAS_DHT
    airTemp = dht.readTemperature();
    airHumidity = dht.readHumidity();
    if(isnan(airTemp)) {
      Serial.println("Error getting airTemp");
      airTemp = 0.0;
    }
    else {
      Serial.println("Air Temp: " + String(airTemp) + "C");
#ifdef _HAS_THINGSPEAK
      ThingSpeak.setField(1, airTemp);
#endif      
    }
    if(isnan(airHumidity)) {
      Serial.println("Error getting airHumidity");
      airHumidity = 0.0;
    }
    else {
      Serial.println("Humidity: " + String(airHumidity) + "%");
#ifdef _HAS_THINGSPEAK
      ThingSpeak.setField(2, airHumidity);
#endif      
    }
#endif
#ifdef _HAS_DALLAS
    sensors.requestTemperatures();
    fluidTemp = sensors.getTempCByIndex(0);
    if(isnan(fluidTemp)) {
      Serial.println("Error getting fluidTemp");
      fluidTemp = 0.0;
    }
    else {
      Serial.println("Nutrient Temp: " + String(fluidTemp) + "C");
#ifdef _HAS_THINGSPEAK
      ThingSpeak.setField(3, fluidTemp);
#endif
      if(fluidTemp < fluidAlarmLow) {
        Serial.println("ALARM: LOW NUTRIENT TEMPERATURE");
      }
      if(fluidTemp > fluidAlarmHigh) {
        Serial.println("ALARM: HIGH NUTRIENT TEMPERATURE");
      }
    }
#endif
#ifdef _HAS_PH
    pH = (float)analogRead(PIN_PH);
    if(isnan(pH)) {
      Serial.println("Error getting pH");
      pH = 7.0;
    }
    else {
      Serial.println("pH: " + String(pH));
#ifdef _HAS_THINGSPEAK
      ThingSpeak.setField(4, pH);
#endif
      
    }
#endif
#ifdef _HAS_DALLAS
#ifdef _HAS_EC
    eC = readEc(fluidTemp);
    if(isnan(eC)) {
      Serial.println("Error getting eC");
      eC = 0.0;
    }
    else {
      Serial.println("Ec: " + String(eC));
#ifdef _HAS_THINGSPEAK
      ThingSpeak.setField(5, eC);
#endif
      
    }
#endif
#endif
#ifdef _HAS_VBATT
    vBatt = (float)analogRead(PIN_VBATT) / 1024 * 5.0 * batFactor;
    if(isnan(pH)) {
      Serial.println("Error getting vBatt");
      vBatt = 12.0;
    }
    else {
      Serial.println("vBatt: " + String(vBatt));
#ifdef _HAS_THINGSPEAK
      ThingSpeak.setField(8, vBatt);
#endif
      
    }
#endif
#ifdef _HAS_THINGSPEAK
    int x = ThingSpeak.writeFields(channelId, writeApiKey);
    if(x != 200) {
      Serial.println("Error " + String(x) + " sending to ThingSpeak");
    }
#endif
  }
  delay(1000);
  secs++;
}
