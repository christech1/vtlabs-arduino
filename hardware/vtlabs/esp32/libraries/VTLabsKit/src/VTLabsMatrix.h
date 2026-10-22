/*
 * VTLabsMatrix - one small drawing API for different LED matrix modules.
 *
 * Hardware is selected in Arduino IDE: Tools > LED Matrix (add-on)
 *
 *   MAX7219 modules -> MD_MAX72XX + MD_Parola libraries (SPI: DIN=MOSI, CLK=SCK, CS=VTL_PIN_MAX7219_CS)
 *   WS2812 panels   -> Adafruit NeoPixel + Adafruit NeoMatrix + Adafruit GFX (one data pin VTL_PIN_WS2812_DIN)
 *
 * The two drivers work very differently (single-colour SPI LED driver vs. timed one-wire
 * RGB data stream). Lessons L7_* show each driver's own library; this API is for the
 * later lessons that should run on either module.
 *
 * Coordinates: (0,0) is top-left, x grows to the right, y grows downwards.
 * Drawing goes into a buffer; call show() to update the LEDs.
 */
#pragma once

#include "VTLabsKit.h"

#if !VTL_HAS_MATRIX
#error "This sketch needs an LED matrix. Select your module in Tools > LED Matrix (add-on). If your kit has no matrix, choose another lesson."
#else /* rest of file only when the add-on is selected */

#if VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_MAX7219
#include <SPI.h>
#include <MD_MAX72xx.h> /* library: MD_MAX72XX */
#include <MD_Parola.h>  /* library: MD_Parola   */
#elif VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_WS2812
#include <Adafruit_GFX.h>       /* library: Adafruit GFX Library */
#include <Adafruit_NeoMatrix.h> /* library: Adafruit NeoMatrix  */
#include <Adafruit_NeoPixel.h>  /* library: Adafruit NeoPixel   */
#else
#error "VTL_MATRIX_DRIVER must be VTL_MATRIX_DRIVER_MAX7219 or VTL_MATRIX_DRIVER_WS2812"
#endif

/* Colours as 0xRRGGBB. On single-colour (MAX7219) modules any non-zero colour means "on". */
#define VTL_RGB(r, g, b) ((uint32_t)(((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b)))
#define VTL_BLACK VTL_RGB(0, 0, 0)
#define VTL_WHITE VTL_RGB(255, 255, 255)
#define VTL_RED VTL_RGB(255, 0, 0)
#define VTL_GREEN VTL_RGB(0, 255, 0)
#define VTL_BLUE VTL_RGB(0, 0, 255)
#define VTL_YELLOW VTL_RGB(255, 180, 0)
#define VTL_ORANGE VTL_RGB(255, 80, 0)
#define VTL_CYAN VTL_RGB(0, 200, 255)

