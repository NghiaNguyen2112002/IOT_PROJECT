#include <ESP8266WiFi.h>
#include <espnow.h>

#include "ThingsBoard.h"
#include "scheduler.h"
#include "timer.h"
#include "global.h"
// #include "data_processing.h"

#define WIFI_AP                 "Holmes"
#define WIFI_PASSWORD           "0906631096"

#define WIFI_GATEWAY_STA        "GATEWAY"
#define WIFI_GATEWAY_PASS       "123456789"

#define TOKEN               "nJPiwgmzksNoWXcJHZMc"
#define THINGSBOARD_SERVER  "demo.thingsboard.io"

// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD   9600

// Initialize ThingsBoard client
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);
// the Wifi radio's status
int status = WL_IDLE_STATUS;


void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len);
void InitSystem(void);
void DisconnectHandler(void);

void setup() {
  InitSystem();

  SCH_Add_Task(DisconnectHandler, 0, 100);      invoked every 1s
}

void loop() {
  SCH_Dispatch_Tasks();
}



void InitSystem(void){
  Serial.begin(SERIAL_DEBUG_BAUD);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);

  InitWiFi();
  Init_Timer();
  SCH_Init();

    // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("*");
  }
  Serial.println("Connected to AP");
  Serial.print("IP sta: "); Serial.println(WiFi.localIP());

  WiFi.softAP(WIFI_GATEWAY_STA, WIFI_GATEWAY_PASS);
  Serial.print("IP ap: "); Serial.println(WiFi.softAPIP());
  
}

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Receive from NODE ID: ");
  Serial.println(myData.sensor_node_id);
  Serial.print("HUMI: ");
  Serial.println(myData.humi);
  Serial.print("TEMP: ");
  Serial.println(myData.temp);
     
  tb.sendTelemetryInt("NODE ID", myData.sensor_node_id);
  tb.sendTelemetryInt("TEMP", myData.temp);
  tb.sendTelemetryInt("HUMI", myData.humi);

}

void Reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}

void DisconnectHandler(void){
  if (WiFi.status() != WL_CONNECTED) {
    Reconnect();
  }

  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }

  tb.loop();
}
