# Changelog

##  0.2.0
### Added
- Hardware profiles (`boards/*.toml`) as the single source of pins, buses, I2C addresses and add-on modules;
  `tools/vtlabs_board.py` checks them (duplicates, flash pins, input-only, ADC, strapping, curriculum names)
  and generates `boards.txt`, variant headers and pin tables. Unit tests for the checker.
- Tools menus **Keypad (add-on)** (PCF8574 4x4) and **LED Matrix (add-on)** (MAX7219 32x8, MAX7219 8x8, WS2812 8x8).
- VTLabsKit library (header-only): VTLButton, VTLLineInput, VTLMovingAverage, VTLKeypad (GPIO and PCF8574,
  debounce, press/hold/release), VTLMatrix (MAX7219 and WS2812 backends, icons, 3x5 digits, scrolling).
- 64 lessons in 9 levels incl. 8 keypad lessons, 5 driver-specific and 7 portable LED-matrix lessons,
  7 integrated exercises. Generated catalogue `docs/CURRICULUM.md`.
- Custom-board template with a direct-GPIO keypad; `new` command for new hardware revisions.
- `libraries.txt` + `scripts/install_libraries.py`; package index now lists `libraryDependencies`.
- `scripts/compile_examples.py` (per-lesson Tools-menu combinations, expected-error tests, report).
- Docs: customise & deploy guide, add-on hardware guide, learning outcomes, hardware validation checklist, libraries.
### Changed
- Examples moved into the bundled library (File > Examples > VTLabsKit).
- Variant now sets Serial2 to GPIO16/17 explicitly (core 3.3.x default GPIO4/25 would hit the relay and red LED).
- Board macros: `VTLABS_BOARD`, `VTLABS_HW_REV` (replaces `VTLABS_BOARD_IOT_ESP32`).
### Fixed (found by the compile matrix)
- L9_03: matrix icon call compiled even with LED Matrix = Not fitted.
- L9_07: status value `OK` clashed with an ESP32 networking name; renamed to `ST_OK` etc.

## [0.1.0] - not released
- First board definition: VTLabs IoT Training Kit ESP32 (Rev 1), referencing esp32:esp32 3.3.x.
- Variant `vtlabs_iot_esp32_r1` with standard pin names and an Espressif core version guard.
- Boards Manager release pipeline: archive builder, index generator, GitHub Actions CI and tag-triggered release to GitHub Releases + gh-pages.
- Example L1_01_Blink; compile-check script; Stage 1 and Stage 2 docs.
