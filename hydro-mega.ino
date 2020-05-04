
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
#ifdef _HAS_EC
    eC = (float)analogRead(PIN_EC);
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
