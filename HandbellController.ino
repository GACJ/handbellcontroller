#define ENABLE_LIS3DH
#define ENABLE_MPU6050
#define ENABLE_ADXL345

#include "Joystick.h"
#include <Wire.h>

#ifdef ENABLE_LIS3DH
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#endif

typedef Joystick_ Joystick;

enum
{
    ADDR_18_LIS3DH = 0x18,
    ADDR_19_LIS3DH = 0x19,
    ADDR_1D_ADXL345 = 0x1D,
    ADDR_53_ADXL345 = 0x53,
    ADDR_68_MPU9250 = 0x68,
    ADDR_69_MPU9250 = 0x69,
};

typedef enum
{
    ACCEL_NONE,
    ACCEL_LIS3DH,
    ACCEL_MPU6050,
    ACCEL_ADXL345
} accel_t;

class Accelerometer
{
public:
    virtual accel_t GetType() const = 0;
    virtual void Setup(Joystick*) = 0;
    virtual void Update(Joystick*) = 0;
};

////////////////////////////////////////////////////////////////////
// LIS3DH
////////////////////////////////////////////////////////////////////
#ifdef ENABLE_LIS3DH
class LIS3DHAccelerometer : public Accelerometer
{
private:
    int _address;
    Adafruit_LIS3DH _lis = Adafruit_LIS3DH();

public:
    LIS3DHAccelerometer(int address)
    {
        _address = address;
    }

    accel_t GetType() const override { return ACCEL_LIS3DH; }

    void Setup(Joystick* joystick) override
    {
        _lis.begin(_address);
        _lis.setRange(LIS3DH_RANGE_2_G);
        _lis.setDataRate(LIS3DH_DATARATE_50_HZ);
        joystick->setXAxisRange(-16384, 16383);
        joystick->setYAxisRange(-16384, 16383);
        joystick->setZAxisRange(-16384, 16383);
    }

    void Update(Joystick* joystick) override
    {
        _lis.read();
        joystick->setXAxis(_lis.x);
        joystick->setYAxis(_lis.y);
        joystick->setZAxis(_lis.z);
    }
};
#endif

////////////////////////////////////////////////////////////////////
// MPU6050
////////////////////////////////////////////////////////////////////
#ifdef ENABLE_MPU6050
#define MPU6050_RA_ACCEL_XOUT_H 0x3B
#define MPU6050_RA_ACCEL_XOUT_L 0x3C
#define MPU6050_RA_ACCEL_YOUT_H 0x3D
#define MPU6050_RA_ACCEL_YOUT_L 0x3E
#define MPU6050_RA_ACCEL_ZOUT_H 0x3F
#define MPU6050_RA_ACCEL_ZOUT_L 0x40
#define MPU6050_RA_GYRO_XOUT_H 0x43
#define MPU6050_RA_GYRO_XOUT_L 0x44
#define MPU6050_RA_GYRO_YOUT_H 0x45
#define MPU6050_RA_GYRO_YOUT_L 0x46
#define MPU6050_RA_GYRO_ZOUT_H 0x47
#define MPU6050_RA_GYRO_ZOUT_L 0x48
#define MPU6050_RA_PWR_MGMT_1 0x6B

class MPU6050Accelerometer : public Accelerometer
{
private:
    int _address;

public:
    MPU6050Accelerometer(int address)
    {
        _address = address;
    }

    accel_t GetType() const override { return ACCEL_MPU6050; }

    void Setup(Joystick* joystick) override
    {
        Wire.beginTransmission(_address);
        Wire.write(MPU6050_RA_PWR_MGMT_1); // Talk to the register 6B
        Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
        Wire.endTransmission();
        joystick->setXAxisRange(-16384, 16383);
        joystick->setYAxisRange(-16384, 16383);
        joystick->setZAxisRange(-16384, 16383);
        joystick->setRxAxisRange(-16384, 16383);
        joystick->setRyAxisRange(-16384, 16383);
        joystick->setRzAxisRange(-16384, 16383);
    }

    void Update(Joystick* joystick) override
    {
        // Read acceleratometer data
        Wire.beginTransmission(_address);
        Wire.write(MPU6050_RA_ACCEL_XOUT_H);
        Wire.endTransmission();
        Wire.requestFrom(_address, 6, true);
        float accX = (Wire.read() << 8 | Wire.read());
        float accY = (Wire.read() << 8 | Wire.read());
        float accZ = (Wire.read() << 8 | Wire.read());

        // Read gyroscope data
        Wire.beginTransmission(_address);
        Wire.write(MPU6050_RA_GYRO_XOUT_H);
        Wire.endTransmission();
        Wire.requestFrom(_address, 6, true);
        float gyroX = (Wire.read() << 8 | Wire.read());
        float gyroY = (Wire.read() << 8 | Wire.read());
        float gyroZ = (Wire.read() << 8 | Wire.read());

        // Set joystick state
        joystick->setXAxis(accX);
        joystick->setYAxis(accY);
        joystick->setZAxis(accZ);
        joystick->setRxAxis(gyroX);
        joystick->setRyAxis(gyroY);
        joystick->setRzAxis(gyroZ);
    }
};
#endif

