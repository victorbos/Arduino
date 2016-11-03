#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ThingSpeakClient.h"

ThingSpeakClient::ThingSpeakClient() {};

void ThingSpeakClient::writeTempHum(unsigned long channel, String writeAPIKey, float temperature, float humidity) {
    String url = "/update?api_key=";
    url += writeAPIKey;
    url += "&field1=" + String(temperature, 2);
    url += "&field2=" + String(humidity, 2);

    WiFiClient wifiClient ;
    const int httpPort = 80;
    if (!wifiClient.connect("api.thingspeak.com", httpPort)) {
        Serial.println("connection to thingspeak failed");
    }
    Serial.println("connected to thingspeak");

    Serial.print("Requesting URL: ");
    Serial.println(url);

    wifiClient.println(String("GET ") + url + " HTTP/1.1");
    wifiClient.println(String("Host: ") + "api.thingspeak.com");
    wifiClient.println("Connection: close");
    wifiClient.println();

    delay(10);
    int count = 0;
    while(!wifiClient.available() && count < 10){
      delay(1000);
      Serial.print(".");
      count++ ;
    }

    while(wifiClient.connected() && wifiClient.available()){
      String line = wifiClient.readStringUntil('\r');
      Serial.print(line);
      delay(2);
    }

    Serial.println();
    Serial.println("closing connection");
}


