#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUDP.h>
#include "DHT.h"

extern "C" {
  #include "user_interface.h"
}

const char* ssid = "*******";
const char* password = "*******";
const char* statsdsIP = "*******";
const int statsdsPort = 8125;

const int STATSD_PACKET_SIZE = 48;
byte statsdPacketBuffer[STATSD_PACKET_SIZE];
struct {
  const char* temperature = "sleep.temperature";
  const char* humidity = "sleep.humidity";
} statsOptions;

#define DHTPIN D4
#define DHTTYPE DHT11

#define DELAY_SECONDS_PERIOD 60

const int led = 13;

WiFiUDP udp;
DHT dht(DHTPIN, DHTTYPE);

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);

  Serial.println("Termometer started");
  dht.begin();
}

void loop(void) {
  float humidity;
  float temperatureCelsius;

  sleep();
  humidity = dht.readHumidity();
  temperatureCelsius = dht.readTemperature();

  wake();
  sendStatsdData(statsOptions.temperature, temperatureCelsius);
  sendStatsdData(statsOptions.humidity, humidity);
}

void sleep() {
  WiFi.setSleepMode(WIFI_LIGHT_SLEEP);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  wifi_fpm_open(); // Enables force sleep
  wifi_fpm_do_sleep(DELAY_SECONDS_PERIOD*1000000);

  delay(DELAY_SECONDS_PERIOD*1000);
}

void wake() {
  WiFi.forceSleepWake();
  connectWiFi();
}

void connectWiFi(){
  WiFi.begin(ssid, password);

  // Wait for connection
  int connRes = WiFi.waitForConnectResult();
  Serial.print ( "WiFi connRes: " );
  Serial.println ( connRes );
}

void sendStatsdData(const char* key, float value) {
  if (isnan(value)) {
    Serial.print("will not send empty data");
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

  int error = udp.beginPacket(statsdsIP, statsdsPort);
  udp.write(statsdPacketBuffer, STATSD_PACKET_SIZE);
  error = udp.endPacket();
}
