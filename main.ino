#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstring>
#include "esp_system.h"
#include "button.h"
#include "display.h"
#include "coinapi.h"
#include "WiFi.h"
#include "ArduinoJson.h"
#include <WebServer.h>

WebServer server(80);

const char* ssid = "ESP32_AP";
const char* password = "12345678";

char WIFI_NETWORK[100] = "";
char WIFI_PASSWORD[100] = "";


void connectToWifi(const char* network, const char* password) {
  Serial.println("Connecting to wifi...");
  WiFi.mode(WIFI_STA); // setting the board as a station
  WiFi.begin(network, password);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime <= WIFI_TIMEOUT_MS) {
    Serial.print(".");
    delay(100);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(" Connection failed");
    // Reboot if not connected
  } else {
    Serial.println(" Connection succseful");
    Serial.println(WiFi.localIP());
    display_mode = 0;
  }
}

void handleRoot() {
  server.send(200, "text/plain", "ESP32 Web Server");
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

void handleJsonPost() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body not received");
    return;
  }

  String json = server.arg("plain");
  Serial.println("Received JSON: " + json);

  // Allocate the JSON document
  const size_t capacity = JSON_OBJECT_SIZE(3) + 60;
  DynamicJsonDocument doc(capacity);

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }

  const char* task = doc["task"];
  if (!strcmp(task, "network")) {
    const char* network = doc["network"];
    const char* password = doc["password"];
    strcpy(WIFI_NETWORK, network);
    strcpy(WIFI_PASSWORD, password);
    server.send(200, "text/plain", "Connection is now starting.");
    connectToWifi(WIFI_NETWORK, WIFI_PASSWORD);
  } else if (!strcmp(task, "add crypto")) {
    const char* coin = doc["name"];
    addCryptoCurrency(coin);
    server.send(200, "text/plain", "Crypto currency added.");
  } else {
    server.send(400, "text/plain", "Invalid task.");
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  u8g2.begin();

  setCurrencies();
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/json", HTTP_POST, handleJsonPost);
  server.onNotFound(handleNotFound);
  server.begin();

  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);

  xTaskCreate(drawScreenTask, "Draw screen", 10000, NULL, 1, NULL);
  xTaskCreate(buttonTask, "Button Task", 1024, NULL, 1, NULL);
}

void loop() {
  server.handleClient();
  updateCryptoCoin(crypto_currencies[current_crypto_idx]);
  if (price >= 0) {
    for (int i = 0; i < 25 && price >= 0; ++i) {
      delay(1000);
    }
  } else {
    delay(1000);
  }
}
