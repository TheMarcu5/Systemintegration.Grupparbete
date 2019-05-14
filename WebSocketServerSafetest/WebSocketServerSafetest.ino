#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>

const char* ssid = "Cheapster-Spider";
const char* password = "Homofob1";
String temp = "25";
String WebPage = "<!DOCTYPE html><html><style>input[type=\"text\"]{width: 90%; height: 3vh;}input[type=\"button\"]{width: 9%; height: 3.6vh;}.rxd{height: 90vh;}textarea{width: 99%; height: 100%; resize: none;}</style><script>var Socket;function start(){Socket=new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage=function(evt){document.getElementById(\"rxConsole\").value +=evt.data;}}function enterpressed(){Socket.send(document.getElementById(\"txbuff\").value); document.getElementById(\"txbuff\").value=\"\";}</script><body onload=\"javascript:start();\"> <div><input class=\"txd\" type=\"text\" id=\"txbuff\" onkeydown=\"if(event.keyCode==13) enterpressed();\"><input class=\"txd\" type=\"button\" onclick=\"enterpressed();\" value=\"Send\" > </div><br><div class=\"rxd\"> <textarea id=\"rxConsole\" readonly></textarea> </div></body></html>";

//time
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 3000;
bool change = false;

WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
   }
    
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.on("/", [](){
    server.send(200, "text/html", WebPage);
    });
    
    server.begin();
    
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    currentMillis = millis();
    if(currentMillis - startMillis >= period)
    {
      change = true;
      double stemp = atof(temp.c_str());
      stemp = stemp+1;
      temp = String(stemp, 2);
      startMillis = currentMillis;
      
      Serial.println("Here");
      }

    webSocket.loop();
    server.handleClient();
    if (change == true){
      char c[temp.length()+1];
      strcpy(c, temp.c_str());
      webSocket.broadcastTXT(c, sizeof(c));
      change = false;
      Serial.println(change);
    }
    
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
   if (type == WStype_TEXT){
    for(int i = 0; i < length; i++) Serial.print((char) payload[i]);
    Serial.println();
   }
}
