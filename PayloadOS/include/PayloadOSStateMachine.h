#pragma once
#include "PayloadOSGeneral.h"
#include "PayloadOSCommandList.h"
#include <array>
#include <IntervalTimer.h>

namespace PayloadOS{
    namespace State{
        enum class States : uint_t{
            Fail, Debug, Startup, Standby, Armed, Boost, Coast, Drogue, Main, Landed, Processing, Transmit, RRS, SENTINAL_COUNT
        };
        #define PayloadOS_NumberOfStates static_cast<std::size_t>(States::SENTINAL_COUNT)

        //State Implementation Classes---------------------------------------------------------
        class Fail{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static Interpreter::CommandList commands;
            //commands

        //helpers
        private:

        };

        class Debug{
            static bool exit;
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static Interpreter::CommandList commands;
            //commands
            static void exitDebug(const Interpreter::Token*);
        //helpers
        private:
            

        };

        class Startup{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static Interpreter::CommandList commands;
            //commands
            
        //helpers
        private:
            static error_t initAllPeripherals();

        };

        class Standby{
            static bool armed;
            static bool debug;
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static Interpreter::CommandList commands;
            //commands
            static void toDebug(const Interpreter::Token*);
            static void softwareArm(const Interpreter::Token*);
            static void softwareDisarm(const Interpreter::Token*);
        //helpers
        private:

        };

        class Armed{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static Interpreter::CommandList commands;
            //commands

        //helpers
        private:

        };

        class Boost{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static Interpreter::CommandList commands;
            //commands

        //helpers
        private:

        };

        class Coast{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static Interpreter::CommandList commands;
            //commands

        //helpers
        private:

        };

        class Drogue{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static Interpreter::CommandList commands;
            //commands

        //helpers
        private:

        };

        class Main{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static Interpreter::CommandList commands;
            //commands

        //helpers
        private:

        };

        class Landed{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static Interpreter::CommandList commands;
            //commands

        //helpers
        private:

        };

        class Processing{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static Interpreter::CommandList commands;
            //commands

        //helpers
        private:

        };

        class Transmit{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static Interpreter::CommandList commands;
            //commands

        //helpers
        private:
        };

        class RRS{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static Interpreter::CommandList commands;
            //commands

        //helpers
        private:
            
        };

        //State Machine------------------------------------------------------------------------

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
            uint_t period;
            IntervalTimer timer;
            bool paused;
        public:
            //operation------------------------
            void doProcess();
            void nextState();
            //fail state
            void initiateFailure();
            void recoverFailure(States=States::SENTINAL_COUNT);
            //interface & debug
            States getCurrentState() const;
            const char* getCurrentStateName() const;
            //interface with the interpreter 
            Interpreter::CommandList* getBaseCommands();
            Interpreter::CommandList* getCurrentCommands();
            //clock
            void reset();
            void pause();
            void resume();
            static void ClockISR();

        //singleton implementation
        private:
            constexpr ProgramState() : states(mapStateData()), currentState(States::Startup), fail(PayloadOS::GOOD), period(PayloadOS_DefaultSamplePeriod), paused(true){}
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
                    {"Fail",        Fail::init,         Fail::loop,         Fail::end,          Fail::next,         &Fail::commands},       //Fail
                    {"Debug",       Debug::init,        Debug::loop,        Debug::end,         Debug::next,        &Debug::commands},      //Debug
                    {"Startup",     Startup::init,      Startup::loop,      Startup::end,       Startup::next,      &Startup::commands},    //Startup
                    {"Standby",     Standby::init,      Standby::loop,      Standby::end,       Standby::next,      &Standby::commands},    //Standby
                    {"Armed",       Armed::init,        Armed::loop,        Armed::end,         Armed::next,        &Armed::commands},      //Armed
                    {"Boost",       Boost::init,        Boost::loop,        Boost::end,         Boost::next,        &Boost::commands},      //Boost
                    {"Coast",       Coast::init,        Coast::loop,        Coast::end,         Coast::next,        &Coast::commands},      //Coast
                    {"Drogue",      Drogue::init,       Drogue::loop,       Drogue::end,        Drogue::next,       &Drogue::commands},     //Drogue
                    {"Main",        Main::init,         Main::loop,         Main::end,          Main::next,         &Main::commands},       //Main
                    {"Landed",      Landed::init,       Landed::loop,       Landed::end,        Landed::next,       &Landed::commands},     //Landed
                    {"Processing",  Processing::init,   Processing::loop,   Processing::end,    Processing::next,   &Processing::commands}, //Processing
                    {"Transmit",    Transmit::init,     Transmit::loop,     Transmit::end,      Transmit::next,     &Transmit::commands},   //Transmit
                    {"RRS",         RRS::init,          RRS::loop,          RRS::end,           RRS::next,          &RRS::commands},        //RRS
                }};
            }
            //---------------------------------------------------------------------------------
        };
    }
}