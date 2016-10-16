#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Ziggo68968";
const char* password = "duivendak";

const char* wunderground_api_key = "116b5c9cc26deeec";
const char* wunderground_url = "api.wunderground.com";
const int   wunderground_http_port = 80;
const char* wunderground_conditions = "conditions/lang:NL";
const char* wunderground_forecast = "forecast/lang:NL";
const char* wunderground_astronomy = "astronomy/lang:NL";
const char* wunderground_location = "q/NL/Soest.json";

const int mainLoopDelay = 10;
const int intervalConditions = 5 * mainLoopDelay ;
const int intervalAstronomy = 240 * mainLoopDelay;
const int intervalForecast = 120 * mainLoopDelay;
int lastUpdateConditions = intervalConditions;
int lastUpdateAstronomy = intervalAstronomy;
int lastUpdateForecast = intervalForecast;



// Conditions
String city;
long localTimeEpoch;
long observationTimeEpoch;
String temp;
String tempFeelsLike;
String weather;
String humidity;
String windSpeed;
String windDirection;
String pressure;
String pressureTrend;
String precip1hr;
String precipToday;

// Astronomy
String localTime;
String sunRise;
String sunSet;

// Forecast
String todayTempMax;
String todayTempLow;
String todayWeather;
String todayWindDirection;
String todayWindAvg;
String todayWindMax;
String todayRainChance;
String tomorrowTempMax;
String tomorrowTempLow;
String tomorrowWeather;
String tomorrowWindDirection;
String tomorrowWindAvg;
String tomorrowWindMax;
String tomorrowRainChance;

const int nStats = 6;
int curStat = 1;
String weatherData [nStats];

int wifiLedPin = D5;
int dataLedPin = D6;
WiFiClient wifiClient;

String response;

LiquidCrystal_I2C lcd(0x27,2,16);

void setup() {
  Serial.begin(9600);
  delay(10);
  pinMode(wifiLedPin, OUTPUT);
  pinMode(dataLedPin, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.clear();  
}

void loop() {
  if (lastUpdateConditions == intervalConditions ) {
    printLcd("Bijwerken", "waarnemingen...");
    Serial.println("Updating conditions...");
    updateConditions();
    lastUpdateConditions = 0 ;
  }

  if (lastUpdateAstronomy == intervalAstronomy ) {
    printLcd("Bijwerken", "astro...");
    Serial.println("Updating astronomy...");
    updateAstronomy();
    lastUpdateAstronomy = 0;
  }

  if (lastUpdateForecast == intervalForecast ) {
    printLcd("Bijwerken", "verwachtingen...");
    Serial.println("Updating forecast...");
    updateForecast();
    lastUpdateForecast = 0;
  }

  lcdWeatherData();
  printWeatherData();

  lastUpdateConditions++;
  lastUpdateAstronomy++;
  lastUpdateForecast++;

  delay(mainLoopDelay * 1000);

  digitalWrite(wifiLedPin, WiFi.status() == WL_CONNECTED) ;
}

void connectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    printSerial("Connecting to ", ssid);
    lcd.clear();
    lcd.home();
    lcd.print("Connecting");
    lcd.setCursor(0,1);
    boolean ledMode = HIGH;

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(wifiLedPin, ledMode);
      delay(500);
      Serial.print(".");
      lcd.print(".");
      ledMode = !ledMode;
    }

    printSerial("WiFi connected, IP=", WiFi.localIP().toString());
    printLcd("WiFi connected", WiFi.localIP().toString());
    digitalWrite(wifiLedPin, HIGH);
    delay(5000);
  }
}


void updateConditions() {
  digitalWrite(dataLedPin, LOW);
  connectWiFi();
  getConditions();
  parseConditions();
  weatherData[0] = "Waarnemingen";
  weatherData[1] = city ;
  weatherData[2] = "Temp: " + temp;
  weatherData[3] = "Weer: " + weather;
  weatherData[4] = "Wind: " + windDirection + ", " + windSpeed + "km/h";
  weatherData[5] = "Druk: " + pressure + " " + pressureTrend;
}

void updateAstronomy() {
  digitalWrite(dataLedPin, LOW);
  connectWiFi();
  getAstronomy();
  parseAstronomy();
}

void updateForecast() {
  digitalWrite(dataLedPin, LOW);
  connectWiFi();
  getForecast();
  parseForecast();
}

void connectWunderground() {
  if (!wifiClient.connect(wunderground_url, wunderground_http_port)) {
    Serial.println("Connection to Wunderground failed.");
    return;
  }
  Serial.println("Connected to Wunderground");
}

