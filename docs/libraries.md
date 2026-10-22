# Library Dependencies

The single source of truth is [`libraries.txt`](../libraries.txt). Versions are the ones tested
(2026-10-22) and are treated as **minimum** versions.

| Library (Library Manager name) | Tested | Needed for | Source |
|---|---|---|---|
| *(none)* - VTLabsKit | bundled | all lessons: pins, buttons, keypad, matrix API | inside the VTLabs board package |
| WiFi, WebServer, ESPmDNS, LittleFS, SD, SPI, Wire, Preferences | core 3.3.x | L3, L4, L5, L6, L9 | part of the Espressif ESP32 core |
| Adafruit SSD1306 | 2.5.17 | OLED: L3_07, L5_03, L5_04, L6_06, L6_07, L6_08 | https://github.com/adafruit/Adafruit_SSD1306 |
| Adafruit GFX Library | 1.13.0 | OLED and WS2812 text | https://github.com/adafruit/Adafruit-GFX-Library |
| Adafruit BusIO | 1.17.4 | dependency of GFX/BME280 | https://github.com/adafruit/Adafruit_BusIO |
| DHT sensor library | 1.4.7 | DHT22 lessons | https://github.com/adafruit/DHT-sensor-library |
| Adafruit Unified Sensor | 1.1.15 | dependency of DHT/BME280 | https://github.com/adafruit/Adafruit_Sensor |
| Adafruit BME280 Library | 2.3.0 | L3_06 (optional module) | https://github.com/adafruit/Adafruit_BME280_Library |
| MD_MAX72XX | 3.5.1 | MAX7219 matrix: L7_01, L8_*, L9_* | https://github.com/MajicDesigns/MD_MAX72XX |
| MD_Parola | 3.7.7 | MAX7219 text/scrolling: L7_02, L8_*, L9_* | https://github.com/MajicDesigns/MD_Parola |
| Adafruit NeoPixel | 1.15.5 | WS2812 matrix: L7_03, L7_04, L8_*, L9_* | https://github.com/adafruit/Adafruit_NeoPixel |
| Adafruit NeoMatrix | 1.3.3 | WS2812 x/y and text: L7_05, L8_*, L9_* | https://github.com/adafruit/Adafruit_NeoMatrix |
| PubSubClient | 2.8 | MQTT: L4_06 | https://github.com/knolleary/pubsubclient |

The keypad needs no external library (scanning is in VTLabsKeypad.h, so students can read it).
A sketch only needs the libraries it `#include`s - each sketch header lists them.

## How they get installed

| Situation | How |
|---|---|
| Arduino IDE **2.4 or newer**, installing VTLabs from Boards Manager | Installed automatically with the platform (`libraryDependencies` in the package index). |
| Arduino IDE 2.3.x | Tools > Manage Libraries, search each name above, Install (accept "Install all" for dependencies). |
| Arduino CLI / CI / lab PCs | `python3 scripts/install_libraries.py` (runs `arduino-cli lib install "Name@version"` for each line) |
| Offline lab | On a PC with internet, Sketch > Include Library > Add .ZIP Library on each downloaded release zip, then copy `Documents/Arduino/libraries` to the lab PCs. |

## Adding or upgrading a library

1. Edit `libraries.txt` (exact Library Manager name, `@` version).
2. `python3 scripts/install_libraries.py` and `python3 scripts/compile_examples.py`.
3. Mention it in the sketch header ("Libraries:") and in this table.
4. Release - the next index entry carries the new dependency list.
