#pragma once
#include "PayloadOSPeripheralInterfaces.h"

namespace PayloadOS{
    namespace Simulation{
        //General Backdoors------------------------------------
        class GeneralAltimeterBackdoor : public Peripherals::AltimeterInterface{
            bool init_m;
            float_t currentAltitude_m;
            float_t currentPressure_mBar;
            float_t currentTemperature_K;
        public:
            GeneralAltimeterBackdoor();
            float_t getAltitude_m() override;
            float_t getPressure_mBar() override;
            float_t getTemperature_K() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit() override;
            void printReport() override;
            //backdoor interface
            void setCurrentAltitude_m(float_t);
            void setCurrrentPressure_mBar(float_t);
            void setCurrentTemperature_K(float_t);
        };

        class GeneralIMUBackdoor : public Peripherals::IMUInterface{
            bool init_m;
            Peripherals::LinearVector currentAcceleration_m_s2;
            Peripherals::RotationVector currentAngularVelocity_deg_s;
            Peripherals::LinearVector gravity;
        public:
            GeneralIMUBackdoor();
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_deg_s() override;
            Peripherals::LinearVector getGravityVector() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;
            //backdoor interface
            void setCurrentAcceleration_m_s2(Peripherals::LinearVector);
            void setCurrentAngularVelocity_deg_s(Peripherals::RotationVector);
            void setGravity(Peripherals::LinearVector);
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
            bool init_m;
        public:
            TransmitterBackdoor();
            error_t transmitString(const char*) override;
            bool available() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;
        };

        class GPSBackdoor : public Peripherals::GPSInterface{
            bool init_m;
            Peripherals::GPSData data;
        public:
            GPSBackdoor();
            Peripherals::GPSData getData() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;
            //backdoor interface
            void setData(Peripherals::GPSData);
        };

        class PowerCheckBackdoor : public Peripherals::PowerCheckInterface{
            bool init_m;
            float_t power;
        public:
            PowerCheckBackdoor();
            float_t getVoltage() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;
            //backdoor interface
            void setPower(float_t);
        };

        class ArmSwitchBackdoor : public Peripherals::ArmSwitchInterface{
            bool init_m;
            bool state;
        public:
            ArmSwitchBackdoor();
            bool isOn() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;
            //backdoor interface
            void setState(bool);
        };
    }
}