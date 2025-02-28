#include "PayloadOSHardwareImplementations.h"

using namespace PayloadOS;
using namespace PayloadOS::Hardware;

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

//IMU----------------------------------------------------------
Peripherals::EulerAngle IMUHardware::getOrientation_eulerDeg(){
    return {0,0,0}; //for now
}
Peripherals::LinearVector IMUHardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}
Peripherals::RotationVector IMUHardware::getAngularVelocity_rad_s(){
    return {0,0,0}; //for now
}

//STEMnaut1----------------------------------------------------
Peripherals::EulerAngle STEMnaut1Hardware::getOrientation_eulerDeg(){
    return {0,0,0}; //for now
}
Peripherals::LinearVector STEMnaut1Hardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}
Peripherals::RotationVector STEMnaut1Hardware::getAngularVelocity_rad_s(){
    return {0,0,0}; //for now
}

//STEMnuat2----------------------------------------------------
Peripherals::EulerAngle STEMnaut2Hardware::getOrientation_eulerDeg(){
    return {0,0,0}; //for now
}
Peripherals::LinearVector STEMnaut2Hardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}
Peripherals::RotationVector STEMnaut2Hardware::getAngularVelocity_rad_s(){
    return {0,0,0}; //for now
}

//STEMnaut3----------------------------------------------------
Peripherals::EulerAngle STEMnaut3Hardware::getOrientation_eulerDeg(){
    return {0,0,0}; //for now
}
Peripherals::LinearVector STEMnaut3Hardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}
Peripherals::RotationVector STEMnaut3Hardware::getAngularVelocity_rad_s(){
    return {0,0,0}; //for now
}

//STEMnaut4----------------------------------------------------
Peripherals::EulerAngle STEMnaut4Hardware::getOrientation_eulerDeg(){
    return {0,0,0}; //for now
}
Peripherals::LinearVector STEMnaut4Hardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}
Peripherals::RotationVector STEMnaut4Hardware::getAngularVelocity_rad_s(){
    return {0,0,0}; //for now
}

//GPS----------------------------------------------------------
Peripherals::GPSData GPSHardware::getData(){
    return {{0,0},0,0,0};
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

//Transmitter--------------------------------------------------
error_t TransmitterHardware::transmitString(const char*){
    return PayloadOS::GOOD;
}
bool TransmitterHardware::available(){
    return false;
}