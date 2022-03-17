#include <Arduino.h>
#include <stdio.h>

#include <GFXcanvas16Opt.h> // canvas layer
#include <tetris_core.h>
#include <sprites.h>
#include <engine_core.h>

#define STAGE_TITLE_SCREEN 0
#define STAGE_INGAME 1
#define STAGE_GAMEOVER 2

#define MAGENTA 0xF81F

#define PRESS_ANY_KEY "-PRESS ANY KEY-"

Engine *engine;
GFXcanvas16Opt *canvas = new GFXcanvas16Opt(128, 128);
TetrisCore *tetrisCore = new TetrisCore();

uint lastMillisMovePiece, lastMillisTitleScreen;
bool titleScreenOn = true;
uint millisToMovePiece = 500;
uint millisToTitleScreen = 700;
int currentStage = STAGE_TITLE_SCREEN;

void setup()
{
  Serial.begin(115200);

  engine = new Engine();

  // game
  tetrisCore->reset();
  lastMillisMovePiece = millis();
  lastMillisTitleScreen = millis();
}

void input_gyro(void)
{
  // gyro_state state = engine->input_gyro();
}

void input_joy(void)
{
  joystick_state joy = engine->input_joy();

  if (joy.novalue)
    return;

  if (currentStage == STAGE_TITLE_SCREEN)
  {
    engine->calibrateStick();
  }

  // control tetris
  if (currentStage == STAGE_INGAME)
  {
    if (joy.x >= 0.8)
      tetrisCore->movePiece(0);
    else if (joy.x <= -0.8)
      tetrisCore->movePiece(1);
    // else if (joy.y >= 0.8)
    //   tetrisCore->movePiece(2);
  }

  if (joy.b1)
  {
    if (currentStage == STAGE_INGAME)
    {
      tetrisCore->rotatePiece(2);
    }
    if (currentStage == STAGE_GAMEOVER)
    {
      tetrisCore->reset();
      currentStage = STAGE_INGAME;
    }
    if (currentStage == STAGE_TITLE_SCREEN)
    {
      tetrisCore->reset();
      currentStage = STAGE_INGAME;
      engine->rgb(0, 0, 0);
    }
  }

  if (joy.b2down)
  {
    if (currentStage == STAGE_INGAME)
    {
      tetrisCore->movePiece(2);
    }
  }

  if (joy.b2)
  {
    if (currentStage == STAGE_GAMEOVER)
    {
      tetrisCore->reset();
      currentStage = STAGE_INGAME;
    }
    if (currentStage == STAGE_TITLE_SCREEN)
    {
      tetrisCore->reset();
      currentStage = STAGE_INGAME;
      engine->rgb(0, 0, 0);
    }
  }
}

void inputs(void)
{
  input_gyro();
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
      engine->rgb(0, 0, 255);
    else
      engine->rgb(0, 0, 0);
  }

  if (currentStage == STAGE_INGAME)
  {
    // move piece down
    if (millis() - lastMillisMovePiece < millisToMovePiece)
      return;

    lastMillisMovePiece = millis();

    tetrisCore->movePiece(2); // down

    if (tetrisCore->isGameOver())
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

    joystick_state joy = engine->input_joy();
    /*
        if (joy.x != -1)
        {
          Serial.print("x: ");
          Serial.print(joy.x);
          Serial.print(" y: ");
          Serial.println(joy.y);

          char score_value[7];
          snprintf(score_value, sizeof(score_value), "%u", joy.x);
          canvas->print(2, 2, score_value, MAGENTA);
          snprintf(score_value, sizeof(score_value), "%u", joy.y);
          canvas->print(2, 16, score_value, MAGENTA);
        }*/
  }

  if (currentStage == STAGE_INGAME || currentStage == STAGE_GAMEOVER)
  {
    // background
    canvas->fillBitmap(bgImage, MAGENTA);

    // sprites
    for (int i = 0; i < BOARD_WIDTH; i++)
      for (int j = 0; j < (BOARD_HEIGHT - 1); j++)
        if (tetrisCore->hasBlock(i, j))
          canvas->drawRGBBitmap(i * 8, j * 8, block_still, 8, 8);

    if (currentStage == STAGE_INGAME)
    {
      Piece currentPiece = tetrisCore->getCurrentPiece();
      for (int i = 0; i < BLOCKS_PER_PIECE; i++)
        canvas->drawRGBBitmap(
            (currentPiece.x + currentPiece.blocks[i].x) * 8,
            (currentPiece.y + currentPiece.blocks[i].y) * 8,
            block, 8, 8, MAGENTA);
    }

    // fonts
    canvas->print(2, 2, (char *)"Score", MAGENTA);
    char score_value[7];
    snprintf(score_value, sizeof(score_value), "%06d", tetrisCore->getScore());
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

  engine->loop();
}
