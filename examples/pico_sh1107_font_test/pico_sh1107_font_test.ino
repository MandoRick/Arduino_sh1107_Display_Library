#include "sh1107.h"

#define dispWidth 128
#define dispHeight 128
#define OLED_SPEED 999999ul

int countingUp;
int increment = 1;
float fontSize = 0.1;
float reverseFontSize = 11.9;
float fontIncrement = 0.1;
float reverseFontIncrement = 0.1;

SH1107 display;

void setup() {
  delay(250);
  display.begin();
  Wire.setClock(OLED_SPEED);
  delay(250);
  clearDisplay();
  delay(250);
}

void loop() {
  clearDisplay();
  display.drawString(0, 0, "Scale:", WHITE, 1.0);
  String fontScale1 = (String)fontSize;
  String fontScale2 = (String)reverseFontSize;
  display.drawString(50, 0, fontScale1.c_str(), WHITE, 1.0);
  display.drawString(95, 0, fontScale2.c_str(), WHITE, 1.0);
  display.drawString(10, 30, "H", WHITE, reverseFontSize);
  display.drawString(75, 30, "I", WHITE, fontSize);
  //for (int x = 0; x < 2; x++) {
  //  display.drawCircle(random(128), random(128), random(20), WHITE);
  //  display.drawLine(random(128), random(128), random(128), random(128), WHITE);
  //}
  //display.drawLine(0, countingUp, 127, countingUp, BLACK);
  //display.drawLine(0, countingUp + 1, 127, countingUp + 1, BLACK);
  display.display();
  countingUp++;
  fontSize = fontSize + fontIncrement;
  reverseFontSize = reverseFontSize + reverseFontIncrement;
  if (countingUp >= 128) {
    countingUp = 0;
  }
  if (fontSize <= 0.1 || fontSize >= 12) {
    fontIncrement = -fontIncrement;
  }
  if (reverseFontSize <= 0.1 || reverseFontSize >= 12) {
    reverseFontIncrement = -reverseFontIncrement;
  }
}

void clearDisplay() {
  for (int x = 0; x < dispWidth; x++) {
    for (int y = 0; y < dispHeight; y++) {
      display.drawPixel(x, y, BLACK);
    }
  }
}