////////////////////////////////////////////////////////////////////
// ADXL345
////////////////////////////////////////////////////////////////////
#ifdef ENABLE_ADXL345

#define ADXL345_POWER_CTL 0x2D
#define ADXL345_MEASUREMENT_RANGE 0x31
#define ADXL345_ACCEL_XOUT_H 0x32

class ADXL345Accelerometer : public Accelerometer
{
private:
    int _address;

public:
    ADXL345Accelerometer(int address)
    {
        _address = address;
    }

    accel_t GetType() const override { return ACCEL_ADXL345; }

    void Setup(Joystick* joystick) override
    {
        Wire.beginTransmission(_address);
        Wire.write(ADXL345_POWER_CTL); // Access / talk to POWER_CTL Register - 0x2D
        Wire.write(8);                 // Enable measurement (8dec -> 0000 1000 binary) Bit D3 High for measuring enable
        Wire.endTransmission();

        Wire.beginTransmission(_address);
        Wire.write(ADXL345_MEASUREMENT_RANGE); // Set the measurement range
        Wire.write(0);                         // Set 10-bit mode with range -2g..+2g
        Wire.endTransmission();

        joystick->setXAxisRange(-256, 255);
        joystick->setYAxisRange(-256, 255);
        joystick->setZAxisRange(-256, 255);
    }

    void Update(Joystick* joystick) override
    {
        Wire.beginTransmission(_address);
        Wire.write(ADXL345_ACCEL_XOUT_H);
        Wire.endTransmission();
        Wire.requestFrom(_address, 6, true);
        float accX = (Wire.read() | Wire.read() << 8);
        float accY = (Wire.read() | Wire.read() << 8);
        float accZ = (Wire.read() | Wire.read() << 8);

        joystick->setXAxis(accX);
        joystick->setYAxis(accY);
        joystick->setZAxis(accZ);
    }
};
#endif

static Accelerometer* CreateAccelerometer()
{
    // Try every I2C address to see which one responds.
    // Addresses 0..7 and 120..127 are reserved.
    // We are assuming that only an accelerometer is connected.
    for (int address = 8; address < 120; address++)
    {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0)
        {
            switch (address)
            {
                case ADDR_18_LIS3DH:
                case ADDR_19_LIS3DH:
                    return new LIS3DHAccelerometer(address);
                case ADDR_1D_ADXL345:
                case ADDR_53_ADXL345:
                    return new ADXL345Accelerometer(address);
                case ADDR_68_MPU9250:
                case ADDR_69_MPU9250:
                    return new MPU6050Accelerometer(address);
            }
        }
    }
    return nullptr;
}

static Joystick* gJoystick;
static Accelerometer* gAccelerometer;

static void SetupButtons()
{
    pinMode(9, INPUT_PULLUP);
    pinMode(10, INPUT_PULLUP);
}

static void UpdateButtons(Joystick* joystick)
{
    int btn0 = !digitalRead(10);
    int btn1 = !digitalRead(9);
    joystick->setButton(0, btn0);
    joystick->setButton(1, btn1);
}

void setup()
{
    Wire.begin();

    SetupButtons();

    gAccelerometer = CreateAccelerometer();
    if (gAccelerometer != nullptr)
    {
        bool hasGyro = gAccelerometer->GetType() == ACCEL_MPU6050;
        gJoystick = new Joystick(
            JOYSTICK_DEFAULT_REPORT_ID,
            JOYSTICK_TYPE_JOYSTICK,
            2,
            0,
            true,
            true,
            true,
            hasGyro,
            hasGyro,
            hasGyro,
            false,
            false,
            false,
            false,
            false);
        gAccelerometer->Setup(gJoystick);
    }
    else
    {
        gJoystick = new Joystick(
            JOYSTICK_DEFAULT_REPORT_ID,
            JOYSTICK_TYPE_JOYSTICK,
            2,
            0,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false);
    }

    gJoystick->begin(false);

    delay(10);
}

void loop()
{
    if (gAccelerometer != nullptr)
    {
        gAccelerometer->Update(gJoystick);
    }

    UpdateButtons(gJoystick);
    gJoystick->sendState();

    delay(5);
}
