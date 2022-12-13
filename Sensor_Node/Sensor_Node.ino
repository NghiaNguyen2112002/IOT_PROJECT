#include <espnow.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include "scheduler.h"
#include "timer.h"
#include "global.h"

#define WIFI_AP                 "GATEWAY"
#define WIFI_PASSWORD           "123456789"


// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD   9600
#define DHTTYPE DHT11 // DHT 11


#define DHT_PIN       D4
DHT dht(DHT_PIN, DHTTYPE);     // Initialize DHT sensor.

// the Wifi radio's status
int status = WL_IDLE_STATUS;

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


void DataProcessing(void);
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus);
void InitWiFi();
void Reconnect();
void InitSystem(void);


void setup() {
  InitSystem();
  SCH_Add_Task(DataProcessing, 0, 200);     //invoked every 2s
}

void loop() {
   SCH_Dispatch_Tasks();

  if (WiFi.status() != WL_CONNECTED) {
    Reconnect();
  }
}



void InitSystem(void){
  Serial.begin(SERIAL_DEBUG_BAUD);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  dht.begin();

  InitWiFi();
  
  SCH_Init();
  Init_Timer();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);  
}

void DataProcessing(void){
  myData.humi = dht.readHumidity();
  myData.temp = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(myData.humi) || isnan(myData.temp)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  else {
    Serial.print("HUMI: "); Serial.println(myData.humi);
    Serial.print("TEMP: "); Serial.println(myData.temp);   
    
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  }
}

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
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

