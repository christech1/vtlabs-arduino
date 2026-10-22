/*
 * VTLabsKeypad - matrix keypad scanner with debouncing and press/hold/release events.
 *
 * Hardware is selected in Arduino IDE: Tools > Keypad (add-on)
 *   - PCF8574 backend: rows on P0..P3, columns on P4..P7, I2C address VTL_KEYPAD_I2C_ADDR
 *   - GPIO backend:    rows VTL_PIN_KEYPAD_R1.., columns VTL_PIN_KEYPAD_C1..
 * Key labels come from VTL_KEYPAD_KEYMAP (row by row), e.g. "123A456B789C*0#D".
 *
 * One key at a time is reported (the first one found), which is what the lessons need.
 */
#pragma once

#include "VTLabsKit.h"

#if !VTL_HAS_KEYPAD
#error "This sketch needs the matrix keypad. Select it in Tools > Keypad (add-on). If your kit has no keypad, choose another lesson."
#else /* rest of file only when the add-on is selected */

#if VTL_KEYPAD_BACKEND == VTL_KEYPAD_BACKEND_PCF8574
#include <Wire.h>
#elif VTL_KEYPAD_BACKEND != VTL_KEYPAD_BACKEND_GPIO
#error "VTL_KEYPAD_BACKEND must be VTL_KEYPAD_BACKEND_GPIO or VTL_KEYPAD_BACKEND_PCF8574"
#endif

#ifndef VTL_KEYPAD_DEBOUNCE_MS
#define VTL_KEYPAD_DEBOUNCE_MS 30
#endif
#ifndef VTL_KEYPAD_HOLD_MS
#define VTL_KEYPAD_HOLD_MS 700
#endif
#ifndef VTL_KEYPAD_COL_INPUT_MODE
#define VTL_KEYPAD_COL_INPUT_MODE INPUT_PULLUP /* use INPUT if the columns have external pull-ups */
#endif

static_assert(sizeof(VTL_KEYPAD_KEYMAP) - 1 == VTL_KEYPAD_ROWS * VTL_KEYPAD_COLS,
              "VTL_KEYPAD_KEYMAP must have ROWS x COLS characters");

enum VTLKeyEventType : uint8_t { VTL_KEY_NONE = 0, VTL_KEY_PRESSED, VTL_KEY_HELD, VTL_KEY_RELEASED };

struct VTLKeyEvent {
  VTLKeyEventType type;
  char key;
  explicit operator bool() const { return type != VTL_KEY_NONE; }
};

class VTLKeypad {
public:
  static const uint8_t ROWS = VTL_KEYPAD_ROWS;
  static const uint8_t COLS = VTL_KEYPAD_COLS;

  /* Returns false if the keypad hardware does not answer (PCF8574 only). */
  bool begin() {
#if VTL_KEYPAD_BACKEND == VTL_KEYPAD_BACKEND_PCF8574
    Wire.begin(VTL_PIN_I2C_SDA, VTL_PIN_I2C_SCL);
    Wire.beginTransmission(VTL_KEYPAD_I2C_ADDR);
    _ok = (Wire.endTransmission() == 0);
    pcfWrite(0xFF);
#else
    for (uint8_t r = 0; r < ROWS; r++) pinMode(_rows[r], INPUT); /* idle rows float */
    for (uint8_t c = 0; c < COLS; c++) pinMode(_cols[c], VTL_KEYPAD_COL_INPUT_MODE);
    _ok = true;
#endif
    return _ok;
  }
  bool connected() const { return _ok; }

  /* Low-level scan: index (row*COLS+col) of the first key held down right now, or -1.
     No debouncing - use it to learn how scanning works. */
  int scanRaw() {
    for (uint8_t r = 0; r < ROWS; r++) {
      int c = readRow(r);
      if (c >= 0) return r * COLS + c;
    }
    return -1;
  }

