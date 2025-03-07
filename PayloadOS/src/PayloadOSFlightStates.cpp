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
const Interpreter::CommandList* Boost::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

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
const Interpreter::CommandList* Coast::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

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
const Interpreter::CommandList* Drogue::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

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
const Interpreter::CommandList* Main::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(&arr.front(), arr.size());
    return &list;
}

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
const Interpreter::CommandList* Landed::getCommands(){
    static constexpr auto arr = std::array<Interpreter::Command, 0>{};
    static const Interpreter::CommandList list(arr.data(), arr.size());
    return &list;
}