#include <Arduino.h>
#include <ESP8266WiFi.h>

class ThingSpeakClient {
    public:
        ThingSpeakClient();
        void writeTempHum(unsigned long channel, String writeAPIKey, float temperature, float humidity);

};