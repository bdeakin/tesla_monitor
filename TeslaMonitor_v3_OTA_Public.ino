/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
  Arduino IDE example: Examples > Arduino OTA > BasicOTA.ino
*********/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>  // Include the http client
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>        // Include JSON parsing
#include <LiquidCrystal_I2C.h>
#include <iostream>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

// Replace with your network credentials
const char* ssid = "<###NETWORK NAME###>";
const char* password = "<###NETWORK CREDENTIALS###>";

const int ESP_BUILTIN_LED = 2;

void setup() {
  
    // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("TeslaMonitoringStation");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(ESP_BUILTIN_LED, OUTPUT);
}

void loop() {

  Serial.println("Initiating http object...");
  HTTPClient http;                                        //Declare an object of class HTTPClient

  ### ADD YOUR PUBLIC ID BELOW
  
  http.begin("https://teslascope.com/api/vehicle/<###ADD PUBLIC ID HERE###>?api_key=<###ADD API KEY HERE###>", "85:CB:D3:CB:16:B9:22:74:79:DF:66:D9:18:C8:4A:14:C0:33:D5:94");  //Specify request destination

  ### For whatever reason, the http library requires that hex fingerprint after the actual URL. 
  ### I went to https://www.grc.com/fingerprints.htm and entered the Teslascope URL and got that back
  ### It may change periodically so setting up your 8266 for OTA updates so you can fix it without much hassle is important
  
  http.addHeader("content-type", "application/json");

    while (1) {
    
      int httpCode = http.GET();                              //Send the request
      int i;
      
      Serial.println("Sending http request...");
    
      Serial.println(httpCode);
      
      if (httpCode > 0) { //Check the returning code
     
        String payload = http.getString();   //Get the request response payload
        Serial.println("Getting the payload...");
        Serial.println(payload);                     //Print the response payload

        const size_t capacity = 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(9) + JSON_OBJECT_SIZE(11) + 390;
        DynamicJsonDocument doc(capacity);
    
        deserializeJson(doc, payload);

        JsonObject response = doc["response"];

        int response_battery_level = response["battery"]["level"]; // 77
        int response_battery_range = response["battery"]["range"]; // "202.27"
        const char* response_year = response["year"]; // "2016"
        const char* response_model = response["model"]; // "Model S"
        const char* response_trim = response["trim"]; // "85D"
        int response_climate_inside = response["climate"]["inside"]; // 88.9
        int response_climate_outside = response["climate"]["outside"]; // 0
        const char* response_battery_charging_state = response["battery"]["charging_state"]; // "Disconnected"
        int response_battery_minutes_remaining = response["battery"]["minutes_remaining"]; // 0
        int response_battery_charge_limit_soc = response["battery"]["charge_limit_soc"]; // 80
        bool response_climate_is_climate_on = response["climate"]["is_climate_on"]; // false

        int hours_remaining;
        int minutes_remaining;
        
        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("Batt: ");
        lcd.print(response_battery_level);
        lcd.print("%");
        lcd.setCursor(12,0);
        lcd.print("SoC: ");
        lcd.print(response_battery_charge_limit_soc);
        lcd.print("%");
        lcd.setCursor(0,1);
        
        lcd.print("Rang: ");
        lcd.print(response_battery_range);

        lcd.setCursor(13,1);
        lcd.print("AC: ");
        if (response_climate_is_climate_on) {
          lcd.print("ON");
        }
        else if (!response_climate_is_climate_on) {
          lcd.print("OFF");
        }
        else {
          lcd.print("ERR");
        }
        
        lcd.setCursor(2,2);
        lcd.print("In: ");
        // if the car is asleep, the inside reading is often 0
        if (response_climate_inside == 0) {
          lcd.print("SLP");
        } else {
          lcd.print(response_climate_inside);
          lcd.print("F");
        }
        lcd.setCursor(12,2);
        lcd.print("Out: ");

        // if the car is asleep, the outside reading is often 0
        if (response_climate_outside == 0) {
          lcd.print("SLP"); }
        else { 
          lcd.print(response_climate_outside); 
          lcd.print("F");      
        }
        
        if (strcmp(response_battery_charging_state, "Complete") == 0) {
          lcd.setCursor(2,3);
          lcd.print(">>> COMPLETE <<<");        
        }
        else if (strcmp(response_battery_charging_state, "Disconnected") == 0) {
          lcd.setCursor(0,3);
          lcd.print(">>> DISCONNECTED <<<");  
        }
        else if (strcmp(response_battery_charging_state, "Charging") == 0) {
          lcd.setCursor(0,3);
          lcd.print(">>> CHARGING ");
          hours_remaining = floor(response_battery_minutes_remaining / 60);
          minutes_remaining = response_battery_minutes_remaining % 60;
          lcd.print(hours_remaining);
          lcd.print("h");
          lcd.print(minutes_remaining);
          lcd.print("m");
        }
        else {
          lcd.print("ERROR");
        } // end if charging state
        
      } // end if httpCode > 0
      
      else {
        Serial.printf("[HTTP] GET... failed, error: %s\r\n", http.errorToString(httpCode).c_str()); 
        lcd.setCursor(0,0);
        lcd.print("Error code: ");
        lcd.print(httpCode);
      } // end else
      
      http.end();   //Close connection

      
      for (i = 1; i <= 2500; i++) {
        delay(120); // wait 120ms
        // listen for OTA updates
        ArduinoOTA.handle();
      } // end for
      
  } // end while

}
