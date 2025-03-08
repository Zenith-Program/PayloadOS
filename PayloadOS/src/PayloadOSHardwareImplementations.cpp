#include "PayloadOSHardwareImplementations.h"
#include <Wire.h>

using namespace PayloadOS;
using namespace PayloadOS::Hardware;

#define AltimeterAdress 0x77
#define LightAPRSAdress 0x38
#define STEMnaut4Adress 0x4B

#define FalseStatus {0,0,0,0}

//Altimeter1---------------------------------------------------
AltimeterHardware::AltimeterHardware() : init_m(false){}

float_t AltimeterHardware::getAltitude_m(){
    return 0; //for now
}

float_t AltimeterHardware::getPressure_mBar(){
    return 0; //for now
}

float_t AltimeterHardware::getTemperature_K(){
    return 0; // for now
}

error_t AltimeterHardware::init(){
    Wire.begin();
    init_m = true;
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus AltimeterHardware::status(){
    bool responsive = false;
    Wire.begin();
    Wire.beginTransmission(AltimeterAdress);
    if(Wire.endTransmission() == 0) responsive = true;
    return {init_m, responsive, init_m && responsive, init_m && responsive};
}

error_t AltimeterHardware::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

//IMU----------------------------------------------------------
#define IMU_MinimumCalibration 3
IMUHardware::IMUHardware() : init_m(false), imu(Adafruit_BNO055()){}

Peripherals::LinearVector IMUHardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}

Peripherals::RotationVector IMUHardware::getAngularVelocity_deg_s(){
    return {0,0,0}; //for now
}

Peripherals::LinearVector IMUHardware::getGravityVector(){
    return {0,0,0}; //for now
}

error_t IMUHardware::init(){
    Wire1.begin();
    imu = Adafruit_BNO055(55, 0x28, &Wire1);
    if(imu.begin()){
        init_m = true;
        return PayloadOS::GOOD;
    } 
    init_m = false;
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus IMUHardware::status(){
    uint8_t systemStatus, systemTest, systemError;
    if(!imu.begin()) return {init_m, false, false, false};
    imu.getSystemStatus(&systemStatus, &systemTest, &systemError);
    bool ready = systemStatus == 5 && systemTest == 0xF && systemError == 0;
    uint8_t system, gyro, accel, mag;
    imu.getCalibration(&system, &gyro, &accel, &mag);
    bool readyForLaunch = system >= IMU_MinimumCalibration && gyro >= IMU_MinimumCalibration && accel >= IMU_MinimumCalibration && mag >= IMU_MinimumCalibration;
    return {init_m, true, init_m && ready, init_m && readyForLaunch};
}

error_t IMUHardware::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}


//STEMnaut1----------------------------------------------------

Peripherals::LinearVector STEMnaut1Hardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}

Peripherals::RotationVector STEMnaut1Hardware::getAngularVelocity_deg_s(){
    return {0,0,0}; //for now
}
Peripherals::LinearVector STEMnaut1Hardware::getGravityVector(){
    return {0,0,0}; //for now
}

error_t STEMnaut1Hardware::init(){
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus STEMnaut1Hardware::status(){
    return FalseStatus;
}

error_t STEMnaut1Hardware::deInit(){
    return PayloadOS::ERROR;
}

//STEMnuat2----------------------------------------------------

Peripherals::LinearVector STEMnaut2Hardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}

Peripherals::RotationVector STEMnaut2Hardware::getAngularVelocity_deg_s(){
    return {0,0,0}; //for now
}

Peripherals::LinearVector STEMnaut2Hardware::getGravityVector(){
    return {0,0,0}; //for now
}

error_t STEMnaut2Hardware::init(){
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus STEMnaut2Hardware::status(){
    return FalseStatus;
}

error_t STEMnaut2Hardware::deInit(){
    return PayloadOS::ERROR;
}

//STEMnaut3----------------------------------------------------

Peripherals::LinearVector STEMnaut3Hardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}

