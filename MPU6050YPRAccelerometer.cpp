#include "MPU6050YPRAccelerometer.h"
#include "HandbellController.h"
#include "Joystick.h"

#ifdef ENABLE_MPU6050_YPR
#include "MPU6050_6Axis_MotionApps20.h"

// MPU control/status vars
static bool dmpReady = false;  // set true if DMP init was successful
static uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
static uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
static uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
static uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
static Quaternion q;        // [w, x, y, z]         quaternion container
static VectorInt16 aa;      // [x, y, z]            accel sensor measurements
static VectorInt16 aaReal;  // [x, y, z]            gravity-free accel sensor measurements
static VectorInt16 aaWorld; // [x, y, z]            world-frame accel sensor measurements
static VectorFloat gravity; // [x, y, z]            gravity vector
static float euler[3];      // [psi, theta, phi]    Euler angle container
static float ypr[3];        // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

static volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high

static MPU6050 mpu;

static void dmpDataReady()
{
    mpuInterrupt = true;
}

void MPU6050YPRAccelerometer::Setup(Joystick* joystick)
{
    mpu.initialize();
    delay(500);

    // load and configure the DMP
    devStatus = mpu.dmpInitialize();

    // turn on the DMP, now that it's ready
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    attachInterrupt(0, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();

    joystick->setXAxisRange(-16384, 16383);
    joystick->setYAxisRange(-16384, 16383);
    joystick->setZAxisRange(-16384, 16383);
    joystick->setRxAxisRange(-180, 180);
    joystick->setRyAxisRange(-180, 180);
    joystick->setRzAxisRange(-180, 180);
}

void MPU6050YPRAccelerometer::Update(Joystick* joystick)
{
    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // wait for correct available data length, should be a VERY short wait
    auto fifoCount = mpu.getFIFOCount();
    while (fifoCount < packetSize)
    {
        fifoCount = mpu.getFIFOCount();
    }

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;

    // Acceleration
    VectorInt16 acc;
    mpu.dmpGetAccel(&acc, fifoBuffer);
    joystick->setXAxis(acc.x);
    joystick->setYAxis(acc.y);
    joystick->setZAxis(acc.z);

    // Gyro
    VectorInt16 gyro;
    mpu.dmpGetGyro((int16_t*)&gyro, fifoBuffer);
    joystick->setRxAxis(gyro.x);
    joystick->setRyAxis(gyro.y);
    joystick->setRzAxis(gyro.z);

    // Yaw / pitch / roll returned as RotationX/Y/Z
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    joystick->setRxAxis(ypr[0] * 180 / M_PI);
    joystick->setRyAxis(ypr[1] * 180 / M_PI);
    joystick->setRzAxis(ypr[2] * 180 / M_PI);
}

#endif