  /* Call every loop. Returns an event when something happened. */
  VTLKeyEvent update() {
    VTLKeyEvent ev{VTL_KEY_NONE, 0};
    uint32_t now = millis();
    if (now - _lastScan < 5) return ev; /* scan at most every 5 ms */
    _lastScan = now;
    int raw = scanRaw();
    if (raw != _candidate) { _candidate = raw; _candidateSince = now; return ev; }
    if (raw != _stable && now - _candidateSince >= VTL_KEYPAD_DEBOUNCE_MS) {
      if (_stable >= 0) { ev = {VTL_KEY_RELEASED, keyAt(_stable)}; _stable = -1; return ev; }
      _stable = raw;
      _pressedAt = now;
      _holdSent = false;
      return {VTL_KEY_PRESSED, keyAt(_stable)};
    }
    if (_stable >= 0 && !_holdSent && now - _pressedAt >= VTL_KEYPAD_HOLD_MS) {
      _holdSent = true;
      return {VTL_KEY_HELD, keyAt(_stable)};
    }
    return ev;
  }

  /* Convenience: the key that was just pressed, or 0. */
  char getKey() {
    VTLKeyEvent e = update();
    return e.type == VTL_KEY_PRESSED ? e.key : 0;
  }
  /* Key currently held down (debounced), or 0. */
  char currentKey() const { return _stable >= 0 ? keyAt(_stable) : 0; }
  static char keyAt(int index) { return (index >= 0 && index < ROWS * COLS) ? VTL_KEYPAD_KEYMAP[index] : 0; }

private:
  /* Drive one row LOW, return the column that reads LOW (or -1). */
  int readRow(uint8_t r) {
#if VTL_KEYPAD_BACKEND == VTL_KEYPAD_BACKEND_PCF8574
    /* P0..P3 rows, P4..P7 columns. Writing 1 to a PCF8574 pin makes it a weak-pull-up input. */
    if (!pcfWrite((uint8_t)~(1u << r))) return -1;
    int v = pcfRead();
    pcfWrite(0xFF);
    if (v < 0) return -1;
    for (uint8_t c = 0; c < COLS; c++)
      if (!(v & (1u << (4 + c)))) return c;
    return -1;
#else
    pinMode(_rows[r], OUTPUT);
    digitalWrite(_rows[r], LOW);
    delayMicroseconds(10);
    int found = -1;
    for (uint8_t c = 0; c < COLS && found < 0; c++)
      if (digitalRead(_cols[c]) == LOW) found = c;
    pinMode(_rows[r], INPUT);
    return found;
#endif
  }

#if VTL_KEYPAD_BACKEND == VTL_KEYPAD_BACKEND_PCF8574
  bool pcfWrite(uint8_t v) {
    Wire.beginTransmission(VTL_KEYPAD_I2C_ADDR);
    Wire.write(v);
    return Wire.endTransmission() == 0;
  }
  int pcfRead() {
    if (Wire.requestFrom((uint8_t)VTL_KEYPAD_I2C_ADDR, (uint8_t)1) != 1) return -1;
    return Wire.read();
  }
#else
  const uint8_t _rows[ROWS] = {
    VTL_PIN_KEYPAD_R1,
#if VTL_KEYPAD_ROWS > 1
    VTL_PIN_KEYPAD_R2,
#endif
#if VTL_KEYPAD_ROWS > 2
    VTL_PIN_KEYPAD_R3,
#endif
#if VTL_KEYPAD_ROWS > 3
    VTL_PIN_KEYPAD_R4,
#endif
  };
  const uint8_t _cols[COLS] = {
    VTL_PIN_KEYPAD_C1,
#if VTL_KEYPAD_COLS > 1
    VTL_PIN_KEYPAD_C2,
#endif
#if VTL_KEYPAD_COLS > 2
    VTL_PIN_KEYPAD_C3,
#endif
#if VTL_KEYPAD_COLS > 3
    VTL_PIN_KEYPAD_C4,
#endif
  };
#endif

  bool _ok = false, _holdSent = false;
  int _candidate = -1, _stable = -1;
  uint32_t _lastScan = 0, _candidateSince = 0, _pressedAt = 0;
};

#endif /* VTL_HAS_KEYPAD */
