#include <Arduino.h>
#include <Adafruit_ST7735.h> // gpu driver
#include <SPI.h>
#include <stdio.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

/*
pico spi0 pins
---------------------------
#define PIN_SPI_MISO  (16u)
#define PIN_SPI_MOSI  (19u)
#define PIN_SPI_SCK   (18u)
#define PIN_SPI_SS    (17u)
*/

#define TFT_DC 21
#define TFT_CS 17
#define TFT_RST 20

#define JOY_AX 26
#define JOY_AY 27
#define JOY_BT 22

#define JOY_B1 8
#define JOY_B2 9
#define JOY_B3 10

#define RGB_R 2
#define RGB_G 3
#define RGB_B 4

#define STICK_MIN 0.0
#define STICK_MAX 1023.0

struct joystick_state
{
  float x;
  float y;
  bool novalue;
  bool b1;
  bool b2;
  bool b3;
  bool b1down;
  bool b2down;
  bool b3down;
  bool b1pressed;
  bool b2pressed;
  bool b3pressed;
};

class Engine
{
public:
    Engine();
    void calibrateStick(void);
    void rgb(short r, short g, short b);
    joystick_state input_joy(void);
    void loop(void);
    void draw(uint16_t* buffer);
    float stickXCenter = 512; float stickYCenter = 512;// default ideal value
    short fps;
};