/* 8x8 icons, one byte per row, most significant bit = left pixel. */
namespace vtl_icons {
static const uint8_t HEART[8] = {0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18};
static const uint8_t SMILE[8] = {0x3C, 0x42, 0xA5, 0x81, 0xA5, 0x99, 0x42, 0x3C};
static const uint8_t CHECK[8] = {0x00, 0x01, 0x03, 0x06, 0x8C, 0xD8, 0x70, 0x20};
static const uint8_t CROSS[8] = {0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81};
static const uint8_t WARNING[8] = {0x18, 0x24, 0x24, 0x5A, 0x5A, 0x81, 0x99, 0xFF};
static const uint8_t LOCK[8] = {0x3C, 0x42, 0x42, 0xFF, 0xE7, 0xE7, 0xFF, 0xFF};
static const uint8_t UNLOCK[8] = {0x3C, 0x42, 0x40, 0xFF, 0xE7, 0xE7, 0xFF, 0xFF};
static const uint8_t WIFI[8] = {0x00, 0x3C, 0x42, 0x99, 0x24, 0x00, 0x18, 0x18};
static const uint8_t ARROW_UP[8] = {0x18, 0x3C, 0x7E, 0xFF, 0x18, 0x18, 0x18, 0x18};
static const uint8_t ARROW_DOWN[8] = {0x18, 0x18, 0x18, 0x18, 0xFF, 0x7E, 0x3C, 0x18};
static const uint8_t BELL[8] = {0x18, 0x3C, 0x3C, 0x3C, 0x7E, 0xFF, 0x00, 0x18};
static const uint8_t THERMO[8] = {0x08, 0x14, 0x14, 0x14, 0x1C, 0x3E, 0x3E, 0x1C};
static const uint8_t FULL[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
}  // namespace vtl_icons

/* Tiny 3x5 font so two digits fit on an 8x8 matrix. Each glyph: 5 rows of 3 bits (MSB left). */
inline const uint8_t *vtlGlyph3x5(char c) {
  static const char chars[] = "0123456789 -.:%+CEFHLAPKrnoBD#*";
  static const uint8_t g[][5] = {
    {7, 5, 5, 5, 7}, {2, 6, 2, 2, 7}, {7, 1, 7, 4, 7}, {7, 1, 7, 1, 7}, {5, 5, 7, 1, 1},
    {7, 4, 7, 1, 7}, {7, 4, 7, 5, 7}, {7, 1, 1, 2, 2}, {7, 5, 7, 5, 7}, {7, 5, 7, 1, 7},
    {0, 0, 0, 0, 0}, {0, 0, 7, 0, 0}, {0, 0, 0, 0, 2}, {0, 2, 0, 2, 0}, {5, 1, 2, 4, 5},
    {0, 2, 7, 2, 0}, {7, 4, 4, 4, 7}, {7, 4, 7, 4, 7}, {7, 4, 6, 4, 4}, {5, 5, 7, 5, 5},
    {4, 4, 4, 4, 7}, {2, 5, 7, 5, 5}, {6, 5, 6, 4, 4}, {5, 5, 6, 5, 5}, {0, 5, 6, 4, 4},
    {0, 0, 6, 5, 5}, {0, 0, 7, 5, 7}, {6, 5, 6, 5, 6}, {6, 5, 5, 5, 6}, {5, 7, 5, 7, 5},
    {0, 5, 2, 5, 0},
  };
  const char *p = strchr(chars, c);
  if (!p || c == 0) p = chars + 10; /* unknown -> blank */
  return g[p - chars];
}

class VTLMatrix {
public:
  static const int16_t W = VTL_MATRIX_WIDTH;
  static const int16_t H = VTL_MATRIX_HEIGHT;

#if VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_MAX7219
  VTLMatrix() : _p(VTL_MAX7219_HW_TYPE, SPI, VTL_PIN_MAX7219_CS, VTL_MAX7219_DEVICES) {}
#else
  VTLMatrix() : _m(VTL_MATRIX_WIDTH, VTL_MATRIX_HEIGHT, VTL_PIN_WS2812_DIN, VTL_WS2812_LAYOUT,
                   VTL_WS2812_COLOR_ORDER + NEO_KHZ800) {}
#endif

  bool begin() {
#if VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_MAX7219
    SPI.begin(VTL_PIN_SPI_SCK, VTL_PIN_SPI_MISO, VTL_PIN_SPI_MOSI);
    bool ok = _p.begin();
    _mx = _p.getGraphicObject();
    _p.displayClear();
    setBrightness(30);
    return ok;
#else
    _m.begin();
    _m.setTextWrap(false);
    setBrightness(30);
    _m.fillScreen(0);
    _m.show();
    return true;
#endif
  }

  int16_t width() const { return W; }
  int16_t height() const { return H; }
  bool isColour() const { return VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_WS2812; }
  const char *driverName() const { return VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_MAX7219 ? "MAX7219" : "WS2812"; }

  /* 0..100 %. WS2812 is additionally capped at VTL_WS2812_MAX_BRIGHTNESS (power budget). */
  void setBrightness(uint8_t percent) {
    if (percent > 100) percent = 100;
#if VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_MAX7219
    _p.setIntensity((uint8_t)((percent * MAX_INTENSITY + 50) / 100));
#else
    _m.setBrightness((uint8_t)((uint16_t)percent * VTL_WS2812_MAX_BRIGHTNESS / 100));
#endif
  }

  void setColor(uint32_t rgb) { _colour = rgb; }
  uint32_t color() const { return _colour; }

  void clear() {
    _scrolling = false;
#if VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_MAX7219
    _mx->update(MD_MAX72XX::OFF);
    _mx->clear();
#else
    _m.fillScreen(0);
#endif
  }

  void setPixel(int16_t x, int16_t y, bool on = true) { setPixelColor(x, y, on ? _colour : VTL_BLACK); }

