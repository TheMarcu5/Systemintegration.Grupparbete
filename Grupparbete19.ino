#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <Dns.h>
#include "config.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHT_PIN 2
#define DHT_TYPE DHT11
#define TWELVE_HRS 60000UL

#define LED 16

bool ledState = false;

int pinButton = 4;
int stateLED = LOW;
int stateButton;
int previous = LOW;
long theTime = 0;
long debounce = 200;


unsigned long startTime;
static DHT dht(DHT_PIN, DHT_TYPE);

IPAddress server_ip;

WiFiClient client;
MySQL_Connection conn((Client *)&client);
DNSClient dns_client;

void initWifi() {

  Serial.printf("Attempting to connect to SSID: %s.\r\n", ssid);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {

    String mac = WiFi.macAddress();
    WiFi.begin(ssid, pass);
    delay(10000);
  }

  Serial.printf("Now Connected to wifi %s.\r\n", ssid);
}

void setup() {

  Serial.begin(115200);
  dht.begin();
  initWifi();
  WiFi.hostByName("iot18grupparbete.crkcsxh8msun.us-east-2.rds.amazonaws.com", server_ip);
  startTime = millis();
  pinMode(LED, OUTPUT);
  pinMode(pinButton, INPUT);
}


void sendToSQL(int stateLed) {

  int temp = dht.readTemperature();
  int hum = dht.readHumidity();

  char INSERT_SQL[] = "INSERT INTO iot18storage.Data (DeviceID, Status, Temperature, Humidity) VALUES ( %d, %d, %d, %d)";
  char query[128];
  char msg[10];

  ledstatus = stateLed;

  dtostrf(50.125, 1, 1, msg);
  sprintf(query, INSERT_SQL, deviceID, ledstatus, temp, hum, msg);

  Serial.println("Connecting...");
  if (conn.connect(server_ip, 3306, user, password)) {

    delay(1000);
    Serial.println("Connected to SQL.");

    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    cur_mem->execute(query);
    delete cur_mem;

    delay(6000);

  }
  else
    Serial.println("Connection to SQL failed.");
  conn.close();

}

void loop() {

  while (millis() - startTime < TWELVE_HRS)

  {

  stateButton = digitalRead(pinButton);
  delay(200);

  if (stateButton == HIGH && previous == LOW && millis() - theTime > debounce){
    
    if (stateLED == HIGH) {
      stateLED = LOW;
      Serial.println("STATELED = LOW");
      delay(200);
      
    } else {
      stateLED = HIGH;
      Serial.println("STATELED = HIGH");
      delay(200);
    }
    
    theTime = millis();
    
  }
  
  digitalWrite(LED, stateLED);
  previous == stateButton;
  delay(200);

    if (millis() - startTime > TWELVE_HRS) { //ändra till WHILE NOT TIME IS UP

    Serial.println("TIMES UP!");

    sendToSQL(stateLED);

    startTime = millis();
  }
  
    
  }
  
}
