#ifndef BUTTON_H
#define BUTTON_H

#define BTN1 2
#define BTN2 4

#include "display.h"

unsigned int extractButtonCommand() {
  return command;
}

void buttonTask(void * parameter) {
  while (true) {
    // Read button states
    bool button1State = digitalRead(BTN1) == HIGH;
    bool button2State = digitalRead(BTN2) == HIGH;

    // Check if buttons are pressed
    if (button1State && button2State) {
      Serial.println("Both buttons pressed");
      toggleDisplayMode();
      command = BTN12_PRESSED_COMMAND;
      Serial.println(display_mode);
    } else if (button1State) {
      Serial.println("Button 1 pressed");
      command = BTN1_PRESSED_COMMAND;

      switch (display_mode) {
        case MODE_CLOCK:
          break;
        case MODE_COIN:
          toggleCryptoCurrency();
          break;
        default:
          break;
      }

    } else if (button2State) {
      Serial.println("Button 2 pressed");
      command = BTN2_PRESSED_COMMAND;

       switch (display_mode) {
        case MODE_CLOCK:
          break;
        case MODE_COIN:
          toggleCurrency();
          break;
        default:
          break;
      }
    } else {
      command = NO_COMMAND;
    }
    vTaskDelay(pdMS_TO_TICKS(250)); // Delay to avoid excessive looping
  }

}

#endif