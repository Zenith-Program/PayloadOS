#include "PayloadOSBackdoorImplementations.h"

using namespace PayloadOS;
using namespace PayloadOS::Simulation;

//Altimeter1---------------------------------------------------
float_t AltimeterBackdoor::getAltitude_m(){
    return 0; //for now
}
float_t AltimeterBackdoor::getPressure_mBar(){
    return 0; //for now
}
float_t AltimeterBackdoor::getTemperature_K(){
    return 0; // for now
}

//IMU----------------------------------------------------------
Peripherals::EulerAngle IMUBackdoor::getOrientation_eulerDeg(){
    return {0,0,0}; //for now
}
Peripherals::LinearVector IMUBackdoor::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}
Peripherals::RotationVector IMUBackdoor::getAngularVelocity_rad_s(){
    return {0,0,0}; //for now
}

//STEMnaut1----------------------------------------------------
Peripherals::EulerAngle STEMnaut1Backdoor::getOrientation_eulerDeg(){
    return {0,0,0}; //for now
}
Peripherals::LinearVector STEMnaut1Backdoor::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}
Peripherals::RotationVector STEMnaut1Backdoor::getAngularVelocity_rad_s(){
    return {0,0,0}; //for now
}

//STEMnuat2----------------------------------------------------
Peripherals::EulerAngle STEMnaut2Backdoor::getOrientation_eulerDeg(){
    return {0,0,0}; //for now
}
Peripherals::LinearVector STEMnaut2Backdoor::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}
Peripherals::RotationVector STEMnaut2Backdoor::getAngularVelocity_rad_s(){
    return {0,0,0}; //for now
}

//STEMnaut3----------------------------------------------------
Peripherals::EulerAngle STEMnaut3Backdoor::getOrientation_eulerDeg(){
    return {0,0,0}; //for now
}
Peripherals::LinearVector STEMnaut3Backdoor::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}
Peripherals::RotationVector STEMnaut3Backdoor::getAngularVelocity_rad_s(){
    return {0,0,0}; //for now
}

//STEMnaut4----------------------------------------------------
Peripherals::EulerAngle STEMnaut4Backdoor::getOrientation_eulerDeg(){
    return {0,0,0}; //for now
}
Peripherals::LinearVector STEMnaut4Backdoor::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}
Peripherals::RotationVector STEMnaut4Backdoor::getAngularVelocity_rad_s(){
    return {0,0,0}; //for now
}

//GPS----------------------------------------------------------
Peripherals::GPSData GPSBackdoor::getData(){
    return {{0,0},0,0,0};
}

//Altimeter2---------------------------------------------------
float_t Altimeter2Backdoor::getAltitude_m(){
    return 0; //for now
}
float_t Altimeter2Backdoor::getPressure_mBar(){
    return 0; //for now
}
float_t Altimeter2Backdoor::getTemperature_K(){
    return 0; // for now
}

//Transmitter--------------------------------------------------
error_t TransmitterBackdoor::transmitString(const char*){
    return PayloadOS::GOOD;
}
bool TransmitterBackdoor::available(){
    return false;
}