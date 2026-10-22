/*
  VTLabs IoT Training Kit - L3_08 SPI Peripheral: micro-SD Card (OPTIONAL MODULE)
  ------------------------------------------------------------------
  Purpose:     Use the SPI bus (SCK, MOSI, MISO + one chip-select per device)
               to write and read a file.
  Hardware:    OPTIONAL micro-SD module on the SPI header:
               SCK->VTL_PIN_SPI_SCK, MOSI->VTL_PIN_SPI_MOSI, MISO->VTL_PIN_SPI_MISO,
               CS->VTL_PIN_SD_CS, VCC->3V3 (or 5V if the module has a regulator), GND.
               Card formatted FAT32.
  Libraries:   none (SD and SPI are part of the ESP32 core)
  Expected:    Prints card type and size, appends a line to /vtlabs.txt each
               time BTN1 is pressed, and prints the file.
  Experiments: 1. Log the POT value with a timestamp every 10 s.
               2. Share the bus with the MAX7219 matrix (different CS pin) -
                  why can both devices use the same SCK/MOSI wires?
  Troubleshooting:
    - "Card mount failed": check CS pin, FAT32 format, 3.3 V logic levels.
  @requires: none
*/
#include <VTLabsKit.h>
#include <SPI.h>
#include <SD.h>

VTLButton btn;
bool cardOk = false;

void printFile() {
  File f = SD.open("/vtlabs.txt");
  if (!f) { Serial.println("(no file yet)"); return; }
  Serial.println("---- /vtlabs.txt ----");
  while (f.available()) Serial.write(f.read());
  f.close();
}

void setup() {
  Serial.begin(115200);
  btn.begin(VTL_PIN_BTN1, VTL_PIN_BTN1_ACTIVE_LOW, VTL_PIN_BTN1_MODE);
  SPI.begin(VTL_PIN_SPI_SCK, VTL_PIN_SPI_MISO, VTL_PIN_SPI_MOSI, VTL_PIN_SD_CS);
  cardOk = SD.begin(VTL_PIN_SD_CS, SPI);
  if (!cardOk) { Serial.println("Card mount failed - optional SD module fitted?"); return; }
  Serial.printf("Card type %d, size %llu MB\n", SD.cardType(), SD.cardSize() / (1024 * 1024));
  printFile();
}

void loop() {
  btn.update();
  if (cardOk && btn.pressed()) {
    File f = SD.open("/vtlabs.txt", FILE_APPEND);
    if (f) {
      f.printf("t=%lus pot=%d\n", millis() / 1000, analogRead(VTL_PIN_POT));
      f.close();
      printFile();
    }
  }
}
