#include "PayloadOSBackdoorImplementations.h"
#include <Arduino.h>

using namespace PayloadOS;
using namespace PayloadOS::Simulation;

//Altimeter---------------------------------------------------
float_t GeneralAltimeterBackdoor::getAltitude_m(){
    return currentAltitude_m;
}
float_t GeneralAltimeterBackdoor::getPressure_mBar(){
    return currentPressure_mBar;
}
float_t GeneralAltimeterBackdoor::getTemperature_K(){
    return currentTemperature_K;
}

error_t GeneralAltimeterBackdoor::init(){
    return PayloadOS::GOOD;
}

error_t GeneralAltimeterBackdoor::status(){
    return PayloadOS::GOOD;
}

error_t GeneralAltimeterBackdoor::deInit(){
    return PayloadOS::GOOD;
}

//backdoor interface
void GeneralAltimeterBackdoor::setCurrentAltitude_m(float_t newAltitude){
    currentAltitude_m = newAltitude;
}

void GeneralAltimeterBackdoor::setCurrrentPressure_mBar(float_t newPressure){
    currentPressure_mBar = newPressure;
}

void GeneralAltimeterBackdoor::setCurrentTemperature_K(float_t newTemp){
    currentTemperature_K = newTemp;
}

//IMU----------------------------------------------------------
Peripherals::LinearVector GeneralIMUBackdoor::getAcceleration_m_s2(){
    return currentAcceleration_m_s2;
}

Peripherals::RotationVector GeneralIMUBackdoor::getAngularVelocity_deg_s(){
    return currentAngularVelocity_deg_s;
}

Peripherals::LinearVector GeneralIMUBackdoor::getGravityVector(){
    return gravity;
}

error_t GeneralIMUBackdoor::init(){
    return PayloadOS::GOOD;
}

error_t GeneralIMUBackdoor::status(){
    return PayloadOS::GOOD;
}

error_t GeneralIMUBackdoor::deInit(){
    return PayloadOS::GOOD;
}

//backdoor interface
void GeneralIMUBackdoor::setCurrentAcceleration_m_s2(Peripherals::LinearVector newAcceleration){
    currentAcceleration_m_s2 = newAcceleration;
}
void GeneralIMUBackdoor::setCurrentAngularVelocity_deg_s(Peripherals::RotationVector newAngularVelocity){
    currentAngularVelocity_deg_s = newAngularVelocity;
}
void GeneralIMUBackdoor::setGravity(Peripherals::LinearVector newGravity){
    gravity = newGravity;
}
//GPS----------------------------------------------------------
Peripherals::GPSData GPSBackdoor::getData(){
    return data;
}

error_t GPSBackdoor::init(){
    return PayloadOS::GOOD;
}

error_t GPSBackdoor::status(){
    return PayloadOS::GOOD;
}

error_t GPSBackdoor::deInit(){
    return PayloadOS::GOOD;
}

//backdoor interface
void GPSBackdoor::setData(Peripherals::GPSData newData){
    data = newData;
}

//Transmitter--------------------------------------------------
error_t TransmitterBackdoor::transmitString(const char* message){
    Serial.print("[Transmission] ");
    Serial.println(message);
    return PayloadOS::GOOD;
}
bool TransmitterBackdoor::available(){
    return true;
}

error_t TransmitterBackdoor::init(){
    return PayloadOS::GOOD;
}

error_t TransmitterBackdoor::status(){
    return PayloadOS::GOOD;
}

error_t TransmitterBackdoor::deInit(){
    return PayloadOS::GOOD;
}

//Power check--------------------------------------------------
float_t PowerCheckBackdoor::getVoltage(){
    return power;
}

error_t PowerCheckBackdoor::init(){
    return PayloadOS::GOOD;
}

error_t PowerCheckBackdoor::status(){
    return PayloadOS::GOOD;
}

error_t PowerCheckBackdoor::deInit(){
    return PayloadOS::GOOD;
}

//backdoor interface
void PowerCheckBackdoor::setPower(float_t newPower){
    power = newPower;
}

//arm switch---------------------------------------------------
bool ArmSwitchBackdoor::isOn(){
    return state;
}

error_t ArmSwitchBackdoor::init(){
    return PayloadOS::GOOD;
}

error_t ArmSwitchBackdoor::status(){
    return PayloadOS::GOOD;
}

error_t ArmSwitchBackdoor::deInit(){
    return PayloadOS::GOOD;
}

//backdoor interface
void ArmSwitchBackdoor::setState(bool newState){
    state = newState;
}