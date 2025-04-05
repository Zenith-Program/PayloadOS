#include "PayloadOSStateMachine.h"
#include "PayloadOSPeripheralSelector.h"
#include "PayloadOSVariance.h"
#include "PayloadOSSD.h"
#include "PayloadOSFlightParameters.h"
#include <Arduino.h>
#include <cstring>

using namespace PayloadOS;
using namespace State;

#define FLIGHT_STATES_SKIP_BLACKBOX

//Armed--------------------------------------------------------
bool Armed::armed = false;
bool Armed::liveMode = false;

void Armed::init(){
    liveMode = PayloadOS_DefaultToLiveMode;
    //clear time checks
    Ascent::clearFirstEntry();
    Descent::clearFirstEntry();
    //initialize detection system
    FlightData::AltimeterVariances::getAltimeter1()->setSize(FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::CovarianceWindowSize)->value);
    FlightData::AltimeterVariances::getAltimeter1()->clear();
    FlightData::AltimeterVariances::getAltimeter2()->setSize(FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::CovarianceWindowSize)->value);
    FlightData::AltimeterVariances::getAltimeter2()->clear();
    //begin blackbox recording
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->openForWrite(FlightData::OpenTypes::New) == PayloadOS::ERROR){
         Serial.println("Failed to open blackbox file");
         armed = false;
    }
    //reset analysis file
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->close();
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->openForWrite(FlightData::OpenTypes::New) == PayloadOS::ERROR){
        Serial.println("Failed to open analysis file");
        armed = false;
    }
    //open message file if not already
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->currentMode() != FlightData::SDStates::Write){
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->close();
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open event file");
            armed = false;
        }
    }
    if(armed){
        Serial.println("The payload is armed");
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Payload was armed");
    } 
}
void Armed::loop(){
    //push altimeter readings into detection system
    float_t altimeter1Reading = Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getAltitude_ft();
    float_t altimeter2Reading = Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getAltitude_ft();
    FlightData::AltimeterVariances::getAltimeter1()->push(altimeter1Reading);
    FlightData::AltimeterVariances::getAltimeter2()->push(altimeter2Reading);
    //log to blackbox
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
    //live altimeter data
    if(liveMode){
        Serial.print("A1: ");
        Serial.print(altimeter1Reading - FlightData::AltimeterVariances::getAltimeter1Zero());
        Serial.print(" A2: ");
        Serial.print(altimeter2Reading - FlightData::AltimeterVariances::getAltimeter1Zero());
    }
}

void Armed::end(){
    //NOP
}

State::States Armed::next(){
    if(!Peripherals::PeripheralSelector::get()->getArmSwitch()->isOn() || !armed){
        //close files if disarmed
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->close();
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->close();
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Payload was disarmed");
        return States::Standby;
    } 
    uint_t numOutliers = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::OutlierCount)->value;
    float_t upperThreshold = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::UpwardMotionThreshold)->value;
    float_t altimeter1Covariance = FlightData::AltimeterVariances::getAltimeter1()->getIdentityCovariance(numOutliers);
    float_t altimeter2Covariance = FlightData::AltimeterVariances::getAltimeter2()->getIdentityCovariance(numOutliers);
    //live data
    if(liveMode){
        Serial.print(" C1: ");
        Serial.print(altimeter1Covariance);
        Serial.print(" C2: ");
        Serial.println(altimeter2Covariance);
    }
    //choose next state
    if(altimeter1Covariance > upperThreshold && altimeter2Covariance > upperThreshold) return States::Ascent;
    return States::Armed;
}

bool Armed::isArmed(){
 return armed;
}

void Armed::setArm(bool newState){
    armed = newState;
}

