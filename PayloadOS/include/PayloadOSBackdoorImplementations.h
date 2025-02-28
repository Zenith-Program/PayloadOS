#pragma once
#include "PayloadOSPeripheralInterfaces.h"

namespace PayloadOS{
    namespace Simulation{
        class AltimeterBackdoor : public Peripherals::AltimeterInterface{
        public:
            float_t getAltitude_m() override;
            float_t getPressure_mBar() override;
            float_t getTemperature_K() override;
        };

        class IMUBackdoor : public Peripherals::IMUInterface{
        public:
            Peripherals::EulerAngle getOrientation_eulerDeg() override;
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_rad_s() override;
        };

        class TransmitterBackdoor : public Peripherals::TransmitterInterface{
        public:
            error_t transmitString(const char*) override;
            bool available() override;
        };

        class GPSBackdoor : public Peripherals::GPSInterface{
        public:
            Peripherals::GPSData getData() override;
        };

        class Altimeter2Backdoor : public Peripherals::AltimeterInterface{
        public:
            float_t getAltitude_m() override;
            float_t getPressure_mBar() override;
            float_t getTemperature_K() override;
        };

        class STEMnaut1Backdoor : public Peripherals::IMUInterface{
        public:
            Peripherals::EulerAngle getOrientation_eulerDeg() override;
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_rad_s() override;
        };

        class STEMnaut2Backdoor : public Peripherals::IMUInterface{
        public:
            Peripherals::EulerAngle getOrientation_eulerDeg() override;
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_rad_s() override;
        };

        class STEMnaut3Backdoor : public Peripherals::IMUInterface{
        public:
            Peripherals::EulerAngle getOrientation_eulerDeg() override;
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_rad_s() override;
        };

        class STEMnaut4Backdoor : public Peripherals::IMUInterface{
        public:
            Peripherals::EulerAngle getOrientation_eulerDeg() override;
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_rad_s() override;    
        };
    }
}