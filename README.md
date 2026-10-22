# VTLabs Arduino Board Support

Arduino **Boards Manager** package for VTLabs training kits, with a 64-lesson curriculum.
Boards reuse the official Espressif ESP32 core; this repository adds board definitions generated from
editable hardware profiles, the VTLabsKit library (buttons, matrix keypad, LED matrix), lessons and docs.

| Component | Version |
|---|---|
| VTLabs platform (`vtlabs:esp32`) | 0.2.0 (unreleased) |
| VTLabsKit library (bundled) | 0.2.0 |
| Espressif core (`esp32:esp32`) | 3.3.x required, 3.3.12 tested |
| Arduino IDE / CLI | IDE 2.3.3+ (2.4+ installs libraries automatically) / CLI 1.0.4+ |

**Boards:** `vtlabs:esp32:vtlabs_iot_esp32_r1` - VTLabs IoT Training Kit ESP32 (Rev 1),
with Tools menus **Keypad (add-on)** and **LED Matrix (add-on)** (MAX7219 32x8 / 8x8, WS2812 8x8).

## Where to start

| You are | Read |
|---|---|
| Student / teacher | [docs/CURRICULUM.md](docs/CURRICULUM.md), [docs/learning-outcomes.md](docs/learning-outcomes.md), pin table [docs/pinmaps/](docs/pinmaps/vtlabs_iot_esp32_r1.md) |
| Changing pins, adding pins or boards, releasing | [docs/04-customise-and-deploy.md](docs/04-customise-and-deploy.md) |
| Keypad / LED matrix hardware | [docs/05-addons-keypad-and-led-matrix.md](docs/05-addons-keypad-and-led-matrix.md) |
| Testing a physical kit | [docs/hardware-validation-checklist.md](docs/hardware-validation-checklist.md) |
| Libraries | [docs/libraries.md](docs/libraries.md) |
| GitHub release setup | [docs/03-online-deployment.md](docs/03-online-deployment.md) |
| Background research | [docs/01-stage1-dependency-assessment.md](docs/01-stage1-dependency-assessment.md), [docs/02-stage2-board-definition.md](docs/02-stage2-board-definition.md) |

## Repository layout

```
boards/*.toml                      hardware profiles - the ONLY place pins are defined
boards/templates/                  start here for a custom board
tools/vtlabs_board.py              check + generate boards.txt, variants, pin docs
tools/make_catalogue.py            docs/CURRICULUM.md from the lesson headers
tools/release/                     archive + Boards Manager index builders
hardware/vtlabs/esp32/             the platform (what students install)
  boards.txt, variants/            generated
  libraries/VTLabsKit/src          VTLabsKit.h, VTLabsKeypad.h, VTLabsMatrix.h
  libraries/VTLabsKit/examples     L1..L9 lessons (File > Examples > VTLabsKit)
scripts/compile_examples.py        compile every lesson for every Tools-menu combination
scripts/install_libraries.py       install libraries.txt with arduino-cli
tests/                             test sketches + pin-checker unit tests
.github/workflows/                 CI and tag-triggered release
```

## Common commands

```
python3 tools/vtlabs_board.py generate        # after editing boards/*.toml
python3 tools/make_catalogue.py               # after editing lesson headers
python3 scripts/compile_examples.py           # compile everything (needs arduino-cli)
python3 -m unittest discover -s tests         # pin-checker tests
git tag v0.2.0 && git push origin v0.2.0      # publish (see docs/03)
```

Student install URL (after the first release and GitHub Pages):
`https://<owner>.github.io/<repo>/package_vtlabs_index.json`
