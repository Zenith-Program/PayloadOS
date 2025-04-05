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
    //FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
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
    //FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
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
    float_t landingG = Peripherals::IMUInterface::magnitude(telemetry.accel0);//hack
    while(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->readLine(telemetry) != PayloadOS::ERROR && !telemetry.endOfFile){
        float_t accel = Peripherals::IMUInterface::magnitude(telemetry.accel0);//hack
        //assuming landing is at peak g
        if(accel > landingG){ 
            landingG =  accel;
            landingTime = telemetry.time;
        }
    }
    //logs---------------
    //FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
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
    currentStep = Transmissions::Temperature;
    transmissionCount = 0;
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
    const char* survived1 = (g1 < ACCELERATION_TOLERANCE)? "survived" : "killed";
    const char* survived2 = (g2 < ACCELERATION_TOLERANCE)? "survived" : "killed";
    const char* survived3 = (g3 < ACCELERATION_TOLERANCE)? "survived" : "killed";
    const char* survived4 = (g4 < ACCELERATION_TOLERANCE)? "survived" : "killed";
    if(Peripherals::PeripheralSelector::get()->getTransmitter()->available()){
        switch(currentStep){
        case Transmissions::Temperature:
            std::snprintf(transmission, sizeof(transmission), "Temp: %.2fF", data->temperature);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::Apogee;
            else nextStep = Transmissions::Temperature;
            break;
        case Transmissions::Apogee:
            std::snprintf(transmission, sizeof(transmission), "Apogee: %.2f", data->apogee);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::Battery;
            else nextStep = Transmissions::Apogee;
            break;
        case Transmissions::Battery:
            std::snprintf(transmission, sizeof(transmission), "Battery: %.2fV", data->power);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::Time;
            else nextStep = Transmissions::Battery;
            break;
        case Transmissions::Time:
            std::snprintf(transmission, sizeof(transmission), "Flight time: %.2fs", data->timeOfLanding);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::PeakVelocity;
            else nextStep = Transmissions::Time;
            break;
        case Transmissions::PeakVelocity:
            std::snprintf(transmission, sizeof(transmission), "Peak velocity: %.2fft/s", data->peakVelocity);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::LandingVelocity;
            else nextStep = Transmissions::PeakVelocity;
            break;
        case Transmissions::LandingVelocity:
            std::snprintf(transmission, sizeof(transmission), "Landing velocity: %.2fft/s", data->landingVelocity);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::LandingG;
            else nextStep = Transmissions::LandingVelocity;
            break;
        case Transmissions::LandingG:
            std::snprintf(transmission, sizeof(transmission), "Landing g-force: %.2f", data->landingG);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut1g;
            else nextStep = Transmissions::LandingG;
            break;
        case Transmissions::STEMnaut1g:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut1 g: %.2fg", g1);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut1Orientation;
            else nextStep = Transmissions::STEMnaut1g;
            break;
        case Transmissions::STEMnaut1Orientation:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut1 Orientation: %.2fdeg", data->Orientation1);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut1Survivability;
            else nextStep = Transmissions::STEMnaut1Orientation;
            break;
        case Transmissions::STEMnaut1Survivability:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut1: %s", survived1);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut2g;
            else nextStep = Transmissions::STEMnaut1Survivability;
            break;
        case Transmissions::STEMnaut2g:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut2 g: %.2fg", g2);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut2Orientation;
            else nextStep = Transmissions::STEMnaut2g;
            break;
        case Transmissions::STEMnaut2Orientation:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut2 Orientation: %.2fdeg", data->Orientation2);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut2Survivability;
            else nextStep = Transmissions::STEMnaut2Orientation;
            break;
        case Transmissions::STEMnaut2Survivability:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut2: %s", survived2);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut3g;
            else nextStep = Transmissions::STEMnaut2Survivability;
            break;
        case Transmissions::STEMnaut3g:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut3 g: %.2fg", g3);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut3Orientation;
            else nextStep = Transmissions::STEMnaut3g;
            break;
        case Transmissions::STEMnaut3Orientation:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut3 Orientation: %.2fdeg", data->Orientation3);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut3Survivability;
            else nextStep = Transmissions::STEMnaut3Orientation;
            break;
        case Transmissions::STEMnaut3Survivability:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut3: %s", survived3);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut4g;
            else nextStep = Transmissions::STEMnaut3Survivability;
            break;
        case Transmissions::STEMnaut4g:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut4 g: %.2fg", g4);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut4Orientation;
            else nextStep = Transmissions::STEMnaut4g;
            break;
        case Transmissions::STEMnaut4Orientation:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut4 Orientation: %.2fdeg", data->Orientation4);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::STEMnaut4Survivability;
            else nextStep = Transmissions::STEMnaut4Orientation;
            break;
        case Transmissions::STEMnaut4Survivability:
            std::snprintf(transmission, sizeof(transmission), "STEMnaut4: %s", survived4);
            if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::GOOD) nextStep = Transmissions::DONE;
            else nextStep = Transmissions::STEMnaut4Survivability;
            break;
            default:
            nextStep = Transmissions::DONE;
            break;
        }
        currentStep = nextStep;
    }
    //log blackbox telemetry
    //FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->logLine();
}
//Temperature, Apogee, Battery, Time, PeakVelocity, LandingVelocity, LandingG, STEMnaut1g, STEMnaut1Orientation, STEMnaut1Survivability, STEMnaut2g, STEMnaut2Orientation, STEMnaut2Survivability, STEMnaut3g, STEMnaut3Orientation, STEMnaut3Survivability, STEMnaut4g, STEMnaut4Orientation, STEMnaut4Survivability, DONE
void Transmit::end(){
    //NOP
}

State::States Transmit::next(){
    if(currentStep == Transmissions::DONE){
        transmissionCount++;
        currentStep = Transmissions::Temperature;
        if(transmissionCount == FlightData::FlightParameters::get()->getData(FlightData::FlightParameterNames::NumTransmissions)->value)
            return States::Recovery;
    } 
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
Transmissions Transmit::currentStep = Transmissions::Temperature;
uint_t Transmit::transmissionCount;

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
    Armed::setArm(false);
}