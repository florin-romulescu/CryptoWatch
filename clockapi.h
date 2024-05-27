#ifndef CLOCK_API_H
#define CLOCK_API_H

#include <Arduino.h>
#include <time.h>
#include "ArduinoJson.h"

unsigned int year, month, day, hour, minute, second;

void extractTime(const char* datetime_str) {
  struct tm tm;
  char* parsed = strptime(datetime_str, "%Y-%m-%dT%H:%M:%S", &tm);

  if (parsed != NULL) {
    year = tm.tm_year + 1900;
    month = tm.tm_mon + 1;
    day = tm.tm_mday;
    hour = tm.tm_hour;
    minute = tm.tm_min;
    second = tm.tm_sec;
  } else {
    Serial.println("A problem occured when extracting the time.");
  }
}

void updateTime() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient client;
    const char endpoint[] = "http://worldtimeapi.org/api/timezone/Europe/Bucharest";
    client.begin(endpoint);
    int http_code = client.GET();
    if (http_code == 429) return;
    if (http_code > 0) {
      String payload = client.getString();
      char json[MEM];
      payload.toCharArray(json, MEM);
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, json);

      extractTime(doc["datetime"]);
    }
  }
}

#endif