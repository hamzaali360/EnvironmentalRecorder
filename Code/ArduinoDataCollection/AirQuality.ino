#include "MQ131.h"
#include "DHT.h"
#include "MHZ19.h"
#include <SoftwareSerial.h>
#include "PMS.h"
#include <Wire.h>
#include "RTClib.h"

#define DHTPIN 7
#define DHTTYPE DHT11
#define RX_PIN 3
#define TX_PIN 2
#define TXP_PIN 4
#define RXP_PIN 5
#define TXE_PIN 8
#define RXE_PIN 9

DHT dht(DHTPIN, DHTTYPE); // Temp and Humitidy sensor
MHZ19 myMHZ19;    // CO2 Sensor
SoftwareSerial CO2Sensor(RX_PIN, TX_PIN); 
SoftwareSerial PMSSerial(RXP_PIN, TXP_PIN);
//SoftwareSerial espSerial(RXE_PIN, TXE_PIN);
PMS pms(PMSSerial);
PMS::DATA data;
RTC_DS3231 rtc;

float temp = 0;
float humid = 0;
float CO2 = 0;
float TVOC = 0;
float pm25 = 0;
float o3 = 0;
//String data = String(temp,2) + "," + String(humid,2) + String(CO2,2) + "," + String(TVOC,2) + "," + String(pm25,2);
void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (! rtc.begin()) {
//    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  pinMode(6, OUTPUT); // OZONE
  pinMode(11, OUTPUT); // VOC
  digitalWrite(6,HIGH);
  digitalWrite(11,HIGH);
  delay(5000);
  digitalWrite(6,LOW);
  digitalWrite(11,LOW);
  CO2Sensor.begin(9600);
  PMSSerial.begin(9600);
//  espSerial.begin(9600);
  myMHZ19.begin(CO2Sensor);
  myMHZ19.autoCalibration();
  

  dht.begin();
  MQ131.begin(6,A0, LOW_CONCENTRATION, 1000000);
   MQ131.setTimeToRead(5); // Set how many seconds we will read from the Ozone sensor. It blocks flow
  MQ131.setR0(9000); // We get this value using the calirabrate() function from the Library calibration example
//  Serial.println("Calibration in progress...");
//   MQ131.calibrate();
//  
//  Serial.println("Calibration done!");
//  Serial.print("R0 = ");
//  Serial.print(MQ131.getR0());
//  Serial.println(" Ohms");
//  Serial.print("Time to heat = ");
//  Serial.print(MQ131.getTimeToRead());
//  Serial.println(" s");
  // put your setup code here, to run once:

}

void loop() {
  DateTime now = rtc.now();
  humid = dht.readHumidity();
  temp = dht.readTemperature(true);
  digitalWrite(11, HIGH);
  delay(5000);
  TVOC = analogRead(A1);
  digitalWrite(11, LOW);
//  Serial.print("Temperature == ");
//  Serial.println(temp);
//  Serial.print("Humidity == ");
//  Serial.println(humid);
//  Serial.print("TVOC == ");
//  Serial.println(TVOC);

  CO2Sensor.listen();
  unsigned long times = millis();
  while(millis() - times <= 3000) {
    CO2 = myMHZ19.getCO2();
  }

  PMSSerial.listen();
  unsigned long dataTimer3 = millis();
  while (millis() - dataTimer3 <= 1000) {
    pms.readUntil(data);
    pm25 = data.PM_AE_UG_2_5;
  }
  
//  Serial.print("CO2 == ");
//  Serial.println(CO2);
//  Serial.print("PM 2.5 == ");
//  Serial.println(pm25);
//  
//  Serial.println("Sampling...");
//  MQ131.sample();
//  Serial.print("Concentration O3 : ");
//  Serial.print(MQ131.getO3(PPM));
//  Serial.println(" ppm");
//  Serial.print("Concentration O3 : ");
    o3 = analogRead(A0);
//  Serial.print(MQ131.getO3(PPB));
//  Serial.println(" ppb");
//  Serial.print("Concentration O3 : ");
//  Serial.print(MQ131.getO3(MG_M3));
//  Serial.println(" mg/m3");
//  Serial.print("Concentration O3 : ");
//  Serial.print(MQ131.getO3(UG_M3));
//  Serial.println(" ug/m3");
  String curTime = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  String data = String(temp,2) + "," + String(humid,2) + "," + String(CO2,2) + "," + String(TVOC,2) + "," + String(pm25,2) + "," + String(o3) + "," + curTime +"\n";
//  uint8_t bytes[data.length()];
//  data.getBytes(bytes, data.length());
  Serial.write(data.c_str(), data.length());
//  Serial.println();
//Serial.print(now.year(), DEC);
//  Serial.print('/');
//  Serial.print(now.month(), DEC);
//  Serial.print('/');
//  Serial.print(now.day(), DEC);
//  Serial.print(now.hour(), DEC);
//  Serial.print(':');
//  Serial.print(now.minute(), DEC);
//  Serial.print(':');
//  Serial.print(now.second(), DEC);
//  Serial.println();
//  espSerial.println(data);
//  byte d[30];
//  data.getBytes(d, 30);
//  espSerial.write(data);
//  Serial.println("data send to esp " + data); 
  delay(2000);
  // put your main code here, to run repeatedly:

}
