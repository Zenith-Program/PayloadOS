#include "PayloadOSStateMachine.h"

using namespace PayloadOS;
using namespace State;

//Armed--------------------------------------------------------
void Armed::init(){

}
void Armed::loop(){

}
void Armed::end(){

}
State::States Armed::next(){
    return States::Armed; //for now
}
const Interpreter::CommandList* Armed::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

//Flight--------------------------------------------------------
void Flight::init(){

}
void Flight::loop(){

}
void Flight::end(){

}
State::States Flight::next(){
    return States::Flight; //for now
}
const Interpreter::CommandList* Flight::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}