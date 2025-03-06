#include "PayloadOSStateMachine.h"
#include <Arduino.h>

using namespace PayloadOS;
using namespace State;

//operation------------------------------------
void ProgramState::doProcess(){
    if(paused) return;
    //check that the state has a process and execute it
    void (*process)() = states[getIndex(currentState)].loop;
    if(process != nullptr) process();

}
void ProgramState::nextState(){
    if(paused) return;
    States next;
    if(fail == PayloadOS::ERROR){
        next = States::Fail;
    }
    else{
        States (*findNext)() = states[getIndex(currentState)].next;
        if(findNext == nullptr){
            //log message (state table isn't initialized)
            next = States::Fail;
        }
        else{
            next = findNext();
        }
    }
    if(next != currentState){
        currentState = next;
        void (*enterNextState)() = states[getIndex(currentState)].init;
        if(enterNextState == nullptr){
            if(currentState == States::Fail){
                //error message (cannot enter fail state)
                while(1);
            }
            //log message (state table isn't initialized)
            initiateFailure();
            return;
        }
        enterNextState();
    }
}

//fail state-----------------------------------
void ProgramState::initiateFailure(){
    fail = PayloadOS::ERROR;
}


void ProgramState::recoverFailure(States state){
    //default argument clears fail flag before end of cycle
    if(state == States::SENTINAL_COUNT) fail = PayloadOS::GOOD;
    else{
        //recovers to a specific state
        auto restart = states[getIndex(state)].init;
        if(restart != nullptr){
            restart();
            currentState = state;
            fail = PayloadOS::GOOD;
        }
    }
}


//interfacing----------------------------------
States ProgramState::getCurrentState() const{
    return currentState;
}

const char* ProgramState::getCurrentStateName() const{
    return states[getIndex(currentState)].name; // for now
}

const Interpreter::CommandList* ProgramState::getCurrentCommands(){
    return states[getIndex(currentState)].commands; //for now
}

//clock----------------------------------------
void ProgramState::reset(){
    auto endRoutine = states[getIndex(currentState)].end;
    if(endRoutine != nullptr) endRoutine();
    currentState = State::States::Startup;
    paused = false;
    timer.begin(ClockISR, period);
}

void ProgramState::pause(){
    timer.end();
    paused = true;
}

void ProgramState::resume(){
    paused = false;
    timer.begin(ClockISR, period);
}

void ProgramState::ClockISR(){
    ProgramState::get()->doProcess();
    ProgramState::get()->nextState();
}

//singleton------------------------------------
ProgramState* ProgramState::get(){
    static ProgramState instance;
    return &instance;
}

//helpers--------------------------------------
 bool ProgramState::isValidEnum(uint_t inQuestion){
    return inQuestion < PayloadOS_NumberOfStates;
 }

 uint8_t ProgramState::getIndex(States state){
    return static_cast<uint_t>(state);
 }

 //commands----------------------------------------------------
 const Interpreter::CommandList* ProgramState::getBaseCommands(){
    static const Interpreter::CommandList base = {
        {"echo", "", echo},
        {"state", "", getState},
        {"commands", "", commands},
        {"pause", "", pause_C},
        {"play", "", play},
        {"reset", "", reset_C},
        {"getClock", "", getClock},
        {"setClock", "", setClock},
        {"do", "", do_C}
    };
    return &base; // for now
}

void ProgramState::echo(const Interpreter::Token*){
    Serial.println("echo");
}
void ProgramState::getState(const Interpreter::Token*){
    Serial.println(ProgramState::get()->getCurrentStateName());
}
void ProgramState::commands(const Interpreter::Token*){

}
void ProgramState::pause_C(const Interpreter::Token*){
    ProgramState::get()->pause();
    Serial.println("paused");
}
void ProgramState::play(const Interpreter::Token*){
    ProgramState::get()->resume();
    Serial.println("playing");
}
void ProgramState::reset_C(const Interpreter::Token*){
    Serial.println("reseting");
    ProgramState::get()->reset();
}
void ProgramState::getClock(const Interpreter::Token*){

}
void ProgramState::setClock(const Interpreter::Token*){

}
void ProgramState::do_C(const Interpreter::Token*){

}