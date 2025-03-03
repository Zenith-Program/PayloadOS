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
Interpreter::CommandList Processing::commands = {};

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
Interpreter::CommandList Transmit::commands = {};

//RRS State----------------------------------------------------
void RRS::init(){

}
void RRS::loop(){

}
void RRS::end(){

}
State::States RRS::next(){
    return States::Transmit; //for now
}
Interpreter::CommandList RRS::commands = {};