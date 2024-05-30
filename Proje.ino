

#include <ESP8266HTTPClient.h>

#include <ArduinoWiFiServer.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

#include <Servo.h>

#define trigPin D1
#define echoPin D2
#define servoPin D3

const char* ssid = "Ayşe Ebrar";
const char* password = "987654321";
const char* server = "http://api.thingspeak.com/update"; 
const char* apiKey = "089INWY40NRAH0KE";

Servo servo;

int aracSayisi = 0;
bool bariyerAcik = false;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  servo.attach(servoPin);
  servo.write(90);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  long sure;
  int mesafe;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  sure = pulseIn(echoPin, HIGH);
  mesafe = sure * 0.034 / 2;

  Serial.print("Mesafe: ");
  Serial.print(mesafe);
  Serial.println(" cm");

  if (mesafe < 15 && !bariyerAcik) {
    acBariyer();
    aracSayisi++;
    Serial.print("Toplam Araç Sayısı: ");
    Serial.println(aracSayisi);
    bariyerAcik = true;
  }

  if (bariyerAcik && mesafe >= 10) {
    kapatBariyer();
    bariyerAcik = false;
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(mesafe) + "&field2=" + String(aracSayisi);
    http.begin(client, url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("ThinkSpeak'e veri gönderildi, HTTP code: %d\n", httpCode);
    } else {
      Serial.printf("HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.println("WiFi disconnected, reconnecting...");
    WiFi.begin(ssid, password);
  }

  delay(700);
}

void acBariyer() {
  servo.write(0);
}

void kapatBariyer() {
  servo.write(180);
}