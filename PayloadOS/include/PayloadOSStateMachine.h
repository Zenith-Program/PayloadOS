#pragma once
#include "PayloadOSGeneral.h"
#include "PayloadOSCommandList.h"
#include <array>
#include <IntervalTimer.h>

namespace PayloadOS{
    namespace State{
        enum class States : uint_t{
            Fail, Debug, Startup, Standby, Armed, Flight, Processing, Transmit, Recovery, SENTINAL_COUNT
        };
        #define PayloadOS_NumberOfStates static_cast<std::size_t>(States::SENTINAL_COUNT)

        //State Implementation Classes---------------------------------------------------------
        class Fail{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static const Interpreter::CommandList* getCommands();
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
            static const Interpreter::CommandList* getCommands();
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
            static const Interpreter::CommandList* getCommands();
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
            static const Interpreter::CommandList* getCommands();
            //commands
            static void toDebug(const Interpreter::Token*);
            static void softwareArm(const Interpreter::Token*);
            static void softwareDisarm(const Interpreter::Token*);
        //helpers
        private:

        };

        class Armed{
            static uint_t varianceSize;
            static float_t launchThreshold;
            static bool armed;
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static const Interpreter::CommandList* getCommands();
            //commands
            static void softwareDisarm(const Interpreter::Token*);
            static void setVarSize(const Interpreter::Token*);
            static void setThreshold(const Interpreter::Token*);
            static void getVarSize(const Interpreter::Token*);
            static void getThreshold(const Interpreter::Token*);

            static constexpr uint_t defaultVarianceSize = 16;
            static constexpr float_t defaultLaunchThreshold = 10;

        //helpers
        private:

        };

        class Flight{
            static float_t landingThreshold;
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static const Interpreter::CommandList* getCommands();
            //commands

            static constexpr float_t defaultLandingThreshold = 7.5;
    
        //helpers
        private:
    
        };

        class Processing{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static const Interpreter::CommandList* getCommands();
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
            static const Interpreter::CommandList* getCommands();
            //commands

        //helpers
        private:
        };

        class Recovery{
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static const Interpreter::CommandList* getCommands();
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
            const Interpreter::CommandList* commands;
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
            void doProcess(bool=false);
            void nextState(bool=false);
            //fail state
            void initiateFailure();
            void recoverFailure(States=States::SENTINAL_COUNT);
            //interface & debug
            States getCurrentState() const;
            const char* getCurrentStateName() const;
            const Interpreter::CommandList* getBaseCommands();
            const Interpreter::CommandList* getCurrentCommands();
            //clock
            void reset();
            void pause();
            void resume();
            static void ClockISR();
            //internal command interface
        private:
            uint_t getPeriod();
            void changePeriod(uint_t);

        //singleton implementation
        private:
            ProgramState() : states(mapStateData()), currentState(States::Startup), fail(PayloadOS::GOOD), period(PayloadOS_DefaultSamplePeriod), paused(true){}
        public:
            static ProgramState* get();
            ProgramState(const ProgramState&) = delete;
            ProgramState operator=(const ProgramState&) = delete;
        //helper functions
        private:
            inline static bool isValidEnum(uint_t);
            inline static uint8_t getIndex(States);

            //creation of the state table------------------------------------------------------
            static std::array<StateData, PayloadOS_NumberOfStates> mapStateData(){
                return{{
                    {"Fail",        Fail::init,         Fail::loop,         Fail::end,          Fail::next,         Fail::getCommands()},       //Fail
                    {"Debug",       Debug::init,        Debug::loop,        Debug::end,         Debug::next,        Debug::getCommands()},      //Debug
                    {"Startup",     Startup::init,      Startup::loop,      Startup::end,       Startup::next,      Startup::getCommands()},    //Startup
                    {"Standby",     Standby::init,      Standby::loop,      Standby::end,       Standby::next,      Standby::getCommands()},    //Standby
                    {"Armed",       Armed::init,        Armed::loop,        Armed::end,         Armed::next,        Armed::getCommands()},      //Armed
                    {"Flight",      Flight::init,       Flight::loop,       Flight::end,        Flight::next,       Flight::getCommands()},     //Flight
                    {"Processing",  Processing::init,   Processing::loop,   Processing::end,    Processing::next,   Processing::getCommands()}, //Processing
                    {"Transmit",    Transmit::init,     Transmit::loop,     Transmit::end,      Transmit::next,     Transmit::getCommands()},   //Transmit
                    {"Recovery",    Recovery::init,     Recovery::loop,     Recovery::end,      Recovery::next,     Recovery::getCommands()},   //RRS
                }};
            }
            //---------------------------------------------------------------------------------
        //Command Implementations
        public:
            static void echo(const Interpreter::Token*);
            static void getState(const Interpreter::Token*);
            static void commands(const Interpreter::Token*);
            static void pause_C(const Interpreter::Token*);
            static void play(const Interpreter::Token*);
            static void isPlaying_C(const Interpreter::Token*);
            static void reset_C(const Interpreter::Token*);
            static void getClock(const Interpreter::Token*);
            static void setClock(const Interpreter::Token*);
            static void do_C(const Interpreter::Token*);
            //peripherals
            static void units(const Interpreter::Token*);
            static void mode(const Interpreter::Token*);
            static void status(const Interpreter::Token*);
            static void report(const Interpreter::Token*);
            static void init_C(const Interpreter::Token*);
            static void read(const Interpreter::Token*);
            static void hardware_C(const Interpreter::Token*);
            static void transmitRF(const Interpreter::Token*);
        };
    }
}