#include "PayloadOSStateMachine.h"
#include "PayloadOSSD.h"
#include "PayloadOSVariance.h"
#include "PayloadOSFlightParameters.h"
#include <Arduino.h>
#include <cstdio>

using namespace PayloadOS;
using namespace State;
/*
Temperature of the landing site
Apogee reached ^
Battery check ^
STEMnaut orientation ^
time of landing ^*
Maximum velocity ^
Landing velocity, g-forces
STEMnaut survivability ^*
*/

#define FEET_TO_M 0.30488

//Processing State---------------------------------------------
void Processing::init(){
    //check that files are open
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening event file");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open event file");
        }
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open event file in Processing");
    }
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening blackBox file");
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open blackBox file in Processing");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open blackBox file");
            FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Failed to reopen blackbox file");
        }
    }
    //begin processing analysis file
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->close();
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->openForRead() == PayloadOS::ERROR){
        Serial.println("Filed to open analysis file to read");
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Failed to open analysis file for processing");
    }
    //messaging
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Payload began processing");
    Serial.println("Processing");
}
void Processing::loop(){
    //Power check
    float_t power = Peripherals::PeripheralSelector::get()->getPowerCheck()->getVoltage();
    //Temperature of landing site
    float_t temperature = Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getTemperature_F();
    FlightData::TelemetryData telemetry;
    //STEMnaut orientation
    float_t o1 = Peripherals::PeripheralSelector::get()->getSTEMnaut1()->getVerticalAngle_deg();
    float_t o2 = Peripherals::PeripheralSelector::get()->getSTEMnaut1()->getVerticalAngle_deg();
    float_t o3 = Peripherals::PeripheralSelector::get()->getSTEMnaut1()->getVerticalAngle_deg();
    float_t o4 = Peripherals::PeripheralSelector::get()->getSTEMnaut1()->getVerticalAngle_deg();
    //parsing-----------------------------------------------
    uint_t launchTime = 0, landingTime = 0;
    float_t apogee = 0;
    float_t maxVelocity = 0;
    float_t maxAccel1 = 0, maxAccel2 = 0, maxAccel3 = 0, maxAccel4 = 0;
    float_t previousAltitude = 0;
    uint_t line = 0;
    uint_t previousTime = 0;
    //logs---------------
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
    //-------------------
    while(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->readLine(telemetry) != PayloadOS::ERROR && !telemetry.endOfFile){
        //time of landing
        float_t timeStamp = telemetry.time;
        if(telemetry.state == static_cast<uint_t>(States::Ascent) && launchTime == 0) launchTime = timeStamp;
        if(previousTime == 0) previousTime = timeStamp - ProgramState::get()->getPeriod() / 1000;
        //apogee
        float_t altitude =  (telemetry.altitude1 - FlightData::AltimeterVariances::getAltimeter1Zero() + telemetry.altitude2 - FlightData::AltimeterVariances::getAltimeter2Zero())/2.0;
        if(altitude > apogee) apogee = altitude;
        //maximum velocity
        float_t velocity = (altitude - previousAltitude) / (timeStamp - previousTime) * 1000;
        if(std::abs(velocity) > maxVelocity) maxVelocity = std::abs(velocity);
        previousAltitude = altitude;
        //update position
        line++;
        //stemnaut accelerations
        float_t acceleration1 = Peripherals::IMUInterface::magnitude(telemetry.accel1);
        float_t acceleration2 = Peripherals::IMUInterface::magnitude(telemetry.accel2);
        float_t acceleration3 = Peripherals::IMUInterface::magnitude(telemetry.accel3);
        float_t acceleration4 = Peripherals::IMUInterface::magnitude(telemetry.accel4);
        if(acceleration1 > maxAccel1) maxAccel1 = acceleration1;
        if(acceleration1 > maxAccel2) maxAccel2 = acceleration2;
        if(acceleration1 > maxAccel3) maxAccel3 = acceleration3;
        if(acceleration1 > maxAccel4) maxAccel4 = acceleration4;
        previousTime = timeStamp;
    }
    //logs---------------
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
    Serial.println("Finished first pass");
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Finished first processing pass");
    //------------------- 
    //landing velocity
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->close();
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->openForRead();
    uint_t LookBackTime_ms = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::MinimumLandingTime)->value * 1000;
    uint_t index;
    for(index = 0; index < line - LookBackTime_ms * 1000/State::ProgramState::get()->getPeriod(); index++)
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->readLine(telemetry);
    float_t altitude1 = (telemetry.altitude1 - FlightData::AltimeterVariances::getAltimeter1Zero() + telemetry.altitude2 - FlightData::AltimeterVariances::getAltimeter2Zero())/2.0;
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->readLine(telemetry);
    float_t altitude0 = (telemetry.altitude1 - FlightData::AltimeterVariances::getAltimeter1Zero() + telemetry.altitude2 - FlightData::AltimeterVariances::getAltimeter2Zero())/2.0;
    float_t landingVelocity = (altitude0 - altitude1)/(ProgramState::get()->getPeriod() / 1000000.0);
    //sustained g-force
    float_t landingG = Peripherals::IMUInterface::magnitude(telemetry.accel0);
    landingTime = telemetry.time;
    while(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->readLine(telemetry) != PayloadOS::ERROR && !telemetry.endOfFile){
        float_t accel = Peripherals::IMUInterface::magnitude(telemetry.accel0);
        if(accel > landingG){ 
            landingG =  accel;
            landingTime = telemetry.time;
        }
    }
    //logs---------------
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
    Serial.println("Finished second pass");
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Finished second processing pass");
    //------------------- 
    TransmittedData* data = Transmit::getData();
    data->apogee = apogee;
    data->temperature = temperature;
    data->timeOfLanding = (landingTime - launchTime)/1000.0;
    data->landingVelocity = landingVelocity;
    data->peakVelocity = maxVelocity;
    data->landingG = landingG * FEET_TO_M / 9.8;
    data->power = power;
    data->Orientation1 = o1;
    data->Orientation2 = o2;
    data->Orientation3 = o3;
    data->Orientation4 = o4;
    data->survive1 = maxAccel1;
    data->survive2 = maxAccel2;
    data->survive3 = maxAccel3;
    data->survive4 = maxAccel4;
}
void Processing::end(){
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->close() == PayloadOS::ERROR){
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Failed to close analysis file after processing");
        Serial.println("Failed to close analysis file");
    }
}

