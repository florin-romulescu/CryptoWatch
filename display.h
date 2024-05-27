#ifndef DISPLAY_H
#define DISPLAY_H

#include "U8g2lib.h"
#include "coinapi.h"
#include "clockapi.h"
#define MODE_CLOCK 0
#define MODE_COIN 1
#define MODE_WIFI 2

#define NO_COMMAND 0
#define BTN1_PRESSED_COMMAND 1
#define BTN1_HOLD_COMMAND 2
#define BTN2_PRESSED_COMMAND 3
#define BTN2_HOLD_COMMAND 4
#define BTN12_PRESSED_COMMAND 5

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
unsigned int command = 0;

volatile unsigned int display_mode = 2;

void displayMsg(const char* msg) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(20, 20, msg);
  u8g2.sendBuffer();
} 

void toggleCryptoCurrency() {
  price = -1;
  price_change = -1;
  current_crypto_idx = current_crypto_idx + 1 >= length ? 0 : current_crypto_idx + 1; 
}

void toggleCurrency() {
  current_currency_idx = current_currency_idx + 1 >= 3 ? 0 : current_currency_idx + 1;
}

void toggleDisplayMode() {
  display_mode = display_mode + 1 >= 2? 0 : display_mode + 1;
}

void _drawClockScreen(unsigned int hour, unsigned int minute) {
  u8g2.clearBuffer();

  char timeString[6];
  snprintf(timeString, sizeof(timeString), "%02d:%02d", hour, minute);

  u8g2.setFont(u8g2_font_ncenB24_tr);
  u8g2.drawStr(20, 40, timeString);
  u8g2.sendBuffer();
}

void _drawClock() {
    updateTime();
    _drawClockScreen(hour, minute);
    for (int i = 0; i < 100 && command != BTN12_PRESSED_COMMAND; ++i) {
      vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void _drawCoinScreen() {
  u8g2.clearBuffer();

  char currency_row[128];

  long long int value = (long long int)convertCurrency(price);
  char value_row[128];
  char change_row[128];
  char calculating_row[] = "Calculating...";
  snprintf(value_row, sizeof(value_row), "%lld", value);
  snprintf(change_row, sizeof(change_row), "24h: %.2f\%", price_change);
  
  

  if (value >= 0) {
    snprintf(currency_row, sizeof(currency_row), "%s -> %s", symbol, currencies[current_currency_idx]);
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(20, 15, currency_row);
    u8g2.drawStr(0, 23, "-------------------------");
    u8g2.setFont(u8g2_font_ncenB12_tr);
    u8g2.drawStr(10, 40, value_row);
    u8g2.drawStr(10, 60, change_row);
  } else {
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(20, 20, calculating_row);
  }
  u8g2.sendBuffer();
}

void _drawCoin() {
    _drawCoinScreen();
    vTaskDelay(pdMS_TO_TICKS(950));
}

void drawScreenTask(void* parameter) {
  Serial.println("Task is running...");
  for (;;) {
      switch (display_mode) {
        case MODE_CLOCK:
          _drawClock();
          break;
        case MODE_COIN:
          _drawCoin();
          break;
        case MODE_WIFI:
          displayMsg("Set wifi.");
          break;
        default:
          Serial.println("Error when extracting the mode.");
      }
  }
}

#endif