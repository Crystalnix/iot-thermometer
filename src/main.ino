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
char statsdPacketBuffer[STATSD_PACKET_SIZE];
struct {
  const char* temperature = "office.sensor.2.temperature";
  const char* humidity = "office.sensor.2.humidity";
} statsOptions;

#define DHTPIN SCL
#define DHTTYPE DHT22
#define DELAY_SECONDS_PERIOD 60

const int led = 13;

WiFiUDP udp;
DHT dht(DHTPIN, DHTTYPE);

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);

  Serial.println("Thermometer started");
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
  delay(DELAY_SECONDS_PERIOD*1000);
}

void wake() {
  connectWiFi();
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting to wifi");

  // Wait for connection
  int connRes = WiFi.waitForConnectResult();
  Serial.print("WiFi connRes: ");
  Serial.println(connRes);
  if (connRes != WL_CONNECTED) {
    Serial.println ( "WiFi not connected" );
  } else {
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void sendStatsdData(const char* key, float value) {
  if (isnan(value)) {
    Serial.print("will not send empty data");
    Serial.println(key);
    return;
  }
  memset(statsdPacketBuffer, 0, STATSD_PACKET_SIZE);

  char str_value[6];

  /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
  dtostrf(value, 4, 2, str_value);

  sprintf((char*)statsdPacketBuffer, "%s:%s|g\n", key, str_value);

  int error = udp.beginPacket(statsdsIP, statsdsPort);
  Serial.print("begin packet ");
  Serial.println(error); // 1 for OK

  udp.write(statsdPacketBuffer);

  error = udp.endPacket(); // 1 for OK
  Serial.print("end packet ");
  Serial.println(error);
}
