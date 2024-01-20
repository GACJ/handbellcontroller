@echo off
set PATH=C:\Program Files\Arduino IDE\resources\app\lib\backend\resources;%PATH%
cd out
arduino-cli upload -b arduino:avr:leonardo -i HandbellController.ino.hex
