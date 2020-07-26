#pragma once

// #define ENABLE_LIS3DH
#define ENABLE_MPU6050
// #define ENABLE_MPU6050_YPR
#define ENABLE_ADXL345

typedef enum
{
    ACCEL_NONE,
    ACCEL_LIS3DH,
    ACCEL_MPU6050,
    ACCEL_ADXL345
} accel_t;

class Joystick_;
typedef Joystick_ Joystick;

class Accelerometer
{
public:
    virtual accel_t GetType() const = 0;
    virtual void Setup(Joystick*) = 0;
    virtual void Update(Joystick*) = 0;
};
