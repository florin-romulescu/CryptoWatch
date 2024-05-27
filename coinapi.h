#ifndef COIN_API_H
#define COIN_API_H

#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include "esp_system.h"
#include "cstring"

#define WIFI_TIMEOUT_MS 20000
#define STORAGE 50
#define MEM 500

volatile long long int price = -1;
volatile float price_change = -1;
char* symbol = 0;

char coin_api_key[] = "CG-TUEMa47AT3dsDZ7KEmMvmMHH";
const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(1) + 30;

char* currencies[STORAGE];
int current_currency_idx = 0;

char* crypto_currencies[STORAGE];
int length = 0;
int current_crypto_idx = 0;

void addCryptoCurrency(const char* currency) {
  if (length == STORAGE) return;
  strcpy(crypto_currencies[length], currency);
  length += 1;
}

void setCurrencies() {
  symbol = new char[10];
  symbol[0] = 0;
  for (int i = 0; i < STORAGE; ++i) {
    crypto_currencies[i] = new char[100];
  }
  addCryptoCurrency("bitcoin");
  addCryptoCurrency("ethereum");
  addCryptoCurrency("tether");
  addCryptoCurrency("solana");

  currencies[0] = "USD";
  currencies[1] = "RON";
  currencies[2] = "EUR";
}

float convertCurrency(long long int usd) {
  if (!strcmp(currencies[current_currency_idx], "USD")) {
    return usd;
  } else if (!strcmp(currencies[current_currency_idx], "RON")) {
    return usd * 4.58;
  } else if (!strcmp(currencies[current_currency_idx], "EUR")) {
    return usd * 0.92;
  }
}

void updatePrice(long long int new_price) {
  if (new_price >= 0) price = new_price;
}

void updatePriceChange(float new_price_change) {
  if (new_price_change >= 0) price_change = new_price_change;
}

void updateSymbol(const char* new_symbol) {
  if (price >= 0) memcpy(symbol, new_symbol, 10);
}

void ping() {
  if (WiFi.status() == WL_CONNECTED) {
    char global_endpoint[] = "https://api.coingecko.com/api/v3/ping";
    HTTPClient client;
    char endpoint[256];
    snprintf(endpoint, sizeof(endpoint), "%s?x_cg_api_key=%s", global_endpoint, coin_api_key);
    client.begin(endpoint);
    int http_code = client.GET();

    if (http_code > 0) {
      String payload = client.getString();
      char json[MEM];
      payload.toCharArray(json, MEM);

      StaticJsonDocument<200> doc;
      deserializeJson(doc, json);

      const char* message = doc["gecko_says"];
      Serial.println(message);
    } else {
      Serial.println("Ping request failed.");
    }
  }
}

void updateCryptoCoin(const char* crypto_name) {
  if (WiFi.status() == WL_CONNECTED) {
    char global_endpoint[] = "https://api.coingecko.com/api/v3/coins/markets";
    HTTPClient client;
    char endpoint[256];
    snprintf(endpoint, sizeof(endpoint), "%s?x_cg_api_key=%s&ids=%s&vs_currency=usd", global_endpoint, coin_api_key, crypto_name);
    client.begin(endpoint);
    int http_code = client.GET();
    if (http_code == 429) return;
    if (http_code > 0) {
      String payload = client.getString();
      char json[MEM];
      payload.toCharArray(json, MEM);
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, json);

      updatePrice(doc[0]["current_price"]);
      updatePriceChange(doc[0]["price_change_percentage_24h"]);
      updateSymbol(doc[0]["symbol"]);
    }
  }
}


#endif