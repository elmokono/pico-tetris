#include <Arduino.h>
#include <GFXcanvas16Opt.h>  // canvas layer
#include <Adafruit_ST7735.h> // gpu driver
#include <SPI.h>
#include <core.h>
#include "sprites.h"
#include <stdio.h>

#define TFT_DC 21
#define TFT_CS 17
#define TFT_RST 20

#define JOY_AX 26
#define JOY_AY 27
#define JOY_BT 22

#define JOY_B1 6
#define JOY_B2 7

#define STAGE_TITLE_SCREEN 0
#define STAGE_INGAME 1
#define STAGE_GAMEOVER 2

#define MAGENTA 0xF81F

Adafruit_ST7735 *tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16Opt *canvas = new GFXcanvas16Opt(128, 128);
Core *core = new Core();

uint lastMillis, lastMillisMovePiece, lastMillisJoy;
bool button1Pressed = false, button2Pressed = false;
float fps;
int16_t sx = 0;
int16_t sy = 0;
int16_t sw = 128;
int16_t sh = 128;
uint millisToMovePiece = 500;
uint millisToJoy = 100;
float stickXCenter = 512; // default ideal value
int currentStage = STAGE_TITLE_SCREEN;

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
  core->reset();
  lastMillisMovePiece = millis();

  currentStage = STAGE_INGAME;
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

void input_joy(void)
{
  if (millis() - lastMillisJoy < millisToJoy)
    return;
  lastMillisJoy = millis();

  // 0-stickXCenter-1023
  float x = analogRead(JOY_AX);
  float y = analogRead(JOY_AY);

  // control tetris
  if (currentStage == STAGE_INGAME)
  {
    if (x >= 1000)
      core->movePiece(1);
    if (x <= 24)
      core->movePiece(0);
    if (y >= 1000)
      core->movePiece(2);
  }
}

void input_buttons(void)
{
  if (digitalRead(JOY_B2) == HIGH && button2Pressed)
    button2Pressed = false;
  else if (digitalRead(JOY_B2) == LOW && !button2Pressed)
  {
    button2Pressed = true;
    if (currentStage == STAGE_INGAME)
    {
      core->rotatePiece(2);
    }
    if (currentStage == STAGE_GAMEOVER)
    {
      core->reset();
      currentStage = STAGE_INGAME;
    }
  }

  if (digitalRead(JOY_B1) == HIGH && button1Pressed)
    button1Pressed = false;
  else if (digitalRead(JOY_B1) == LOW && !button1Pressed)
  {
    button1Pressed = true;
    if (currentStage == STAGE_INGAME)
    {
      // ideas?
    }
    if (currentStage == STAGE_GAMEOVER)
    {
      core->reset();
    }
  }
}

void inputs(void)
{
  input_joy();
  input_buttons();
}

void gameCore(void)
{
  if (currentStage == STAGE_INGAME)
  {
    // move piece down
    if (millis() - lastMillisMovePiece < millisToMovePiece)
      return;

    lastMillisMovePiece = millis();

    core->movePiece(2); // down

    if (core->isGameOver())
    {
      currentStage = STAGE_GAMEOVER;
    }
  }
}

void draw(void)
{
  // background
  canvas->fillBitmap(bgImage, MAGENTA);

  if (currentStage == STAGE_INGAME || currentStage == STAGE_GAMEOVER)
  {
    // sprites
    for (int i = 0; i < BOARD_WIDTH; i++)
      for (int j = 0; j < (BOARD_HEIGHT - 1); j++)
        if (core->hasBlock(i, j))
          canvas->drawRGBBitmap(i * 8, j * 8, block_still, 8, 8);
    
    if (currentStage == STAGE_INGAME)
    {
      Piece currentPiece = core->getCurrentPiece();
      for (int i = 0; i < BLOCKS_PER_PIECE; i++)
        canvas->drawRGBBitmap(
            (currentPiece.x + currentPiece.blocks[i].x) * 8,
            (currentPiece.y + currentPiece.blocks[i].y) * 8,
            block, 8, 8, MAGENTA);
    }

    // fonts
    canvas->print(2, 2, (char *)"Score", MAGENTA);
    char score_value[7];
    snprintf(score_value, sizeof(score_value), "%06d", core->getScore());
    canvas->print(78, 2, score_value, MAGENTA);
  }

  if (currentStage == STAGE_GAMEOVER)
  {
    canvas->drawRGBBitmap(0, 48, gameover, 128, 32, MAGENTA);
  }

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
