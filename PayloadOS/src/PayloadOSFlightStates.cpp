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
Interpreter::CommandList Armed::commands = {};

//Boost--------------------------------------------------------
void Boost::init(){

}
void Boost::loop(){

}
void Boost::end(){

}
State::States Boost::next(){
    return States::Boost; //for now
}
Interpreter::CommandList Boost::commands = {};

//Coast--------------------------------------------------------
void Coast::init(){

}
void Coast::loop(){

}
void Coast::end(){

}
State::States Coast::next(){
    return States::Coast; //for now
}
Interpreter::CommandList Coast::commands = {};

//Drogue-------------------------------------------------------
void Drogue::init(){

}
void Drogue::loop(){

}
void Drogue::end(){

}
State::States Drogue::next(){
    return States::Drogue; //for now
}
Interpreter::CommandList Drogue::commands = {};

//Main---------------------------------------------------------
void Main::init(){

}
void Main::loop(){

}
void Main::end(){

}
State::States Main::next(){
    return States::Main; //for now
}
Interpreter::CommandList Main::commands = {};

//Landed-------------------------------------------------------
void Landed::init(){

}
void Landed::loop(){

}
void Landed::end(){

}
State::States Landed::next(){
    return States::Landed; //for now
}
Interpreter::CommandList Landed::commands = {};