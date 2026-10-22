/*
 * VTLabsKit - small helpers for VTLabs training kits.
 *
 *   #include <VTLabsKit.h>        board pins (VTL_PIN_...), VTLButton, VTLLineInput, vtlPrintBoardInfo()
 *   #include <VTLabsKeypad.h>     VTLKeypad   (needs Tools > Keypad (add-on) set)
 *   #include <VTLabsMatrix.h>     VTLMatrix   (needs Tools > LED Matrix (add-on) set)
 *
 * All pin numbers come from the board variant (vtlabs_board.h), which is generated
 * from boards/<board>.toml. To change a pin for ONE sketch, #define it before the include:
 *
 *   #define VTL_PIN_LED_RED 13
 *   #include <VTLabsKit.h>
 *
 * The library is header-only on purpose: such overrides then apply consistently.
 * Students are still expected to use pinMode()/digitalRead()/analogRead() directly;
 * these helpers only remove repetitive code in the larger lessons.
 */
#pragma once

#include <Arduino.h>

#if !defined(VTLABS_BOARD)
#error "VTLabsKit: select a VTLabs board in Tools > Board > VTLabs Training Kits."
#endif

#include "vtlabs_board.h"

#define VTLABSKIT_VERSION "0.2.0"

/* Level that means "active" for a named pin, e.g. VTL_ACTIVE_LEVEL(BTN1) -> LOW */
#define VTL_ACTIVE_LEVEL(NAME) (VTL_PIN_##NAME##_ACTIVE_LOW ? LOW : HIGH)
#define VTL_INACTIVE_LEVEL(NAME) (VTL_PIN_##NAME##_ACTIVE_LOW ? HIGH : LOW)

/* Relay helpers honour the board's relay polarity (VTL_RELAY_ACTIVE_HIGH). */
inline void vtlRelayWrite(bool on) {
#if VTL_PIN_RELAY >= 0
  digitalWrite(VTL_PIN_RELAY, (on == (bool)VTL_RELAY_ACTIVE_HIGH) ? HIGH : LOW);
#else
  (void)on;
#endif
}

/* Print what board and add-ons this sketch was compiled for. */
inline void vtlPrintBoardInfo(Stream &out) {
  out.println();
  out.println(F("=== VTLabs board info ==="));
  out.print(F("Board:        ")); out.println(VTL_BOARD_NAME);
  out.print(F("Board ID:     ")); out.println(VTL_BOARD_ID);
  out.print(F("HW revision:  ")); out.println(VTL_HW_REV);
  out.print(F("Pins checked on hardware: ")); out.println(VTL_BOARD_VERIFIED ? F("yes") : F("NO (design assumption)"));
#ifdef VTL_KEYPAD_OPTION_LABEL
  out.print(F("Keypad:       ")); out.println(VTL_KEYPAD_OPTION_LABEL);
#endif
#ifdef VTL_LEDMATRIX_OPTION_LABEL
  out.print(F("LED matrix:   ")); out.println(VTL_LEDMATRIX_OPTION_LABEL);
#endif
  out.print(F("ESP32 core:   ")); out.println(ESP_ARDUINO_VERSION_STR);
  out.print(F("VTLabsKit:    ")); out.println(VTLABSKIT_VERSION);
  out.println(F("========================="));
}

/* ---------------------------------------------------------------------------
 * VTLButton - debounced push button / switch with edge events.
 *
 *   VTLButton btn;
 *   btn.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);
 *   loop: btn.update(); if (btn.pressed()) ...
 * ------------------------------------------------------------------------- */
class VTLButton {
public:
  void begin(int pin, bool activeLow = true, uint8_t mode = INPUT, uint16_t debounceMs = 25) {
    _pin = pin;
    _activeLow = activeLow;
    _debounce = debounceMs;
    pinMode(_pin, mode);
    _raw = _stable = readRaw();
    _changed = millis();
  }
  /* Call often (every loop). Returns true if the debounced state changed. */
  bool update() {
    bool raw = readRaw();
    uint32_t now = millis();
    if (raw != _raw) {
      _raw = raw;
      _changed = now;
      return false;
    }
    if (raw != _stable && (now - _changed) >= _debounce) {
      _stable = raw;
      if (_stable) { _pressedEvt = true; _downSince = now; }
      else { _releasedEvt = true; _lastHeld = now - _downSince; }
      return true;
    }
    return false;
  }
  bool isDown() const { return _stable; }
  /* true once per press (clears itself) */
  bool pressed() { bool e = _pressedEvt; _pressedEvt = false; return e; }
  /* true once per release (clears itself) */
  bool released() { bool e = _releasedEvt; _releasedEvt = false; return e; }
  uint32_t heldMs() const { return _stable ? millis() - _downSince : 0; }
  /* how long the button was held before the last release */
  uint32_t lastHoldMs() const { return _lastHeld; }

private:
  bool readRaw() const { return (digitalRead(_pin) == (_activeLow ? LOW : HIGH)); }
  int _pin = -1;
  bool _activeLow = true, _raw = false, _stable = false, _pressedEvt = false, _releasedEvt = false;
  uint16_t _debounce = 25;
  uint32_t _changed = 0, _downSince = 0, _lastHeld = 0;
};

/* ---------------------------------------------------------------------------
 * VTLLineInput - collects keypad (or serial) characters into a short line.
 *   digits '0'..'9' are appended (letters too when allowLetters is true)
 *   '*' deletes the last character, '#' finishes the entry
 * ------------------------------------------------------------------------- */
class VTLLineInput {
public:
  static const uint8_t MAX_LEN = 16;
  void begin(uint8_t maxLen = 6, bool allowLetters = false) {
    _max = maxLen > MAX_LEN ? MAX_LEN : maxLen;
    _letters = allowLetters;
    clear();
  }
  /* Feed one key. Returns true when '#' completes a non-empty entry. */
  bool feed(char key) {
    if (key == '#') return _len > 0;
    if (key == '*') { if (_len) _buf[--_len] = 0; return false; }
    bool ok = (key >= '0' && key <= '9') || (_letters && key >= 'A' && key <= 'D');
    if (ok && _len < _max) { _buf[_len++] = key; _buf[_len] = 0; }
    return false;
  }
  void clear() { _len = 0; _buf[0] = 0; }
  const char *text() const { return _buf; }
  long value() const { return atol(_buf); }
  uint8_t length() const { return _len; }
  bool full() const { return _len >= _max; }
  /* text with every character replaced by '*' (for passwords) */
  String masked() const { String s; for (uint8_t i = 0; i < _len; i++) s += '*'; return s; }

private:
  char _buf[MAX_LEN + 1] = {0};
  uint8_t _len = 0, _max = 6;
  bool _letters = false;
};

/* ---------------------------------------------------------------------------
 * VTLMovingAverage - simple filter for noisy analog readings.
 * ------------------------------------------------------------------------- */
template <uint8_t N>
class VTLMovingAverage {
public:
  float add(float v) {
    _sum += v - _buf[_i];
    _buf[_i] = v;
    _i = (_i + 1) % N;
    if (_count < N) _count++;
    return value();
  }
  float value() const { return _count ? _sum / _count : 0; }

private:
  float _buf[N] = {0};
  float _sum = 0;
  uint8_t _i = 0, _count = 0;
};
