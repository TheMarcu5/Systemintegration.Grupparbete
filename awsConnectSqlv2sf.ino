/*
  MySQL Connector/Arduino Example : connect by wifi

  This example demonstrates how to connect to a MySQL server from an
  Arduino using an Arduino-compatible Wifi shield. Note that "compatible"
  means it must conform to the Ethernet class library or be a derivative
  thereof. See the documentation located in the /docs folder for more
  details.

  INSTRUCTIONS FOR USE

  1) Change the address of the server to the IP address of the MySQL server
  2) Change the user and password to a valid MySQL user and password
  3) Change the SSID and pass to match your WiFi network
  4) Connect a USB cable to your Arduino
  5) Select the correct board and port
  6) Compile and upload the sketch to your Arduino
  7) Once uploaded, open Serial Monitor (use 115200 speed) and observe

  If you do not see messages indicating you have a connection, refer to the
  manual for troubleshooting tips. The most common issues are the server is
  not accessible from the network or the user name and password is incorrect.

  Note: The MAC address can be anything so long as it is unique on your network.

  Created by: Dr. Charles A. Bell
*/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>                 // Use this for WiFi instead of Ethernet.h
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <Dns.h>

IPAddress server_ip;  // IP of the MySQL *server* here
char user[] = "iot18grupparbete";              // MySQL user login username
char password[] = "kanelbulle18";        // MySQL user login password

// WiFi card example
char ssid[] = "DittSSIDHär";    // your SSID
char pass[] = "DittLösenHär";       // your SSID Password

// Sample query
char INSERT_SQL[] = "INSERT INTO iot18storage.Data (DeviceID, Status, Temperature, Humidity) VALUES ('2', '1', '25', '20')";


WiFiClient client;            // Use this for WiFi instead of EthernetClient
MySQL_Connection conn((Client *)&client);
DNSClient dns_client;

void initWifi()
{
    // Attempt to connect to Wifi network:
    Serial.printf("Attempting to connect to SSID: %s.\r\n", ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        // Get Mac Address and show it.
        // WiFi.macAddress(mac) save the mac address into a six length array, but the endian may be different. The huzzah board should
        // start from mac[0] to mac[5], but some other kinds of board run in the oppsite direction.
        uint8_t mac[6];
        WiFi.macAddress(mac);
        Serial.printf("You device with MAC address %02x:%02x:%02x:%02x:%02x:%02x connects to %s failed! Waiting 10 seconds to retry.\r\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ssid);
        WiFi.begin(ssid, pass);
        delay(10000);
    }
    Serial.printf("Connected to wifi %s.\r\n", ssid);
}

void setup() {
  Serial.begin(115200);
  initWifi();
  WiFi.hostByName("iot18grupparbete.crkcsxh8msun.us-east-2.rds.amazonaws.com",server_ip);
  // End WiFi section

  Serial.println("Connecting...");
  if (conn.connect(server_ip, 3306, user, password)) {
    delay(1000);
  }
  else
    Serial.println("Connection failed.");
  //conn.close();  
}

void loop() {
  delay(2000);

  Serial.println("Recording data.");

  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Execute the query
  cur_mem->execute(INSERT_SQL);
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
  delete cur_mem;
  delay(10000);
}
