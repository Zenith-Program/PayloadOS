#pragma once 
#include "PayloadOSGeneral.h"

namespace PayloadOS{
    namespace Peripherals{
        enum class PeripheralNames{
            PayloadAltimeter, PayloadIMU, STEMnaut1, STEMnaut2, STEMnaut3, STEMnaut4, GPS, LightAPRSAltimeter, Transmitter, PowerCheck, SENTINAL_COUNT
        };
        #define PayloadOS_NumberOfPeripherals static_cast<uint_t>(PeripheralNames::SENTINAL_COUNT)

        class AltimeterInterface{
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

        class IMUInterface{
            public:
            virtual EulerAngle getOrientation_eulerDeg() = 0;
            virtual LinearVector getAcceleration_m_s2() = 0;
            virtual RotationVector getAngularVelocity_rad_s() = 0;

            EulerAngle getOrientation_eulerRad();
            LinearVector getOrientation_unitVec();
            LinearVector getAcceleration_ft_s2();
            RotationVector getAngularVelocity_deg_s();
            float_t getAccelerationMagnitude();
            float_t getHorizonAngle();
            float_t getAngularVelocityMagnitude();
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

        class GPSInterface{
            public:
            virtual GPSData getData() = 0;

            float getAltitude_ft();
            float getAltitude_m();
            Coordinate getPosition();

        };

        class TransmitterInterface{
            public:
            virtual error_t transmitString(const char*) = 0;
            virtual bool available() = 0;
        };

        class PowerCheckInterface{
            public:
            virtual float_t getVoltage() = 0;
        };
    }
}