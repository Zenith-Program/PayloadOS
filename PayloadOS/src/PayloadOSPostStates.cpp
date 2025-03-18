#include "PayloadOSStateMachine.h"

using namespace PayloadOS;
using namespace State;

//Processing State---------------------------------------------
void Processing::init(){

}
void Processing::loop(){

}
void Processing::end(){

}
State::States Processing::next(){
    return States::Processing; //for now
}
const Interpreter::CommandList* Processing::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

//Transmit State-----------------------------------------------

void Transmit::init(){

}
void Transmit::loop(){
<<<<<<< Updated upstream
<<<<<<< Updated upstream

=======
=======
>>>>>>> Stashed changes
    constexpr float_t ACCELERATION_TOLERANCE = 9 * 9.8 / FEET_TO_M;
    TransmittedData* data = getData();
    Transmissions nextStep = currentStep;
    if(Peripherals::PeripheralSelector::get()->getTransmitter()->available()){
        Serial.println("Next Peice"); //debug
        switch(currentStep){
        case Transmissions::PayloadStatus:
            sprintf(transmission, "Time: %.2fs, Temp: %.2fF, Power %.2fV\n", data->timeOfLanding, data->temperature, data->power);
            Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString("hi");
            nextStep = Transmissions::FlightParameters;
            break;
        case Transmissions::FlightParameters:
            sprintf(transmission, "Apogee: %.2fft, Max velocity: %.2fft/s\n", data->apogee, data->peakVelocity);
            //Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission);
            nextStep = Transmissions::Landing;
            break;
        case Transmissions::Landing:
            sprintf(transmission, "Landing velocity: %.2fft/s, g-force: %.2fg\n", data->landingVelocity, data->landingG);
            //Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission);
            nextStep = Transmissions::STEMnaut1;
            break;
        case Transmissions::STEMnaut1:
            sprintf(transmission, "STEMnaut1 g-force: %.2fg, Survived: %s\n", data->survive1 * FEET_TO_M / 9.8, (data->survive1 * FEET_TO_M / 9.8 < ACCELERATION_TOLERANCE)? "yes" : "no");
            //Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission);
            nextStep = Transmissions::STEMnaut2;
            break;
        case Transmissions::STEMnaut2:
            sprintf(transmission, "STEMnaut2 g-force: %.2fg, Survived: %s\n", data->survive2 * FEET_TO_M / 9.8, (data->survive2 * FEET_TO_M / 9.8 < ACCELERATION_TOLERANCE)? "yes" : "no");
            //Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission);
            nextStep = Transmissions::STEMnaut3;
            break;
        case Transmissions::STEMnaut3:
            sprintf(transmission, "STEMnaut3 g-force: %.2fg, Survived: %s\n", data->survive3 * FEET_TO_M / 9.8, (data->survive3 * FEET_TO_M / 9.8 < ACCELERATION_TOLERANCE)? "yes" : "no");
            //Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission);
            nextStep = Transmissions::STEMnaut4;
            break;
        case Transmissions::STEMnaut4:
            sprintf(transmission, "STEMnaut4 g-force: %.2fg, Survived: %s\n", data->survive4 * FEET_TO_M / 9.8, (data->survive4 * FEET_TO_M / 9.8 < ACCELERATION_TOLERANCE)? "yes" : "no");
            //Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission);
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
>>>>>>> Stashed changes
}
void Transmit::end(){

}
State::States Transmit::next(){
    return States::Transmit; //for now
}
const Interpreter::CommandList* Transmit::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

<<<<<<< Updated upstream
//RRS State----------------------------------------------------
void RRS::init(){
=======
TransmittedData* Transmit::getData(){
    return &data;
}
TransmittedData Transmit::data = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
Transmissions Transmit::currentStep = Transmissions::PayloadStatus;
char Transmit::transmission[50];

//Recovery State----------------------------------------------------
void Recovery::init(){
>>>>>>> Stashed changes

}
void RRS::loop(){

}
void RRS::end(){

}
State::States RRS::next(){
    return States::Transmit; //for now
}
const Interpreter::CommandList* RRS::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}