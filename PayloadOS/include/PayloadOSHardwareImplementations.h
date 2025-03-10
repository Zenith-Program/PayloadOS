#pragma once
#include "PayloadOSPeripheralInterfaces.h"
#include "Adafruit_BNO055.h"
#include "SparkFun_BNO080_Arduino_Library.h"
#include <MS5607.h>

namespace PayloadOS{
    namespace Hardware{
        class AltimeterHardware : public Peripherals::AltimeterInterface{
            MS5607 altimeter;
            bool init_m;
            float_t lastAltitude, lastTemp, lastPressure;
            uint_t lastUpdateTime;
        public:
            AltimeterHardware();
            float_t getAltitude_m() override;
            float_t getPressure_mBar() override;
            float_t getTemperature_K() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;
        private:
            error_t updateReadings();
            error_t updateInit();
        };

        class IMUHardware : public Peripherals::IMUInterface{
            bool init_m;
            Adafruit_BNO055 imu;
        public:
            IMUHardware();
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_deg_s() override;
            Peripherals::LinearVector getGravityVector() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;

            
        private:
            error_t updateInitStatus(int_t = -1);
            static const char* getStatusMeaning(uint8_t);
            static const char* getErrorMeaning(uint8_t);
        };

        class TransmitterHardware : public Peripherals::TransmitterInterface{
        public:
            error_t transmitString(const char*) override;
            bool available() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;
        };

        class GPSHardware : public Peripherals::GPSInterface{
        public:
            Peripherals::GPSData getData() override ;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;
        };

        class Altimeter2Hardware : public Peripherals::AltimeterInterface{
        public:
            float_t getAltitude_m() override;
            float_t getPressure_mBar() override;
            float_t getTemperature_K() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;
        };

        class STEMnaut1Hardware : public Peripherals::IMUInterface{
        public:
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_deg_s() override;
            Peripherals::LinearVector getGravityVector() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;
        };

        class STEMnaut2Hardware : public Peripherals::IMUInterface{
        public:
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_deg_s() override;
            Peripherals::LinearVector getGravityVector() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;
        };

        class STEMnaut3Hardware : public Peripherals::IMUInterface{
        public:
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_deg_s() override;
            Peripherals::LinearVector getGravityVector() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;
        };

        class STEMnaut4Hardware : public Peripherals::IMUInterface{
            bool init_m;
            BNO080 imu;
            Peripherals::LinearVector acceleration;
            Peripherals::RotationVector angularVelocity;
            Peripherals::LinearVector gravity;
        public:
            STEMnaut4Hardware();
            Peripherals::LinearVector getAcceleration_m_s2() override;
            Peripherals::RotationVector getAngularVelocity_deg_s() override;
            Peripherals::LinearVector getGravityVector() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit() override;
            void printReport() override;
        private:
            bool updateReadings();
            uint_t updateInitStatus();
            static const char* getResetMeaning(uint_t);
        };

        class PowerCheckHardware : public Peripherals::PowerCheckInterface{
            bool init_m;
        public:
            PowerCheckHardware();
            float_t getVoltage() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit()override;
            void printReport() override;
        };

        class ArmSwitchHardware : public Peripherals::ArmSwitchInterface{
            bool init_m;
        public:
            ArmSwitchHardware();
            bool isOn() override;
            error_t init() override;
            Peripherals::PeripheralStatus status() override;
            error_t deInit() override;
            void printReport() override;
        };
    }
}