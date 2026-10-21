# Stage 2 & 3 — Minimal VTLabs Board Definition, Compile and Upload

Date: 2026-10-21
Status: **Stage 2 done and verified by compilation. Stage 3 (upload to real hardware) is waiting for you — follow Part B.**

---

## 1. What was built

```
vtlabs-arduino/
├── hardware/vtlabs/esp32/                 <- the VTLabs platform (vendor "vtlabs", arch "esp32")
│   ├── platform.txt                       <- name, version, one required override
│   ├── boards.txt                         <- "VTLabs IoT Training Kit ESP32 (Rev 1)"
│   └── variants/vtlabs_iot_esp32_r1/
│       └── pins_arduino.h                 <- standard pin names + core-version guard
├── examples/L1_Digital/L1_01_Blink/       <- first curriculum example
├── tests/sketches/                        <- BoardMacrosCheck, WifiWebServerCheck
├── scripts/compile-check.sh               <- compiles everything for every FQBN
└── docs/
```

**FQBN:** `vtlabs:esp32:vtlabs_iot_esp32_r1`

### Design decisions
| Decision | Reason |
|---|---|
| `build.core=esp32:esp32` | Uses the official Espressif core; nothing copied or modified |
| Hardware revision in the board ID (`_r1`) | A future Rev 2 gets a new ID; Rev 1 student projects never break |
| Flash size, flash mode, PSRAM fixed (no menu) | Students cannot pick settings the hardware doesn't have |
| Only 5 menus kept | Upload speed, CPU freq, partition scheme, debug level, erase flash |
| `-DVTLABS_BOARD_IOT_ESP32 -DVTLABS_HW_REV=1` plus core's `ARDUINO_VTLABS_IOT_ESP32_R1` | Libraries/examples can detect the board and revision |
| Core version guard in `pins_arduino.h` | Boards Manager cannot declare a dependency on `esp32:esp32`; this gives a clear error instead of confusing failures |
| `runtime.use_core_platform_path_for_runtime_platform_path=true` | Without it the build fails (see §2, test T1) |

### Assumptions still open (must be confirmed on the real board)
| ID | Assumption | Where it's used | How to confirm |
|---|---|---|---|
| A1 | Classic ESP32 (ESP32-WROOM-32 family) | `build.mcu/target/tarch`, bootloader 0x1000 | Part B step 6 (`esptool chip-id`) |
| A2 | 4 MB flash | `build.flash_size`, partition menu | Part B step 6 (`esptool flash-id`) |
| A3 | No PSRAM | no PSRAM menu / define | Part B step 6 (chip features line) |
| A4 | USB-UART bridge with auto-reset | upload `default-reset` | Upload works without pressing BOOT |
| A5 | On-board LED on GPIO2 | `LED_BUILTIN` | Blink lights the LED |

If A1–A3 turn out different (e.g. WROVER with PSRAM, 8/16 MB flash, or ESP32-S3), only `boards.txt` changes — tell me what `esptool` reports.

---

## 2. Verification performed (sandbox, 2026-10-21)

Environment: Linux x86-64, **Arduino CLI 1.5.0**, **esp32:esp32 3.3.12** (official release archive `esp32-core-3.3.12.zip`, with its `esp-x32`, `esptool_py 5.3.1` and `esp32-libs` tools).
Sandbox caveat: `downloads.arduino.cc` was unreachable, so the `ctags` tool was built from Arduino's own fork (github.com/arduino/ctags) and unused tool dependencies (RISC-V chips, debugger, dfu-util) were not installed. Neither affects the ESP32 build path.

| Test | Command (abbreviated) | Result |
|---|---|---|
| T0 Baseline Espressif board | `compile --fqbn esp32:esp32:esp32 Blink` | PASS (257,628 B) |
| T1 VTLabs **without** override | `compile --fqbn vtlabs:esp32:vtlabs_iot_esp32_r1 Blink` | **FAIL as predicted**: `can't open .../hardware/vtlabs/esp32/tools/gen_esp32part.py` |
| T2 VTLabs **with** override | same | PASS (257,644 B), produces `.bin`, `.bootloader.bin`, `.partitions.bin`, `.merged.bin` |
| T3 Board listed | `board listall vtlabs` | `VTLabs IoT Training Kit ESP32 (Rev 1)` |
| T4 Board macros + Wire + SPI | `tests/sketches/BoardMacrosCheck` | PASS |
| T5 WiFi + WebServer, `huge_app`, 80 MHz | `tests/sketches/WifiWebServerCheck` | PASS (max 3,145,728 B shown, so the menu applies) |
| T6 Core version guard | temporarily required ≥3.4.0 | PASS: build stops with the VTLabs `#error` message |
| T7 Upload command | `upload --dry-run -p /dev/ttyUSB0` | Resolves to the **core's** `flasher.py`, `esptool 5.3.1`, `boot_app0.bin`; offsets 0x1000/0x8000/0xe000/0x10000 |
| T8 Full matrix | `scripts/compile-check.sh` | 6/6 PASS |

**Not yet tested:** real upload, Windows/macOS hosts, Arduino IDE GUI. These are Part B.

### Compatibility (verified)
- **Arduino IDE 2.3.3 or newer** (2.3.3 bundles Arduino CLI 1.0.4, the first version supporting the override).
- Arduino CLI 1.0.4 or newer.
- Arduino IDE 1.8.x: **not supported**.
- Espressif core: 3.3.x (enforced by the guard).

