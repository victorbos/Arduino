#include <Arduino.h>
#include <DHT.h>
#include "DhtClient.h"

DhtClient::DhtClient(uint8_t pin, uint8_t type) :
  dht(pin, type)
{
    dht.begin();
    delay(500);
}

void DhtClient::updateData() {
  Serial.println("Updating DHT...");
  this->temperature = dht.readTemperature();
  this->humidity = dht.readHumidity();
  Serial.println(String (temperature) + "-" + String(humidity));
}

float DhtClient::getTemperature() {
  return temperature;
}

float DhtClient::getHumidity() {
  return humidity;
}

String DhtClient::getTemperatureAsString() {
  return String(temperature, 1) + "°C";
}

String DhtClient::getHumidityAsString() {
  return String(this->humidity, 1) + "%";
}
