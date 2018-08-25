#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUDP.h>
#include "DHT.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

#define DHTPIN D4
#define DHTTYPE DHT11
#define DELAY_SECONDS_PERIOD 60

const int led = 13;

#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

WiFiUDP udp;
DHT dht(DHTPIN, DHTTYPE);

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);

  Serial.println("Thermometer started");
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  showOnDisplay(0, 0);
}

void loop(void) {
  float humidity;
  float temperatureCelsius;

  sleep();
  humidity = dht.readHumidity();
  temperatureCelsius = dht.readTemperature();

  Serial.println(humidity);
  Serial.println(temperatureCelsius);

  // wake();
  // sendStatsdData(statsOptions.temperature, temperatureCelsius);
  // sendStatsdData(statsOptions.humidity, humidity);
  showOnDisplay(humidity, temperatureCelsius);
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

#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void showOnDisplay(float humidity, float temperatureCelsius)   {
  char str_value[6];
  if (isnan(humidity) || isnan(temperatureCelsius)) {
    return;
  }

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  dtostrf(humidity, 2, 0, str_value);
  display.print("H:"); display.print(str_value); display.println('%');

  dtostrf(temperatureCelsius, 2, 0, str_value);
  display.print("t:"); display.print(str_value); display.println('C');

  display.display();
}
