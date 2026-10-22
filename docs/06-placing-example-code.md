# Where to Place Example Code (Repository Structure)

A release tag runs CI first. CI fails if an example is in the wrong folder, is named wrongly,
has no header tags, or the curriculum list was not regenerated. Follow these rules exactly.

---

## 1. The only correct place

```
vtlabs-arduino/                                   <- repository root
└── hardware/
    └── vtlabs/
        └── esp32/
            └── libraries/
                └── VTLabsKit/
                    ├── library.properties
                    ├── src/                        <- library code (.h) - NOT examples
                    └── examples/                   <- ALL lessons go here
                        ├── L1_Digital/             <- level folder
                        │   ├── L1_01_Blink/        <- sketch folder
                        │   │   └── L1_01_Blink.ino <- same name as its folder
                        │   └── L1_02_ButtonRead/
                        │       └── L1_02_ButtonRead.ino
                        ├── L4_IoT/
                        │   └── L4_01_WiFiConnect/
                        │       ├── L4_01_WiFiConnect.ino
                        │       └── arduino_secrets.h   <- extra tabs stay in the same folder
                        └── L10_MyNewLevel/          <- a new level: same pattern
                            └── L10_01_MyLesson/
                                └── L10_01_MyLesson.ino
```

Always exactly **three** levels below `examples/`: level folder → sketch folder → `.ino` file.

Test sketches (not lessons) go in `tests/sketches/<Name>/<Name>.ino` with the same rules.

---

## 2. Common mistakes that break the tag

| Wrong | Why it fails | Right |
|---|---|---|
| `vtlabs-arduino/examples/L1_01_Blink/...` (repo root) | Not inside the board package; not compiled, not shipped, catalogue misses it | `hardware/vtlabs/esp32/libraries/VTLabsKit/examples/L1_Digital/L1_01_Blink/` |
| `examples/L1_01_Blink/L1_01_Blink.ino` (no level folder) | Catalogue expects level → sketch | `examples/L1_Digital/L1_01_Blink/L1_01_Blink.ino` |
| `examples/L1_Digital/L1_01_Blink.ino` (no sketch folder) | Arduino requires every sketch in its own folder | add the folder `L1_01_Blink/` |
| `L1_01_Blink/blink.ino` or `Blink.ino` | File name must equal folder name exactly (case too); the compiler skips it | `L1_01_Blink/L1_01_Blink.ino` |
| Spaces or dashes: `L1 01 Blink`, `L1-01-Blink` | Arduino sketch names: letters, digits, `_` only | `L1_01_Blink` |
| Two `.ino` files with `setup()` in one folder | Arduino merges all `.ino` tabs → "redefinition of setup" | one main `.ino`; helpers as `.h` |
| Sketch in `src/` | `src/` is compiled into every sketch → duplicate `setup()` | only `.h` library code in `src/` |
| Header without `@requires:` | CI reports `missing @requires tag` and fails | add the tag (section 3) |
| Title line missing or different | Catalogue tool cannot read the sketch | `VTLabs IoT Training Kit - L1_01 Blink` |
| Forgot `python3 tools/make_catalogue.py` | `catalogue ... out of date` | run it, commit `docs/CURRICULUM.md` |
| A GPIO number in the code (`pinMode(25, OUTPUT)`) | Breaks on other revisions (not a CI error, but a rule) | `pinMode(VTL_PIN_LED_RED, OUTPUT)` |
| A library not in `libraries.txt` | CI does not install it → `No such file or directory` | add `Name@version` to `libraries.txt` |
| Real Wi-Fi password committed | Security | keep placeholders in `arduino_secrets.h` |

---

## 3. Required header (copy this into every new sketch)

```cpp
/*
  VTLabs IoT Training Kit - L10_01 My Lesson Title
  ------------------------------------------------------------------
  Purpose:     One sentence first (it goes into the curriculum table).
  Hardware:    Which kit parts / add-ons, and wiring.
  Libraries:   none   (or the exact Library Manager names)
  Expected:    What the student should see.
  Experiments: 1. ...  2. ...
  Troubleshooting:
    - ...
  @requires: none
*/
#include <VTLabsKit.h>

void setup() {
}

void loop() {
}
```

Rules for the header:
- First line inside the comment: `VTLabs IoT Training Kit - ` followed by the title.
- Field names start with **two spaces** and end with `:` (`  Purpose:`). Continuation lines are indented more.
- `@requires:` is mandatory. Allowed values:

| Value | Use when the sketch includes |
|---|---|
| `none` | only base kit parts |
| `keypad` | `<VTLabsKeypad.h>` |
| `matrix` | `<VTLabsMatrix.h>` (works on MAX7219 and WS2812) |
| `matrix-max7219` | MD_MAX72XX / MD_Parola directly |
| `matrix-ws2812` | Adafruit NeoPixel / NeoMatrix directly |
| `keypad, matrix` | both add-ons |

- Optional: `@optional: matrix` or `@optional: keypad` when the add-on is used inside `#if VTL_HAS_MATRIX` / `#if VTL_HAS_KEYPAD`.
- Test sketches that must fail on purpose: `@expect-error: <text of the #error>`.

---

## 4. Code rules that the compile matrix checks

1. Use pin names (`VTL_PIN_...`), never numbers. The list is in `docs/pinmaps/`.
2. Add-on code that is optional must be fully inside `#if VTL_HAS_MATRIX ... #endif`
   (including any use of `vtl_icons::` or `VTL_RED` - L9_03 broke this way).
3. Do not name variables/functions/enum values after common system names: `open`, `select`, `read`,
   `write`, `OK`, `ERROR`, `HIGH`, `LOW`, `PIN`... Prefix them (`ST_OK`, `selectItem`) - L9_07 broke this way.
4. Wi-Fi credentials only in `arduino_secrets.h` next to the `.ino`.

---

## 5. Checklist before every tag

Run from the repository root:

```
python3 tools/make_catalogue.py                     # updates docs/CURRICULUM.md
python3 tools/vtlabs_board.py generate --check      # board files up to date
python3 -m unittest discover -s tests               # pin checker tests
python3 scripts/compile_examples.py --only L10_     # compile only your new lessons (needs arduino-cli)
```

Then:

1. `git status` - the new sketch folder and `docs/CURRICULUM.md` must both be listed. Add them.
2. Bump `version=` in `hardware/vtlabs/esp32/platform.txt`, add a `CHANGELOG.md` section.
3. `git commit`, `git push`, wait for **CI green** on the commit.
4. Only then: `git tag vX.Y.Z` and `git push origin vX.Y.Z`.

If a tag failed: fix, commit, push, then delete and recreate the tag with a **new** version
(`git push --delete origin vX.Y.Z` only if no release was created; otherwise use the next patch version).

---

## 6. Where to look when CI fails

| CI step that is red | Usual cause | Fix |
|---|---|---|
| Curriculum catalogue up to date | new/changed sketch, catalogue not regenerated | `python3 tools/make_catalogue.py`, commit |
| Hardware profiles ... up to date | edited `boards.txt`/variant by hand, or edited `.toml` without generating | `python3 tools/vtlabs_board.py generate`, commit |
| Compile every example | wrong folder/name, missing `@requires`, missing library, code error | open the step log; lines starting `FAIL` or `PROBLEM` name the sketch |
| Release archive and index | tag version differs from `platform.txt` | make them equal |

The compile report is attached to every CI run as the artifact **compile-report**.
