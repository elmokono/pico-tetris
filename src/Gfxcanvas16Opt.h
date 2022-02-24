#include "Adafruit_GFX.h"

class GFXcanvas16Opt : public GFXcanvas16 {
public:
  GFXcanvas16Opt(uint16_t w, uint16_t h);
  ~GFXcanvas16Opt(void);
  void fillBitmap(const uint16_t bitmap[]);
  void fillBitmap(const uint16_t bitmap[], uint16_t keyColor);
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h, uint16_t keyColor);
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t bitmapWidth, int16_t offset_x, int16_t offset_y, int16_t w, int16_t h, uint16_t keyColor);
  void print(int16_t x, int16_t y, char *text, uint16_t keyColor);
};
