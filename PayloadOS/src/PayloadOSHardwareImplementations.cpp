#include "PayloadOSHardwareImplementations.h"
#include <Wire.h>

using namespace PayloadOS;
using namespace PayloadOS::Hardware;

#define AltimeterAdress 0x77
#define LightAPRSAdress 0x38
#define STEMnaut4Adress 0x4B

//Altimeter1---------------------------------------------------
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
    return PayloadOS::GOOD;
}

error_t AltimeterHardware::status(){
    Wire.beginTransmission(AltimeterAdress);
    if(Wire.endTransmission() == 0) return PayloadOS::GOOD;
    return PayloadOS::ERROR;
}

error_t AltimeterHardware::deInit(){
    Wire.end();
    return PayloadOS::GOOD;
}

//IMU----------------------------------------------------------

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
    if(imu.begin()) return PayloadOS::GOOD;
    return PayloadOS::ERROR;
}

error_t IMUHardware::status(){
    unsigned char a, b, c;
    imu.getSystemStatus(&a,&b,&c);
    if(c != 0) return PayloadOS::ERROR;
    return PayloadOS::GOOD;
}

error_t IMUHardware::deInit(){
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
    return PayloadOS::GOOD;
}

error_t STEMnaut1Hardware::status(){
    return PayloadOS::GOOD;
}

error_t STEMnaut1Hardware::deInit(){
    return PayloadOS::GOOD;
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
    return PayloadOS::GOOD;
}

error_t STEMnaut2Hardware::status(){
    return PayloadOS::GOOD;
}

error_t STEMnaut2Hardware::deInit(){
    return PayloadOS::GOOD;
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
    return PayloadOS::GOOD;
}

error_t STEMnaut3Hardware::status(){
    return PayloadOS::GOOD;
}

error_t STEMnaut3Hardware::deInit(){
    return PayloadOS::GOOD;
}


//STEMnaut4----------------------------------------------------

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
    return PayloadOS::GOOD;
}

error_t STEMnaut4Hardware::status(){
    Wire1.beginTransmission(STEMnaut4Adress);
    if(Wire1.endTransmission() == 0) return PayloadOS::GOOD;
    return PayloadOS::ERROR;
}

error_t STEMnaut4Hardware::deInit(){
    return PayloadOS::GOOD;
}

//GPS----------------------------------------------------------
Peripherals::GPSData GPSHardware::getData(){
    return {{0,0},0,0,0};
}

error_t GPSHardware::init(){
    return PayloadOS::GOOD;
}

error_t GPSHardware::status(){
    return PayloadOS::GOOD;
}

error_t GPSHardware::deInit(){
    return PayloadOS::GOOD;
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
    return PayloadOS::GOOD;
}

error_t Altimeter2Hardware::status(){
    return PayloadOS::GOOD;
}

error_t Altimeter2Hardware::deInit(){
    return PayloadOS::GOOD;
}

//Transmitter--------------------------------------------------
error_t TransmitterHardware::transmitString(const char*){
    return PayloadOS::GOOD;
}

bool TransmitterHardware::available(){
    return false;
}

error_t TransmitterHardware::init(){
    return PayloadOS::GOOD;
}

error_t TransmitterHardware::status(){
    return PayloadOS::GOOD;
}

error_t TransmitterHardware::deInit(){
    return PayloadOS::GOOD;
}

//Power check--------------------------------------------------
float_t PowerCheckHardware::getVoltage(){
    return 5;
}

error_t PowerCheckHardware::init(){
    Wire2.begin();
    return PayloadOS::GOOD;
}

error_t PowerCheckHardware::status(){
    Wire2.beginTransmission(LightAPRSAdress);
    if(Wire2.endTransmission() == 0) return PayloadOS::GOOD;
    return PayloadOS::ERROR;
}

error_t PowerCheckHardware::deInit(){
    Wire2.end();
    return PayloadOS::GOOD;
}

//arm switch---------------------------------------------------
bool ArmSwitchHardware::isOn(){
    return digitalRead(0) == HIGH;
}
error_t ArmSwitchHardware::init(){
    pinMode(0, INPUT);
    return PayloadOS::GOOD;
}
error_t ArmSwitchHardware::status(){
    return PayloadOS::GOOD;
}
error_t ArmSwitchHardware::deInit(){
    pinMode(0, INPUT_DISABLE);
    return PayloadOS::GOOD;
}