void getConditions() {
  connectWunderground();

  String url = String("/api/") +
               wunderground_api_key + "/" +
               wunderground_conditions + "/" +
               wunderground_location ;

  Serial.println(String("Requesting URL: ") + url );

  wifiClient.println(String("GET ") + url + " HTTP/1.1");
  wifiClient.println(String("Host: ") + wunderground_url);
  wifiClient.println("Connection: close");
  wifiClient.println();

  getResponse();
}

void getAstronomy() {
  connectWunderground();

  String url = String("/api/") +
               wunderground_api_key + "/" +
               wunderground_astronomy + "/" +
               wunderground_location ;

  Serial.println(String("Requesting URL: ") + url );

  wifiClient.println(String("GET ") + url + " HTTP/1.1");
  wifiClient.println(String("Host: ") + wunderground_url);
  wifiClient.println("Connection: close");
  wifiClient.println();

  getResponse();
}

void getForecast() {
  connectWunderground();

  String url = String("/api/") +
               wunderground_api_key + "/" +
               wunderground_forecast + "/" +
               wunderground_location ;

  Serial.println(String("Requesting URL: ") + url );

  wifiClient.println(String("GET ") + url + " HTTP/1.1");
  wifiClient.println(String("Host: ") + wunderground_url);
  wifiClient.println("Connection: close");
  wifiClient.println();

  getResponse();
}

void getResponse() {
  while (!wifiClient.available()) {
    delay(1000);
    Serial.println("client not available, retry in 1 second:");
  }

  response = "";

  char c;
  int openBracketCount = 0;
  int closeBracketCount = 0;
  boolean inJson = false;
  wifiClient.setNoDelay(false);
  while (wifiClient.connected() && wifiClient.available()) {
    c =  wifiClient.read();
    if (c == '{') {
      inJson = true;
      openBracketCount++;
    }
    if (c == '}') {
      closeBracketCount++;
    }
    if (inJson) {
      Serial.print(c);
      response = response + c;
    }
    if (openBracketCount == closeBracketCount) {
      inJson = false;
    }
    delay(2);
  }
  Serial.println();
}

void parseConditions() {
  DynamicJsonBuffer jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(response);
  if (!root.success()) {
    Serial.println("parseobject() failed");
    digitalWrite(dataLedPin, LOW);
    return;
  }

  String version = root["response"]["version"];
  Serial.print("Conditions response Version: ");
  Serial.println(version);

  city = root["current_observation"]["display_location"]["full"].asString();
  localTimeEpoch = root["current_observation"]["local_epoch"].as<time_t>();
  observationTimeEpoch = root["current_observation"]["observation_epoch"].as<time_t>();
  temp = root["current_observation"]["temp_c"].asString() + String(char(223)) + "C";
  tempFeelsLike = root["current_observation"]["feelslike_c"].asString();
  weather = root["current_observation"]["weather"].asString();
  windSpeed = root["current_observation"]["wind_kph"].asString();
  windDirection = root["current_observation"]["wind_dir"].asString();
  pressure = root["current_observation"]["pressure_mb"].asString();
  pressureTrend = root["current_observation"]["pressure_trend"].asString();
  precip1hr = root["current_observation"]["precip_1hr_metric"].asString();
  precipToday = root["current_observation"]["precip_today_metric"].asString();

  digitalWrite(dataLedPin, HIGH);
}

void parseAstronomy() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(response);
  if (!root.success()) {
    Serial.println("parseobject() failed");
    digitalWrite(dataLedPin, LOW);
    return;
  }

  String version = root["response"]["version"];
  printSerial("Astronomy response Version: ", version);

  localTime = root["moon_phase"]["current_time"]["hour"].asString()
              + String(":") + root["moon_phase"]["current_time"]["minute"].asString();
  sunRise = root["sun_phase"]["sunrise"]["hour"].asString()
            + String(":") + root["sun_phase"]["sunrise"]["minute"].asString();
  sunSet = root["sun_phase"]["sunset"]["hour"].asString()
           + String(":") + root["sun_phase"]["sunset"]["minute"].asString() ;

  digitalWrite(dataLedPin, HIGH);
}

