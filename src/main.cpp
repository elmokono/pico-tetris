#include <Arduino.h>
#include <GFXcanvas16Opt.h>  // canvas layer
#include <Adafruit_ST7735.h> // gpu driver
#include <SPI.h>
#include <tetris_core.h>
#include "sprites.h"
#include <stdio.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define TFT_DC 21
#define TFT_CS 17
#define TFT_RST 20

#define JOY_AX 26
#define JOY_AY 27
#define JOY_BT 22

//#define GYRO_SDA 6
//#define GYRO_SCL 7

#define JOY_B1 8
#define JOY_B2 9
#define JOY_B3 10

#define RGB_R 16
#define RGB_G 18
#define RGB_B 19

#define STAGE_TITLE_SCREEN 0
#define STAGE_INGAME 1
#define STAGE_GAMEOVER 2

#define MAGENTA 0xF81F

#define PRESS_ANY_KEY "-PRESS ANY KEY-"

Adafruit_ST7735 *tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16Opt *canvas = new GFXcanvas16Opt(128, 128);
Adafruit_MPU6050 mpu;
TetrisCore *core = new TetrisCore();

uint lastMillis, lastMillisMovePiece, lastMillisJoy, lastMillisTitleScreen;
bool titleScreenOn = true;
bool button1Pressed = false, button2Pressed = false;
float fps;
int16_t sx = 0;
int16_t sy = 0;
int16_t sw = 128;
int16_t sh = 128;
uint millisToMovePiece = 500;
uint millisToJoy = 100;
uint millisToTitleScreen = 700;
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

void rgb(short r, short g, short b)
{
  analogWrite(RGB_R, r);
  analogWrite(RGB_G, g);
  analogWrite(RGB_B, b);
}

void setup_gyro(void)
{
  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
    {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange())
  {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange())
  {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth())
  {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}

void setup()
{
  // while (!Serial) { delay(10); }
  Serial.begin(115200);
  tft->initR(INITR_144GREENTAB);
  tft->setRotation(2);
  tft->fillScreen(ST7735_CYAN);

  // rgb
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);

  // game
  fps = 0;
  lastMillis = millis();

  // joy
  calibrateStick();
  lastMillisJoy = millis();

  // game
  core->reset();
  lastMillisMovePiece = millis();
  lastMillisTitleScreen = millis();

  // gyro
  setup_gyro();
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

void input_gyro(void)
{
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");
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
    if (currentStage == STAGE_TITLE_SCREEN)
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
      currentStage = STAGE_INGAME;
    }
    if (currentStage == STAGE_TITLE_SCREEN)
    {
      core->reset();
      currentStage = STAGE_INGAME;
    }
  }
}

void inputs(void)
{
  //input_gyro();
  input_joy();
  input_buttons();
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
      rgb(0, 0, 1024);
    else
      rgb(0, 0, 0);
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
