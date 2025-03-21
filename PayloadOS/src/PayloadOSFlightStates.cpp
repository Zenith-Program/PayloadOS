#include "PayloadOSStateMachine.h"
#include "PayloadOSPeripheralSelector.h"
#include "PayloadOSVariance.h"
#include "PayloadOSSD.h"
#include <Arduino.h>

using namespace PayloadOS;
using namespace State;

//Armed--------------------------------------------------------
bool Armed::armed = true;
uint_t Armed::varianceSize = Armed::defaultVarianceSize;
float_t Armed::launchThreshold = Armed::defaultLaunchThreshold;
void Armed::init(){
    armed = true;
    FlightData::AltimeterVariances::getAltimeter1()->setSize(varianceSize);
    FlightData::AltimeterVariances::getAltimeter1()->clear();
    FlightData::AltimeterVariances::getAltimeter2()->setSize(varianceSize);
    FlightData::AltimeterVariances::getAltimeter2()->clear();
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->openForWrite();
    Serial.println("The payload is armed");
}
void Armed::loop(){
    float_t altimeter1Reading = Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getAltitude_ft();
    float_t altimeter2Reading = Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getAltitude_ft();
    FlightData::AltimeterVariances::getAltimeter1()->push(altimeter1Reading);
    FlightData::AltimeterVariances::getAltimeter2()->push(altimeter2Reading);
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->logLine();
}
void Armed::end(){
    //NOP
}
State::States Armed::next(){
    if(!Peripherals::PeripheralSelector::get()->getArmSwitch()->isOn() || !armed) return States::Standby;
    if(FlightData::AltimeterVariances::getAltimeter1()->getStandardDeviation() > launchThreshold && FlightData::AltimeterVariances::getAltimeter2()->getStandardDeviation() > launchThreshold) return States::Flight;
    return States::Armed;
}

//commands
const Interpreter::CommandList* Armed::getCommands(){
    static constexpr auto arr = std::array{
        CMD{"disarm", "", Armed::softwareDisarm}
    };
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

void Armed::softwareDisarm(const Interpreter::Token*){
    armed = false;
}

void Armed::setVarSize(const Interpreter::Token* args){
    uint_t newSize = args[0].getUnsignedData();
    if(newSize < 1) Serial.println("Invalid size");
    else varianceSize = newSize;
}
void Armed::setThreshold(const Interpreter::Token* args){
    float_t newThreshold = args[0].getFloatData();
    if(newThreshold <= 0) Serial.println("launch threshold must be positive");
    else launchThreshold = newThreshold;
}
void Armed::getVarSize(const Interpreter::Token*){
    Serial.println(varianceSize);
}
void Armed::getThreshold(const Interpreter::Token*){
    Serial.println(launchThreshold);
}

//Flight--------------------------------------------------------
float_t Flight::landingThreshold = Flight::defaultLandingThreshold;
void Flight::init(){
    Serial.println("Flight");
}
void Flight::loop(){
    float_t altimeter1Reading = Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getAltitude_ft();
    float_t altimeter2Reading = Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getAltitude_ft();
    FlightData::AltimeterVariances::getAltimeter1()->push(altimeter1Reading);
    FlightData::AltimeterVariances::getAltimeter2()->push(altimeter2Reading);
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->logLine();
}
void Flight::end(){

}
State::States Flight::next(){
    if(FlightData::AltimeterVariances::getAltimeter1()->getStandardDeviation() < landingThreshold && FlightData::AltimeterVariances::getAltimeter2()->getStandardDeviation() < landingThreshold) return States::Processing;
    return States::Flight;
}
const Interpreter::CommandList* Flight::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}