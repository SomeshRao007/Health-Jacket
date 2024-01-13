//#include <WiFi.h>
//#include <PubSubClient.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define DS18B20 5
#define REPORTING_PERIOD_MS  1000
////up to here temp and spo2 sensors 
//#define WIFISSID "" 
//#define PASSWORD "" 
//#define TOKEN "BBFF-VR3W2LCkuM8dQpYhmJH5BWUo5bgM5e" //our key
//#define MQTT_CLIENT_NAME "esp32123456" // some unique name
//#define VARIABLE_LABEL1 "ECG" // variable name in unbibots
//#define VARIABLE_LABEL2 "PULSE"
//#define VARIABLE_LABEL3 "TEMP DS18B20"
//#define VARIABLE_LABEL4 "SPO2"
//#define DEVICE_LABEL "esp32" // first create a device then varibles in it. the device name 
#define ECG A0 // basically analog pins ..mana daniki 6-7 pins unidi A0 propritary analog pin so ade use chasa 

float BPM, SpO2, bodytemperature;

//char mqttBroker[]  = "industrial.api.ubidots.com";
//char payload_1[700];
//char payload_2[700];
//char topic[150];
//// Space to store values by ecg sensor .
//char str_ecg[10];
//char str_temp[10];
//char str_spo2[10];
//char str_pulse[10];
//
//WiFiClient ubidots;
//PubSubClient client(ubidots);

PulseOximeter pox;
uint32_t tsLastReport = 0;
OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);

 void onBeatDetected()
{
  Serial.println("Beat!");
}

//void callback(char* topic, byte* payload, unsigned int length) {
//  char p[length + 1];
//  memcpy(p, payload, length);
//  p[length] = NULL;
//  Serial.write(payload, length);
//  Serial.println(topic);
//}
// 
//void reconnect() {
//  while (!client.connected()) {
//    Serial.println("Attempting MQTT connection...");
//    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
//      Serial.println("Connected");
//    } else {
//      Serial.print("Failed, rc=");
//      Serial.print(client.state());
//      Serial.println(" try again in 2 seconds");
//      delay(2000);
//    }
//  }
//}

 
void setup() {
   Serial.begin(115200);
//  WiFi.begin(WIFISSID, PASSWORD);
//  Serial.print("Waiting for WiFi...");
//  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print(".");
//    delay(500);
//  }
//  Serial.println("");
//  Serial.println("WiFi Connected");
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());
//  client.setServer(mqttBroker, 1883);
//  client.setCallback(callback);  
  
  Serial.print("Initializing pulse oximeter..");
 
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
    pox.setOnBeatDetectedCallback(onBeatDetected);
  }
 
   pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);//pulse oxi input-till here
  

  pinMode(ECG, INPUT);
  Serial.println();// ecg input
   
  pinMode(5, OUTPUT);
  delay(1000);// temp input

  
}
 
void loop() {
//  if (!client.connected()) {
//    reconnect();
//  }
  float ecg = analogRead(ECG); 
  pox.update();
  sensors.requestTemperatures();
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();
  bodytemperature = sensors.getTempCByIndex(0);

if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
  
    Serial.print("ECG: ");
    Serial.println(ecg);
    Serial.print("BPM: ");
    Serial.println(BPM);
    
    Serial.print("SpO2: ");
    Serial.print(SpO2);
    Serial.println("%");
 
    Serial.print("Body Temperature: ");
    Serial.print(bodytemperature);
    Serial.println("°C");
    
    Serial.println("*********************************");
    Serial.println();
  
  
//  dtostrf(sensor, 4, 2, str_ecg);
//  
//  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
//  
//  sprintf(payload_1, "%s", ""); // Cleans the payload
//  sprintf(payload_1, "{\"%s\":", VARIABLE_LABEL1); // adding our the ecg's variable label..
//  sprintf(payload, "%s {\"value\": %s}}", payload, str_ecg);
//  sprintf(payload_1, "{\"%s\":", VARIABLE_LABEL2); // adding our the variable label..
//  
//  
//  
//  
//  /* 4 is minimum width, 2 is precision and float value is copied onto str_sensor*/
// 
//  
//   
//  Serial.println("Publishing data to Ubidots Cloud");
//  client.publish(topic, payload);
//  client.loop();
//  delay(500);
}
}
