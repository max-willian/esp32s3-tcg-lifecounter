#pragma once

#include <Arduino.h>

// BOOT is active LOW on GPIO0. No GPIO assignments of display/touch are changed.
class MenuButton {
public:
  void begin() {
    pinMode(PIN, INPUT_PULLUP);
    rawPressed = stablePressed = digitalRead(PIN) == LOW;
    changedAt = millis();
    // Ignore an already-held button until released.
    armed = !stablePressed;
  }

  bool pressed() {
    const unsigned long now = millis();
    const bool sample = digitalRead(PIN) == LOW;
    if (sample != rawPressed) {
      rawPressed = sample;
      changedAt = now;
    }
    if (now - changedAt < DEBOUNCE_MS || stablePressed == rawPressed) return false;
    stablePressed = rawPressed;
    if (!stablePressed) {
      armed = true;
      return false;
    }
    const bool event = armed;
    armed = false;
    return event;
  }

private:
  static constexpr uint8_t PIN = 0;
  static constexpr unsigned long DEBOUNCE_MS = 35;
  bool rawPressed = false;
  bool stablePressed = false;
  bool armed = true;
  unsigned long changedAt = 0;
};
