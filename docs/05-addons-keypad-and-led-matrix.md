# Add-on Hardware: Matrix Keypad and LED Matrix

Status: **software ready and compile-tested; the physical add-ons are not yet identified.**
The profile currently offers the most common parts (PCF8574 keypad, MAX7219 4-in-1 / 8x8, WS2812 8x8).
Do sections 1 and 2 with the real hardware *before* teaching, then adjust the profile if anything differs.

---

## 1. Identify the keypad

| Question | How to find out | Where it goes in the profile |
|---|---|---|
| Rows x columns? | Count keys; 4x4 = 16 keys (with A-D), 4x3 = 12 keys. Count the wires: 8 = 4x4, 7 = 4x3. | `KEYPAD_ROWS`, `KEYPAD_COLS` |
| Key labels and order | Read the keys row by row from top-left | `KEYPAD_KEYMAP` |
| Which wires are rows? | Multimeter in continuity mode: press "1", find the two wires that connect. Pin 1 is usually R1. On most membrane keypads: pins 1-4 = rows, 5-8 = columns. | PCF8574 wiring order or `KEYPAD_R1..C4` |
| Expander fitted? | Look for a small 16-pin chip marked **PCF8574** or **PCF8574A** (or a blue "I2C keypad" board). Run lesson **L3_04 I2C scanner**. | Tools menu choice, `KEYPAD_I2C_ADDR` |
| Expander address | L3_04 prints it (PCF8574: 0x20-0x27, PCF8574A: 0x38-0x3F; set by A0-A2 jumpers) | `KEYPAD_I2C_ADDR` |
| Direct GPIO instead? | Keypad wires go straight to the ESP32 header | Custom profile (template), 8 free GPIOs needed |

### Electrical arrangement (why scanning works)

```
           C1    C2    C3    C4         each key = a switch between one row and one column
      R1 ──[1]───[2]───[3]───[A]
      R2 ──[4]───[5]───[6]───[B]        scan: drive ONE row LOW, others released;
      R3 ──[7]───[8]───[9]───[C]              a column reading LOW = key at (row, column)
      R4 ──[*]───[0]───[#]───[D]
```

- **PCF8574 backend** (default menu choice): rows on P0-P3, columns on P4-P7. Writing `1` to a PCF8574 pin
  makes it a weak pull-up input, so columns idle HIGH. Uses only the I2C bus (SDA/SCL), no extra GPIO.
- **GPIO backend**: rows are switched between OUTPUT LOW and INPUT (high impedance), so two pressed keys
  never short two driven outputs. Columns need pull-ups: internal (`INPUT_PULLUP`) or, on GPIO34-39,
  external 10 k resistors.
- **Ghosting**: three keys pressed in an "L" make a fourth key appear pressed. Membrane keypads have no
  diodes. The library reports one key at a time; lesson L6_01 demonstrates the effect.
- Debounce: a key must be stable for `VTL_KEYPAD_DEBOUNCE_MS` (30 ms). Hold event after `VTL_KEYPAD_HOLD_MS` (700 ms).

### Wiring (PCF8574 module)

| Keypad pin (left to right, typical) | PCF8574 | ESP32 side |
|---|---|---|
| 1 R1 | P0 | |
| 2 R2 | P1 | |
| 3 R3 | P2 | |
| 4 R4 | P3 | |
| 5 C1 | P4 | |
| 6 C2 | P5 | |
| 7 C3 | P6 | |
| 8 C4 | P7 | |
| | VCC | 3V3 |
| | GND | GND |
| | SDA | `VTL_PIN_I2C_SDA` (GPIO21) |
| | SCL | `VTL_PIN_I2C_SCL` (GPIO22) |

Power the PCF8574 from **3.3 V** so the I2C lines stay at ESP32 logic level.

---

## 2. Identify the LED matrix

