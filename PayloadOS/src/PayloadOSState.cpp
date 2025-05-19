#include "PayloadOSStateMachine.h"
#include <Arduino.h>


using namespace PayloadOS;
using namespace State;

//operation------------------------------------
void ProgramState::doProcess(bool ignorePause){
    if(paused && !ignorePause) return;
    //check that the state has a process and execute it
    void (*process)() = states[getIndex(currentState)].loop;
    if(process != nullptr) process();

}
void ProgramState::nextState(bool ignorePause){
    if(paused && !ignorePause) return;
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
        void (*exitPreviousState)() = states[getIndex(currentState)].end;
        void (*enterNextState)() = states[getIndex(next)].init;
        currentState = next;
        if(enterNextState == nullptr || exitPreviousState == nullptr){
            if(currentState == States::Fail){
                //error message (cannot enter fail state)
                while(1);
            }
            //log message (state table isn't initialized)
            initiateFailure();
            return;
        }
        exitPreviousState();
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


