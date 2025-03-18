#include "PayloadOSStateMachine.h"
#include <Arduino.h>
#include "PayloadOSPeripheralSelector.h"
#include "PayloadOSModelSimProcess.h"

using namespace PayloadOS;
using namespace State;

//Debug State------------------------------------------------------------------

//global---------------------------------------
bool Debug::exit = false;
//state table implementation-------------------
void Debug::init(){
    Serial.println("Entered Debug Mode");
    exit = false;
<<<<<<< Updated upstream
    

=======
    FlightData::AltimeterVariances::getAltimeter1()->setSize(8);
    FlightData::AltimeterVariances::getAltimeter2()->setSize(8);
    FlightData::AltimeterVariances::getAltimeter1()->clear();
    FlightData::AltimeterVariances::getAltimeter2()->clear();
    Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString("test message");
>>>>>>> Stashed changes
}
void Debug::loop(){
    // do nothing
    Serial.println(Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getAltitude_ft());
}
void Debug::end(){
    Serial.println("Exited Debug Mode");
}
State::States Debug::next(){
    if(exit) return States::Standby;
    return States::Debug;
}

//helpers--------------------------------------

//commands-------------------------------------
const Interpreter::CommandList* Debug::getCommands(){
    static constexpr auto arr = std::array{
        CMD{"exit", "", exitDebug}
    };
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

void Debug::exitDebug(const Interpreter::Token*){
    exit = true;
}

//Standby State----------------------------------------------------------------

//global---------------------------------------
bool Standby::armed = false;
bool Standby::debug = false;
//state table implementation-------------------
void Standby::init(){
    debug = false;
    armed = false;
}
void Standby::loop(){

}
void Standby::end(){

}
State::States Standby::next(){
    if(debug) return States::Debug;
    if(armed && Peripherals::PeripheralSelector::get()->getArmSwitch()->isOn()) return States::Armed;
    return States::Standby;
}

//helpers--------------------------------------

//commands-------------------------------------
const Interpreter::CommandList* Standby::getCommands(){
    static constexpr auto arr = std::array{
        CMD{"debug", "", toDebug},
        CMD{"arm", "", softwareArm},
        CMD{"disarm", "", softwareDisarm}
    };
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

void Standby::toDebug(const Interpreter::Token*){
    debug = true;
}
void Standby::softwareArm(const Interpreter::Token*){
    armed = true;
}
void Standby::softwareDisarm(const Interpreter::Token*){
    armed = false;
}

//Startup State----------------------------------------------------------------

//global---------------------------------------

//state table implementation-------------------
void Startup::init(){
    Serial.println("Starting Up...");
    if(initAllPeripherals() == PayloadOS::ERROR) State::ProgramState::get()->initiateFailure();
}
void Startup::loop(){
    //NOP
}
void Startup::end(){
    Serial.println("Startup Complete");
}
State::States Startup::next(){
    return States::Standby;
}

//helpers--------------------------------------
error_t Startup::initAllPeripherals(){
    if(Peripherals::PeripheralSelector::get()->getArmSwitch()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getPayloadIMU()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getTransmitter()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getGPS()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getSTEMnaut1()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getSTEMnaut2()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getSTEMnaut3()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getSTEMnaut4()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    return PayloadOS::GOOD;
}

//commands-------------------------------------
const Interpreter::CommandList* Startup::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

//Fail State-------------------------------------------------------------------

//global---------------------------------------

//state table implementation-------------------
void Fail::init(){
    Serial.println("Entered Fail State");
}
void Fail::loop(){
    //do nothing for now
}
void Fail::end(){
    //do nothing
}
State::States Fail::next(){
    return States::Fail;
}

//helpers--------------------------------------

//commands-------------------------------------
const Interpreter::CommandList* Fail::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

//-----------------------------------------------------------------------------