# Compile report

Generated 2026-10-22 14:24 by `scripts/compile_examples.py`.
Board: `vtlabs:esp32:vtlabs_iot_esp32_r1`  |  arduino-cli  Version: 1.5.0 Commit: dd407d42d Date: 2026-05-19T14:07:51Z

Compilation only proves the code builds for the selected target. It does NOT prove the hardware works - see docs/hardware-validation-checklist.md.

Option sets: `none` = `Keypad: Not fitted, LED Matrix: Not fitted`, `K` = `Keypad=pcf8574_4x4`, `M4` = `LedMatrix=max7219_4in1`, `W` = `LedMatrix=ws2812_8x8`, `K+M4` = `Keypad=pcf8574_4x4,LedMatrix=max7219_4in1`, `K+M8` = `Keypad=pcf8574_4x4,LedMatrix=max7219_8x8`, `K+W` = `Keypad=pcf8574_4x4,LedMatrix=ws2812_8x8`

**153/153 passed** (level 9 re-run after two fixes, see CHANGELOG)

| Sketch | Option set | Result | Flash used |
|---|---|---|---|
| BoardMacrosCheck.ino | K+W | PASS | 290 KB (22%) |
| BoardMacrosCheck.ino | none | PASS | 290 KB (22%) |
| ExpectKeypadMissing.ino | none | PASS | expected error |
| ExpectMatrixMissing.ino | none | PASS | expected error |
| L1_01 Blink | K+W | PASS | 265 KB (20%) |
| L1_01 Blink | none | PASS | 265 KB (20%) |
| L1_02 Read a Push Button | K+W | PASS | 265 KB (20%) |
| L1_02 Read a Push Button | none | PASS | 265 KB (20%) |
| L1_03 Button Controls an LED | K+W | PASS | 251 KB (19%) |
| L1_03 Button Controls an LED | none | PASS | 251 KB (19%) |
| L1_04 Read a Switch (operating mode) | K+W | PASS | 265 KB (20%) |
| L1_04 Read a Switch (operating mode) | none | PASS | 265 KB (20%) |
| L1_05 Debouncing | K+W | PASS | 265 KB (20%) |
| L1_05 Debouncing | none | PASS | 265 KB (20%) |
| L1_06 Digital Sensor: PIR Motion | K+W | PASS | 265 KB (20%) |
| L1_06 Digital Sensor: PIR Motion | none | PASS | 265 KB (20%) |
| L1_07 Buzzer and Relay - Safely | K+W | PASS | 274 KB (21%) |
| L1_07 Buzzer and Relay - Safely | none | PASS | 274 KB (21%) |
| L1_08 Traffic Light State Machine | K+W | PASS | 275 KB (21%) |
| L1_08 Traffic Light State Machine | none | PASS | 274 KB (21%) |
| L2_01 Read a Potentiometer (ADC) | K+W | PASS | 271 KB (21%) |
| L2_01 Read a Potentiometer (ADC) | none | PASS | 270 KB (21%) |
| L2_02 Light Sensor (LDR) | K+W | PASS | 269 KB (21%) |
| L2_02 Light Sensor (LDR) | none | PASS | 269 KB (21%) |
| L2_03 From Raw ADC Counts to Engineering Units | K+W | PASS | 271 KB (21%) |
| L2_03 From Raw ADC Counts to Engineering Units | none | PASS | 271 KB (21%) |
| L2_04 Filtering Noisy Readings | K+W | PASS | 269 KB (21%) |
| L2_04 Filtering Noisy Readings | none | PASS | 269 KB (21%) |
| L2_05 Dim an LED with PWM | K+W | PASS | 257 KB (20%) |
| L2_05 Dim an LED with PWM | none | PASS | 257 KB (20%) |
| L2_06 ADC vs PWM vs true DAC | K+W | PASS | 279 KB (21%) |
| L2_06 ADC vs PWM vs true DAC | none | PASS | 279 KB (21%) |
| L2_07 Potentiometer Dimmer | K+W | PASS | 263 KB (20%) |
| L2_07 Potentiometer Dimmer | none | PASS | 263 KB (20%) |
| L2_08 Tones and a Melody | K+W | PASS | 266 KB (20%) |
| L2_08 Tones and a Melody | none | PASS | 266 KB (20%) |
| L3_01 Diagnostics over USB Serial | K+W | PASS | 272 KB (21%) |
| L3_01 Diagnostics over USB Serial | none | PASS | 272 KB (21%) |
| L3_02 Exchanging Data Between Two UART Devices | K+W | PASS | 271 KB (21%) |
| L3_02 Exchanging Data Between Two UART Devices | none | PASS | 271 KB (21%) |
| L3_03 Serial Command Parser | K+W | PASS | 283 KB (22%) |
| L3_03 Serial Command Parser | none | PASS | 283 KB (22%) |
| L3_04 I2C Address Scanner | K+W | PASS | 286 KB (22%) |
| L3_04 I2C Address Scanner | none | PASS | 286 KB (22%) |
| L3_05 Temperature and Humidity (DHT22) | K+W | PASS | 272 KB (21%) |
| L3_05 Temperature and Humidity (DHT22) | none | PASS | 272 KB (21%) |
| L3_06 I2C Sensor: BME280 (OPTIONAL MODULE) | K+W | PASS | 300 KB (23%) |
| L3_06 I2C Sensor: BME280 (OPTIONAL MODULE) | none | PASS | 300 KB (23%) |
| L3_07 Show Measurements on the OLED Display | K+W | PASS | 306 KB (23%) |
| L3_07 Show Measurements on the OLED Display | none | PASS | 306 KB (23%) |
| L3_08 SPI Peripheral: micro-SD Card (OPTIONAL MODULE) | K+W | PASS | 326 KB (25%) |
| L3_08 SPI Peripheral: micro-SD Card (OPTIONAL MODULE) | none | PASS | 326 KB (25%) |
| L3_09 Introduction to RS485 (OPTIONAL MODULE) | K+W | PASS | 271 KB (21%) |
| L3_09 Introduction to RS485 (OPTIONAL MODULE) | none | PASS | 271 KB (21%) |
| L4_01 Connect to Wi-Fi | K+W | PASS | 860 KB (67%) |
| L4_01 Connect to Wi-Fi | none | PASS | 860 KB (67%) |
| L4_02 Host a Local Web Server | K+W | PASS | 933 KB (72%) |
| L4_02 Host a Local Web Server | none | PASS | 933 KB (72%) |
| L4_03 Sensor Dashboard in the Browser | K+W | PASS | 911 KB (71%) |
| L4_03 Sensor Dashboard in the Browser | none | PASS | 911 KB (71%) |
| L4_04 Control an Output from a Web Page - Safely | K+W | PASS | 903 KB (70%) |
| L4_04 Control an Output from a Web Page - Safely | none | PASS | 903 KB (70%) |
| L4_05 Log Measurements to Flash (LittleFS) | K+W | PASS | 949 KB (74%) |
| L4_05 Log Measurements to Flash (LittleFS) | none | PASS | 948 KB (74%) |
| L4_06 MQTT: Publish and Subscribe (advanced) | K+W | PASS | 886 KB (69%) |
| L4_06 MQTT: Publish and Subscribe (advanced) | none | PASS | 886 KB (69%) |
| L5_01 Project: Automatic Lighting | K+W | PASS | 279 KB (21%) |
| L5_01 Project: Automatic Lighting | none | PASS | 279 KB (21%) |
| L5_02 Project: Temperature-Based Fan Control | K+W | PASS | 281 KB (22%) |
| L5_02 Project: Temperature-Based Fan Control | none | PASS | 281 KB (22%) |
| L5_03 Project: Environmental Monitor (offline) | K+W | PASS | 307 KB (24%) |
| L5_03 Project: Environmental Monitor (offline) | none | PASS | 307 KB (24%) |
| L5_04 Project: Time-Stamped Data Logger | K+W | PASS | 1011 KB (78%) |
| L5_04 Project: Time-Stamped Data Logger | none | PASS | 1011 KB (78%) |
| L5_05 Project: Local Wi-Fi Control (no router) | K+W | PASS | 920 KB (71%) |
| L5_05 Project: Local Wi-Fi Control (no router) | none | PASS | 920 KB (71%) |
| L5_06 Project: Complete Sensor-to-Dashboard System | K+W | PASS | 928 KB (72%) |
| L5_06 Project: Complete Sensor-to-Dashboard System | none | PASS | 928 KB (72%) |
| L6_01 How a Matrix Keypad is Scanned | K | PASS | 287 KB (22%) |
| L6_01 How a Matrix Keypad is Scanned | K+M4 | PASS | 287 KB (22%) |
| L6_02 Read Individual Keys | K | PASS | 296 KB (23%) |
| L6_02 Read Individual Keys | K+M4 | PASS | 296 KB (23%) |
| L6_03 Debouncing, Hold and Release Events | K | PASS | 287 KB (22%) |
| L6_03 Debouncing, Hold and Release Events | K+M4 | PASS | 287 KB (22%) |
| L6_04 Numeric Input with Validation | K | PASS | 296 KB (23%) |
| L6_04 Numeric Input with Validation | K+M4 | PASS | 296 KB (23%) |
| L6_05 Password (PIN) Entry | K | PASS | 306 KB (23%) |
| L6_05 Password (PIN) Entry | K+M4 | PASS | 306 KB (23%) |
| L6_06 Menu Navigation on the OLED | K | PASS | 306 KB (23%) |
| L6_06 Menu Navigation on the OLED | K+M4 | PASS | 306 KB (23%) |
| L6_07 Configurable System Settings | K | PASS | 308 KB (24%) |
| L6_07 Configurable System Settings | K+M4 | PASS | 308 KB (24%) |
| L6_08 Keypad as the Kit's Control Panel | K | PASS | 308 KB (24%) |
| L6_08 Keypad as the Kit's Control Panel | K+M4 | PASS | 308 KB (24%) |
| L7_01 MAX7219 LED Matrix: Pixels and Rows | K+M8 | PASS | 273 KB (21%) |
| L7_01 MAX7219 LED Matrix: Pixels and Rows | M4 | PASS | 273 KB (21%) |
| L7_02 MAX7219: Text and Effects with MD_Parola | K+M8 | PASS | 287 KB (22%) |
| L7_02 MAX7219: Text and Effects with MD_Parola | M4 | PASS | 287 KB (22%) |
| L7_03 WS2812 Matrix: Data Order and Pixel Mapping | W | PASS | 282 KB (22%) |
| L7_04 WS2812: Brightness, Power and Gamma | W | PASS | 288 KB (22%) |
| L7_05 WS2812: x/y Drawing and Text with NeoMatrix | W | PASS | 300 KB (23%) |
| L8_01 Individual Pixel Control (any matrix) | K+M8 | PASS | 294 KB (22%) |
| L8_01 Individual Pixel Control (any matrix) | M4 | PASS | 294 KB (22%) |
| L8_01 Individual Pixel Control (any matrix) | W | PASS | 317 KB (24%) |
| L8_02 Numbers and Symbols (any matrix) | K+M8 | PASS | 274 KB (21%) |
| L8_02 Numbers and Symbols (any matrix) | M4 | PASS | 274 KB (21%) |
| L8_02 Numbers and Symbols (any matrix) | W | PASS | 301 KB (23%) |
| L8_03 Predefined Patterns (any matrix) | K+M8 | PASS | 288 KB (22%) |
| L8_03 Predefined Patterns (any matrix) | M4 | PASS | 288 KB (22%) |
| L8_03 Predefined Patterns (any matrix) | W | PASS | 311 KB (24%) |
| L8_04 Scrolling Text (any matrix) | K+M8 | PASS | 293 KB (22%) |
| L8_04 Scrolling Text (any matrix) | M4 | PASS | 293 KB (22%) |
| L8_04 Scrolling Text (any matrix) | W | PASS | 317 KB (24%) |
| L8_05 Animations (any matrix) | K+M8 | PASS | 279 KB (21%) |
| L8_05 Animations (any matrix) | M4 | PASS | 279 KB (21%) |
| L8_05 Animations (any matrix) | W | PASS | 307 KB (23%) |
| L8_06 Showing Sensor Measurements (any matrix) | K+M8 | PASS | 281 KB (22%) |
| L8_06 Showing Sensor Measurements (any matrix) | M4 | PASS | 281 KB (22%) |
| L8_06 Showing Sensor Measurements (any matrix) | W | PASS | 308 KB (24%) |
| L8_07 System Status and Warnings (any matrix) | K+M8 | PASS | 303 KB (23%) |
| L8_07 System Status and Warnings (any matrix) | M4 | PASS | 303 KB (23%) |
| L8_07 System Status and Warnings (any matrix) | W | PASS | 327 KB (25%) |
| L9_01 Exercise 1: Keypad-Controlled LEDs | K | PASS | 296 KB (23%) |
| L9_01 Exercise 1: Keypad-Controlled LEDs | K+M4 | PASS | 319 KB (24%) |
| L9_01 Exercise 1: Keypad-Controlled LEDs | K+M8 | PASS | 319 KB (24%) |
| L9_01 Exercise 1: Keypad-Controlled LEDs | K+W | PASS | 321 KB (25%) |
| L9_02 Exercise 2: Keypad Value on the LED Matrix | K+M4 | PASS | 320 KB (25%) |
| L9_02 Exercise 2: Keypad Value on the LED Matrix | K+M8 | PASS | 320 KB (25%) |
| L9_02 Exercise 2: Keypad Value on the LED Matrix | K+W | PASS | 322 KB (25%) |
| L9_03 Exercise 3: Access-Control Demonstration | K | PASS | 298 KB (23%) |
| L9_03 Exercise 3: Access-Control Demonstration | K+M4 | PASS | 321 KB (25%) |
| L9_03 Exercise 3: Access-Control Demonstration | K+M8 | PASS | 321 KB (25%) |
| L9_03 Exercise 3: Access-Control Demonstration | K+W | PASS | 322 KB (25%) |
| L9_04 Exercise 4: Sensor Measurement on the Matrix | K+M8 | PASS | 281 KB (22%) |
| L9_04 Exercise 4: Sensor Measurement on the Matrix | M4 | PASS | 281 KB (21%) |
| L9_04 Exercise 4: Sensor Measurement on the Matrix | W | PASS | 308 KB (24%) |
| L9_05 Exercise 5: Threshold Set Through the Keypad | K | PASS | 311 KB (24%) |
| L9_05 Exercise 5: Threshold Set Through the Keypad | K+M4 | PASS | 333 KB (26%) |
| L9_05 Exercise 5: Threshold Set Through the Keypad | K+M8 | PASS | 333 KB (26%) |
| L9_05 Exercise 5: Threshold Set Through the Keypad | K+W | PASS | 335 KB (26%) |
| L9_06 Exercise 6: Alarm System with Keypad Acknowledgement | K | PASS | 298 KB (23%) |
| L9_06 Exercise 6: Alarm System with Keypad Acknowledgement | K+M4 | PASS | 321 KB (25%) |
| L9_06 Exercise 6: Alarm System with Keypad Acknowledgement | K+M8 | PASS | 321 KB (25%) |
| L9_06 Exercise 6: Alarm System with Keypad Acknowledgement | K+W | PASS | 323 KB (25%) |
| L9_07 Exercise 7: IoT Monitor with Matrix Status | K+M4 | PASS | 972 KB (75%) |
| L9_07 Exercise 7: IoT Monitor with Matrix Status | K+M8 | PASS | 972 KB (75%) |
| L9_07 Exercise 7: IoT Monitor with Matrix Status | K+W | PASS | 972 KB (75%) |
| L9_07 Exercise 7: IoT Monitor with Matrix Status | M4 | PASS | 949 KB (74%) |
| L9_07 Exercise 7: IoT Monitor with Matrix Status | W | PASS | 971 KB (75%) |
| PinOverrideCheck.ino | K+W | PASS | 251 KB (19%) |
| PinOverrideCheck.ino | none | PASS | 251 KB (19%) |
| WifiWebServerCheck.ino | K+W | PASS | 884 KB (69%) |
| WifiWebServerCheck.ino | none | PASS | 884 KB (69%) |

## Custom board template

`boards/templates/custom_devkit_keypad_gpio.toml` copied into `boards/`, generated, and all 8 L6 keypad lessons compiled for `vtlabs:esp32:custom_devkit_keypad` with `Keypad=gpio_4x4` (direct-GPIO backend): **8/8 passed**.
