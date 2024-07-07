#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
 
String apiKey = "2K7OI4IOZUU9AQ70"; // Enter your Write API key from ThingSpeak
//String apiKey = "YGQLKXWN21DMCO7O"; // Enter your Write API key from ThingSpeak
const char* ssid = "Nusricq702";
const char* password = "Nusricq702..";
const char* server = "api.thingspeak.com";
 
#define SS 5
#define RST 14
#define DI0 2
 
#define TX_P 17
#define BAND 433E6
#define ENCRYPT 0x88
 
String device_id;
String temperature;
String pressure;
String altitude;
String humidity;
String dewPoint;
String rainfall;
String lux;
String windSpeed;
String windDirection;
 
WiFiClient client;
 
void setup()
{
  Serial.begin(115200);
  Serial.println("LoRa Receiver");
  
  LoRa.setPins(SS, RST, DI0);
  //LoRa.setTxPower(TX_P);
  
  if (!LoRa.begin(BAND))
  {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  LoRa.setSpreadingFactor(12);
  LoRa.setSyncWord(ENCRYPT);
 
  Serial.println("Connecting to ");
  Serial.println(ssid);
 
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);
 
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());
 
}
 
void loop()
{
  // Try to parse packet
  int pos1, pos2, pos3, pos4, pos5, pos6, pos7, pos8, pos9;
 
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // Received a packet
    Serial.print("Received packet:  ");
    String LoRaData = LoRa.readString();
    Serial.print(LoRaData);
    // Read packet
    while (LoRa.available())
    {
      Serial.print((char)LoRa.read());
    }
    // Print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
 
    pos1 = LoRaData.indexOf('/');
    pos2 = LoRaData.indexOf('&');
    pos3 = LoRaData.indexOf('#');
    pos4 = LoRaData.indexOf('@');
    pos5 = LoRaData.indexOf('$');
    pos6 = LoRaData.indexOf('^');
    pos7 = LoRaData.indexOf('!');
    pos8 = LoRaData.indexOf('~');
    pos9 = LoRaData.indexOf('|');
 
    device_id = LoRaData.substring(0, pos1);
    temperature = LoRaData.substring(pos1 + 1, pos2);
    pressure = LoRaData.substring(pos2 + 1, pos3);
    altitude = LoRaData.substring(pos3 + 1, pos4);
    humidity = LoRaData.substring(pos4 + 1, pos5);
    dewPoint = LoRaData.substring(pos5 + 1, pos6);
    rainfall = LoRaData.substring(pos6 + 1, pos7);
    lux = LoRaData.substring(pos7 + 1, pos8);
    windSpeed = LoRaData.substring(pos8 + 1, pos9);
    windDirection = LoRaData.substring(pos9 + 1, LoRaData.length());
 
    Serial.print(F("Device ID = "));
    Serial.println(device_id);
 
    Serial.print(F("Temperature = "));
    Serial.print(temperature);
    Serial.println(F("*C"));
 
    Serial.print(F("Pressure = "));
    Serial.print(pressure);
    Serial.println(F("hPa"));
 
    Serial.print(F("Approx. Altitude = "));
    Serial.print(altitude);
    Serial.println(F("m"));
 
    Serial.print(F("Humidity = "));
    Serial.print(humidity);
    Serial.println(F("%"));
 
    Serial.print("Dew point = ");
    Serial.print(dewPoint);
    Serial.println(" *C");
 
    Serial.print(F("Rainfall = "));
    Serial.print(rainfall);
    Serial.println(F("%"));
 
    Serial.print(F("Light = "));
    Serial.print(lux);
    Serial.println(F(" lx"));

    Serial.print(F("Wind speed = "));
    Serial.print(windSpeed);
    Serial.println(F(" m/s"));

    Serial.print(F("Wind direction = "));
    Serial.println(windDirection);
 
    Serial.println();
 
    if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
    {
      // Check if the data error occurrs
      if (!checkFloat(temperature)||!checkFloat(pressure)||!checkFloat(altitude)||!checkFloat(humidity)||!checkFloat(dewPoint)
      ||!checkFloat(rainfall)||!checkFloat(lux)||!checkFloat(windSpeed)){
         Serial.println("Data transmission error");
         delay(500);
      }
      else{
        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(temperature);
        postStr += "&field2=";
        postStr += String(pressure);
        postStr += "&field3=";
        postStr += String(altitude);
        postStr += "&field4=";
        postStr += String(humidity);
        postStr += "&field5=";
        postStr += String(dewPoint);
        postStr += "&field6=";
        postStr += String(rainfall);
        postStr += "&field7=";
        postStr += String(lux);
        postStr += "&field8=";
        postStr += String(windSpeed);
        postStr += "r\n";
 
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);
 
        Serial.println("Data Send to Thingspeak");
        delay(500); 
      }
    }
    client.stop();
    Serial.println("Waiting...");
    Serial.println();
  }
}

// Checking
bool checkFloat(String str){
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (!isDigit(c) && c != '.') {
      return false;
    }
  }
  return true;
}
