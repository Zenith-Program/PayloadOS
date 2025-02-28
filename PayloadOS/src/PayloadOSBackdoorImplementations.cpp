#include "PayloadOSBackdoorImplementations.h"

using namespace PayloadOS;
using namespace PayloadOS::Simulation;

//Altimeter---------------------------------------------------
float_t GeneralAltimeterBackdoor::getAltitude_m(){
    return 0; //for now
}
float_t GeneralAltimeterBackdoor::getPressure_mBar(){
    return 0; //for now
}
float_t GeneralAltimeterBackdoor::getTemperature_K(){
    return 0; // for now
}

//IMU----------------------------------------------------------
Peripherals::EulerAngle GeneralIMUBackdoor::getOrientation_eulerDeg(){
    return {0,0,0}; //for now
}
Peripherals::LinearVector GeneralIMUBackdoor::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}
Peripherals::RotationVector GeneralIMUBackdoor::getAngularVelocity_rad_s(){
    return {0,0,0}; //for now
}

//GPS----------------------------------------------------------
Peripherals::GPSData GPSBackdoor::getData(){
    return {{0,0},0,0,0};
}

//Transmitter--------------------------------------------------
error_t TransmitterBackdoor::transmitString(const char*){
    return PayloadOS::GOOD;
}
bool TransmitterBackdoor::available(){
    return false;
}

//Power check--------------------------------------------------
float_t PowerCheckBackdoor::getVoltage(){
    return 5;
}