| You see | It is | Menu choice |
|---|---|---|
| Module with a 24-pin chip **MAX7219** (or MAX7221), 5 pins: VCC GND DIN CS CLK, red single-colour LEDs | MAX7219 | MAX7219 8x8 or 32x8 |
| Four such modules on one PCB (blue, "FC-16") | MAX7219 4-in-1 | MAX7219 32x8 (FC-16) |
| Flexible/rigid panel, each LED a 5050 square with a tiny chip inside, 3 wires: 5V, GND, DIN | WS2812 / WS2812B / SK6812 RGB | WS2812 RGB 8x8 |
| HT16K33, TM1637, HUB75 connector | other driver | not supported yet - add a new option (see customise guide 3.3) |

These drivers are fundamentally different. The kit therefore uses each driver's own library
(lessons L7) and offers a small common API only for the later lessons (L8, L9).

---

## 3. MAX7219 - serial interface, driver configuration, wiring

### Interface
- SPI-like, write only: **DIN** (data), **CLK** (clock), **CS/LOAD** (latch). The ESP32 sends 16-bit words
  (4-bit register address + 8-bit data) per chip. Chained modules pass data from DOUT to the next DIN;
  one CS pulse latches all of them.
- The kit uses the ESP32 hardware SPI bus: DIN = `VTL_PIN_SPI_MOSI` (GPIO23), CLK = `VTL_PIN_SPI_SCK` (GPIO18),
  CS = `VTL_PIN_MAX7219_CS` (GPIO15). The SD card can share SCK/MOSI because it has its own CS.
- The MAX7219 multiplexes the LEDs itself: the ESP32 only sends a change. Registers: digit 0-7 (= rows),
  decode mode, intensity (0-15), scan limit, shutdown, display test.

### Driver configuration (library MD_MAX72XX + MD_Parola)
| Setting | Value | Why it matters |
|---|---|---|
| `MAX7219_HW_TYPE` | `FC16_HW` for the common blue 4-in-1; `GENERIC_HW` for most single red modules | Modules wire rows/columns differently; wrong type = mirrored or rotated text |
| `MAX7219_DEVICES` | number of 8x8 blocks | width = 8 x devices |
| Intensity | `setBrightness()` 0-100 % -> MAX7219 0-15 | current and heat |
| Column order | MD_MAX72XX counts columns **from the right**; VTLMatrix converts to (0,0) = top-left | portable code |

### Wiring
| Module | Connect to | Note |
|---|---|---|
| VCC | 5 V | 4-in-1 at full brightness can draw ~1 A - use a 5 V supply, not the 3V3 pin |
| GND | GND | common with ESP32 |
| DIN | GPIO23 (MOSI) | 3.3 V logic is accepted by most modules (MAX7219 V_IH = 3.5 V at 5 V supply is marginal: if unreliable, add a 74AHCT125 level shifter) |
| CS | GPIO15 | strapping pin: fine as a CS output (idles HIGH) |
| CLK | GPIO18 (SCK) | |

Chain direction: data goes in on the side marked DIN. On 4-in-1 modules DIN is on the right when the text reads correctly.

Validation: lesson **L7_01** (walking pixel, rows, brightness), then **L8_01** corner test.

---

## 4. WS2812 - data signalling, power, colour order, mapping, brightness, level shifting

### Data signalling
- One data wire, no clock. Each bit is a pulse with precise timing at **800 kHz**: "0" = short HIGH
  (~0.4 us), "1" = long HIGH (~0.8 us), 1.25 us per bit. 24 bits per LED. A pause > 50 us (reset) latches.
- Each LED takes the first 24 bits and passes the rest on: the panel is one long **strip**.
- On the ESP32 the Adafruit NeoPixel library generates the timing with the RMT peripheral; Wi-Fi does not
  disturb it.

### Power requirements
| | Value |
|---|---|
| Per LED, full white | ~60 mA (20 mA per colour) |
| 8x8 = 64 LEDs, full white | **~3.8 A at 5 V** |
| USB port | 0.5-0.9 A |
| Kit default cap | `WS2812_MAX_BRIGHTNESS = 40` (/255) -> about 0.6 A worst case |

