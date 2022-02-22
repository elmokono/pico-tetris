#include <Arduino.h>
#include <GFXcanvas16Opt.h>  // canvas layer
#include <Adafruit_ST7735.h> // gpu driver
#include <SPI.h>
#include <core.h>
#include "sprites.h"

#define TFT_DC 21
#define TFT_CS 17
#define TFT_RST 20

#define JOY_AX 26
#define JOY_AY 27
#define JOY_BT 22

#define JOY_B1 6
#define JOY_B2 7

#define MAGENTA 0xF81F

Adafruit_ST7735 *tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16Opt *canvas = new GFXcanvas16Opt(128, 128);
Core *core = new Core();

uint lastMillis, lastMillisMovePiece, lastMillisJoy;
float fps;
int16_t sx = 0;
int16_t sy = 0;
int16_t sw = 128;
int16_t sh = 128;
uint millisToMovePiece = 500;
uint millisToJoy = 100;
float stickXCenter = 512; // default ideal value

void calibrateStick(void)
{
  pinMode(JOY_BT, INPUT_PULLUP);
  pinMode(JOY_B1, INPUT_PULLUP);
  pinMode(JOY_B2, INPUT_PULLUP);

  float xCenterAvg = 0;
  int samples = 0;
  while (millis() - lastMillis < 500)
  {
    xCenterAvg += analogRead(JOY_AX);
    samples++;
  }

  stickXCenter = (xCenterAvg / samples);
}

void setup()
{
  // while (!Serial) { delay(10); }
  Serial.begin(115200);
  tft->initR(INITR_144GREENTAB);
  tft->setRotation(0);
  tft->fillScreen(ST7735_CYAN);

  // game
  fps = 0;
  lastMillis = millis();

  // joy
  calibrateStick();
  lastMillisJoy = millis();

  // game
  core->addPiece();
  lastMillisMovePiece = millis();
}

void getFps(void)
{
  fps++;
  if (millis() - lastMillis >= 1000)
  {
    Serial.print(fps);
    Serial.println(" fps");
    fps = 0;
    lastMillis = millis();
  }
}

void inputs(void)
{
  if (millis() - lastMillisJoy < millisToJoy)
    return;
  lastMillisJoy = millis();

  // 0-stickXCenter-1023
  float f = analogRead(JOY_AX);

  if (f >= 1000)
    core->movePiece(1);
  if (f <= 24)
    core->movePiece(0);

  if (digitalRead(JOY_B2) == LOW)
    core->rotatePiece(true);

  if (digitalRead(JOY_B1) == LOW)
    core->reset();
}

void gameCore(void)
{
  // move piece down
  if (millis() - lastMillisMovePiece < millisToMovePiece)
    return;

  lastMillisMovePiece = millis();

  core->movePiece(2); // down
}

void draw(void)
{
  // background
  canvas->fillBitmap(bgImage, MAGENTA);

  // sprites
  Piece currentPiece = core->getCurrentPiece();

  for (int i = 0; i < 16; i++)
    for (int j = 0; j < 16; j++)
      if (core->hasBlock(i, j))
        canvas->drawRGBBitmap(i * 8, j * 8, block_still, 8, 8, MAGENTA);

  for (int i = 0; i < 4; i++)
    canvas->drawRGBBitmap(
        (currentPiece.x + currentPiece.blocks[i].x) * 8,
        (currentPiece.y + currentPiece.blocks[i].y) * 8,
        block, 8, 8, MAGENTA);

  // fonts

  // buffer to screen
  tft->drawRGBBitmap(sx, sy, canvas->getBuffer(), sw, sh);
}

void loop(void)
{
  inputs();

  gameCore();

  draw();

  getFps();
}
