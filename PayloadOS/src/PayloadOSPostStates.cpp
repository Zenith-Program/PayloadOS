#include "PayloadOSStateMachine.h"
#include <Arduino.h>

using namespace PayloadOS;
using namespace State;

//Processing State---------------------------------------------
void Processing::init(){
    Serial.println("Processing");
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

//Recovery State----------------------------------------------------
void Recovery::init(){

}
void Recovery::loop(){

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