#pragma once
#include "PayloadOSGeneral.h"
#include "PayloadOSPeripheralInterfaces.h"
#include <IntervalTimer.h>

namespace PayloadOS{
    namespace Simulation{
        namespace Model{
            struct UpdateParams{
                bool change;
                Peripherals::GPSData gps;
                float_t altitude, altitude2;
                Peripherals::LinearVector acceleration0, acceleration1, acceleration2, acceleration3, acceleration4;
                Peripherals::RotationVector angularVelocity0, angularVelocity1, angularVelocity2, angularVelocity3, angularVelocity4;
                Peripherals::LinearVector gravity0, gravity1, gravity2, gravity3, gravity4;
                float_t voltage, temp, pressure, temp2, pressure2;
                bool arm;
            };

            class SimState {
            public:
                virtual void init() = 0;
                virtual UpdateParams update() = 0;
                virtual void end() = 0;
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
                //configuration
                void setClockPeriod(uint_t);
            private:
                void initTimer(uint8_t);
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
            };
    }
    }
}