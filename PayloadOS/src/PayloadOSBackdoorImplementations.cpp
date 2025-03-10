#include "PayloadOSBackdoorImplementations.h"
#include "PayloadOSConsoleInterpreter.h"
#include <Arduino.h>

using namespace PayloadOS;
using namespace PayloadOS::Simulation;

#define FalseStatus {0,0,0,0}
#define EmptyVector {0,0,0}
#define EmptyGPS {{0,0}, 0,0,0}

//Altimeter---------------------------------------------------
GeneralAltimeterBackdoor::GeneralAltimeterBackdoor() : init_m(false), currentAltitude_m(0), currentPressure_mBar(0), currentTemperature_K(0){}

float_t GeneralAltimeterBackdoor::getAltitude_m(){
    if(!init_m) return 0;
    return currentAltitude_m;
}
float_t GeneralAltimeterBackdoor::getPressure_mBar(){
    if(!init_m) return 0;
    return currentPressure_mBar;
}
float_t GeneralAltimeterBackdoor::getTemperature_K(){
    if(!init_m) return 0;
    return currentTemperature_K;
}

error_t GeneralAltimeterBackdoor::init(){
    init_m = true;
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus GeneralAltimeterBackdoor::status(){
    return {init_m, true, init_m, false};
}

error_t GeneralAltimeterBackdoor::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

void GeneralAltimeterBackdoor::printReport(){
    Serial.println("Version: backdoor");
    Serial.print("Initialized: ");
    Serial.println((init_m)? "yes" : "no");
    bool units = Interpreter::ConsoleInterpreter::get()->getCurrentUnits();
    Serial.printf("Altitude: %d\nTemperature: %d\n Pressure: %d\n", (units)? getAltitude_ft() : getAltitude_m(), (units)? getTemperature_F() : getTemperature_C(), (units)? getPressure_psi() : getPressure_mBar());
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
GeneralIMUBackdoor::GeneralIMUBackdoor() : init_m(false), currentAcceleration_m_s2(EmptyVector), currentAngularVelocity_deg_s(EmptyVector), gravity(EmptyVector) {}
Peripherals::LinearVector GeneralIMUBackdoor::getAcceleration_m_s2(){
    if(!init_m) return EmptyVector;
    return currentAcceleration_m_s2;
}

Peripherals::RotationVector GeneralIMUBackdoor::getAngularVelocity_deg_s(){
    if(!init_m) return EmptyVector;
    return currentAngularVelocity_deg_s;
}

Peripherals::LinearVector GeneralIMUBackdoor::getGravityVector(){
    if(!init_m) return EmptyVector;
    return gravity;
}

error_t GeneralIMUBackdoor::init(){
    init_m = true;
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus GeneralIMUBackdoor::status(){
    return {init_m, true, init_m, false};
}

error_t GeneralIMUBackdoor::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

void GeneralIMUBackdoor::printReport(){
    Serial.println("Version: backdoor");
    Serial.print("Initialized: ");
    Serial.println((init_m)? "yes" : "no");
    Serial.print("Acceleration: ");
    if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
        printLinear(getAcceleration_ft_s2());
        Serial.println("ft/s^2");
    }
    else{
        printLinear(getAcceleration_m_s2());
        Serial.println("m/s^2");
    }
    Serial.print("Angular Velocity: ");
    if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
        printRotatation(getAngularVelocity_deg_s());
        Serial.println("deg/s");
    }
    else{
        printRotatation(getAngularVelocity_rad_s());
        Serial.println("rad/s");
    }
    Serial.print("Direction: ");
    printLinear(getDirection());
    Serial.println();
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
GPSBackdoor::GPSBackdoor() : init_m (false), data(EmptyGPS){}

Peripherals::GPSData GPSBackdoor::getData(){
    if(!init_m) return EmptyGPS;
    return data;
}

error_t GPSBackdoor::init(){
    init_m = true;
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus GPSBackdoor::status(){
    return {init_m, true, init_m, false};
}

error_t GPSBackdoor::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

void GPSBackdoor::printReport(){
    Serial.println("Version: backdoor");
    Serial.print("Initialized: ");
    Serial.println((init_m)? "yes" : "no");
    printGPSData(data);
}

//backdoor interface
void GPSBackdoor::setData(Peripherals::GPSData newData){
    data = newData;
}

//Transmitter--------------------------------------------------
TransmitterBackdoor::TransmitterBackdoor() : init_m(false){}

error_t TransmitterBackdoor::transmitString(const char* message){
    if(!init_m) return PayloadOS::ERROR;
    Serial.print("[Transmission] ");
    Serial.println(message);
    return PayloadOS::GOOD;
}
bool TransmitterBackdoor::available(){
    return init_m;
}

error_t TransmitterBackdoor::init(){
    init_m = true;
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus TransmitterBackdoor::status(){
    return {init_m, true, init_m, false};
}

error_t TransmitterBackdoor::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

void TransmitterBackdoor::printReport(){
    Serial.println("Version: backdoor");
    Serial.print("Initialized: ");
    Serial.println((init_m)? "yes" : "no");
    Serial.print("Available: ");
    Serial.println((available())? "yes" : "no");
}

//Power check--------------------------------------------------
PowerCheckBackdoor::PowerCheckBackdoor() : init_m(0), power(0){}

float_t PowerCheckBackdoor::getVoltage(){
    if(!init_m) return 0;
    return power;
}

error_t PowerCheckBackdoor::init(){
    init_m = true;
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus PowerCheckBackdoor::status(){
    return {init_m, true, init_m, false};
}

error_t PowerCheckBackdoor::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

void PowerCheckBackdoor::printReport(){
    Serial.println("Version: backdoor");
    Serial.print("Initialized: ");
    Serial.println((init_m)? "yes" : "no");
    Serial.print("Voltage: ");
    Serial.print(power);
    Serial.println("V");
}

//backdoor interface
void PowerCheckBackdoor::setPower(float_t newPower){
    power = newPower;
}

//arm switch---------------------------------------------------
ArmSwitchBackdoor::ArmSwitchBackdoor() : init_m(false), state(false){}

bool ArmSwitchBackdoor::isOn(){
    if(!init_m) return false;
    return state;
}

error_t ArmSwitchBackdoor::init(){
    init_m = true;
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus ArmSwitchBackdoor::status(){
    return {init_m, true, init_m, false};
}

error_t ArmSwitchBackdoor::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

void ArmSwitchBackdoor::printReport(){
    Serial.println("Version: backdoor");
    Serial.print("Initialized: ");
    Serial.println((init_m)? "yes" : "no");
    Serial.print("State: ");
    Serial.println((state)? "on" : "off");
}

//backdoor interface
void ArmSwitchBackdoor::setState(bool newState){
    state = newState;
}