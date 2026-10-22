/*
  VTLabs IoT Training Kit - L6_06 Menu Navigation on the OLED
  ------------------------------------------------------------------
  Purpose:     Build a menu system: a list, a cursor, enter and back.
               Keys: A = up, B = down, # = select, * = back, 1-6 = jump to item.
  Hardware:    Matrix keypad add-on, OLED, LEDs, DHT22, LDR, POT, BUZZER.
  Libraries:   "Adafruit SSD1306" + "Adafruit GFX Library" + "Adafruit BusIO",
               "DHT sensor library" + "Adafruit Unified Sensor"
  Expected:    Main menu with 6 items. Selecting:
                 Temperature / Light / Potentiometer -> live value page
                 Red LED / Green LED -> toggles the LED (state shown in the menu)
                 Beep -> plays a tone
               '*' returns to the menu.
  Experiments: 1. Add a "Settings" sub-menu (see L6_07).
               2. Wrap around: pressing up on the first item jumps to the last.
               3. Show only 4 items at a time and scroll the list.
  Troubleshooting:
    - Blank screen: run L3_07 first.
  @requires: keypad
*/
#include <VTLabsKit.h>
#include <VTLabsKeypad.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

Adafruit_SSD1306 oled(VTL_OLED_WIDTH, VTL_OLED_HEIGHT, &Wire, -1);
DHT dht(VTL_PIN_DHT, VTL_DHT_TYPE);
VTLKeypad keypad;

const char *items[] = {"Temperature", "Light", "Potentiometer", "Red LED", "Green LED", "Beep"};
const int N = sizeof(items) / sizeof(items[0]);
int cursor = 0, openItem = -1;  // openItem = item whose page is shown, -1 = menu
bool red = false, green = false;

void drawMenu() {
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.println("MENU   A/B move  # ok");
  for (int i = 0; i < N; i++) {
    oled.print(i == cursor ? "> " : "  ");
    oled.print(items[i]);
    if (i == 3) oled.print(red ? " [ON]" : " [off]");
    if (i == 4) oled.print(green ? " [ON]" : " [off]");
    oled.println();
  }
  oled.display();
}

void drawPage() {
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.println(items[openItem]);
  oled.println("--------------  * back");
  oled.setTextSize(2);
  oled.setCursor(0, 28);
  if (openItem == 0) { float t = dht.readTemperature(); isnan(t) ? oled.print("--") : oled.printf("%.1f C", t); }
  if (openItem == 1) oled.printf("%d %%", (int)map(analogRead(VTL_PIN_LDR), 0, 4095, 0, 100));
  if (openItem == 2) oled.printf("%d", analogRead(VTL_PIN_POT));
  oled.setTextSize(1);
  oled.display();
}

void selectItem(int i) {
  if (i == 3) { red = !red; digitalWrite(VTL_PIN_LED_RED, red); }
  else if (i == 4) { green = !green; digitalWrite(VTL_PIN_LED_GREEN, green); }
  else if (i == 5) tone(VTL_PIN_BUZZER, 1800, 200);
  else openItem = i;  // value pages
}

void setup() {
  Wire.begin(VTL_PIN_I2C_SDA, VTL_PIN_I2C_SCL);
  oled.begin(SSD1306_SWITCHCAPVCC, VTL_I2C_OLED);
  oled.setTextColor(SSD1306_WHITE);
  dht.begin();
  keypad.begin();
  pinMode(VTL_PIN_LED_RED, OUTPUT);
  pinMode(VTL_PIN_LED_GREEN, OUTPUT);
  drawMenu();
}

void loop() {
  char k = keypad.getKey();
  if (k) {
    if (openItem >= 0) { if (k == '*') openItem = -1; }
    else if (k == 'A') cursor = max(0, cursor - 1);
    else if (k == 'B') cursor = min(N - 1, cursor + 1);
    else if (k == '#') selectItem(cursor);
    else if (k >= '1' && k <= '0' + N) { cursor = k - '1'; selectItem(cursor); }
    if (openItem < 0) drawMenu();
  }
  static unsigned long last = 0;
  if (openItem >= 0 && millis() - last > 2500) { last = millis(); drawPage(); }  // DHT needs 2 s
  if (openItem >= 0 && k) drawPage();
}