State::States Processing::next(){
    return States::Transmit; //for now
}

const Interpreter::CommandList* Processing::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

//Transmit State-----------------------------------------------
void Transmit::init(){
    //check that files are open
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening event file");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open event file");
        }
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open event file in Transmission");
    }
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening blackBox file");
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open blackBox file in Transmission");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open blackBox file");
            FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Failed to reopen blackbox file");
        }
    }
    //initialize step
    currentStep = Transmissions::PayloadStatus;
    //messaging
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Payload began transmitting");
    Serial.println("Transmit");
}
void Transmit::loop(){
    char transmission[50];
    constexpr float_t ACCELERATION_TOLERANCE = 9 * 9.8 / FEET_TO_M;
    TransmittedData* data = getData();
    Transmissions nextStep = currentStep;
    if(Peripherals::PeripheralSelector::get()->getTransmitter()->available()){
        switch(currentStep){
        case Transmissions::PayloadStatus:
            std::snprintf(transmission, sizeof(transmission), "Time: %.2fs, Temp: %.2fF, Power %.2fV\n", data->timeOfLanding, data->temperature, data->power);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::FlightParameters;
            else nextStep = Transmissions::PayloadStatus;
            break;
        case Transmissions::FlightParameters:
            std::snprintf(transmission, sizeof(transmission), "Apogee: %.2fft, Max velocity: %.2fft/s\n", data->apogee, data->peakVelocity);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::Landing;
            else nextStep = Transmissions::FlightParameters;
            break;
        case Transmissions::Landing:
            std::snprintf(transmission, sizeof(transmission), "Landing velocity: %.2fft/s, g-force: %.2fg\n", data->landingVelocity, data->landingG);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut1;
            else nextStep = Transmissions::Landing;
            break;
        case Transmissions::STEMnaut1:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut1 g-force: %.2fg, Survived: %s\n", data->survive1 * FEET_TO_M / 9.8, (data->survive1 * FEET_TO_M / 9.8 < ACCELERATION_TOLERANCE)? "yes" : "no");
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut2;
            else nextStep = Transmissions::STEMnaut1;
            break;
        case Transmissions::STEMnaut2:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut2 g-force: %.2fg, Survived: %s\n", data->survive2 * FEET_TO_M / 9.8, (data->survive2 * FEET_TO_M / 9.8 < ACCELERATION_TOLERANCE)? "yes" : "no");
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut3;
            else nextStep = Transmissions::STEMnaut2;
            break;
        case Transmissions::STEMnaut3:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut3 g-force: %.2fg, Survived: %s\n", data->survive3 * FEET_TO_M / 9.8, (data->survive3 * FEET_TO_M / 9.8 < ACCELERATION_TOLERANCE)? "yes" : "no");
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut4;
            else nextStep = Transmissions::STEMnaut3;
            break;
        case Transmissions::STEMnaut4:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut4 g-force: %.2fg, Survived: %s\n", data->survive4 * FEET_TO_M / 9.8, (data->survive4 * FEET_TO_M / 9.8 < ACCELERATION_TOLERANCE)? "yes" : "no");
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::DONE;
            else nextStep = Transmissions::STEMnaut4;
            break;
            default:
            nextStep = Transmissions::DONE;
            break;
        }
        currentStep = nextStep;
    }
    //log blackbox telemetry
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
}

void Transmit::end(){
    //NOP
}

State::States Transmit::next(){
    if(currentStep == Transmissions::DONE) return States::Recovery;
    return States::Transmit;
}
const Interpreter::CommandList* Transmit::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

TransmittedData* Transmit::getData(){
    return &data;
}
TransmittedData Transmit::data = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
Transmissions Transmit::currentStep = Transmissions::PayloadStatus;

//Recovery State----------------------------------------------------
bool Recovery::recovered;
void Recovery::init(){
    recovered = false;
    //check that files are open
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening event file");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open event file");
        }
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open event file in Processing");
    }
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->currentMode() != FlightData::SDStates::Write){
        Serial.println("Opening blackBox file");
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Had to re-open blackBox file in Processing");
        if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->openForWrite(FlightData::OpenTypes::End) == PayloadOS::ERROR){
            Serial.println("Failed to open blackBox file");
            FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Failed to reopen blackbox file");
        }
    }
    //messaging
    Serial.println("Recovery");
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Payload is awaiting recovery");
}
void Recovery::loop(){
    //log blackbox telemetry
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
}
void Recovery::end(){
    Serial.println("End of flight");
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Payload has been recovered");
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->close() == PayloadOS::ERROR){
        Serial.println("Failed to close blackBox file");
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Failed to close blackBox file");
    }
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->close() == PayloadOS::ERROR){
        Serial.println("Failed to close events file");
    }
}
State::States Recovery::next(){
    if(recovered) return States::Standby;
    return States::Recovery;
}

//commands
const Interpreter::CommandList* Recovery::getCommands(){
    static constexpr auto arr = std::array{
        CMD{"recover", "", recover_c}
    };
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

void Recovery::recover_c(const Interpreter::Token*){
    recovered = true;
}