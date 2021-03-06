# Raspberry Pi Pico Tetris

Simple Tetris using graphics backbuffer technique. Working at 62 to 55 fps.
Also using custom bitmap-fonts

Dependencies

- adafruit/Adafruit BusIO
- adafruit/Adafruit GFX Library
- adafruit/Adafruit ST7735 and ST7789 Library

Hardware

- 1 Raspberry Pi Pico
- 1 TFT 128x128 SPI Panel
- 1 Stick module
- 2 push buttons

Wiring

**TFT Panel**

- SLK gpio 2
- MISO gpio 3
- GND GND
- 5V VBUS
- LED3V 3V
- DC gpio 21 (customizable)
- CS gpio 17 (customizable)
- RST gpio 20 (customizable)

**Joystick**

- 5V VBUS
- GND GND
- AxisX gpio 26 (customizable)
- AxisY gpio 27 (customizable)
- Button gpio 22 (customizable)

**RGB**
- GND GND
- R gpio 16
- G gpio 18
- B gpio 19

**Gyro**
- 5V VBUS
- GND GND
- SDA gpio 6
- SCL gpio 7

**Buttons**

- Button1 gpio 8 (customizable)
- Button2 gpio 9 (customizable)
- Button3 gpio 10 (customizable)
- GND GND

References for rotation and collisions: \
https://tetris.fandom.com/wiki/SRS \
https://medium.com/swlh/matrix-rotation-in-javascript-269cae14a124

Image convertion: \
http://www.rinkydinkelectronics.com/t_imageconverter565.php
