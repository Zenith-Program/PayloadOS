#pragma once 
#include "PayloadOSGeneral.h"

namespace PayloadOS{
    namespace Peripherals{
        enum class PeripheralNames{
            PayloadAltimeter, PayloadIMU, STEMnaut1, STEMnaut2, STEMnaut3, STEMnaut4, GPS, LightAPRSAltimeter, Transmitter, PowerCheck, ArmSwitch, SENTINAL_COUNT
        };
        #define PayloadOS_NumberOfPeripherals static_cast<uint_t>(PeripheralNames::SENTINAL_COUNT)

        struct PeripheralStatus{
            bool init;
            bool responsive;
            bool ready;
            bool launchReady;
        };

        class PeripheralInterface{
        public:
            virtual error_t init() = 0;
            virtual PeripheralStatus status() = 0;
            virtual error_t deInit() = 0;
        };

        class AltimeterInterface : public PeripheralInterface{
        public:
            virtual float_t getAltitude_m() = 0;
            virtual float_t getPressure_mBar() = 0;
            virtual float_t getTemperature_K() = 0;

            float_t getAltitude_ft();
            float_t getTemperature_F();
            float_t getTemperature_C();
            float_t getPressure_psi();
        };


        struct LinearVector{
            float x, y, z;
        };
        struct RotationVector{
            float x_rot, y_rot, z_rot;
        };
        struct EulerAngle{
            float yaw, pitch, roll;
        };
        struct Matrix3x3{
            float r1c1, r1c2, r1c3,
                r2c1, r2c2, r2c3, 
                r3c1, r3c2, r3c3;
        };

        class IMUInterface : public PeripheralInterface{
        public:
            virtual LinearVector getAcceleration_m_s2() = 0;
            virtual RotationVector getAngularVelocity_deg_s() = 0;
            virtual LinearVector getGravityVector() = 0;

            LinearVector getAcceleration_ft_s2();
            RotationVector getAngularVelocity_rad_s();
            float_t getAccelerationMagnitude_m_s2();
            float_t getAccelerationMagnitude_ft_s2();
            LinearVector getDirection();
            float_t getVerticalAngle_deg();
            float_t getVerticalAngle_rad();
            float_t getAngularVelocityMagnitude_deg_s();
            float_t getAngularVelocityMagnitude_rad_s();
        };

        struct Coordinate{
            float x, y;
        };

        struct GPSData{
            Coordinate position;
            float_t altitude;
            int_t satalites;
            int_t fixAge;
        };

        class GPSInterface : public PeripheralInterface{
        public:
            virtual GPSData getData() = 0;

            float getAltitude_ft();
            float getAltitude_m();
            Coordinate getPosition();

        };

        class TransmitterInterface : public PeripheralInterface{
        public:
            virtual error_t transmitString(const char*) = 0;
            virtual bool available() = 0;
        };

        class PowerCheckInterface : public PeripheralInterface{
        public:
            virtual float_t getVoltage() = 0;
        };

        class ArmSwitchInterface : public PeripheralInterface{
        public:
            virtual bool isOn() = 0;
        };
    }
}