---

## 3. Part B — your steps on a real PC and board

Do these on the computer you use with the trainer. Windows paths are shown; macOS/Linux equivalents are in brackets.

### Step 1 — Arduino IDE
Install or update to **Arduino IDE 2.3.3 or newer** from arduino.cc/en/software. Check with *Help > About*.

### Step 2 — Install the Espressif ESP32 platform
1. *File > Preferences > Additional boards manager URLs*: add
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
   (if it fails to load, use `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`, which was verified working here).
2. *Tools > Board > Boards Manager*, search **esp32**, install **"esp32 by Espressif Systems" version 3.3.12** (any 3.3.x passes the guard; 3.3.12 is the tested one).

### Step 3 — Install the VTLabs platform (developer install, before we have a Boards Manager release)
1. Find your sketchbook folder: *File > Preferences > Sketchbook location*. Default: `C:\Users\<you>\Documents\Arduino` [`~/Documents/Arduino` or `~/Arduino`].
2. Create the folder `hardware` inside it if it doesn't exist.
3. Copy the repo folder `hardware/vtlabs` there, so you end up with:
   ```
   Documents\Arduino\hardware\vtlabs\esp32\boards.txt
   Documents\Arduino\hardware\vtlabs\esp32\platform.txt
   Documents\Arduino\hardware\vtlabs\esp32\variants\vtlabs_iot_esp32_r1\pins_arduino.h
   ```
   Wrong depth is the #1 mistake: `boards.txt` must be exactly two folders below `hardware`.
4. **Restart the IDE.**

### Step 4 — Select the board
*Tools > Board > VTLabs Training Kits > VTLabs IoT Training Kit ESP32 (Rev 1)*.
Under *Tools* you should see exactly five options: Upload Speed, CPU Frequency, Partition Scheme, Core Debug Level, Erase All Flash.

### Step 5 — Connect the board and pick the port
1. Plug in the USB cable (must be a data cable, not charge-only).
2. If no new port appears, install the driver for the bridge chip printed on the dev module: CP2102 → Silicon Labs CP210x driver; CH340/CH9102 → WCH driver.
3. *Tools > Port* → choose the new COM port.

### Step 6 — Identify the hardware (answers A1–A3)
In the IDE, open the bundled esptool via a terminal [replace COM5 with your port]:

Windows (PowerShell):
```
& "$env:LOCALAPPDATA\Arduino15\packages\esp32\tools\esptool_py\5.3.1\esptool.exe" --port COM5 chip-id
& "$env:LOCALAPPDATA\Arduino15\packages\esp32\tools\esptool_py\5.3.1\esptool.exe" --port COM5 flash-id
```
macOS: `~/Library/Arduino15/packages/esp32/tools/esptool_py/5.3.1/esptool --port /dev/cu.usbserial-XXXX chip-id`
Linux: `~/.arduino15/packages/esp32/tools/esptool_py/5.3.1/esptool --port /dev/ttyUSB0 chip-id`

If the exact folder name differs, look inside `...\packages\esp32\tools\esptool_py\` for the installed version. Copy the full output of both commands — especially the lines **Chip type**, **Features**, **Crystal**, and **Detected flash size**.

### Step 7 — Compile and upload Blink (Stage 3)
1. Open `examples/L1_Digital/L1_01_Blink/L1_01_Blink.ino`.
2. Click **Verify**. Expected: "Sketch uses ~257 KB (19%)".
3. Click **Upload**. Expected end of log: `Hard resetting via RTS pin...`
4. Expected on the board: LED blinks at 1 Hz. *Tools > Serial Monitor* at 115200: `LED ON` / `LED OFF`.

### Step 8 — Report back
Send me:
- esptool output from Step 6
- did upload succeed **without** pressing BOOT? (A4)
- did the LED blink? which one (colour / label near it)? (A5)
- IDE version and OS
- the schematic or a GPIO list for the trainer, for Stage 4 (pin mapping)

---

## 4. Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| VTLabs board missing from menu | Wrong folder depth, or IDE not restarted | Check Step 3 layout; restart IDE |
| `can't open ... hardware/vtlabs/esp32/tools/gen_esp32part.py` | IDE older than 2.3.3 / CLI older than 1.0.4, or `platform.txt` missing the override line | Update IDE; check `platform.txt` |
| `#error "VTLabs IoT Training Kit needs ... 3.3.x"` | Wrong Espressif core version | Boards Manager → esp32 → install 3.3.12 |
| `esp32:esp32` platform not installed / core not found | Step 2 skipped | Install Espressif platform first |
| No COM port | Charge-only cable or missing driver | Change cable; install CP210x or CH34x driver |
| `Failed to connect to ESP32: No serial data received` / stuck at `Connecting....` | Auto-reset not working | Hold **BOOT**, tap **EN/RST**, release BOOT, upload; try Upload Speed 115200 |
| Upload OK, LED never blinks | LED not on GPIO2 on this board | Report it (A5); Stage 4 fixes the pin map |
| Serial Monitor shows garbage | Baud mismatch | Set 115200 |

---

## 5. Next stage

**Stage 4 — Pin mapping** starts when the schematic arrives: a single `vtlabs_board.h` hardware description (LEDs, buttons, relay, buzzer, display, sensors, reserved pins), plus an automated check for GPIO conflicts and for pins that must not be used (flash pins 6–11, strapping pins 0/2/5/12/15, input-only 34–39).