void parseForecast() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(response);
  if (!root.success()) {
    Serial.println("parseobject() failed");
    digitalWrite(dataLedPin, LOW);
    return;
  }

  String version = root["response"]["version"];
  printSerial("Forecast response Version: ", version);

  todayTempMax = root["forecast"]["simpleforecast"]["forecastday"][0]["high"]["celsius"].asString();
  todayTempLow = root["forecast"]["simpleforecast"]["forecastday"][0]["low"]["celsius"].asString();;
  todayWeather = root["forecast"]["simpleforecast"]["forecastday"][0]["conditions"].asString();
  todayWindDirection = root["forecast"]["simpleforecast"]["forecastday"][0]["avewind"]["dir"].asString();
  todayWindAvg = root["forecast"]["simpleforecast"]["forecastday"][0]["avewind"]["kph"].asString();
  todayWindMax = root["forecast"]["simpleforecast"]["forecastday"][0]["maxwind"]["kph"].asString();
  todayRainChance  = root["forecast"]["simpleforecast"]["forecastday"][0]["pop"].asString();

  tomorrowTempMax = root["forecast"]["simpleforecast"]["forecastday"][1]["high"]["celsius"].asString();
  tomorrowTempLow = root["forecast"]["simpleforecast"]["forecastday"][1]["low"]["celsius"].asString();;
  tomorrowWeather = root["forecast"]["simpleforecast"]["forecastday"][1]["conditions"].asString();
  tomorrowWindDirection = root["forecast"]["simpleforecast"]["forecastday"][1]["avewind"]["dir"].asString();
  tomorrowWindAvg = root["forecast"]["simpleforecast"]["forecastday"][1]["avewind"]["kph"].asString();
  tomorrowWindMax = root["forecast"]["simpleforecast"]["forecastday"][1]["maxwind"]["kph"].asString();
  tomorrowRainChance  = root["forecast"]["simpleforecast"]["forecastday"][1]["pop"].asString();

  digitalWrite(dataLedPin, HIGH);
}

void printWeatherData() {
  Serial.println("Current conditions");
  Serial.println("------------------");
  printSerial("City: ", city);
  printSerial("Time: ", String(localTimeEpoch));
  printSerial("Observation time: ", String(observationTimeEpoch));
  printSerial("Temperature: ", temp);
  printSerial("Temperature feels like: ", tempFeelsLike);
  printSerial("Weer: ", weather);
  printSerial("Wind speed (km/h): ", windSpeed);
  printSerial("Wind direction: ", windDirection);
  printSerial("Pressure (mBar): ", pressure);
  printSerial("Pressure trend: ", pressureTrend);
  printSerial("Precipitation last hour(mm): ", precip1hr);
  printSerial("Precipitation today(mm): ", precipToday);
  Serial.println();

  Serial.println("Sunrise");
  Serial.println("------------------");
  printSerial("Local time: ", localTime);
  printSerial("Sunrise: ", sunRise);
  printSerial("Sunset: ", sunSet);
  Serial.println();

  Serial.println("Forecast");
  Serial.println("------------------");
  printSerial("Today high temp: ", todayTempMax);
  printSerial("Today low temp: ", todayTempLow);
  printSerial("Today weather: ", todayWeather);
  printSerial("Today wind: ", todayWindDirection);
  printSerial("Today average wind: ", todayWindAvg);
  printSerial("Today max wind: ", todayWindMax);
  printSerial("Today rain chance: ", todayRainChance);
  printSerial("Tomorrow high temp: ", tomorrowTempMax);
  printSerial("Tomorrow low temp: ", tomorrowTempLow);
  printSerial("Tomorrow weather: ", tomorrowWeather);
  printSerial("Tomorrow wind: ", tomorrowWindDirection);
  printSerial("Tomorrow average wind: ", tomorrowWindAvg);
  printSerial("Tomorrow max wind: ", tomorrowWindMax);
  printSerial("Tomorrow rain chance: ", tomorrowRainChance);
  Serial.println();
}

void lcdWeatherData() {
  String line1;
  String line2;
  
  if (curStat > nStats) {
    curStat = 1;
  }

  line1 = weatherData[curStat - 1] ;
  
  if (curStat > (nStats - 1)) {
    line2 = "-------------";
  } else {
    line2 = weatherData[curStat];
  }
  
  printLcd(line1, line2);
  curStat++;
}

void printSerial(String title, String message) {
  Serial.print(title);
  Serial.println(message);
}

void splitPrintLcd(String message) {
  int splitPos = message.indexOf('|');
  printLcd(message.substring(0, splitPos - 1), message.substring(splitPos + 1));
}

void printLcd(String msg1, String msg2) {
  lcd.clear();
  lcd.home();
  
  lcd.print(msg1);
  lcd.setCursor(0,1);
  lcd.print(msg2);  
}




