#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUDP.h>
#include "DHT.h"

const char* ssid = "*******";
const char* password = "*******";
const char* statsdsIP = "*******";
const int statsdsPort = 8125;

const int STATSD_PACKET_SIZE = 48;
byte statsdPacketBuffer[STATSD_PACKET_SIZE];
struct {
  const char* temperature = "temperature";
  const char* humidity = "humidity";
} statsOptions;

#define DHTPIN SCL
#define DHTTYPE DHT22

const int led = 13;

WiFiUDP udp;
DHT dht(DHTPIN, DHTTYPE);

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  Serial.println("Before connect to wifi");
  WiFi.begin(ssid, password);
  Serial.println("Connecting to wifi");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Termometer started");
  dht.begin();
}

void sendStatsdData(const char* key, float value) {
  if (isnan(value)) {
    Serial.print("can not write ");
    Serial.print(key);
    Serial.println("");
    return;
  }
  memset(statsdPacketBuffer, 0, STATSD_PACKET_SIZE);

  char str_value[6];

  /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
  dtostrf(value, 4, 2, str_value);

  Serial.println(str_value);
  sprintf((char*)statsdPacketBuffer, "%s:%s|g\n", key, str_value);

  udp.beginPacket(statsdsIP, statsdsPort);
  udp.write(statsdPacketBuffer, STATSD_PACKET_SIZE);
  udp.endPacket();
}

void loop(void){
  float humidity;
  float temperatureCelsius;

  while(true) {
    delay(500);
    Serial.println("working...");
    humidity = dht.readHumidity();
    temperatureCelsius = dht.readTemperature();
    sendStatsdData(statsOptions.temperature, temperatureCelsius);
    sendStatsdData(statsOptions.humidity, humidity);
  }
}
