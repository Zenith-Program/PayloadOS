#pragma once
#include "PayloadOSPeripheralInterfaces.h"

namespace PayloadOS{
    namespace Hardware{
        class AltimeterHardware : public Peripherals::AltimeterInterface{
        public:
            float_t getAltitude_m() override;
            float_t getPressure_mBar() override;
            float_t getTemperature_K() override;
        };

        class IMUHardware : public Peripherals::IMUInterface{
        public:
            Peripherals::EulerAngle getOrientation_eulerDeg() override;
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_deg_s() override;
            Peripherals::LinearVector getGravityVector() override;
        };

        class TransmitterHardware : public Peripherals::TransmitterInterface{
        public:
            error_t transmitString(const char*) override;
            bool available() override;
        };

        class GPSHardware : public Peripherals::GPSInterface{
        public:
            Peripherals::GPSData getData() override ;
        };

        class Altimeter2Hardware : public Peripherals::AltimeterInterface{
        public:
            float_t getAltitude_m() override;
            float_t getPressure_mBar() override;
            float_t getTemperature_K() override;
        };

        class STEMnaut1Hardware : public Peripherals::IMUInterface{
        public:
            Peripherals::EulerAngle getOrientation_eulerDeg() override;
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_deg_s() override;
            Peripherals::LinearVector getGravityVector() override;
        };

        class STEMnaut2Hardware : public Peripherals::IMUInterface{
        public:
            Peripherals::EulerAngle getOrientation_eulerDeg() override;
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_deg_s() override;
            Peripherals::LinearVector getGravityVector() override;
        };

        class STEMnaut3Hardware : public Peripherals::IMUInterface{
        public:
            Peripherals::EulerAngle getOrientation_eulerDeg() override;
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_deg_s() override;
            Peripherals::LinearVector getGravityVector() override;
        };

        class STEMnaut4Hardware : public Peripherals::IMUInterface{
        public:
            Peripherals::EulerAngle getOrientation_eulerDeg() override;
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_deg_s() override;
            Peripherals::LinearVector getGravityVector() override;
        };

        class PowerCheckHardware : public Peripherals::PowerCheckInterface{
        public:
            float_t getVoltage() override;
        };
    }
}