  void setPixelColor(int16_t x, int16_t y, uint32_t rgb) {
    if (x < 0 || y < 0 || x >= W || y >= H) return;
    if (VTL_MATRIX_FLIP_X) x = W - 1 - x;
    if (VTL_MATRIX_FLIP_Y) y = H - 1 - y;
#if VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_MAX7219
    _mx->update(MD_MAX72XX::OFF);
    _mx->setPoint(y, W - 1 - x, rgb != 0); /* MD_MAX72XX numbers columns from the right */
#else
    _m.drawPixel(x, y, _m.Color((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF));
#endif
  }

  void fill(bool on = true) {
    for (int16_t y = 0; y < H; y++)
      for (int16_t x = 0; x < W; x++) setPixel(x, y, on);
  }

  /* Send the buffer to the LEDs. */
  void show() {
#if VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_MAX7219
    _mx->update();
#else
    _m.show();
#endif
  }

  /* Draw an 8x8 icon (vtl_icons::...) with its left edge at x. */
  void drawIcon(const uint8_t rows[8], int16_t x = 0, int16_t y = 0) {
    for (uint8_t r = 0; r < 8; r++)
      for (uint8_t c = 0; c < 8; c++)
        if (rows[r] & (0x80 >> c)) setPixel(x + c, y + r, true);
  }

  /* ---- small 3x5 text (numbers and a few letters) ---- */
  static int16_t smallTextWidth(const char *s) { int n = strlen(s); return n ? n * 4 - 1 : 0; }
  void drawSmallText(const char *s, int16_t x, int16_t y) {
    for (; *s; s++, x += 4) {
      const uint8_t *g = vtlGlyph3x5(*s);
      for (uint8_t r = 0; r < 5; r++)
        for (uint8_t c = 0; c < 3; c++)
          if (g[r] & (4 >> c)) setPixel(x + c, y + r, true);
    }
  }
  /* Clear, draw centred, show. Returns false (and draws nothing) if the text is too wide. */
  bool showSmallText(const char *s) {
    int16_t w = smallTextWidth(s);
    if (w > W) return false;
    clear();
    drawSmallText(s, (W - w) / 2, (H - 5) / 2);
    show();
    return true;
  }
  /* Shows a number statically if it fits, otherwise starts scrolling it. */
  bool showNumber(long n) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%ld", n);
    if (showSmallText(buf)) return true;
    scrollText(buf);
    return false;
  }

  /* Fill the matrix from the left in proportion to percent (0..100): a simple bar graph. */
  void drawLevel(uint8_t percent) {
    if (percent > 100) percent = 100;
    int16_t cols = (int16_t)((percent * W + 50) / 100);
    for (int16_t x = 0; x < cols; x++)
      for (int16_t y = 0; y < H; y++) setPixel(x, y, true);
  }

  /* ---- scrolling text (non-blocking): start with scrollText(), then call scrollUpdate()
          in loop(); it returns true when the text has scrolled completely out. ---- */
  void scrollText(const char *text, uint16_t stepMs = 60) {
    strncpy(_text, text, sizeof(_text) - 1);
    _text[sizeof(_text) - 1] = 0;
    _step = stepMs;
    _scrolling = true;
#if VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_MAX7219
    _p.displayClear();
    _p.displayText(_text, PA_LEFT, _step, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
#else
    _x = W;
    _last = 0;
#endif
  }
  bool scrollUpdate() {
    if (!_scrolling) return true;
#if VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_MAX7219
    if (_p.displayAnimate()) { _scrolling = false; return true; }
    return false;
#else
    if (millis() - _last < _step) return false;
    _last = millis();
    _m.fillScreen(0);
    _m.setCursor(_x, (H - 7) / 2);
    _m.setTextColor(_m.Color((_colour >> 16) & 0xFF, (_colour >> 8) & 0xFF, _colour & 0xFF));
    _m.print(_text);
    _m.show();
    if (--_x < -(int16_t)(strlen(_text) * 6)) { _scrolling = false; return true; }
    return false;
#endif
  }
  bool scrolling() const { return _scrolling; }

#if VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_MAX7219
  /* Direct access to the driver library for lesson-specific features. */
  MD_Parola &parola() { return _p; }
  MD_MAX72XX &max72xx() { return *_mx; }
#else
  Adafruit_NeoMatrix &neomatrix() { return _m; }
#endif

private:
#if VTL_MATRIX_DRIVER == VTL_MATRIX_DRIVER_MAX7219
  MD_Parola _p;
  MD_MAX72XX *_mx = nullptr;
#else
  Adafruit_NeoMatrix _m;
  int16_t _x = 0;
  uint32_t _last = 0;
#endif
  uint32_t _colour = VTL_GREEN;
  char _text[96] = {0};
  uint16_t _step = 60;
  bool _scrolling = false;
};

#endif /* VTL_HAS_MATRIX */