//commands
const Interpreter::CommandList* Armed::getCommands(){
    static constexpr auto arr = std::array{
        CMD{"disarm", "", Armed::softwareDisarm},
        CMD{"liveMode", "w", Armed::liveMode_c}
    };
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

void Armed::softwareDisarm(const Interpreter::Token*){
    armed = false;
}

void Armed::softwareArm(const Interpreter::Token*){
    armed = true;
}

void Armed::liveMode_c(const Interpreter::Token* args){
    char choice[8];
    args[0].copyStringData(choice, 8);
    if(std::strcmp(choice, "enter") == 0){
        liveMode = true;
        return;
    }
    if(std::strcmp(choice, "exit") == 0){
        liveMode = false;
        return;
    }
    Serial.print("Invalid argument '");
    Serial.print(choice);
    Serial.println("'. Choose 'enter' or 'exit'");
}

//Ascent-------------------------------------------------------
bool Ascent::liveMode;
uint_t Ascent::firstEntry;

void Ascent::init(){
    liveMode = PayloadOS_DefaultToLiveMode;
    //update entryTime
    if(firstEntry == 0) firstEntry = millis();
    //check that files are open
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening event file");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open event file");
        }
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open event file in Ascent");
    }
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening blackBox file");
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open blackBox file in Ascent");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open blackBox file");
            FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Failed to reopen blackbox file");
        }
    }
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening analysis file");
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open analysis file in Ascent");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->openForWrite(FlightData::OpenTypes::New) == PayloadOS::ERROR){
            Serial.println("Failed to open analysis file");
            FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Failed to reopen analysis file");
        }
    }
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Payload detected ascent");
    Serial.println("Ascent");
}

void Ascent::loop(){
    //push altimeter readings to detection system
    float_t altimeter1Reading = Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getAltitude_ft();
    float_t altimeter2Reading = Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getAltitude_ft();
    FlightData::AltimeterVariances::getAltimeter1()->push(altimeter1Reading);
    FlightData::AltimeterVariances::getAltimeter2()->push(altimeter2Reading);
    //log to blackbox and analysis files
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->logLine();
    #ifndef FLIGHT_STATES_SKIP_BLACKBOX
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
    #endif
    //live altimeter data
    if(liveMode){
        Serial.print("A1: ");
        Serial.print(altimeter1Reading - FlightData::AltimeterVariances::getAltimeter1Zero());
        Serial.print(" A2: ");
        Serial.print(altimeter2Reading - FlightData::AltimeterVariances::getAltimeter1Zero());
    }
}

void Ascent::end(){
    //NOP
}

State::States Ascent::next(){
    //calculate identity covariance
    uint_t numOutliers = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::OutlierCount)->value;
    float_t altimeter1Covariance = FlightData::AltimeterVariances::getAltimeter1()->getIdentityCovariance(numOutliers);
    float_t altimeter2Covariance = FlightData::AltimeterVariances::getAltimeter2()->getIdentityCovariance(numOutliers);
    if(liveMode){
        Serial.print(" C1: ");
        Serial.print(altimeter1Covariance);
        Serial.print(" C2: ");
        Serial.println(altimeter2Covariance);
    }
    //get thresholds
    float_t upperThreshold = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::UpwardMotionThreshold)->value;
    float_t lowerThreshold = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::DownwardMotionThreshold)->value;
    //choose next state
    uint_t minimumExitTime = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::MinimumAscentTime)->value;
    if(millis() - firstEntry < 1000 * minimumExitTime){
        if(altimeter1Covariance > upperThreshold || altimeter2Covariance > upperThreshold){
            //upwards motion
            return States::Ascent;
        }
        else{
            Serial.println("False start detected");
            FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Payload detected false start");
            return States::Armed;
        }
    }
    else{
        if(altimeter1Covariance < lowerThreshold && altimeter2Covariance < lowerThreshold){
            //downawards motion
            return States::Descent;
        }
        else{
            //upwards or constant motion
            return States::Ascent;
        }
    }
}

//interface
void Ascent::clearFirstEntry(){
    firstEntry = 0;
}


