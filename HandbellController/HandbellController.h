#pragma once

// #define ENABLE_LIS3DH
// #define ENABLE_MPU6050
#define ENABLE_MPU6050_YPR
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

struct Vec3
{
    int x, y, z;
};

struct SavedConfig
{
    char magic[8];
    Vec3 acc, gyro;
};

class Accelerometer
{
public:
    virtual accel_t GetType() const = 0;
    virtual void Setup(Joystick*, const SavedConfig*) = 0;
    virtual void Update(Joystick*) = 0;
};

void calibration_setup();
bool calibration_loop();
void calibration_get(Vec3* acc, Vec3* gyro);
