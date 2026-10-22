# Customising and Deploying the VTLabs Board Package

This is the everyday guide for VTLabs staff (maintainers) and for teachers/students who want to
change pins. Everything you may want to edit is listed here, with the exact file and command.

Requirements on your PC: **Git**, **Python 3.11+** (Windows: install from python.org and tick
"Add to PATH"; then use `py` instead of `python3` in the commands below), **Arduino IDE 2.3.3+**.
For automated compiling also **Arduino CLI** (optional).

---

## 1. How the pieces fit together

```
 boards/vtlabs_iot_esp32_r1.toml        <- YOU EDIT THIS: pins, buses, add-on modules, menus
          |
          |  python3 tools/vtlabs_board.py generate     (checks the pin map first)
          v
 hardware/vtlabs/esp32/
   boards.txt                            <- generated: board entry + Tools menus
   variants/vtlabs_iot_esp32_r1/
     pins_arduino.h                      <- generated: TX/RX/SDA/SCL/SPI/A0.. names for the core
     vtlabs_board.h                      <- generated: VTL_PIN_LED_RED, VTL_HAS_KEYPAD, ...
   libraries/VTLabsKit/                  <- reusable code: VTLButton, VTLKeypad, VTLMatrix
     examples/L1_.. L9_..                <- 64 lessons; they only use VTL_ names, never numbers
   platform.txt                          <- package version + one core-path setting
 docs/pinmaps/vtlabs_iot_esp32_r1.md     <- generated pin table for students
```

Layers, from hardware to student:

| Layer | Lives in | Changes when |
|---|---|---|
| Hardware profile (single source of truth) | `boards/*.toml` | the PCB or an add-on changes |
| Board definition (generated) | `boards.txt`, `variants/` | after each profile edit |
| MCU core | Espressif `esp32:esp32` (not in this repo) | Espressif releases |
| Reusable library | `libraries/VTLabsKit/src` | new features |
| Lessons | `libraries/VTLabsKit/examples` | curriculum work |
| Documentation | `docs/` | always |

Rule: **examples never contain GPIO numbers.** They use `VTL_PIN_<NAME>`, so a new hardware revision only
needs a new profile — the lessons stay the same.

---

## 2. Quick reference

| I want to... | Do this | Section |
|---|---|---|
| Move a pin on the kit (new PCB) | New profile/revision, never edit the old one | 3.4 |
| Fix a wrong pin before release | Edit the `.toml`, `generate` | 3.1 |
| Add my own pin to the kit | Add a line under `[pins]`, `generate` | 3.2 |
| Use a different pin in ONE sketch | `#define VTL_PIN_X n` before `#include <VTLabsKit.h>` | 4 |
| Make a board for my own breadboard | Copy the template, `generate` | 3.5 |
| Change keypad layout / matrix type / colour order / brightness | Edit the `[[option]]` block | 3.3 |
| Change which Tools-menu entry is the default | Reorder choices (first = default) | 3.3 |
| Test locally | Developer install | 5 |
| Publish to students | Bump version, tag | 6 |
| Undo a bad release | Higher patch version | 6.4 |

---

## 3. Editing the hardware profile

Open `boards/vtlabs_iot_esp32_r1.toml` in any text editor (VS Code shows TOML nicely).

### 3.1 Change a pin

```toml
LED_RED = { gpio = 25, mode = "pwm", desc = "Red LED (also DAC1 output)", verified = false }
```

Change `gpio`, save, then:

```
python3 tools/vtlabs_board.py generate
```

The tool **refuses** to generate if the change is unsafe, and tells you why:

| Check | Example message |
|---|---|
| Same GPIO used twice | `GPIO25 used twice: LED_RED and BUZZER` |
| Flash pins | `GPIO6 is wired to the SPI flash - never use 6..11` |
| Output on input-only pin | `GPIO34 is input-only, cannot be 'out'` |
| Internal pull-up on 34-39 | `GPIO35 has no internal pull-up/down; use pull = "external"` |
| Analog on a pin without ADC | `GPIO23 has no ADC` |
| Boot failure risk | `GPIO12 with a pull-up stops the ESP32 from booting` |
| A curriculum name missing | `[pins] is missing PIR - the curriculum examples use these names` |

