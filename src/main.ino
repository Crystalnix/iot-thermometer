#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUDP.h>
#include "DHT.h"

const char* ssid = "ELTEX-85A8";
const char* password = "GP21277678";
const char* statsdsIP = "192.168.1.7";
const int statsdsPort = 4000;

const int STATSD_PACKET_SIZE = 48;
byte statsdPacketBuffer[STATSD_PACKET_SIZE];
struct {
  const char* temperature = "temperature";
  const char* humidity = "humidity";
} statsOptions;

#define DHTPIN 2
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
  sprintf((char*)statsdPacketBuffer, "%s|%i\n", key, value);

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
