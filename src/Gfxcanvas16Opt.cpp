#include "Gfxcanvas16Opt.h"

GFXcanvas16Opt::GFXcanvas16Opt(uint16_t w, uint16_t h) : GFXcanvas16(w, h) {}

void GFXcanvas16Opt::fillBitmap(const uint16_t bitmap[])
{
  uint16_t color;
  uint16_t *buff = getBuffer();
  for (short i = 0; i < (width() * height()); i++)
  {
    color = pgm_read_word(&bitmap[i]);
    if (color != 0xF81F) // magenta
      buff[i] = color;
  }
}

void GFXcanvas16Opt::drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h, uint16_t keyColor)
{
  startWrite();
  for (int16_t j = 0; j < h; j++, y++)
  {
    for (int16_t i = 0; i < w; i++)
    {
      if (bitmap[j * w + i] != keyColor) // transparent
        writePixel(x + i, y, pgm_read_word(&bitmap[j * w + i]));
    }
  }
  endWrite();
}

void GFXcanvas16Opt::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h, uint16_t keyColor)
{
  startWrite();
  for (int16_t j = 0; j < h; j++, y++)
  {
    for (int16_t i = 0; i < w; i++)
    {
      if (bitmap[j * w + i] != keyColor) // transparent
        writePixel(x + i, y, bitmap[j * w + i]);
    }
  }
  endWrite();
}