Warnings (generation still happens) cover strapping pins, ADC2 pins (no `analogRead()` while Wi-Fi is on)
and pins not yet verified on hardware.

Field reference for `[pins]`:

| Field | Values | Effect |
|---|---|---|
| `gpio` | number, or `-1` | `-1` = not fitted on this board |
| `mode` | `out`, `in`, `io`, `analog`, `pwm` | used by the checker |
| `active_low` | `true` / `false` | creates `VTL_PIN_X_ACTIVE_LOW` and `VTL_ACTIVE_LEVEL(X)` |
| `pull` | `external`, `internal`, `internal-down` | creates `VTL_PIN_X_MODE` = `INPUT` / `INPUT_PULLUP` / `INPUT_PULLDOWN` |
| `shares` | another name | allow two names on one GPIO on purpose |
| `desc` | text | shown in the pin table and generated header |
| `verified` | `true` / `false` | set `true` after the hardware checklist passes |

### 3.2 Add your own pin

Add a line anywhere in `[pins]`:

```toml
SERVO     = { gpio = 12, mode = "pwm", desc = "Servo on header J5" }
SOIL      = { gpio = 32, mode = "analog", desc = "Soil moisture sensor" }
```

Run `generate`. Every sketch can now use `VTL_PIN_SERVO` and `VTL_PIN_SOIL`, and the pin table in
`docs/pinmaps/` lists them. If the GPIO is already taken, the checker says so.

Other sections work the same way:

```toml
[settings]   # becomes VTL_<NAME>
SERVO_MIN_US = 500

[i2c]        # becomes VTL_I2C_<NAME>
RTC = { addr = 0x68, desc = "DS3231 clock" }
```

### 3.3 Add-on modules and Tools menus

Each `[[option]]` block becomes a **Tools menu** in the Arduino IDE. Each `[[option.choice]]` is one entry.
**The first choice is the default.**

```toml
[[option]]
menu  = "LedMatrix"
label = "LED Matrix (add-on)"

  [[option.choice]]
  id    = "none"
  label = "Not fitted"
  defines = { HAS_MATRIX = 0 }

  [[option.choice]]
  id    = "max7219_4in1"
  label = "MAX7219 32x8 (4-in-1 FC-16 module, SPI)"
  defines = { HAS_MATRIX = 1, MATRIX_DRIVER = "VTL_MATRIX_DRIVER_MAX7219", MATRIX_WIDTH = 32, ... }
  pins = { MAX7219_CS = { gpio = 15, mode = "out" } }
```

Things you can tune in `defines` (each becomes `VTL_<NAME>`):

| Setting | Meaning |
|---|---|
| `KEYPAD_BACKEND` | `VTL_KEYPAD_BACKEND_PCF8574` or `VTL_KEYPAD_BACKEND_GPIO` |
| `KEYPAD_ROWS`, `KEYPAD_COLS` | e.g. 4 and 4, or 4 and 3 |
| `KEYPAD_KEYMAP` | the key labels row by row, in quotes inside quotes: `'"123A456B789C*0#D"'` |
| `KEYPAD_I2C_ADDR` | PCF8574 address, `"0x20"`..`"0x27"` (PCF8574A: `0x38`..`0x3F`) |
| `KEYPAD_COL_INPUT_MODE` | `"INPUT"` when the columns have external pull-ups |
| `MAX7219_HW_TYPE` | `"MD_MAX72XX::FC16_HW"`, `GENERIC_HW`, `PAROLA_HW`, `ICSTATION_HW` |
| `MAX7219_DEVICES` | number of chained 8x8 modules |
| `MATRIX_WIDTH`, `MATRIX_HEIGHT` | in pixels |
| `MATRIX_FLIP_X`, `MATRIX_FLIP_Y` | `1` if the picture is mirrored |
| `WS2812_COLOR_ORDER` | `"NEO_GRB"` (most), `"NEO_RGB"`, ... |
| `WS2812_LAYOUT` | NeoMatrix flags, e.g. `"(NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG)"` |
| `WS2812_MAX_BRIGHTNESS` | 0..255 cap that protects the power supply |

