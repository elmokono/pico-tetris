#include <Arduino.h>
#include <GFXcanvas16Opt.h>   // canvas layer
#include <Adafruit_ST7735.h>  // gpu driver
#include <SPI.h>
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

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16Opt *canvas = new GFXcanvas16Opt(128, 128);

uint lastMillis;
float x, y, fps, aX, aY, gravity;
int16_t sx = 0;
int16_t sy = 0;
int16_t sw = 128;
int16_t sh = 128;
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
  tft.initR(INITR_144GREENTAB);
  tft.setRotation(0);
  tft.fillScreen(ST7735_CYAN);

  // game
  x = 48;
  y = 48;
  fps = 0;
  aX = 4;
  aY = 0;
  gravity = 0.25;
  lastMillis = millis();

  // joy
  calibrateStick();
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

void actions(void)
{
  aX = aX / 1.25;
  if (abs(aX) < 0.1)
    aX = 0;

  aY += gravity;
  if (y > 48)
  {
    aY = 0; // floor
    y = 48;
  }

  x += aX;
  y += aY;
}

void inputs(void)
{
  // 0-stickXCenter-1023
  float f = analogRead(JOY_AX);

  if (f >= stickXCenter)
    aX += ((f - stickXCenter) / (1023.0 - stickXCenter));
  else
    aX -= 1 - (f / stickXCenter);

  if (aX > 2)
    aX = 2;

  if (aX < -2)
    aX = -2;

  if (digitalRead(JOY_B1) == LOW && aY == 0)
    aY = -3;
}

void loop(void)
{
  canvas->fillBitmap(bgImage, MAGENTA);

  inputs();

  actions();

  canvas->drawRGBBitmap(x, y, sprite, 16, 16, MAGENTA);

  getFps();

  tft.drawRGBBitmap(sx, sy, canvas->getBuffer(), sw, sh);
}
