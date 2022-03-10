#include <Arduino.h>
#include <Adafruit_ST7735.h> // gpu driver
#include <SPI.h>
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

#define STICK_CENTER 512

struct joystick_state
{
  int16_t x;
  int16_t y;
  bool b1;
  bool b2;
  bool b3;
  bool b1down;
  bool b2down;
  bool b3down;
};

struct gyro_state
{
  float acc_x;
  float acc_y;
  float acc_z;
  float gyr_x;
  float gyr_y;
  float gyr_z;
  float temp;
};

class Engine
{
public:
    Engine();
    void calibrateStick(void);
    void rgb(short r, short g, short b);
    gyro_state input_gyro(void);
    joystick_state input_joy(void);
    void getFps(void);
    void draw(uint16_t* buffer);

private:
    Adafruit_MPU6050 mpu;  
    Adafruit_ST7735 *tft = new Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);  
    uint lastMillis, lastMillisJoy;
    bool button1Pressed = false, button2Pressed = false, button3Pressed = false;
    float fps;
    uint millisToJoy = 100;
    float stickXCenter = 512; float stickYCenter = 512;// default ideal value
    void setup_gyro(void);
    
};