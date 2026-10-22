# VTLabsKit

Bundled with the **VTLabs Training Kits** board package. Header-only, so pin overrides made in a sketch
(`#define VTL_PIN_X n` before the include) apply consistently.

## Headers

| Header | Gives you | Needs |
|---|---|---|
| `<VTLabsKit.h>` | `VTL_PIN_*` names from the board profile, `VTL_ACTIVE_LEVEL(NAME)`, `vtlRelayWrite()`, `vtlPrintBoardInfo()`, `VTLButton`, `VTLLineInput`, `VTLMovingAverage<N>` | a VTLabs board selected |
| `<VTLabsKeypad.h>` | `VTLKeypad`, `VTLKeyEvent` (`VTL_KEY_PRESSED / HELD / RELEASED`) | Tools > Keypad (add-on) set |
| `<VTLabsMatrix.h>` | `VTLMatrix`, colours `VTL_RED`..., icons `vtl_icons::HEART`... | Tools > LED Matrix (add-on) set + driver libraries |

## Cheat sheet

```cpp
VTLButton b;   b.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);
               b.update();  b.pressed();  b.released();  b.isDown();  b.heldMs();

VTLKeypad k;   k.begin();   char c = k.getKey();            // 0 if no new key
               VTLKeyEvent e = k.update();  e.type, e.key;  k.scanRaw();

VTLLineInput in;  in.begin(6);  if (in.feed(c)) { in.value(); in.text(); in.masked(); }  // * = delete, # = enter

VTLMatrix m;   m.begin();  m.clear();  m.setColor(VTL_GREEN);  m.setPixel(x, y);  m.show();
               m.drawIcon(vtl_icons::CHECK);  m.showSmallText("42");  m.showNumber(1234);
               m.drawLevel(percent);  m.setBrightness(percent);
               m.scrollText("Hello");  /* in loop: */ m.scrollUpdate();
```

## Configuration macros (override before the include)

`VTL_KEYPAD_DEBOUNCE_MS` (30), `VTL_KEYPAD_HOLD_MS` (700), `VTL_KEYPAD_KEYMAP`, `VTL_KEYPAD_COL_INPUT_MODE`,
`VTL_WS2812_MAX_BRIGHTNESS`, `VTL_MATRIX_FLIP_X`, `VTL_MATRIX_FLIP_Y`, and every `VTL_PIN_*`.

Full documentation: `docs/` in the source repository.
