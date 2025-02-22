#include "PayloadOSState.h"

using namespace PayloadOS;
using namespace State;

//operation------------------------------------
void ProgramState::doProcess(){
    //check that the state has a process and execute it
    void (*process)() = states[getIndex(currentState)].loop;
    if(process != nullptr) process();
}
void ProgramState::nextState(){
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


//interfacing----------------------------------
States ProgramState::getCurrentState(){
    return States::Fail; //for now
}
const char* ProgramState::getCurrentStateName(){
    return states[getIndex(currentState)].name; // for now
}
//interface with the interpreter 
Interpreter::CommandList* ProgramState::getBaseCommands(){
    return nullptr; // for now
}
Interpreter::CommandList* ProgramState::getCurrentCommands(){
    return states[getIndex(currentState)].commands; //for now
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