- Feed 5 V to the panel directly from a supply rated for the cap you set (5 V / 2 A for the classroom cap,
  5 V / 4 A for full brightness). Never from the ESP32 3V3 pin.
- **Common GND** between supply, panel and ESP32 is mandatory.
- 1000 uF / 6.3 V+ capacitor across 5 V and GND at the panel; 330 ohm resistor in series with DIN, close to the panel.
- Lesson **L7_04** estimates the current live.

### Colour order
Most WS2812B are **GRB** (`NEO_GRB`), some SK6812/clones are RGB. Lesson **L7_03** fills the panel red,
green, blue: if "red" shows green, change `WS2812_COLOR_ORDER`.

### Pixel mapping
Lesson **L7_03** lights pixels 0..63 in strip order. Note:
- Where pixel 0 is: `NEO_MATRIX_TOP/BOTTOM` + `NEO_MATRIX_LEFT/RIGHT`
- Rows or columns first: `NEO_MATRIX_ROWS/COLUMNS`
- Every line same direction (`NEO_MATRIX_PROGRESSIVE`) or alternating (`NEO_MATRIX_ZIGZAG`)
  - Rigid 8x8 PCBs (CJMCU-64 style) are usually TOP+LEFT+ROWS+PROGRESSIVE (kit default).
  - Flexible panels are usually ZIGZAG.
Put the result in `WS2812_LAYOUT`. Lesson **L7_05** draws a frame and diagonal to confirm.

### Brightness control
- `setBrightness()` in VTLMatrix takes 0-100 % and scales to `0..WS2812_MAX_BRIGHTNESS`.
- Perceived brightness is not linear: use gamma correction (`strip.gamma32()`), shown in L7_04.

### Signal-level conversion
At 5 V supply a WS2812 needs **V_IH >= 0.7 x VDD = 3.5 V**; the ESP32 outputs 3.3 V. It often works,
but not reliably (cold, long wires). Options, best first:
1. **74AHCT125** (or 74HCT245) powered from 5 V between GPIO13 and DIN - recommended for class kits.
2. A single "sacrificial" WS2812 powered through a diode (~4.3 V) as the first pixel.
3. Run the panel at ~4.5 V. Do not use plain resistor dividers or slow MOSFET shifters for 800 kHz data.

---

## 5. Where everything is configured

| What | File |
|---|---|
| Which add-ons exist, their pins and settings | `boards/<board>.toml` -> `[[option]]` |
| What the student selects | Arduino IDE **Tools > Keypad (add-on)** and **Tools > LED Matrix (add-on)** |
| Driver code | `libraries/VTLabsKit/src/VTLabsKeypad.h`, `VTLabsMatrix.h` (header-only) |
| Lessons | L6 keypad, L7 drivers, L8 portable matrix, L9 integrated |
| Libraries | MAX7219: MD_MAX72XX, MD_Parola. WS2812: Adafruit NeoPixel, NeoMatrix, GFX. Keypad: none. |

If a sketch needs an add-on that is set to "Not fitted", compilation stops with a message telling the
student which Tools menu to set - so no lesson pretends a module is present.

## 6. Integrated exercises (L9)

| # | Lesson | Needs |
|---|---|---|
| 1 | L9_01 Keypad-controlled LEDs | keypad (matrix optional) |
| 2 | L9_02 Keypad value on the LED matrix | keypad + matrix |
| 3 | L9_03 Access control with status indicators | keypad (matrix optional), relay, LEDs, buzzer |
| 4 | L9_04 Sensor measurement on the matrix | matrix, DHT22, LDR |
| 5 | L9_05 Threshold set through the keypad | keypad (matrix optional), LDR |
| 6 | L9_06 Alarm with keypad acknowledgement | keypad (matrix optional), PIR, DHT22, buzzer |
| 7 | L9_07 IoT monitor with matrix status | matrix (keypad optional), Wi-Fi, DHT22 |
