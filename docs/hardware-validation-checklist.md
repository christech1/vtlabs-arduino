# Hardware Validation Checklist

Compilation (scripts/compile_examples.py, CI) proves only that the code builds. This checklist is the
**physical** test of a real kit. Do it for: the first board of every hardware revision, every add-on
module type, and every new release of the board package (spot check: sections A, B, F).

Record results in a copy of this file named `test-records/<board-id>_<serial>_<date>.md`.
When every row for a pin passes, set `verified = true` for that pin in `boards/<board>.toml`
(and `[board] verified = true` when all pass), run `generate`, and release.

| Field | Value |
|---|---|
| Board ID / revision | |
| Kit serial number | |
| Module marking (on the metal can) | |
| esptool `chip-id` / `flash-id` output (chip, features, flash size) | |
| Keypad add-on (type, expander, address) | |
| LED matrix add-on (driver, size, module type) | |
| VTLabs package version / ESP32 core version / IDE version | |
| Tester / date | |

Legend: P = pass, F = fail, N/A = not fitted.

## A. Programming and identification

| # | Test | Lesson / action | Expected | P/F |
|---|---|---|---|---|
| A1 | Board appears in Tools > Board | - | "VTLabs IoT Training Kit ESP32 (Rev 1)" | |
| A2 | Upload without pressing BOOT | L1_01 | upload ends "Hard resetting via RTS pin" | |
| A3 | Board info | L3_01 | board ID, revision, core version printed | |
| A4 | Chip and flash match profile | esptool `chip-id`, `flash-id` | ESP32, 4 MB, no PSRAM (or update the profile) | |
| A5 | Auto-reset on Serial Monitor open | open monitor | no reset loop, output readable at 115200 | |

## B. Digital I/O

| # | Pin name | Lesson | Expected | P/F |
|---|---|---|---|---|
| B1 | LED_BUILTIN | L1_01 | blinks 1 Hz | |
| B2 | LED_RED / GREEN / YELLOW | L1_08 | each colour in the right position | |
| B3 | BTN1 | L1_02 | 0 when pressed (active low), 1 released | |
| B4 | BTN2 (BOOT) | L5_03 (reset min/max) | responds; board still enters download mode when held at reset | |
| B5 | SWITCH | L1_04 | AUTO/MANUAL follows the switch | |
| B6 | PIR | L1_06 | motion detected after warm-up | |
| B7 | BUZZER | L2_08 | melody audible, pitch follows POT | |
| B8 | RELAY | L1_07 | click ON 5 s, OFF; **no click at power-up or reset** | |

## C. Analog

| # | Pin name | Lesson | Expected | P/F |
|---|---|---|---|---|
| C1 | POT | L2_01 | ~0 to ~4095 over full rotation | |
| C2 | POT calibrated | L2_03 | calibrated mV within +/-50 mV of multimeter | |
| C3 | LDR | L2_02 | value rises with light | |
| C4 | DAC (LED_GREEN) | L2_06 | multimeter ~1.65 V at mid knob | |
| C5 | PWM (LED_YELLOW) | L2_06 | average ~1.65 V at mid knob; square wave on scope | |

## D. Buses and modules

| # | Item | Lesson | Expected | P/F |
|---|---|---|---|---|
| D1 | I2C scan | L3_04 | OLED 0x3C (+ keypad expander, BME280 if fitted) | |
| D2 | OLED | L3_07 | readable text, correct size (128x64) | |
| D3 | DHT22 | L3_05 | plausible T and RH, no "read failed" | |
| D4 | UART2 loopback (TX2-RX2 jumper) | L3_02 | own PING lines received | |
| D5 | SD card (optional) | L3_08 | file written and read back | |
| D6 | RS485 (optional, 2 kits) | L3_09 | request/response both ways | |
| D7 | BME280 (optional) | L3_06 | plausible values | |

## E. Wi-Fi

| # | Test | Lesson | Expected | P/F |
|---|---|---|---|---|
| E1 | Connect + RSSI | L4_01 | connects < 10 s, RSSI shown | |
| E2 | Web dashboard from phone | L4_03 | values update every 2 s | |
| E3 | ADC1 pins with Wi-Fi on | L4_03 | light and pot values still change | |
| E4 | Access point mode | L5_05 | phone joins VTLabs-XXXX, controls work | |

## F. Keypad add-on

| # | Test | Lesson | Expected | P/F |
|---|---|---|---|---|
| F1 | Detected (PCF8574) | L3_04 | address 0x20 (or as configured) | |
| F2 | Raw scan, every key | L6_01 | each of 16 keys shows # at the correct row/column | |
| F3 | Key labels | L6_02 | printed label matches the key cap for all 16 keys | |
| F4 | Debounce | L6_03 | 20 fast presses = 20 PRESSED events, no doubles | |
| F5 | Hold / release | L6_03 | HELD after ~0.7 s, RELEASED with duration | |
| F6 | Password lock-out | L6_05 | 3 wrong -> 30 s lock | |
| F7 | Settings persistence | L6_07 | value survives power cycle | |

## G. LED matrix add-on - MAX7219

| # | Test | Lesson | Expected | P/F |
|---|---|---|---|---|
| G1 | All LEDs | L7_01 step 2 | every row fills, no dead LEDs | |
| G2 | Module order and orientation | L7_02 | text reads left to right, not mirrored, modules in order | |
| G3 | Corners | L8_01 | top-left, top-right, bottom-right, bottom-left in that order | |
| G4 | Brightness | L7_01 step 3 | 16 visible steps | |
| G5 | Power | multimeter / USB meter | all LEDs on at intensity 15: current within supply rating | |

## H. LED matrix add-on - WS2812

| # | Test | Lesson | Expected | P/F |
|---|---|---|---|---|
| H1 | Strip order | L7_03 step 1 | record pixel 0 corner and progressive/zigzag; profile matches | |
| H2 | Colour order | L7_03 step 2 | red, green, blue appear as named | |
| H3 | Layout | L7_05 | frame is a clean rectangle, diagonal from top-left | |
| H4 | Corners | L8_01 | as G3 | |
| H5 | Power at cap | L7_04 full white | measured current <= estimate +20 %, ESP32 does not reset | |
| H6 | Data reliability | L8_05 for 10 min | no random flicker or wrong colours | |
| H7 | Level shifter fitted | visual | 74AHCT125 (or documented alternative) present | |

## I. Integrated exercises (spot check)

| # | Lesson | Expected | P/F |
|---|---|---|---|
| I1 | L9_02 | typed value appears on matrix, confirmed value scrolls | |
| I2 | L9_03 | correct PIN opens relay 5 s; wrong PIN cross icon; lock-out | |
| I3 | L9_06 | arm -> motion -> siren -> PIN # acknowledges -> PIN D disarms | |
| I4 | L9_07 | matrix status equals web /api/status | |

## Result

| | |
|---|---|
| Failed items and actions | |
| Profile changes made (pins, add-on settings) | |
| Approved for class use (name, date) | |
