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

// update freq in seconds
const int mainLoopDelay = 5;
const int intervalConditions = 24 * mainLoopDelay ;
const int intervalAstronomy = 240 * mainLoopDelay;
const int intervalForecast = 240 * mainLoopDelay;
int lastUpdateConditions = intervalConditions;
int lastUpdateAstronomy = intervalAstronomy;
int lastUpdateForecast = intervalForecast;

// conditions
const int conditions = 0;
const int c_city = 1;
const int c_temp = 2;
const int c_tempFeels = 3;
const int c_weather = 4;
const int c_humidity = 5;
const int c_wind = 6;
const int c_pressure = 7;
const int c_precip = 8;

// astronomy
const int a_sunRise = 9;
const int a_sunSet = 10;

// forecast
const int f_temp_0 = 11;
const int f_weather_0 = 12;
const int f_wind_0 = 13;
const int f_rain_0 = 14;
const int f_temp_1 = 15;
const int f_weather_1 = 16;
const int f_wind_1 = 17;
const int f_rain_1 = 18;

// weather data array
const int nStats = 19;
String weatherData [nStats];
int curStat = 1;

const int wifiLedPin = D5;
const int dataLedPin = D6;

WiFiClient wifiClient;
String response;

LiquidCrystal_I2C lcd(0x27, 2, 16);


void setup() {
  pinMode(wifiLedPin, OUTPUT);
  pinMode(dataLedPin, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  if (lastUpdateConditions == intervalConditions ) {
    printLcd("Bijwerken", "waarnemingen...");
    updateConditions();
    lastUpdateConditions = 0 ;
  }

  if (lastUpdateAstronomy == intervalAstronomy ) {
    printLcd("Bijwerken", "astro...");
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

  lastUpdateConditions++;
  lastUpdateAstronomy++;
  lastUpdateForecast++;

  delay(mainLoopDelay * 1000);

  digitalWrite(wifiLedPin, WiFi.status() == WL_CONNECTED) ;
}

void updateConditions() {
  digitalWrite(dataLedPin, LOW);
  connectWiFi();
  getConditions();
  parseConditions();
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

void connectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    lcd.clear();
    lcd.home();
    lcd.print("Connecting");
    lcd.setCursor(0, 1);
    boolean ledMode = HIGH;

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(wifiLedPin, ledMode);
      delay(500);
      lcd.print(".");
      ledMode = !ledMode;
    }

    printLcd("WiFi connected:", WiFi.localIP().toString());
    digitalWrite(wifiLedPin, HIGH);
    delay(5000);
  }
}

void getConditions() {
  connectWunderground();

  String url = String("/api/") +
               wunderground_api_key + "/" +
               wunderground_conditions + "/" +
               wunderground_location ;

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

  wifiClient.println(String("GET ") + url + " HTTP/1.1");
  wifiClient.println(String("Host: ") + wunderground_url);
  wifiClient.println("Connection: close");
  wifiClient.println();

  getResponse();
}

void connectWunderground() {
  if (!wifiClient.connect(wunderground_url, wunderground_http_port)) {
    printLcd("Connection failed", "to Wunderground");
  }
}

void getResponse() {
  while (!wifiClient.available()) {
    delay(1000);
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
      response = response + c;
    }
    if (openBracketCount == closeBracketCount) {
      inJson = false;
    }
    delay(2);
  }
}

void parseConditions() {
  DynamicJsonBuffer jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(response);
  if (!root.success()) {
    printLcd("Failed to parse", "conditions json");
    digitalWrite(dataLedPin, LOW);
    return;
  }

  weatherData[conditions] = String("Waarnemingen ");
  weatherData[c_city] = root["current_observation"]["display_location"]["city"].asString();
  weatherData[c_temp] = String("temp ") + root["current_observation"]["temp_c"].asString() + String(char(223)) + "C";
  weatherData[c_tempFeels] = String("voelt als ") + root["current_observation"]["feelslike_c"].asString()  + String(char(223)) + "C";
  weatherData[c_weather] = root["current_observation"]["weather"].asString();
  weatherData[c_humidity] = String("vocht ") + root["current_observation"]["relative_humidity"].asString();
  weatherData[c_wind] = String("wind ") + root["current_observation"]["wind_dir"].asString() + " " + root["current_observation"]["wind_kph"].asString();
  weatherData[c_pressure] = String("druk ") + root["current_observation"]["pressure_mb"].asString() + " " + root["current_observation"]["pressure_trend"].asString();
  weatherData[c_precip] = String("regen ") + root["current_observation"]["precip_1hr_metric"].asString() + "/" + root["current_observation"]["precip_today_metric"].asString();

  digitalWrite(dataLedPin, HIGH);
}


void parseAstronomy() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(response);
  if (!root.success()) {
    printLcd("Failed to parse", "astro json");
    digitalWrite(dataLedPin, LOW);
    return;
  }

  String version = root["response"]["version"];
  
  weatherData[a_sunRise] = String("zon op ") + root["sun_phase"]["sunrise"]["hour"].asString()
                         + ":" + root["sun_phase"]["sunrise"]["minute"].asString();
  weatherData[a_sunSet] = String("zon onder ") + root["sun_phase"]["sunset"]["hour"].asString()
                         + ":" + root["sun_phase"]["sunset"]["minute"].asString();

  digitalWrite(dataLedPin, HIGH);
}

void parseForecast() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(response);
  if (!root.success()) {
    printLcd("Failed to parse", "forecast json");
    digitalWrite(dataLedPin, LOW);
    return;
  }

  String version = root["response"]["version"];
  weatherData[f_temp_0] = String("vandaag ") + 
    root["forecast"]["simpleforecast"]["forecastday"][0]["low"]["celsius"].asString() + "-" +
    root["forecast"]["simpleforecast"]["forecastday"][0]["high"]["celsius"].asString() + String(char(223)) + "C";
  weatherData[f_weather_0] = root["forecast"]["simpleforecast"]["forecastday"][0]["conditions"].asString();
  weatherData[f_wind_0] = String("wind ") + 
    root["forecast"]["simpleforecast"]["forecastday"][0]["avewind"]["dir"].asString() + " " +
    root["forecast"]["simpleforecast"]["forecastday"][0]["avewind"]["kph"].asString() + String(char(223)) + "C";
  weatherData[f_rain_0] = String("regen ") + root["forecast"]["simpleforecast"]["forecastday"][0]["pop"].asString() + "%";

  weatherData[f_temp_1] = String("morgen ") + 
    root["forecast"]["simpleforecast"]["forecastday"][1]["low"]["celsius"].asString() + "-" +
    root["forecast"]["simpleforecast"]["forecastday"][1]["high"]["celsius"].asString()  + String(char(223)) + "C";
  weatherData[f_weather_1] = root["forecast"]["simpleforecast"]["forecastday"][1]["conditions"].asString();
  weatherData[f_wind_1] = String("wind ") + 
    root["forecast"]["simpleforecast"]["forecastday"][1]["avewind"]["dir"].asString() + " " +
    root["forecast"]["simpleforecast"]["forecastday"][1]["avewind"]["kph"].asString() ;
  weatherData[f_rain_1] = String("regen ") + root["forecast"]["simpleforecast"]["forecastday"][1]["pop"].asString() +"%";

  digitalWrite(dataLedPin, HIGH);
}

void printLcd(String msg1, String msg2) {
  lcd.clear();
  lcd.home();

  lcd.print(msg1);
  lcd.setCursor(0, 1);
  lcd.print(msg2);
}