Peripherals::RotationVector STEMnaut3Hardware::getAngularVelocity_deg_s(){
    return {0,0,0}; //for now
}

Peripherals::LinearVector STEMnaut3Hardware::getGravityVector(){
    return {0,0,0}; //for now
}

error_t STEMnaut3Hardware::init(){
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus STEMnaut3Hardware::status(){
    return FalseStatus;
}

error_t STEMnaut3Hardware::deInit(){
    return PayloadOS::ERROR;
}


//STEMnaut4----------------------------------------------------
STEMnaut4Hardware::STEMnaut4Hardware() : init_m(false){}

Peripherals::LinearVector STEMnaut4Hardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}

Peripherals::RotationVector STEMnaut4Hardware::getAngularVelocity_deg_s(){
    return {0,0,0}; //for now
}

Peripherals::LinearVector STEMnaut4Hardware::getGravityVector(){
    return {0,0,0}; //for now
}

error_t STEMnaut4Hardware::init(){
    Wire1.begin();
    init_m = true;
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus STEMnaut4Hardware::status(){
    bool responsive = false;
    Wire1.beginTransmission(STEMnaut4Adress);
    if(Wire1.endTransmission() == 0) responsive = true;
    return {init_m, responsive, false, false}; //for now
}

error_t STEMnaut4Hardware::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

//GPS----------------------------------------------------------
Peripherals::GPSData GPSHardware::getData(){
    return {{0,0},0,0,0};
}

error_t GPSHardware::init(){
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus GPSHardware::status(){
    return FalseStatus;
}

error_t GPSHardware::deInit(){
    return PayloadOS::ERROR;
}

//Altimeter2---------------------------------------------------
float_t Altimeter2Hardware::getAltitude_m(){
    return 0; //for now
}

float_t Altimeter2Hardware::getPressure_mBar(){
    return 0; //for now
}

float_t Altimeter2Hardware::getTemperature_K(){
    return 0; // for now
}

error_t Altimeter2Hardware::init(){
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus Altimeter2Hardware::status(){
    return FalseStatus;
}

error_t Altimeter2Hardware::deInit(){
    return PayloadOS::ERROR;
}

//Transmitter--------------------------------------------------
error_t TransmitterHardware::transmitString(const char*){
    return PayloadOS::ERROR;
}

bool TransmitterHardware::available(){
    return false;
}

error_t TransmitterHardware::init(){
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus TransmitterHardware::status(){
    return FalseStatus;
}

error_t TransmitterHardware::deInit(){
    return PayloadOS::ERROR;
}

//Power check--------------------------------------------------
PowerCheckHardware::PowerCheckHardware() : init_m(false){}

float_t PowerCheckHardware::getVoltage(){
    if(!init_m) return 0;
    return 5;
}

error_t PowerCheckHardware::init(){
    Wire2.begin();
    init_m = true;
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus PowerCheckHardware::status(){
    bool available = false;
    Wire2.begin();
    Wire2.beginTransmission(LightAPRSAdress);
    if(Wire2.endTransmission() == 0) available = true;
    return {init_m, available, false, false}; //for now
}

error_t PowerCheckHardware::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

//arm switch---------------------------------------------------
ArmSwitchHardware::ArmSwitchHardware() : init_m(false) {}

bool ArmSwitchHardware::isOn(){
    if(!init_m) return false;
    return digitalRead(0) == HIGH;
}
error_t ArmSwitchHardware::init(){
    pinMode(0, INPUT);
    init_m = true;
    return PayloadOS::GOOD;
}
Peripherals::PeripheralStatus ArmSwitchHardware::status(){
    return {init_m, init_m, init_m, init_m};
}
error_t ArmSwitchHardware::deInit(){
    pinMode(0, INPUT_DISABLE);
    init_m = false;
    return PayloadOS::GOOD;
}
