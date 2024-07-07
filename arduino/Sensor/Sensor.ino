#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>       // Adds BH1750 library file 
 
#define SS 10
#define RST 9
#define DI0 2

#define R_LED 7
#define Y_LED 6
#define G_LED 3

#define TX_P 17
#define ENCRYPT 0x88
 
#define BAND 433E6
#define rain_sensor A0
#define wind_speed_sensor A1
#define wind_direction_sensor A2
 
#define SEALEVELPRESSURE_HPA (1013.25)  // Please replace it with the current sea level pressure at your location
Adafruit_BME280 bme;
 
BH1750 lightMeter;
 
String LoRaMessage = "";
char device_id[12] = "MyDevice123";
 
 
void setup()
{
  Serial.begin(115200);
  Wire.begin();
  lightMeter.begin();
 
  pinMode (rain_sensor, INPUT);
  pinMode (wind_speed_sensor, INPUT);
  pinMode (wind_direction_sensor, INPUT);
  pinMode(R_LED, OUTPUT);  // Use LED to indicate that LoRa module is working
  pinMode(Y_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);
  while (!Serial);
 
  Serial.println(F("LoRa Sender"));
 
  LoRa.setPins(SS, RST, DI0);
  //LoRa.setTxPower(TX_P);
 
  if (!LoRa.begin(BAND))
  {
    Serial.println(F("Starting LoRa failed!"));
    while (1);
  }

  LoRa.setSpreadingFactor(12);
  LoRa.setSyncWord(ENCRYPT); 
  
  if (!bme.begin(0x76))
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}
 
void loop()
{
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float humidity = bme.readHumidity();
 
  double dewPoint = dewPointFast(temperature, humidity);
 
  int rainfall = map(analogRead(rain_sensor), 780, 0, 0, 100);  // Read from analog pin
  if (rainfall >= 100)
  {
    rainfall = 100;
  }
  if (rainfall <= 0)
  {
    rainfall = 0;
  }
 
  float lux = lightMeter.readLightLevel();

  //int windSpeedSensorValue = 0;  //Test
  int windSpeedSensorValue = analogRead(wind_speed_sensor);
  float windSpeedVoltage = windSpeedSensorValue * (5 / 1023.0);
  float windSpeed = windSpeedVoltage / 2.5 * 45;

  //int windDirectionSensorValue = 0;  //Test
  int windDirectionSensorValue = analogRead(wind_direction_sensor);
  float windDirectionVoltage = windDirectionSensorValue * (5 / 1023.0);
  String windDirection = windDirectionFast(windDirectionVoltage);
 
  Serial.print(F("Device ID: "));
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
 
  Serial.print(F("Dew point = "));
  Serial.print(dewPoint);
  Serial.println(F(" *C"));
 
  Serial.print(F("Rainfall = "));
  Serial.print(rainfall);
  Serial.println(F("%"));
 
  Serial.print(F("Light = "));
  Serial.print(lux);
  Serial.println(F(" lx"));

  Serial.print(F("Wind speed Voltage = "));
  Serial.println(windSpeedVoltage);
  Serial.print(F("Wind speed = "));
  Serial.print(windSpeed);
  Serial.println(F(" m/s"));

  Serial.print(F("Wind direction Voltage = "));
  Serial.println(windDirectionVoltage);
  Serial.print(F("Wind direction = "));
  Serial.println(windDirection);
 
  LoRaMessage = String(device_id) + "/" + String(temperature) + "&" + String(pressure)
                + "#" + String(altitude) + "@" + String(humidity) + "$" + String(dewPoint)
                + "^" + String(rainfall) + "!" + String(lux) + "~" + String(windSpeed) + "|" + windDirection;
 
  // Send packet
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();
  Serial.println();
  // Change LED
  digitalWrite(R_LED, LOW);
  digitalWrite(Y_LED, LOW);
  digitalWrite(G_LED, HIGH);
  delay(3000);
  digitalWrite(R_LED, LOW);
  digitalWrite(Y_LED, HIGH);
  digitalWrite(G_LED, LOW);
  delay(2000);
  digitalWrite(R_LED, HIGH);
  digitalWrite(Y_LED, LOW);
  digitalWrite(G_LED, LOW);
  delay(5000);
}
 
// Calculate the dew point
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity * 0.01);
  double Td = (b * temp) / (a - temp);
  return Td;
}

// Get the wind direction
String windDirectionFast(float windDirectionVoltage){
  float degree = windDirectionVoltage / 2.5 * 359;
  if (degree>11.25 && degree<33.75){
    return "NNE";  //Northnortheast  
  }
  else if (degree>=33.75 && degree<=56.25){
    return "NE";  //Northeast  
  }
  else if (degree>56.25 && degree<78.75){
    return "ENE";  //Eastnortheast  
  }
  else if (degree>=78.75 && degree<=101.25){
    return "E";  //East  
  }
  else if (degree>101.25 && degree<123.75){
    return "ESE";  //Eastsoutheast  
  }
  else if (degree>=123.75 && degree<=146.25){
    return "SE";  //Southeast  
  }
  else if (degree>146.25 && degree<168.75){
    return "SSE";  //Southsoutheast  
  }
  else if (degree>=168.75 && degree<=191.25){
    return "S";  //South  
  }
  else if (degree>191.25 && degree<213.75){
    return "SSW";  //Southsouthwest  
  }
  else if (degree>=213.75 && degree<=236.25){
    return "SW";  //Southwest  
  }
  else if (degree>236.25 && degree<258.75){
    return "WSW";  //Westsouthwest  
  }
  else if (degree>=258.75 && degree<=281.25){
    return "W";  //West  
  }
  else if (degree>281.25 && degree<303.75){
    return "WNW";  //Westnorthwest  
  }
  else if (degree>=303.75 && degree<=326.25){
    return "NW";  //Northwest  
  }
  else if (degree>326.25 && degree<348.75){
    return "NNW";  //Northnorthwest  
  }
  else{
    return "N";  //North
  }
}
