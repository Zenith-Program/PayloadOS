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

        class Boost{
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

        class Coast{
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

        class Drogue{
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

        class Main{
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

        class Landed{
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
<<<<<<< Updated upstream
<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
=======
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
            static char transmission[50];
            static TransmittedData data;
            static Transmissions currentStep;
>>>>>>> Stashed changes
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

        class RRS{
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
                    {"Boost",       Boost::init,        Boost::loop,        Boost::end,         Boost::next,        Boost::getCommands()},      //Boost
                    {"Coast",       Coast::init,        Coast::loop,        Coast::end,         Coast::next,        Coast::getCommands()},      //Coast
                    {"Drogue",      Drogue::init,       Drogue::loop,       Drogue::end,        Drogue::next,       Drogue::getCommands()},     //Drogue
                    {"Main",        Main::init,         Main::loop,         Main::end,          Main::next,         Main::getCommands()},       //Main
                    {"Landed",      Landed::init,       Landed::loop,       Landed::end,        Landed::next,       Landed::getCommands()},     //Landed
                    {"Processing",  Processing::init,   Processing::loop,   Processing::end,    Processing::next,   Processing::getCommands()}, //Processing
                    {"Transmit",    Transmit::init,     Transmit::loop,     Transmit::end,      Transmit::next,     Transmit::getCommands()},   //Transmit
                    {"RRS",         RRS::init,          RRS::loop,          RRS::end,           RRS::next,          RRS::getCommands()},        //RRS
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