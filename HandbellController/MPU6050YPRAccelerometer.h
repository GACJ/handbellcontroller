#include "HandbellController.h"

#ifdef ENABLE_MPU6050_YPR

class MPU6050YPRAccelerometer : public Accelerometer
{
private:
    int _address;

public:
    MPU6050YPRAccelerometer(int address)
    {
        _address = address;
    }

    accel_t GetType() const override { return ACCEL_MPU6050; }

    void Setup(Joystick* joystick, const SavedConfig* config) override;
    void Update(Joystick* joystick) override;
};

#endif
