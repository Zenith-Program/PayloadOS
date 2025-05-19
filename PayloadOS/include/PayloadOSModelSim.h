#pragma once
#include <IntervalTimer.h>
#include "PayloadOSGeneral.h"
#include "PayloadOSSimulation.h"

namespace PayloadOS{
    namespace Simulation{
        namespace Model{

            class SimState {
            public:
                virtual void init() = 0;
                virtual UpdateParams update() = 0;
                virtual void end() = 0;
            protected:
                //static helpers
                static void addNoise(UpdateParams&);
                static UpdateParams getBaseParams();
                static float_t altitude_mToPressure_mBar(float_t);
                static float_t getNoise(float_t);
                static void addNoiseToVector(Peripherals::LinearVector&, float_t);
                static void addNoiseToVector(Peripherals::RotationVector&, float_t);
                static void setIMUsOrientation(UpdateParams&, Peripherals::LinearVector);
                static void setIMUsAcceleration(UpdateParams&, Peripherals::LinearVector);
                static void setIMUsAngularVelocity(UpdateParams&, Peripherals::RotationVector);
                static void setAltitudesAndPressures(UpdateParams&, float_t);

            };

            enum class SimStates{
                Inactive, Delay, Boost, Coast, Apogee, Drogue, Deploy, Main, Landing, Landed, SENTINAL_COUNT
            };

            #define PayloadOS_ModelSim_NumberOfStates static_cast<uint_t>(SimStates::SENTINAL_COUNT)

            class ModelSim{
                SimStates state;
                uint_t period;
                IntervalTimer timer;
            public:
                void start();
                void stop();
                void clock();
                static void ClockISR();
                const Interpreter::CommandList* getCommands();
                //configuration
                void setClockPeriod(uint_t);
            private:
                void initTimer(uint_t);
                void stopTimer();
                static SimState* getState(SimStates);
                static SimStates nextState(SimStates);
                static void setBackdoors(const UpdateParams&);
            //Singleton Implementation
            public:
                static ModelSim* get();
                ModelSim(const ModelSim&) = delete;
                void operator=(const ModelSim&) = delete;
            private:
                ModelSim();

                //helpers
                static const char* getStateName(SimStates);

                //simulation interface
                static void startSim(const Interpreter::Token*);
                static void stopSim(const Interpreter::Token*);
                static void simStatus(const Interpreter::Token*);
                static void setSimClock(const Interpreter::Token*);
                static void getSimData(const Interpreter::Token*);
                static void setSimData(const Interpreter::Token*);
                static void seedRNG(const Interpreter::Token*);
            };
    }
    }
}