To add a new add-on (for example a TM1637 display), add a new `[[option]]` block with its pins and settings.
The checker validates its pins against all other combinations of menu choices.

### 3.4 A new hardware revision (never break old student projects)

When the PCB changes, **do not edit the existing profile**. Create a new board:

```
python3 tools/vtlabs_board.py new --from vtlabs_iot_esp32_r1 --id vtlabs_iot_esp32_r2 --name "VTLabs IoT Training Kit ESP32 (Rev 2)" --hw-rev 2
```

Edit `boards/vtlabs_iot_esp32_r2.toml`, run `generate`. Both revisions now appear in
**Tools > Board**, and every lesson works on both.

### 3.5 A completely custom board (your own breadboard or another PCB)

1. Copy `boards/templates/custom_devkit_keypad_gpio.toml` to `boards/<your_id>.toml`.
2. Set `[board] id` and `name`. Keep every curriculum name in `[pins]`; use `gpio = -1` for parts you do not have.
3. `python3 tools/vtlabs_board.py generate`, fix any ERROR.
4. Restart the IDE (developer install) or release (section 6).

The template shows a 4x4 keypad wired straight to 8 GPIOs (no expander) and a MAX7219 option.

### 3.6 Check, list, regenerate

```
python3 tools/vtlabs_board.py check               # only check
python3 tools/vtlabs_board.py list                # print all boards and pins
python3 tools/vtlabs_board.py generate            # check + write files
python3 tools/vtlabs_board.py generate --check    # CI: fail if someone forgot to generate
```

Never edit `boards.txt`, `pins_arduino.h`, `vtlabs_board.h` or `docs/pinmaps/*` by hand — they are
overwritten. Their first line says so.

---

## 4. For students: your own pins without changing the package

### 4.1 Override a kit pin in one sketch

```cpp
#define VTL_PIN_LED_RED 13      // my red LED is on GPIO13 on the breadboard
#include <VTLabsKit.h>
```

Only this sketch is affected. It works because VTLabsKit is header-only.

### 4.2 Add pins for your own parts

Simply name them in your sketch, or in a tab:

```cpp
// my_pins.h  (Sketch > New Tab)
#pragma once
#define MY_SERVO_PIN 12
#define MY_SOIL_PIN  32
```

```cpp
#include "my_pins.h"
#include <VTLabsKit.h>
```

Check the free GPIOs in `docs/pinmaps/vtlabs_iot_esp32_r1.md` (bottom of the table) before choosing a pin.

### 4.3 Change keypad/matrix settings for one sketch

```cpp
#define VTL_KEYPAD_KEYMAP "147*2580369#ABCD"   // keypad mounted rotated
#define VTL_WS2812_MAX_BRIGHTNESS 15            // weak power supply
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>
```

---

## 5. Testing locally (developer install)

1. Close the Arduino IDE.
2. Make the platform visible in your sketchbook:
   - **Windows** (PowerShell, as normal user, Developer Mode on — or just copy the folder):
     `New-Item -ItemType SymbolicLink -Path "$HOME\Documents\Arduino\hardware\vtlabs" -Target "C:\path\to\vtlabs-arduino\hardware\vtlabs"`
   - **Linux/macOS**: `ln -s "$PWD/hardware/vtlabs" ~/Arduino/hardware/vtlabs` (macOS: `~/Documents/Arduino/hardware/`)
   A link means every `generate` is live after an IDE restart. If you copied instead, copy again after each change.
3. If the VTLabs package is also installed through Boards Manager, **uninstall it** there (two copies confuse the IDE).
4. Install the libraries once: Tools > Manage Libraries (list in [libraries.md](libraries.md)), or
   `python3 scripts/install_libraries.py`.
5. Start the IDE, select the board, open File > Examples > VTLabsKit.

Compile everything automatically (needs Arduino CLI):

```
python3 scripts/compile_examples.py --report docs/compile-report.md
```

