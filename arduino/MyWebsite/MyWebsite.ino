#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>
 
const char* ssid = "Nusricq702";
const char* password = "Nusricq702..";
 
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

const char* serverUrl = "http://172.17.72.43:3000/update";  // notice the ip address of the server
HTTPClient http;

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

  // try to parse packet
  int pos1, pos2, pos3, pos4, pos5, pos6, pos7, pos8, pos9;
 
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // received a packet
    Serial.print("Received packet:  ");
    String LoRaData = LoRa.readString();
    Serial.print(LoRaData);
    // read packet
    while (LoRa.available())
    {
      Serial.print((char)LoRa.read());
    }
    // print RSSI of packet
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

    // Send request to my server
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String requestBody = "temperature="+temperature+"&pressure="+pressure+"&altitude="+altitude+"&humidity="+humidity+
    "&dewpoint="+dewPoint+"&rainfall="+rainfall+"&light="+lux+"&windspeed="+windSpeed+"&winddirection="+windDirection;
    int httpResponseCode=http.POST(requestBody);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server response: " + response);
    } else {
      Serial.println("Error sending POST request");
    }
    http.end();
  }
  /*
  //TEST...
  Serial.println("TESTING...");
  temperature="25";
  pressure="971.71";
  altitude="315.1";
  humidity="26";
  dewPoint="10.05";
  rainfall="10";
  lux="250";
  windSpeed="0.15";
  windDirection="NNE";
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String requestBody = "temperature="+temperature+"&pressure="+pressure+"&altitude="+altitude+"&humidity="+humidity+
  "&dewpoint="+dewPoint+"&rainfall="+rainfall+"&light="+lux+"&windspeed="+windSpeed+"&winddirection="+windDirection;
  int httpResponseCode=http.POST(requestBody);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Server response: " + response);
  } else {
    Serial.println("Error sending POST request");
  }
  http.end();
  delay(5000);
  */
}
