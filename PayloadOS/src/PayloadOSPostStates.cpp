#include "PayloadOSStateMachine.h"
#include "PayloadOSSD.h"
#include "PayloadOSVariance.h"
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
    FlightData::TelemetryLog::get()->close();
    FlightData::TelemetryLog::get()->openForRead();
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
    while(FlightData::TelemetryLog::get()->readLine(telemetry) != PayloadOS::ERROR && !telemetry.endOfFile){
        //time of landing
        if(telemetry.state == static_cast<uint_t>(States::Flight) && launchTime == 0) launchTime = telemetry.time;
        //apogee
        float_t altitude =  (telemetry.altitude1 - FlightData::AltimeterVariances::getAltimeter1Zero() + telemetry.altitude2 - FlightData::AltimeterVariances::getAltimeter2Zero())/2.0;
        if(altitude > apogee) apogee = altitude;
        //maximum velocity
        float_t velocity = (altitude - previousAltitude) / (ProgramState::get()->getPeriod() / 1000000.0);
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
    }   
    //landing velocity
    FlightData::TelemetryLog::get()->close();
    FlightData::TelemetryLog::get()->openForRead();
    constexpr uint_t LookBackTime_ms = 3000;
    uint_t index;
    for(index = 0; index < line - LookBackTime_ms * 1000/State::ProgramState::get()->getPeriod(); index++)
        FlightData::TelemetryLog::get()->readLine(telemetry);
    float_t altitude1 = (telemetry.altitude1 - FlightData::AltimeterVariances::getAltimeter1Zero() + telemetry.altitude2 - FlightData::AltimeterVariances::getAltimeter2Zero())/2.0;
    FlightData::TelemetryLog::get()->readLine(telemetry);
    float_t altitude0 = (telemetry.altitude1 - FlightData::AltimeterVariances::getAltimeter1Zero() + telemetry.altitude2 - FlightData::AltimeterVariances::getAltimeter2Zero())/2.0;
    float_t landingVelocity = (altitude0 - altitude1)/(ProgramState::get()->getPeriod() / 1000000.0);
    //sustained g-force
    float_t landingG = Peripherals::IMUInterface::magnitude(telemetry.accel0);
    landingTime = telemetry.time;
    while(FlightData::TelemetryLog::get()->readLine(telemetry) != PayloadOS::ERROR && !telemetry.endOfFile){
        float_t accel = Peripherals::IMUInterface::magnitude(telemetry.accel0);
        if(accel > landingG){ 
            landingG =  accel;
            landingTime = telemetry.time;
        }
    }
    TransmittedData* data = Transmit::getData();
    //-------------------------
    Serial.println(landingTime);
    Serial.println(launchTime);
    //-------------------------
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
    //NOP
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
    Serial.println("transmit");
    currentStep = Transmissions::PayloadStatus;
}
void Transmit::loop(){
    char transmission[50];
    constexpr float_t ACCELERATION_TOLERANCE = 9 * 9.8 / FEET_TO_M;
    TransmittedData* data = getData();
    Transmissions nextStep = currentStep;
    if(Peripherals::PeripheralSelector::get()->getTransmitter()->available()){
        switch(currentStep){
        case Transmissions::PayloadStatus:
            std::sprintf(transmission, "Time: %.2fs, Temp: %.2fF, Power %.2fV\n", data->timeOfLanding, data->temperature, data->power);
            Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission);
            nextStep = Transmissions::FlightParameters;
            break;
        case Transmissions::FlightParameters:
            std::sprintf(transmission, "Apogee: %.2fft, Max velocity: %.2fft/s\n", data->apogee, data->peakVelocity);
            Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission);
            nextStep = Transmissions::Landing;
            break;
        case Transmissions::Landing:
            std::sprintf(transmission, "Landing velocity: %.2fft/s, g-force: %.2fg\n", data->landingVelocity, data->landingG);
            Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission);
            nextStep = Transmissions::STEMnaut1;
            break;
        case Transmissions::STEMnaut1:
            std::sprintf(transmission, "STEMnaut1 g-force: %.2fg, Survived: %s\n", data->survive1 * FEET_TO_M / 9.8, (data->survive1 * FEET_TO_M / 9.8 < ACCELERATION_TOLERANCE)? "yes" : "no");
            Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission);
            nextStep = Transmissions::STEMnaut2;
            break;
        case Transmissions::STEMnaut2:
            std::sprintf(transmission, "STEMnaut2 g-force: %.2fg, Survived: %s\n", data->survive2 * FEET_TO_M / 9.8, (data->survive2 * FEET_TO_M / 9.8 < ACCELERATION_TOLERANCE)? "yes" : "no");
            Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission);
            nextStep = Transmissions::STEMnaut3;
            break;
        case Transmissions::STEMnaut3:
            std::sprintf(transmission, "STEMnaut3 g-force: %.2fg, Survived: %s\n", data->survive3 * FEET_TO_M / 9.8, (data->survive3 * FEET_TO_M / 9.8 < ACCELERATION_TOLERANCE)? "yes" : "no");
            Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission);
            nextStep = Transmissions::STEMnaut4;
            break;
        case Transmissions::STEMnaut4:
            std::sprintf(transmission, "STEMnaut4 g-force: %.2fg, Survived: %s\n", data->survive4 * FEET_TO_M / 9.8, (data->survive4 * FEET_TO_M / 9.8 < ACCELERATION_TOLERANCE)? "yes" : "no");
            Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission);
            nextStep = Transmissions::DONE;
            break;
            default:
            nextStep = Transmissions::DONE;
            break;
        }
        currentStep = nextStep;
    }
    
    /*
    TransmittedData* data = getData();
    Serial.println(data->apogee);
    Serial.println(data->landingG);
    Serial.println(data->landingVelocity);
    Serial.println(data->Orientation1);
    Serial.println(data->Orientation2);
    Serial.println(data->Orientation3);
    Serial.println(data->Orientation4);
    Serial.println(data->peakVelocity);
    Serial.println(data->power);
    Serial.println(data->survive1);
    Serial.println(data->survive2);
    Serial.println(data->survive3);
    Serial.println(data->survive4);
    Serial.println(data->temperature);
    Serial.println(data->timeOfLanding);
    */
}
void Transmit::end(){

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
void Recovery::init(){

}
void Recovery::loop(){
    //NOP
}
void Recovery::end(){

}
State::States Recovery::next(){
    return States::Recovery; //for now
}
const Interpreter::CommandList* Recovery::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}