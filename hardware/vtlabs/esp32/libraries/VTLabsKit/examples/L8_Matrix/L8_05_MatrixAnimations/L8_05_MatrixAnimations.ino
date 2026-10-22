/*
  VTLabs IoT Training Kit - L8_05 Animations (any matrix)
  ------------------------------------------------------------------
  Purpose:     An animation is a sequence of frames drawn at a steady rate.
               Three styles: frame list (beating heart), computed (rain),
               and state + rules (Conway's Game of Life).
  Hardware:    LED matrix add-on (Tools > LED Matrix (add-on)). BTN1 = next animation.
  Libraries:   as L8_01
  Expected:    Heart beats -> BTN1 -> rain falls -> BTN1 -> Game of Life evolves
               (restarts randomly when it dies out or every 30 s).
  Experiments: 1. Add a third heart frame for a smoother beat.
               2. Make the rain heavier when the POT is turned up.
               3. Start Life with a "glider" pattern and watch it travel.
  Troubleshooting:
    - Animation stutters: other code in loop() is blocking (delay()).
  @requires: matrix
*/
#include <VTLabsKit.h>
#include <VTLabsMatrix.h>

VTLMatrix matrix;
VTLButton btn;
const int W = VTLMatrix::W, H = VTLMatrix::H;
int mode = 0;
unsigned long lastFrame = 0, lifeStart = 0;
bool cells[H][W], next_[H][W];

const uint8_t HEART_SMALL[8] = {0x00, 0x00, 0x24, 0x7E, 0x7E, 0x3C, 0x18, 0x00};

void seedLife() {
  for (int y = 0; y < H; y++)
    for (int x = 0; x < W; x++) cells[y][x] = random(100) < 35;
  lifeStart = millis();
}

void stepLife() {
  int alive = 0;
  for (int y = 0; y < H; y++)
    for (int x = 0; x < W; x++) {
      int n = 0;
      for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++)
          if ((dx || dy) && cells[(y + dy + H) % H][(x + dx + W) % W]) n++;
      next_[y][x] = cells[y][x] ? (n == 2 || n == 3) : (n == 3);
      alive += next_[y][x];
    }
  memcpy(cells, next_, sizeof(cells));
  if (alive == 0 || millis() - lifeStart > 30000) seedLife();
}

void setup() {
  matrix.begin();
  btn.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);
  randomSeed(analogRead(VTL_PIN_LDR) ^ micros());
  seedLife();
}

void loop() {
  btn.update();
  if (btn.pressed()) { mode = (mode + 1) % 3; matrix.clear(); }
  static int frame = 0;
  const unsigned long FRAME_MS[3] = {300, 90, 200};
  if (millis() - lastFrame < FRAME_MS[mode]) return;
  lastFrame = millis();
  frame++;

  if (mode == 0) {  // frame list
    matrix.clear();
    matrix.setColor(VTL_RED);
    matrix.drawIcon(frame % 2 ? vtl_icons::HEART : HEART_SMALL, (W - 8) / 2);
  } else if (mode == 1) {  // computed: shift every column down, add new drops on top
    matrix.setColor(VTL_BLUE);
    static bool rain[H][W];
    for (int y = H - 1; y > 0; y--)
      for (int x = 0; x < W; x++) rain[y][x] = rain[y - 1][x];
    for (int x = 0; x < W; x++) rain[0][x] = random(100) < 15;
    matrix.clear();
    for (int y = 0; y < H; y++)
      for (int x = 0; x < W; x++)
        if (rain[y][x]) matrix.setPixel(x, y);
  } else {  // rules
    stepLife();
    matrix.clear();
    matrix.setColor(VTL_GREEN);
    for (int y = 0; y < H; y++)
      for (int x = 0; x < W; x++)
        if (cells[y][x]) matrix.setPixel(x, y);
  }
  matrix.show();
}