It compiles each lesson with every Tools-menu combination it supports (see the `@requires:` tag
in each sketch), and checks that add-on lessons fail with the friendly message when the add-on is
"Not fitted". Compiling is not hardware testing: use
[hardware-validation-checklist.md](hardware-validation-checklist.md) on a real kit.

---

## 6. Deploying to students (Boards Manager)

The full GitHub setup is in [03-online-deployment.md](03-online-deployment.md). The routine is:

### 6.1 Prepare a release

```
python3 tools/vtlabs_board.py generate --check     # generated files up to date?
python3 tools/make_catalogue.py                    # refresh docs/CURRICULUM.md
python3 scripts/compile_examples.py                # everything compiles?
```

Then:

1. Bump `version=` in `hardware/vtlabs/esp32/platform.txt` (and `version=` in
   `libraries/VTLabsKit/library.properties` if the library changed).
2. Add a `## [X.Y.Z] - date` section to `CHANGELOG.md`.
3. Commit and push. Wait for CI to be green.

### 6.2 Publish

```
git tag vX.Y.Z
git push origin vX.Y.Z
```

GitHub Actions builds the archive, creates the release, adds it to
`package_vtlabs_index.json` (with the library list from `libraries.txt`), and verifies the download.

### 6.3 What students do (once)

1. Arduino IDE 2.3.3+ (2.4+ also installs the libraries automatically).
2. File > Preferences > Additional boards manager URLs — add both:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   https://<owner>.github.io/<repo>/package_vtlabs_index.json
   ```
3. Boards Manager: install **esp32 by Espressif Systems 3.3.x**, then **VTLabs Training Kits**.
4. On IDE older than 2.4: install the libraries from [libraries.md](libraries.md).
5. Tools > Board > VTLabs Training Kits > the kit. Tools > Keypad / LED Matrix > what is fitted.

Updates: Boards Manager shows **Update** when a new version is published.

### 6.4 Versions and rollback

| Change | Bump |
|---|---|
| Fix, new example, new menu entry | PATCH 0.2.0 -> 0.2.1 |
| New board/revision, new library feature | MINOR 0.2.1 -> 0.3.0 |
| Removed board, renamed FQBN, changed an existing pin | MAJOR - avoid: add a new revision instead |

A bad release is never edited. Publish a higher version with the fix, and optionally mark the bad one
`deprecated` (see 03-online-deployment.md §6). Students can always pick an older version in Boards Manager.

### 6.5 Hosting without GitHub Pages

Build the archive and index yourself and upload both to any HTTPS web server (e.g. your company site):

```
tools/release/build-archive.sh 0.2.0
python3 tools/release/make_index.py add --index package_vtlabs_index.json \
   --archive dist/vtlabs-esp32-0.2.0.zip --version 0.2.0 \
   --boards-txt hardware/vtlabs/esp32/boards.txt --url-base https://example.com/arduino/
```

Upload `dist/vtlabs-esp32-0.2.0.zip` and `package_vtlabs_index.json` to `https://example.com/arduino/`.
Keep old zip files on the server: the index still lists them.

---

## 7. Adding a different microcontroller family later

| Target | What to do |
|---|---|
| Another classic-ESP32 board | New profile (section 3.4/3.5). Nothing else. |
| ESP32-S3 / C3 / C6 | Add the family to `FAMILIES` in `tools/vtlabs_board.py` (valid GPIOs, flash pins, strapping pins, ADC pins, build settings from Espressif's `boards.txt`), then profiles with `family = "esp32s3"`. Same platform (`vtlabs:esp32`), same core. Check every lesson's peripheral use (no DAC on S3/C3, different ADC, no classic Bluetooth). |
| AVR (e.g. ATmega2560 trainer) | A **separate platform** `vtlabs:avr` with `build.core=arduino:arduino` (Arduino AVR core) and its own variant, plus an AVR implementation of the VTLabsKit headers. Only lessons that need no Wi-Fi apply. |
| STM32 | A separate platform `vtlabs:stm32` whose boards reference the official STM32duino core (`build.core=STMicroelectronics:arduino`; verify the core folder name in the installed STM32 package first). |

Each family keeps its own board definition and variant; the lessons are shared only where the hardware
and APIs truly match. Do not promise one binary or one example runs everywhere.
