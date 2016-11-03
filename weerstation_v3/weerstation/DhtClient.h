#include <Arduino.h>
#include <DHT.h>

class DhtClient {
    private:
        float temperature;
        float humidity;
        DHT dht;

    public:
        DhtClient(uint8_t pin, uint8_t type);
        void updateData();
        float getTemperature();
        float getHumidity();
        String getTemperatureAsString();
        String getHumidityAsString();
};