//commands
const Interpreter::CommandList* Ascent::getCommands(){
    static constexpr auto arr = std::array{
        CMD{"liveMode", "w", liveMode_c}
    };
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

void Ascent::liveMode_c(const Interpreter::Token* args){
    char choice[8];
    args[0].copyStringData(choice, 8);
    if(std::strcmp(choice, "enter") == 0){
        liveMode = true;
        return;
    }
    if(std::strcmp(choice, "exit") == 0){
        liveMode = false;
        return;
    }
    Serial.print("Invalid argument '");
    Serial.print(choice);
    Serial.println("'. Choose 'enter' or 'exit'");
}

//Descent-------------------------------------------------------
bool Descent::liveMode;
uint_t Descent::firstEntry;

void Descent::init(){
    liveMode = PayloadOS_DefaultToLiveMode;
    //update entryTime
    if(firstEntry == 0) firstEntry = millis();
    //check that files are open
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening event file");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open event file");
        }
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open event file in Descent");
    }
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening blackBox file");
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open blackBox file in Descent");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open blackBox file");
            FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Failed to reopen blackbox file");
        }
    }
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening analysis file");
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open analysis file in Descent");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open analysis file");
            FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Failed to reopen analysis file");
        }
    }
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Payload detected descent");
    Serial.println("Descent");
}

void Descent::loop(){
    //push altimeter readings to detection system
    float_t altimeter1Reading = Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getAltitude_ft();
    float_t altimeter2Reading = Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getAltitude_ft();
    FlightData::AltimeterVariances::getAltimeter1()->push(altimeter1Reading);
    FlightData::AltimeterVariances::getAltimeter2()->push(altimeter2Reading);
    //log to blackbox and analysis files
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->logLine();
    #ifndef FLIGHT_STATES_SKIP_BLACKBOX
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
    #endif
    //live altimeter data
    if(liveMode){
        Serial.print("A1: ");
        Serial.print(altimeter1Reading - FlightData::AltimeterVariances::getAltimeter1Zero());
        Serial.print(" A2: ");
        Serial.print(altimeter2Reading - FlightData::AltimeterVariances::getAltimeter1Zero());
    }
}

void Descent::end(){
    //NOP
}

State::States Descent::next(){
    //calculate identity covariance
    uint_t numOutliers = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::OutlierCount)->value;
    float_t altimeter1Covariance = FlightData::AltimeterVariances::getAltimeter1()->getIdentityCovariance(numOutliers);
    float_t altimeter2Covariance = FlightData::AltimeterVariances::getAltimeter2()->getIdentityCovariance(numOutliers);
    if(liveMode){
        Serial.print(" C1: ");
        Serial.print(altimeter1Covariance);
        Serial.print(" C2: ");
        Serial.println(altimeter2Covariance);
    }
    //get thresholds
    float_t upperThreshold = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::UpwardMotionThreshold)->value;
    float_t lowerThreshold = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::DownwardMotionThreshold)->value;
    //choose next state
    uint_t minimumExitTime = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::MinimumDescentTime)->value;
    if(millis() - firstEntry < 1000 * minimumExitTime){
        if(altimeter1Covariance > upperThreshold && altimeter2Covariance > upperThreshold){
            //upwards motion
            Serial.println("False apogee detected");
            FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Payload detected false apogee");
            return States::Ascent;
        }
        else{
            return States::Descent;
        }
    }
    else{
        if(altimeter1Covariance < lowerThreshold && altimeter2Covariance < lowerThreshold){
            //downawards motion
            return States::Descent;
        }
        else{
            return States::Landing;
        }
    }
}

//interface
void Descent::clearFirstEntry(){
    firstEntry = 0;
}

