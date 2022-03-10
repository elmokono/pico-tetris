#include <Arduino.h>
#include <GFXcanvas16Opt.h> // canvas layer
#include <tetris_core.h>
#include "sprites.h"
#include <stdio.h>
#include "engine_core.h"

#define STAGE_TITLE_SCREEN 0
#define STAGE_INGAME 1
#define STAGE_GAMEOVER 2

#define MAGENTA 0xF81F

#define PRESS_ANY_KEY "-PRESS ANY KEY-"

Engine *engine;
GFXcanvas16Opt *canvas = new GFXcanvas16Opt(128, 128);
TetrisCore *core = new TetrisCore();

uint lastMillisMovePiece, lastMillisTitleScreen;
bool titleScreenOn = true;
int16_t sx = 0;
int16_t sy = 0;
int16_t sw = 128;
int16_t sh = 128;
uint millisToMovePiece = 500;
uint millisToTitleScreen = 700;
int currentStage = STAGE_TITLE_SCREEN;

void setup()
{
  Serial.begin(115200);

  engine = new Engine();

  // game
  core->reset();
  lastMillisMovePiece = millis();
  lastMillisTitleScreen = millis();
}

void input_gyro(void)
{
  gyro_state state = engine->input_gyro();
}

void input_joy(void)
{
  joystick_state joy = engine->input_joy();

  // control tetris
  if (currentStage == STAGE_INGAME)
  {
    if (joy.x >= 1000)
      core->movePiece(1);
    if (joy.x <= 24)
      core->movePiece(0);
    if (joy.y >= 1000)
      core->movePiece(2);
  }
  
  if (joy.b1)
  {
    if (currentStage == STAGE_INGAME)
    {
      core->rotatePiece(2);
    }
    if (currentStage == STAGE_GAMEOVER)
    {
      core->reset();
      currentStage = STAGE_INGAME;
    }
    if (currentStage == STAGE_TITLE_SCREEN)
    {
      core->reset();
      currentStage = STAGE_INGAME;
      engine->rgb(0, 0, 0);
    }
  }

  if (joy.b2)
  {
    if (currentStage == STAGE_INGAME)
    {
      // ideas?
    }
    if (currentStage == STAGE_GAMEOVER)
    {
      core->reset();
      currentStage = STAGE_INGAME;
    }
    if (currentStage == STAGE_TITLE_SCREEN)
    {
      core->reset();
      currentStage = STAGE_INGAME;
      engine->rgb(0, 0, 0);
    }
  }
}

void inputs(void)
{
  //input_gyro();
  input_joy();
}

void gameCore(void)
{
  if (currentStage == STAGE_TITLE_SCREEN)
  {
    if (millis() - lastMillisTitleScreen < millisToTitleScreen)
      return;
    lastMillisTitleScreen = millis();
    titleScreenOn = !titleScreenOn;
    if (titleScreenOn)
      engine->rgb(0, 0, 1024);
    else
      engine->rgb(0, 0, 0);
  }

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
  if (currentStage == STAGE_TITLE_SCREEN)
  {
    canvas->fillBitmap(title_screen);

    if (titleScreenOn)
      canvas->print(4, 96, (char *)PRESS_ANY_KEY, MAGENTA);
  }

  if (currentStage == STAGE_INGAME || currentStage == STAGE_GAMEOVER)
  {
    // background
    canvas->fillBitmap(bgImage, MAGENTA);

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
}

void loop(void)
{
  inputs();

  gameCore();

  draw();

  // buffer to screen
  engine->draw(canvas->getBuffer());

  engine->getFps();
}
