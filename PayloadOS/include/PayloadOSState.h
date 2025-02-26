#pragma once

#include "PayloadOSGeneral.h"
#include "PayloadOSCommandList.h"
#include <array>

namespace PayloadOS{
    namespace State{
        enum class States : uint_t{
            Fail, Debug, Startup, Standby, Armed, Boost, Coast, Drogue, Main, Landed, Processing, Transmit, RRS, SENTINAL_COUNT
        };
        #define PayloadOS_NumberOfStates static_cast<std::size_t>(States::SENTINAL_COUNT)

        struct StateData{
            const char* name;
            void (*init)();
            void (*loop)();
            void (*end)();
            States (*next)();
            Interpreter::CommandList* commands;
        };

        class ProgramState{
            std::array<StateData, PayloadOS_NumberOfStates> states;
            States currentState;
            error_t fail;
        public:
            //operation------------------------
            void doProcess();
            void nextState();
            //fail state
            void initiateFailure();
            void recoverFailure();
            //interface & debug
            States getCurrentState();
            const char* getCurrentStateName();
            //interface with the interpreter 
            Interpreter::CommandList* getBaseCommands();
            Interpreter::CommandList* getCurrentCommands();

        //singleton implementation
        private:
            constexpr ProgramState() : states(mapStateData()), currentState(States::Startup), fail(PayloadOS::GOOD){}
        public:
            static ProgramState* get();
            ProgramState(const ProgramState&) = delete;
            ProgramState operator=(const ProgramState&) = delete;
        //helper functions
        private:
            inline static bool isValidEnum(uint_t);
            inline static uint8_t getIndex(States);

            //creation of the state table------------------------------------------------------
            static constexpr std::array<StateData, PayloadOS_NumberOfStates> mapStateData(){
                return{{
                    {"Fail", nullptr, nullptr, nullptr, nullptr, nullptr},       //Fail
                    {"Debug", nullptr, nullptr, nullptr, nullptr, nullptr},      //Debug
                    {"Startup", nullptr, nullptr, nullptr, nullptr, nullptr},    //Startup
                    {"Standby", nullptr, nullptr, nullptr, nullptr, nullptr},    //Standby
                    {"Armed", nullptr, nullptr, nullptr, nullptr, nullptr},      //Armed
                    {"Boost", nullptr, nullptr, nullptr, nullptr, nullptr},      //Boost
                    {"Coast", nullptr, nullptr, nullptr, nullptr, nullptr},      //Coast
                    {"Drogue", nullptr, nullptr, nullptr, nullptr, nullptr},     //Drogue
                    {"Main", nullptr, nullptr, nullptr, nullptr, nullptr},       //Main
                    {"Landed", nullptr, nullptr, nullptr, nullptr, nullptr},     //Landed
                    {"Processing", nullptr, nullptr, nullptr, nullptr, nullptr}, //Processing
                    {"Transmit", nullptr, nullptr, nullptr, nullptr, nullptr},   //Transmit
                    {"RRS", nullptr, nullptr, nullptr, nullptr, nullptr},        //RRS
                }};
            }
            //---------------------------------------------------------------------------------
        };
    }
}