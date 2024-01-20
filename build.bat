@echo off
set PATH=C:\Program Files\Arduino IDE\resources\app\lib\backend\resources;%PATH%
arduino-cli compile -b arduino:avr:leonardo HandbellController --output-dir out --library lib\I2Cdev --library lib\MPU6050
