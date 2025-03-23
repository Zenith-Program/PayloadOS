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
    FlightData::RunningVariance velocityVariance(FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::CovarianceWindowSize)->value);
    velocityVariance.clear();
    float_t maxVelocity = 0;
    float_t maxAccel1 = 0, maxAccel2 = 0, maxAccel3 = 0, maxAccel4 = 0;
    uint_t line = 0;
    uint_t landingLine = 0;
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
        velocityVariance.push(altitude);
        float_t velocity = velocityVariance.getIdentityCovariance(FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::OutlierCount)->value);
        if(std::abs(velocity) > maxVelocity) maxVelocity = std::abs(velocity);
        //update position
        if(telemetry.state < static_cast<uint_t>(States::Landing)) landingLine = line;
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
    maxVelocity *= (1000000.0/State::ProgramState::get()->getPeriod());
    //logs---------------
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
    Serial.println("Finished first pass");
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->logMessage("Finished first processing pass");
    //------------------- 
    //landing velocity & g-force
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->close();
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->openForRead();
    uint_t landingDetectionTime_ms = FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::MinimumLandingTime)->value * 1000;
    uint_t LookBackTime_ms = 6*landingDetectionTime_ms;
    uint_t index;
    //traverse to just before landing
    for(index = 0; index < landingLine - LookBackTime_ms * 1000/State::ProgramState::get()->getPeriod(); index++){
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->readLine(telemetry);
    }
    //get landing velocity from covariance just before landing
    uint_t covarianceSize = landingDetectionTime_ms * 1000/State::ProgramState::get()->getPeriod();
    velocityVariance.setSize(covarianceSize);
    velocityVariance.clear();
    for(uint_t i=0; i<covarianceSize; i++){
        FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->readLine(telemetry);
        float_t altitude = (telemetry.altitude1 - FlightData::AltimeterVariances::getAltimeter1Zero() + telemetry.altitude2 - FlightData::AltimeterVariances::getAltimeter2Zero())/2.0;
        velocityVariance.push(altitude);
    }
    float_t landingVelocity = (1000000.0/State::ProgramState::get()->getPeriod()) * std::abs(velocityVariance.getIdentityCovariance(1));
    //look for landing time
    landingTime = telemetry.time;
    //look for landing G
    float_t landingG = Peripherals::IMUInterface::magnitude(telemetry.accel0);
    while(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->readLine(telemetry) != PayloadOS::ERROR && !telemetry.endOfFile){
        float_t accel = Peripherals::IMUInterface::magnitude(telemetry.accel0);
        //assuming landing is at peak g
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
    char transmission[256];
    constexpr float_t ACCELERATION_TOLERANCE = 9 * 9.8 / FEET_TO_M;
    TransmittedData* data = getData();
    Transmissions nextStep = currentStep;
    float_t g1 = data->survive1 * FEET_TO_M / 9.8;
    float_t g2 = data->survive2 * FEET_TO_M / 9.8;
    float_t g3 = data->survive3 * FEET_TO_M / 9.8;
    float_t g4 = data->survive4 * FEET_TO_M / 9.8;
    const char* survived1 = (g1 < ACCELERATION_TOLERANCE)? "survived" : "was killed by";
    const char* survived2 = (g2 < ACCELERATION_TOLERANCE)? "survived" : "was killed by";
    const char* survived3 = (g3 < ACCELERATION_TOLERANCE)? "survived" : "was killed by";
    const char* survived4 = (g4 < ACCELERATION_TOLERANCE)? "survived" : "was killed by";
    if(Peripherals::PeripheralSelector::get()->getTransmitter()->available()){
        switch(currentStep){
        case Transmissions::PayloadStatus:
            std::snprintf(transmission, sizeof(transmission), "The rocket 'Uncertainty' carrying the payload 'Brick' has landed\n");
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::FlightParameters;
            else nextStep = Transmissions::PayloadStatus;
            break;
        case Transmissions::FlightParameters:
            std::snprintf(transmission, sizeof(transmission), "Apogee was %.2fft, peak velocity was %.2fft/s, flight time was %.2fs, and batteries read %.2fV\n", data->apogee, data->peakVelocity, data->timeOfLanding, data->power);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::LandingtParameters;
            else nextStep = Transmissions::FlightParameters;
            break;
        case Transmissions::LandingtParameters:
            std::snprintf(transmission, sizeof(transmission), "Landing velocity was %.2fft/s with a g-force of %.2fg. The landing site has temperature of %.2fF\n", data->landingVelocity, data->landingG, data->temperature);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnauts;
            else nextStep = Transmissions::LandingtParameters;
            break;
        case Transmissions::STEMnauts:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut1 %s a peak g-force of %.2fg\nSTEMnaut2 %s a peak g-force of %.2fg\nSTEMnaut3 %s a peak g-force of %.2fg\nSTEMnaut4 %s a peak g-force of %.2fg\n", survived1, g1, survived2, g2, survived3, g3, survived4, g4);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::FlightParameters2;
            else nextStep = Transmissions::STEMnauts;
            break;
        case Transmissions::FlightParameters2:
        std::snprintf(transmission, sizeof(transmission), "Apogee was %.2fft, peak velocity was %.2fft/s, flight time was %.2fs, and batteries read %.2fV\n", data->apogee, data->peakVelocity, data->timeOfLanding, data->power);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::LandingtParameters2;
            else nextStep = Transmissions::FlightParameters2;
            break;
        case Transmissions::LandingtParameters2:
        std::snprintf(transmission, sizeof(transmission), "Landing velocity was %.2fft/s with a g-force of %.2fg. The landing site has temperature of %.2fF\n", data->landingVelocity, data->landingG, data->temperature);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnauts2;
            else nextStep = Transmissions::LandingtParameters2;
            break;
        case Transmissions::STEMnauts2:
        std::snprintf(transmission, sizeof(transmission), "STEMnaut1 %s a peak g-force of %.2fg\nSTEMnaut2 %s a peak g-force of %.2fg\nSTEMnaut3 %s a peak g-force of %.2fg\nSTEMnaut4 %s a peak g-force of %.2fg\n", survived1, g1, survived2, g2, survived3, g3, survived4, g4);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::DONE;
            else nextStep = Transmissions::STEMnauts2;
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