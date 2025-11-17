#ifndef _SH1107_H_
#define _SH1107_H_

#include <Wire.h>
#include <avr/pgmspace.h>
#include "charMap.h"

#define SH1107_I2C_ADDRESS 0x3C
#define BLACK 0
#define WHITE 1

#define SH1107_COMMAND_MODE 0x00
#define SH1107_DATA_MODE 0x40

#define SH110X_DISPLAYON 0xAF
#define SH110X_DISPLAYOFF 0xAE
#define SH110X_SETDISPLAYCLOCKDIV 0xD5
#define SH110X_MEMORYMODE 0x20
#define SH110X_SETLOWCOLUMN 0x00
#define SH110X_SETHIGHCOLUMN 0x10
#define SH110X_SETSTARTLINE 0x40
#define SH110X_SETPAGEADDR 0xB0

#define BUFSIZE_128x128 (128 * 128 / 8)

class SH1107 {
public:
  SH1107() {}

  void begin() {
    Wire.begin();
    sendCommand(SH110X_DISPLAYOFF);
    sendCommand(SH110X_SETDISPLAYCLOCKDIV, 0x51);
    sendCommand(SH110X_MEMORYMODE);
    sendCommand(SH110X_DISPLAYON);
    clearDisplay();
  }

  void clearDisplay() {
    memset(displayBuffer, 0, BUFSIZE_128x128);
  }

  void drawPixel(int x, int y, uint8_t color) {
    if (x >= 0 && x < 128 && y >= 0 && y < 128) {
      uint8_t page = y / 8;
      uint8_t bitMask = 1 << (y % 8);
      if (color == BLACK) {
        displayBuffer[x + page * 128] &= ~bitMask;
      } else {
        displayBuffer[x + page * 128] |= bitMask;
      }
    }
  }

  void drawChar(int x, int y, char c, uint8_t color, float size) {
    if (c < 32 || c > 127) return;  // Only print standard ASCII characters
    const uint8_t* charBitmap = _charMap[c - 32];
    for (int i = 0; i < 5; i++) {
      uint8_t line = pgm_read_byte(&charBitmap[i]);
      for (int j = 0; j < 8; j++) {
        if (line & 0x1) {
          drawScaledPixel(x + i * size, y + j * size, color, size);
        } else {
          drawScaledPixel(x + i * size, y + j * size, BLACK, size);
        }
        line >>= 1;
      }
    }
  }

  void drawString(int x, int y, const char* str, uint8_t color, float size = 1.0) {
    while (*str) {
      drawChar(x, y, *str, color, size);
      x += 6 * size;  // 5 pixels wide + 1 pixel space, scaled
      str++;
    }
  }

  void drawCircle(int x0, int y0, int radius, uint8_t color) {
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
      drawPixel(x0 - x, y0 - y, color);
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 + y, y0 - x, color);
      drawPixel(x0 + x, y0 - y, color);

      if (err <= 0) {
        y++;
        err += dy;
        dy += 2;
      }
      if (err > 0) {
        x--;
        dx += 2;
        err += dx - (radius << 1);
      }
    }
  }

  void drawLine(int x0, int y0, int x1, int y1, uint8_t color) {
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true) {
      drawPixel(x0, y0, color);
      if (x0 == x1 && y0 == y1) break;
      e2 = 2 * err;
      if (e2 >= dy) {
        err += dy;
        x0 += sx;
      }
      if (e2 <= dx) {
        err += dx;
        y0 += sy;
      }
    }
  }

  void display() {
    for (int page = 0; page < 16; ++page) {
      setCursor(0, page);
      beginData();
      for (int x = 0; x < 128; ++x) {
        sendData(displayBuffer[x + page * 128]);
      }
      endTransm();
    }
  }

private:
  uint8_t displayBuffer[BUFSIZE_128x128];

  void drawScaledPixel(float x, float y, uint8_t color, float size) {
    int intSize = ceil(size);
    for (int i = 0; i < intSize; i++) {
      for (int j = 0; j < intSize; j++) {
        drawPixel(x + i, y + j, color);
      }
    }
  }

  void setCursor(uint8_t x, uint8_t y) {
    sendCommand(SH110X_SETLOWCOLUMN | (x & 0x0F));
    sendCommand(SH110X_SETHIGHCOLUMN | ((x >> 4) & 0x0F));
    sendCommand(SH110X_SETPAGEADDR | (y & 0x0F));
  }

  void sendCommand(uint8_t command, uint8_t arg = 0x00) {
    Wire.beginTransmission(SH1107_I2C_ADDRESS);
    Wire.write(SH1107_COMMAND_MODE);
    Wire.write(command);
    if (arg != 0x00) {
      Wire.write(arg);
    }
    Wire.endTransmission();
  }

  void beginData() {
    Wire.beginTransmission(SH1107_I2C_ADDRESS);
    Wire.write(SH1107_DATA_MODE);
  }

  void endTransm() {
    Wire.endTransmission();
  }

  void sendData(uint8_t data) {
    Wire.write(data);
  }
};

#endif
