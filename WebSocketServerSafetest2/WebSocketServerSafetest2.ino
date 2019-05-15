#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>                 // Use this for WiFi instead of Ethernet.h
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <Dns.h>

//SQL
IPAddress server_ip;
char user[] = "sqluser";
char password[] = "sqlpass";

const char* ssid = "";
const char* pass = "";
String tempt = "25"; //teststart temp
String WebPage = "<!DOCTYPE html><html><style>input[type=\"text\"]{width: 90%; height: 3vh;}input[type=\"button\"]{width: 9%; height: 3.6vh;}.rxd{height: 90vh;}textarea{width: 99%; height: 100%; resize: none;}</style><script>var Socket;function start(){Socket=new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage=function(evt){document.getElementById(\"rxConsole\").value =evt.data;}}function enterpressed(){Socket.send(document.getElementById(\"txbuff\").value); document.getElementById(\"txbuff\").value=\"\";}</script><body onload=\"javascript:start();\"> <div><input class=\"txd\" type=\"text\" id=\"txbuff\" onkeydown=\"if(event.keyCode==13) enterpressed();\"><input class=\"txd\" type=\"button\" onclick=\"enterpressed();\" value=\"Send\" > </div><br><div class=\"rxd\"> <textarea id=\"rxConsole\" readonly></textarea> </div></body></html>";

// Sample query
char SAVE[] = "INSERT INTO iot18storage.Data (DeviceID, Status, Temperature, Humidity) VALUES (%d,%d,%f,%f)";
char query[128];
// sample readings
int dID = 2;
int sqlStatus = 2;
float temp = 21.3f;
float humi = 20.4f;

//time
unsigned long startMillis;
unsigned long currentMillis;
unsigned long startMillisSQL;
unsigned long currentMillisSQL;
const unsigned long period = 3000;
const unsigned long periodSQL = 20000;
bool change = false;

WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);

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
    Serial.println(WiFi.localIP());
    server.on("/", [](){
    server.send(200, "text/html", WebPage);
    });
    
    server.begin();
    
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    
}

void setup() {
  Serial.begin(115200);
  initWifi();
  WiFi.hostByName("iot18grupparbete.crkcsxh8msun.us-east-2.rds.amazonaws.com",server_ip);
  Serial.println("Connecting...");
  if (conn.connect(server_ip, 3306, user, password)) {
    delay(1000);
  }
  else
    Serial.println("Connection failed.");
  //conn.close();  
}

void loop() {
    currentMillis = millis();
    currentMillisSQL = millis();
    if(currentMillis - startMillis >= period)
    {
      change = true;
      double stemp = atof(tempt.c_str());
      stemp = stemp+1;
      tempt = String(stemp, 2);
      startMillis = currentMillis;
      }
    if(currentMillisSQL - startMillisSQL >= periodSQL)
    {
      MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
      sprintf(query, SAVE, dID,sqlStatus,temp,humi); 
      // Execute the query
      cur_mem->execute(query);
      // Note: since there are no results, we do not need to read any data
      // Deleting the cursor also frees up memory used
      delete cur_mem;
      startMillisSQL = currentMillisSQL;
      Serial.println("insertedSQL");
      }
    webSocket.loop();
    server.handleClient();
    if (change == true){
      char c[tempt.length()+1];
      strcpy(c, tempt.c_str());
      webSocket.broadcastTXT(c, sizeof(c));
      change = false;
    }
    
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
   if (type == WStype_TEXT){
    for(int i = 0; i < length; i++) Serial.print((char) payload[i]);
    Serial.println();
   }
}
