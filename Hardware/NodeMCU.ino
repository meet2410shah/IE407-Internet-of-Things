/*
Author  : Meet Shah
Email   : meet2410shah@gmail.com 
Subject : Auto Irrigation System using NodeMCU and ThingSpeak
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "ThingSpeak.h";

// CREDENTIALS : Acess to Internet
const char *ssid = "YOUR WiFi SSID";
const char *password = "YOUR WiFi PASSWORD";

// ThingSpeak API Host
const char *host = "api.thingspeak.com";

// CREDENTIALS : ThingSpeak 
unsigned long channelNumber = 0000000; // Add your ThingSpeak Channel Number / Channel ID
const char *writeAPIKey = "PASTE YOUR WRITE ACCESS API KEY";
const char *readAPIKey = "PASTE YOUR READ ACCESS API KEY";

WiFiClient client;

int threshold;
int statusCode;

void setup() {
  delay(1000);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connection Successful");
  
  ThingSpeak.begin(client);
  ThingSpeak.writeField(channelNumber, 2, 10, writeAPIKey);
}

int iterations = 0;

void loop() {
  const int httpPort = 80;

  if(!client.connect(host, httpPort)) {
    Serial.println("Connection Failed");
    delay(300);
    return;
  }

  // Read Analog Data
  int analogData = analogRead(A0);
  
  // Write the Moisture Data to the ThingSpeak Server
  float moisture = (((1024 - analogData) * 100.0) / 1024);
  if(iterations == 5) {
    statusCode = ThingSpeak.writeField(channelNumber, 1, moisture, writeAPIKey);
    if(statusCode == 200){
      Serial.println("Success : Write");
    } else {
      Serial.println("Error : Write");
      Serial.println(statusCode);
    }
    iterations = 0;
  }
  
  // Read the current Threshold Value
  threshold = ThingSpeak.readLongField(channelNumber, 2, readAPIKey);
  statusCode = ThingSpeak.getLastReadStatus();
  if(statusCode == 200){
    Serial.println("Success : Read");
  } else {  
    Serial.println("Error : Read");
  }
  
  Serial.println("Moisture: " + String(moisture));
  Serial.println("Threshold: " + String(threshold));
  if(moisture < threshold) {
    analogWrite(D10, 1024);
  } else {
    analogWrite(D10, 0);
  }
  Serial.println("");
  iterations++;
  delay(10);
}
