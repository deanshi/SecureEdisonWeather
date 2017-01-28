#include <Wire.h>
#include "rgb_lcd.h"
#include <SPI.h>
#include <WiFi.h>

#define PIR_MOTION_SENSOR 2
#define LED 13

rgb_lcd lcd;  

// LCD Colors
const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

// Wireless Information
char ssid[] = ""; //  your network SSID (name) 
char pass[] = "";
int keyIndex = 0;
String nameOfCity = "Boston";
char server[] = "api.openweathermap.org";
int status = WL_IDLE_STATUS;

WiFiClient client;

void setup() 
{    
    Serial.begin(9600);
    printGoodMorning();
    delay(10000);
    lcd.setCursor(0,1);
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
}

void startMotionInterrupt() {
  connectToWifi();
}

void connectToWifi() {
  while (status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);
  
    // wait 10 seconds for connection:
    delay(10000);
  } 
  Serial.println("Connected to wifi");
  printWifiStatus();
  getWeatherInformation();
}

void getWeatherInformation() {
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /data/2.5/weather?APPID=&q=" + nameOfCity + "&mode=json&units=metric&cnt=2 HTTP/1.1");
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    getWeatherBytes();
  }
 
}

void getWeatherBytes() {
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  disconnectFromServer();
}

void disconnectFromServer() {
  Serial.println();
  Serial.println("disconnecting from server.");
  client.disconnect();
  client.stop();
  status = WL_IDLE_STATUS;
}

void writeToLcd() {
  lcd.setCursor(0, 1);
  if (isMotionDetected()) {
    lcd.write("Moving");
  } else {
    lcd.write("Not");
  }
}

boolean isMotionDetected() {
  int sensorValue = digitalRead(PIR_MOTION_SENSOR);
  if (sensorValue == HIGH) {
    return true;
  } else {
    return false;
  }
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
