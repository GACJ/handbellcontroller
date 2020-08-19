Handbell Controller

This Windows utility flashes an Arduino-based dummy handbell with code required to support an Arduino/Accelerometer combination as a handbell controller that
can be used with Handbell Manager (https://handbellmanager.changeringing.co.uk) or Handbell Stadium (https://handbellstadium.org).

To run, unzip the contents into a folder and run upload.bat at the command prompt (or upload.ps1 in powershell). The utility will not run if more than one
dummy handbell (Arduino) is connected.

The following are supported:

Accelerometer boards:
- LIS3DH
- MPU6050
- ADXL345

Joystick axis 
- X
- Y
- Z

for the MPU6050 calculated Yaw, Pitch and Roll (in place the Gyro values) are provided on the following axis
- Rx
- Ry
- Rz

two joystick buttons
- B0
- B1

Correct configuration of the Arduino can be confirmed by observing the Arduino pin17 Red LED that shows up to 5 flashes followed by a pause. 
These correspond to:
1 - B0 pressed
2 - B1 pressed
3 - X value difference
4 - Y value difference
5 - Z value difference
6 - No flash

All five flashes should occur repeatedly when the two buttons are held down.
