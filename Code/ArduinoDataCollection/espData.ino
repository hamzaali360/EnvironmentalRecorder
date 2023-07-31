#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <map>
//#include <WiFi.h>
//#include <FirebaseArduino.h>
#include <ArduinoJson.h>
//#include <ESP8266HTTPClient.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define FIREBASE_HOST "envirodb-6e5a0-default-rtdb.firebaseio.com"
//#define FIREBASE_AUTH "bVa3La40UKW5dWTSvBMU1gkXSdZqfh4ixGmCnh3c"
#define WIFI_SSID "Hamza iPhone (2)" //provide ssid (wifi name)
#define WIFI_PASSWORD "testing1" //wifi password
#define API_KEY "AIzaSyBC7CYTus4U-4AU1fEMxjPvId0IYHcguvE"
#define USER_EMAIL "hali31@uic.edu"
#define USER_PASSWORD "testing"
#define FIREBASE_PROJECT_ID "envirodb-6e5a0"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String uid;
#include <SoftwareSerial.h>
#define TXE_PIN 8
#define RXE_PIN 9
SoftwareSerial espSerial(D6,D5);
/*
  ESP8266 Blink by Simon Peter
  Blink the blue LED on the ESP-01 module
  This example code is in the public domain

  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)

  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/
const int BUFFER_SIZE = 100;
char buf[BUFFER_SIZE];
float temp = 0;
float humid = 0;
float CO2 = 0;
float TVOC = 0;
float pm25 = 0;
float o3 = 0;
//std::map<std::string, String> data;
float latitude = 41.87209162429151;
float longitude = -87.64801332755813;
int id = 0;
String docPath = "Test/device0";
String historyPath = "history/device0";
String curTime;
void setup() {
  Serial.begin(115200);
  espSerial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected: ");
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback;
  config.max_token_generation_retry = 5;
  config.database_url = FIREBASE_HOST;
  Firebase.begin(&config, &auth);
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
//  uid = auth.token.uid.c_str();
//  docPath += uid;
//  Serial.print("User UID: ");
//  Serial.print(uid);
//  Serial.println(WiFi.localIP);

//  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
//
//  if(Firebase.failed()){
//    Serial.print(Firebase.error());
//  }
}
//DynamicJsonBuffer jsonBuffer;
//JsonObject& data = jsonBuffer.createObject();

// the loop function runs over and over again forever
void loop() {
  uid = auth.token.uid.c_str();
  FirebaseJson content;
  FirebaseJson history;
  if (Firebase.isTokenExpired()){
    Firebase.refreshToken(&config);
    Serial.println("Refresh token");
  }
  if (espSerial.available() > 0) {
    
    String temps = espSerial.readStringUntil(',');
    temps = espSerial.readStringUntil(',');
    String humids = espSerial.readStringUntil(',');
    String CO2s = espSerial.readStringUntil(',');
    String TVOCs = espSerial.readStringUntil(',');
    String pm25s = espSerial.readStringUntil(',');
    String o3s = espSerial.readStringUntil(',');
    curTime = espSerial.readStringUntil('\n');
    temp = temps.toFloat();
    humid = humids.toFloat();
    CO2 = CO2s.toFloat();
    TVOC = TVOCs.toFloat();
    pm25 = pm25s.toFloat();
    o3 = o3s.toFloat();

    Serial.print("Temperature = ");
    Serial.println(temp);
    Serial.print("Humidity = ");
    Serial.println(humid);
    Serial.print("CO2 = ");
    Serial.println(CO2);
    Serial.print("TVOC = ");
    Serial.println(TVOC);
    Serial.print("PM 2.5 = ");
    Serial.println(pm25);
    Serial.print("Ozone = ");
    Serial.println(o3);
    Serial.print("timestamp = ");
    Serial.println(curTime);
    Firebase.RTDB.setFloat(&fbdo,"data/temp", temp);
    Firebase.RTDB.setFloat(&fbdo,"data/humid", humid);
    Firebase.RTDB.setFloat(&fbdo,"data/CO2", CO2);
    Firebase.RTDB.setFloat(&fbdo,"data/TVOC", TVOC);
    Firebase.RTDB.setFloat(&fbdo,"data/Ozone", o3);
    Firebase.RTDB.setFloat(&fbdo,"data/PM25", pm25);
    Firebase.RTDB.setFloat(&fbdo,"lat", latitude);
    Firebase.RTDB.setFloat(&fbdo,"long", longitude);
    Firebase.RTDB.setFloat(&fbdo,"ID", id);
    Firebase.RTDB.setString(&fbdo,"timestamp", curTime);
//    yield();
    
//    content.toString(jsonString);
    content.set("fields/data/mapValue/fields/temp/doubleValue", temp);
    content.set("fields/data/mapValue/fields/humidity/doubleValue", humid);
    content.set("fields/data/mapValue/fields/ozone/doubleValue", o3);
    content.set("fields/data/mapValue/fields/pm25/doubleValue", pm25);
    content.set("fields/data/mapValue/fields/voc/doubleValue", TVOC);
    content.set("fields/data/mapValue/fields/CO2/doubleValue", CO2);
    content.set("fields/lat/doubleValue", latitude);
    content.set("fields/long/doubleValue", longitude);
    content.set("fields/ID/doubleValue", id);
    content.set("fields/Timestamp/stringValue", curTime);

//    history.toString(jsonString);
    
//      content.set("fields/data/mapValue", data);
//content.set("temp", String(temp).c_str());
//    content.set("humidity", String(humid).c_str());
//    content.set("ozone", String(o3).c_str());
//    content.set("pm25", String(pm25).c_str());
//    content.set("voc", String(TVOC).c_str());
//    content.set("CO2", String(CO2).c_str());
    String timePath = curTime;
    timePath.replace("/","\\");
    timePath.replace(" ", "_");
    Serial.println(timePath);
    history.set("fields/dates/mapValue/fields/" + timePath + "/arrayValue/values/[0]/doubleValue", temp);
    history.set("fields/dates/mapValue/fields/" + timePath + "/arrayValue/values/[1]/doubleValue", humid);
    history.set("fields/dates/mapValue/fields/" + timePath + "/arrayValue/values/[2]/doubleValue", o3);
    history.set("fields/dates/mapValue/fields/" + timePath + "/arrayValue/values/[3]/doubleValue", pm25);
    history.set("fields/dates/mapValue/fields/" + timePath + "/arrayValue/values/[4]/doubleValue", TVOC);
    history.set("fields/dates/mapValue/fields/" + timePath + "/arrayValue/values/[5]/doubleValue", CO2);

    String path = docPath + timePath;
//    if (history.readFrom(Serial))
//    {
//        Serial.println("JSON Data received...");
//        history.toString(Serial, true);
//        Serial.println();
//    }
//    Serial.println(path);
    // live data path
    if(Firebase.Firestore.patchDocument(&fbdo,FIREBASE_PROJECT_ID,"", docPath.c_str(), content.raw(),"data.temp,data.humidity,data.ozone,data.pm25,data.voc,data.CO2,lat,long,ID,Timestamp")){
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
//      delay(1);
      return;
      
    } else {
      Serial.println(fbdo.errorReason());
    }
//    if(Firebase.Firestore.createDocument(&fbdo,FIREBASE_PROJECT_ID,"", docPath.c_str(), content.raw())){
//      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
//      delay(1000);
//      return;
//      
//    } else {
//      Serial.println(fbdo.errorReason());
//    }
    // history data path
    String pathSet = "dates."+timePath + ".";
//    if(Firebase.Firestore.patchDocument(&fbdo,FIREBASE_PROJECT_ID,"", historyPath.c_str(), history.raw(),pathSet+"[0],"+pathSet+"[1]"+pathSet+"[2]"+pathSet+"[3]"+pathSet+"[4]"+pathSet+"[5]")){
//      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
//      return;
//      
//    } else {
//      Serial.println(fbdo.errorReason());
//    }
//    if(Firebase.Firestore.createDocument(&fbdo,FIREBASE_PROJECT_ID,"", historyPath.c_str(), history.raw())){
//      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
//      return;
//      
//    } else {
//      Serial.println(fbdo.errorReason());
//    }
  
  } else {
    Serial.println("Waiting for data");
  }
  delay(1000);                      // Wait for two seconds (to demonstrate the active low LED)
}
