#pragma once
#include "PayloadOSGeneral.h"
#include "PayloadOSCommandList.h"
#include <array>
#include <IntervalTimer.h>

namespace PayloadOS{
    namespace State{
        enum class States : uint_t{
            Fail, Debug, Startup, Standby, Armed, Ascent, Descent, Landing, Processing, Transmit, Recovery, SENTINAL_COUNT
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
            static bool debug;
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static const Interpreter::CommandList* getCommands();
            //commands
            static void toDebug(const Interpreter::Token*);
        //helpers
        private:

        };

        class Armed{
            static bool armed;
            static bool liveMode;
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static const Interpreter::CommandList* getCommands();
            //interface
            static bool isArmed();
            static void setArm(bool);
            //commands
            static void softwareDisarm(const Interpreter::Token*);
            static void softwareArm(const Interpreter::Token*);
            static void liveMode_c(const Interpreter::Token*);
        //helpers
        private:

        };

        class Ascent{
            static bool liveMode;
            static uint_t firstEntry;
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static const Interpreter::CommandList* getCommands();
            //interface
            static void clearFirstEntry();
            //commands
            static void liveMode_c(const Interpreter::Token*);

    
        //helpers
        private:
    
        };

        class Descent{
            static bool liveMode;
            static uint_t firstEntry;
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static const Interpreter::CommandList* getCommands();
            //interface
            static void clearFirstEntry();
            //commands
            static void liveMode_c(const Interpreter::Token*);
    
        //helpers
        private:
    
        };

        class Landing{
            static bool liveMode;
            static uint_t entryTime;
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static const Interpreter::CommandList* getCommands();
            //commands
            static void liveMode_c(const Interpreter::Token*);
    
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

        /*
        Temperature of the landing site
        Apogee reached 
        Battery check 
        STEMnaut orientation 
        time of landing 
        Maximum velocity 
        Landing velocity, g-forces
        STEMnaut survivability 
        */

        struct TransmittedData{
            float_t apogee, power, temperature, peakVelocity, landingVelocity, landingG, Orientation1, Orientation2, Orientation3, Orientation4, timeOfLanding;
            float_t survive1, survive2, survive3, survive4;
        };

        enum class Transmissions{
            Temperature, Apogee, Battery, Time, PeakVelocity, LandingVelocity, LandingG, STEMnaut1g, STEMnaut1Orientation, STEMnaut1Survivability, STEMnaut2g, STEMnaut2Orientation, STEMnaut2Survivability, STEMnaut3g, STEMnaut3Orientation, STEMnaut3Survivability, STEMnaut4g, STEMnaut4Orientation, STEMnaut4Survivability, DONE
        };

        class Transmit{
            static TransmittedData data;
            static Transmissions currentStep;
            static uint_t transmissionCount;
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static const Interpreter::CommandList* getCommands();

            static TransmittedData* getData();
            //commands

        //helpers
        private:
        };

        class Recovery{
            static bool recovered;
        public:
            static void init();
            static void loop();
            static void end();
            static State::States next();
            static const Interpreter::CommandList* getCommands();
            //commands
            static void recover_c(const Interpreter::Token*);

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
            uint_t getPeriod();
        private:
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
                    {"Ascent",      Ascent::init,       Ascent::loop,       Ascent::end,        Ascent::next,       Ascent::getCommands()},     //Ascent
                    {"Descent",     Descent::init,      Descent::loop,      Descent::end,       Descent::next,      Descent::getCommands()},    //Descent
                    {"Landing",     Landing::init,      Landing::loop,      Landing::end,       Landing::next,      Landing::getCommands()},    //Landing
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