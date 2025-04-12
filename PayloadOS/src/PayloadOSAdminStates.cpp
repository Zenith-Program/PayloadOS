#include "PayloadOSStateMachine.h"
#include <Arduino.h>
#include "PayloadOSPeripheralSelector.h"
#include "PayloadOSModelSimProcess.h"
#include "PayloadOSVariance.h"
#include "PayloadOSSD.h"
#include "PayloadOSFlightParameters.h"

#include "PayloadOSPersistent.h"

using namespace PayloadOS;
using namespace State;

//Debug State------------------------------------------------------------------

//global---------------------------------------
bool Debug::exit = false;
//state table implementation-------------------
void Debug::init(){
    Serial.println("Entered Debug Mode");
    exit = false;
    FlightData::AltimeterVariances::getAltimeter1()->setSize(8);
    FlightData::AltimeterVariances::getAltimeter2()->setSize(8);
    FlightData::AltimeterVariances::getAltimeter1()->clear();
    FlightData::AltimeterVariances::getAltimeter2()->clear();
}
void Debug::loop(){
    float_t altitude = Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getAltitude_ft();
    float_t altitude2 = Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getAltitude_ft();
    FlightData::AltimeterVariances::getAltimeter1()->push(altitude);
    FlightData::AltimeterVariances::getAltimeter2()->push(altitude2);
    Serial.print("alt1: ");
    Serial.println(altitude);
    //Serial.print("alt2: ");
    //Serial.println(altitude2);
    float_t variance = FlightData::AltimeterVariances::getAltimeter1()->getIdentityCovariance(1);
    float_t variance2 = FlightData::AltimeterVariances::getAltimeter2()->getIdentityCovariance(1);
    Serial.print("var1: ");
    Serial.println(variance);
    //Serial.print("var2: ");
    //Serial.println(variance2);
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
void saveTest_c(const Interpreter::Token*);
void loadTest_c(const Interpreter::Token*);

const Interpreter::CommandList* Debug::getCommands(){
    static constexpr auto arr = std::array{
        CMD{"exit", "", exitDebug},
        CMD{"saveTest", "", saveTest_c},
        CMD{"loadTest", "", loadTest_c}
    };
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

void Debug::exitDebug(const Interpreter::Token*){
    exit = true;
}

void saveTest_c(const Interpreter::Token*){
    Reset::PersistentData data;
    Reset::Persistent::fillData(data);
    data.active = false;
    data.step = 0;
    Reset::Persistent save(FlightData::SDFiles::get()->getCard());
    save.update(data);
}

void loadTest_c(const Interpreter::Token*){
    Reset::PersistentData data;
    Reset::Persistent save(FlightData::SDFiles::get()->getCard());
    save.read(data);
    Reset::Persistent::applyData(data);
}

//Standby State----------------------------------------------------------------

//global---------------------------------------
bool Standby::debug = false;
//state table implementation-------------------
void Standby::init(){
    debug = false;
    Serial.println("Standby");
}
void Standby::loop(){
    //NOP
}
void Standby::end(){
    //NOP
}
State::States Standby::next(){
    if(debug) return States::Debug;
    if(Armed::isArmed() && Peripherals::PeripheralSelector::get()->getArmSwitch()->isOn()) return States::Armed;
    return States::Standby;
}

//helpers--------------------------------------

//commands-------------------------------------
const Interpreter::CommandList* Standby::getCommands(){
    static constexpr auto arr = std::array{
        CMD{"debug", "", toDebug},
        CMD{"arm", "", Armed::softwareArm},
        CMD{"disarm", "", Armed::softwareDisarm},
        CMD{"initSD", "", FlightData::SDFiles::init_c},
        CMD{"fileName", "w", FlightData::SDFiles::getFilename_c},
        CMD{"setFileName", "ws", FlightData::SDFiles::setFileName_C},
        CMD{"fileFlush", "w", FlightData::SDFiles::getFlushPeriod_c},
        CMD{"setFileFlush", "wu", FlightData::SDFiles::setFlushPeriod_c},
        CMD{"displayFile", "w", FlightData::SDFiles::displayFile_c},
        CMD{"zeroAltimeter1", "", FlightData::AltimeterVariances::zeroAltimeter1},
        CMD{"zeroAltimeter2", "", FlightData::AltimeterVariances::zeroAltimeter2},
        CMD{"flightParameters", "", FlightData::FlightParameters::print_c},
        CMD{"setFlightParameter", "sf", FlightData::FlightParameters::setParam_c}
    };
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

void Standby::toDebug(const Interpreter::Token*){
    debug = true;
}

//Startup State----------------------------------------------------------------

//global---------------------------------------

//state table implementation-------------------
void Startup::init(){
    //NOP
}
void Startup::loop(){
    Serial.println("Starting Up...");
    initAllPeripherals();
    FlightData::SDFiles::get()->init();
    Serial.println("Startup Complete");
}
void Startup::end(){
    //NOP
}
State::States Startup::next(){
    return States::Standby;
}

//helpers--------------------------------------
error_t Startup::initAllPeripherals(){
    return Peripherals::PeripheralSelector::get()->initAll();
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