#pragma once
#include "PayloadOSPeripheralInterfaces.h"

namespace PayloadOS{
    namespace Simulation{
        //General Backdoors------------------------------------
        class GeneralAltimeterBackdoor : public Peripherals::AltimeterInterface{
            float_t currentAltitude_m;
            float_t currentPressure_mBar;
            float_t currentTemperature_K;
        public:
            float_t getAltitude_m() override;
            float_t getPressure_mBar() override;
            float_t getTemperature_K() override;
        };

        class GeneralIMUBackdoor : public Peripherals::IMUInterface{
            Peripherals::EulerAngle currentOrientation_eulerDeg;
            Peripherals::LinearVector currentAcceleration_m_s2;
            Peripherals::RotationVector currentAngularVelocity_rad_s;
        public:
            Peripherals::EulerAngle getOrientation_eulerDeg() override;
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_deg_s() override;
            Peripherals::LinearVector getGravityVector() override;
        };

        //Backdoors for each peripheral------------------------
        using AltimeterBackdoor = GeneralAltimeterBackdoor;
        using Altimeter2Backdoor = GeneralAltimeterBackdoor;

        using IMUBackdoor = GeneralIMUBackdoor;
        using STEMnaut1Backdoor = GeneralIMUBackdoor;
        using STEMnaut2Backdoor = GeneralIMUBackdoor;
        using STEMnaut3Backdoor = GeneralIMUBackdoor;
        using STEMnaut4Backdoor = GeneralIMUBackdoor;

        class TransmitterBackdoor : public Peripherals::TransmitterInterface{
        public:
            error_t transmitString(const char*) override;
            bool available() override;
        };

        class GPSBackdoor : public Peripherals::GPSInterface{
        public:
            Peripherals::GPSData getData() override;
        };

        class PowerCheckBackdoor : public Peripherals::PowerCheckInterface{
        public:
            float_t getVoltage() override;
        };
    }
}