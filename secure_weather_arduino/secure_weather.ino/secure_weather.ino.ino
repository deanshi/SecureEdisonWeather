#include <Wire.h>
#include "rgb_lcd.h"
#include <SPI.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <stdlib.h>

#define PIR_MOTION_SENSOR 2
#define LED 13

// Libraries:
// https://github.com/bblanchon/ArduinoJson
// http://wiki.seeed.cc/Grove-LCD_RGB_Backlight/


rgb_lcd lcd;  

// LCD Colors
const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

// Wireless Information
const String APP_ID = "";
const char server[] = "api.openweathermap.org";
const int JSON_SIZE = 1024;

char PERSONAL_SSID[] = ""; //  your network SSID (name) 
char PASS[] = "";

int keyIndex = 0;
String nameOfCity = "Boston";
int status = WL_IDLE_STATUS;

WiFiClient client;
char buffer[JSON_SIZE];
char doubleBuffer[100];

void setup() 
{    
    turnOffLcd();
    Serial.begin(9600);
    //delay(10000);
    attachInterrupt(PIR_MOTION_SENSOR, startMotionInterrupt, RISING);
}  

void loop() 
{
  // Don't loop code, just use interrupts
}

void printGoodMorning() {
  lcd.begin(16, 2);
  
  lcd.setRGB(colorR, colorG,   colorB);
  
  // Print a message to the LCD.
  lcd.print("Good Morning!");
  lcd.setCursor(0,1);
}

void startMotionInterrupt() {
  printGoodMorning();
  connectToWifi();
}

void connectToWifi() {
  while (status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(PERSONAL_SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(PERSONAL_SSID, PASS);
  
    // wait 5 seconds for connection:
    delay(5000);
  } 
  Serial.println("Connected to wifi");
  printWifiStatus();
  getWeatherInformation();
}

void getWeatherInformation() {

  StaticJsonBuffer<JSON_SIZE> jsonBuffer;
 
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /data/2.5/weather?APPID=" + APP_ID + "&q=" + nameOfCity + "&mode=json&units=imperial&cnt=2 HTTP/1.1");
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    
    getWeatherBytes(buffer);

    JsonObject& root = jsonBuffer.parseObject(buffer);

    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    
    const char* weatherType = root["weather"][0]["main"];
    double temp = root["main"]["temp"];
    Serial.println(weatherType);
    Serial.println(temp);
    
    lcd.write(weatherType);
    lcd.write(", ");
    sprintf(doubleBuffer, "%1.1f", temp);
    lcd.write(doubleBuffer);
    lcd.write((char)223);
    delay(10000);
    turnOffLcd();
  }
}

void getWeatherBytes(char* jsonBuffer) {
  boolean isJson = false;
  int index = 0;
  while (client.available()) {
    char c = client.read();
    
    if (c == '{') {
      isJson = true;
    }

    if (isJson) {
       jsonBuffer[index] = c;
       index++;
    }
  }
  Serial.write(jsonBuffer);
  disconnectFromServer();
}

void disconnectFromServer() {
  Serial.println();
  Serial.println("disconnecting from server.");
  client.stop();
  status = WL_IDLE_STATUS;
}

void turnOffLcd() {
  lcd.noDisplay();
  lcd.setRGB(0, 0, 0);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
