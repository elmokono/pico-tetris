#include <Arduino.h>
#include <Adafruit_ST7735.h> // gpu driver
#include <SPI.h>
#include <stdio.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "engine_core.h"

Engine::Engine()
{
    tft->initR(INITR_144GREENTAB);
    tft->setRotation(0);
    tft->fillScreen(ST7735_CYAN);

    pinMode(RGB_R, OUTPUT);
    pinMode(RGB_G, OUTPUT);
    pinMode(RGB_B, OUTPUT);

    fps = intFps = 0;
    lastMillis = millis();

    // joy
    calibrateStick();
    lastMillisJoy = millis();

    setup_gyro();
}

void Engine::draw(uint16_t *buffer)
{
    tft->drawRGBBitmap(0, 0, buffer, tft->height(), tft->width());
}

joystick_state Engine::input_joy(void)
{
    joystick_state joy;

    joy.b1 = joy.b2 = joy.b3 = joy.b1down = joy.b2down = joy.b3down = false;
    joy.x = stickXCenter;
    joy.y = stickYCenter;

    if (millis() - lastMillisJoy < millisToJoy)
        return joy;

    lastMillisJoy = millis();

    // 0-stickXCenter-1023
    joy.x = analogRead(JOY_AX);
    joy.y = analogRead(JOY_AY);

    if (digitalRead(JOY_B3) == HIGH && button3Pressed)
        button3Pressed = false;
    else if (digitalRead(JOY_B3) == LOW && !button3Pressed)
        joy.b3 = button3Pressed = true;

    if (digitalRead(JOY_B2) == HIGH && button2Pressed)
        button2Pressed = false;
    else if (digitalRead(JOY_B2) == LOW && !button2Pressed)
        joy.b2 = button2Pressed = true;

    if (digitalRead(JOY_B1) == HIGH && button1Pressed)
        button1Pressed = false;
    else if (digitalRead(JOY_B1) == LOW && !button1Pressed)
        joy.b1 = button1Pressed = true;

    joy.b1down = button1Pressed;
    joy.b2down = button2Pressed;
    joy.b3down = button3Pressed;

    return joy;
}

void Engine::loop(void)
{
    intFps++;
    if (millis() - lastMillis >= 1000)
    {
        lastMillis = millis();
        fps = intFps;
        intFps = 0;
    }
}

gyro_state Engine::input_gyro(void)
{
    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    gyro_state state;

    state.acc_x = a.acceleration.x;
    state.acc_y = a.acceleration.y;
    state.acc_z = a.acceleration.z;
    state.gyr_x = g.gyro.x;
    state.gyr_y = g.gyro.y;
    state.gyr_z = g.gyro.z;
    state.temp = temp.temperature;

    /* Print out the values */
    /*Serial.print("Acceleration X: ");
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
    Serial.println(" degC");*/

    return state;
}

void Engine::setup_gyro(void)
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

void Engine::rgb(short r, short g, short b)
{
    analogWrite(RGB_R, r * 4);
    analogWrite(RGB_G, g * 4);
    analogWrite(RGB_B, b * 4);
}

void Engine::calibrateStick(void)
{
    pinMode(JOY_BT, INPUT_PULLUP);
    pinMode(JOY_B1, INPUT_PULLUP);
    pinMode(JOY_B2, INPUT_PULLUP);

    float xCenterAvg = 0;
    float yCenterAvg = 0;
    int samples = 0;
    while (millis() - lastMillis < 500)
    {
        xCenterAvg += analogRead(JOY_AX);
        yCenterAvg += analogRead(JOY_AY);
        samples++;
    }

    stickXCenter = (xCenterAvg / samples);
    stickYCenter = (yCenterAvg / samples);
}