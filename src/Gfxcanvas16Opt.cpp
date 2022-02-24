#include "Gfxcanvas16Opt.h"
#include "courier_new.h"

GFXcanvas16Opt::GFXcanvas16Opt(uint16_t w, uint16_t h) : GFXcanvas16(w, h) {}

void GFXcanvas16Opt::fillBitmap(const uint16_t bitmap[])
{
  uint16_t *buff = getBuffer();
  memcpy(buff, bitmap, height() * width() * sizeof(uint16_t));
}

void GFXcanvas16Opt::fillBitmap(const uint16_t bitmap[], uint16_t keyColor)
{
  uint16_t color;
  uint16_t *buff = getBuffer();
  for (short i = 0; i < (width() * height()); i++)
  {
    color = pgm_read_word(&bitmap[i]);
    if (color != keyColor) // magenta
      buff[i] = color;
  }
}

void GFXcanvas16Opt::drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h, uint16_t keyColor)
{
  for (int16_t j = 0; j < h; j++, y++)
    for (int16_t i = 0; i < w; i++)
      if (bitmap[j * w + i] != keyColor) // transparent
        writePixel(x + i, y, bitmap[j * w + i]);
}

void GFXcanvas16Opt::drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h)
{
  uint16_t *buff = getBuffer();
  for (int16_t j = 0; j < h; j++)
    memcpy(&buff[(y + j) * width() + x], &bitmap[j * w], w * sizeof(uint16_t));
}

/*
@param x: drawing position x
@param y: drawing position y
@param fontBitmap: sprite bitmap
@param bitmapWidth: sprite width
@param offset_x: sprite offset x
@param offset_y: sprite offset y
@param w: sprite width
@param h: sprite height
@param keyColor: transparent color
*/
void GFXcanvas16Opt::drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t bitmapWidth, int16_t offset_x, int16_t offset_y, int16_t w, int16_t h, uint16_t keyColor)
{
  int offset = 0;
  for (int16_t j = 0; j < h; j++, y++)
    for (int16_t i = 0; i < w; i++)
    {
      offset = (j + offset_y) * bitmapWidth + (i + offset_x);
      if (bitmap[offset] != keyColor) // transparent
        writePixel(x + i, y, bitmap[offset]);
    }
}

void GFXcanvas16Opt::print(int16_t x, int16_t y, char *text, uint16_t keyColor)
{
  while (*text)
  {
    int symbolIndex = 0;
    for (int i = 0; i < SYMBOLS_COUNT; i++)
      if (symbols[i] == *text)
        symbolIndex = i;

    drawRGBBitmap(x, y, font_bitmap, (SYMBOLS_COUNT - 1) * SYMBOL_WIDTH, symbolIndex * SYMBOL_WIDTH, 0, SYMBOL_WIDTH, SYMBOL_HEIGHT, keyColor);

    x += 8;
    text++;
  }
}