//commands
const Interpreter::CommandList* Descent::getCommands(){
    static constexpr auto arr = std::array{
        CMD{"liveMode", "w", liveMode_c}
    };
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

void Descent::liveMode_c(const Interpreter::Token* args){
    char choice[8];
    args[0].copyStringData(choice, 8);
    if(std::strcmp(choice, "enter") == 0){
        liveMode = true;
        return;
    }
    if(std::strcmp(choice, "exit") == 0){
        liveMode = false;
        return;
    }
    Serial.print("Invalid argument '");
    Serial.print(choice);
    Serial.println("'. Choose 'enter' or 'exit'");
}

//Landing-------------------------------------------------------
uint_t Landing::entryTime;
bool Landing::liveMode;

void Landing::init(){
    liveMode = PayloadOS_DefaultToLiveMode;
    //update entryTime
    entryTime = millis();
    //check that files are open
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening event file");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open event file");
        }
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open event file in Landing");
    }
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening blackBox file");
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open blackBox file in Landing");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open blackBox file");
            FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Failed to reopen blackbox file");
        }
    }
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening analysis file");
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open analysis file in Landing");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open analysis file");
            FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Failed to reopen analysis file");
        }
    }
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Payload detected landing");
    Serial.println("Landing");
}

void Landing::loop(){
    //push altimeter readings to detection system
    float_t altimeter1Reading = Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getAltitude_ft();
    float_t altimeter2Reading = Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getAltitude_ft();
    FlightData::AltimeterVariances::getAltimeter1()->push(altimeter1Reading);
    FlightData::AltimeterVariances::getAltimeter2()->push(altimeter2Reading);
    //log to blackbox and analysis files
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->logLine();
    #ifndef FLIGHT_STATES_SKIP_BLACKBOX
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
    #endif
    //live altimeter data
    if(liveMode){
        Serial.print("A1: ");
        Serial.print(altimeter1Reading - FlightData::AltimeterVariances::getAltimeter1Zero());
        Serial.print(" A2: ");
        Serial.print(altimeter2Reading - FlightData::AltimeterVariances::getAltimeter1Zero());
    }
}

void Landing::end(){
    //NOP
}

State::States Landing::next(){
    //calculate identity covariance
    uint_t numOutliers = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::OutlierCount)->value;
    float_t altimeter1Covariance = FlightData::AltimeterVariances::getAltimeter1()->getIdentityCovariance(numOutliers);
    float_t altimeter2Covariance = FlightData::AltimeterVariances::getAltimeter2()->getIdentityCovariance(numOutliers);
    if(liveMode){
        Serial.print(" C1: ");
        Serial.print(altimeter1Covariance);
        Serial.print(" C2: ");
        Serial.println(altimeter2Covariance);
    }
    //get thresholds
    float_t lowerThreshold = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::DownwardMotionThreshold)->value;
    //choose next state
    uint_t minimumExitTime = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::MinimumLandingTime)->value;
    if(millis() - entryTime < 1000 * minimumExitTime){
        if(altimeter1Covariance < lowerThreshold && altimeter2Covariance < lowerThreshold){
            //downwards motion
            Serial.println("False landing detected");
            FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Payload detected false landing");
            return States::Descent;
        }
        else{
            return States::Landing;
        }
    }
    else{
        if(altimeter1Covariance < lowerThreshold && altimeter2Covariance < lowerThreshold){
            //downawards motion
            Serial.println("Late false landing detected");
            FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Payload detected late false landing");
            return States::Descent;
        }
        else{
            return States::Processing;
        }
    }
}

//commands
const Interpreter::CommandList* Landing::getCommands(){
    static constexpr auto arr = std::array{
        CMD{"liveMode", "w", liveMode_c}
    };
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

void Landing::liveMode_c(const Interpreter::Token* args){
    char choice[8];
    args[0].copyStringData(choice, 8);
    if(std::strcmp(choice, "enter") == 0){
        liveMode = true;
        return;
    }
    if(std::strcmp(choice, "exit") == 0){
        liveMode = false;
        return;
    }
    Serial.print("Invalid argument '");
    Serial.print(choice);
    Serial.println("'. Choose 'enter' or 'exit'");
}


