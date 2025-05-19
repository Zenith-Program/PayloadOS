#pragma once
#include <IntervalTimer.h>
#include "PayloadOSGeneral.h"
#include "PayloadOSSimulation.h"

namespace PayloadOS{
    namespace Simulation{
        namespace Data{
            class DataSim{
                IntervalTimer timer;
                uint_t period;
                bool isActive;
            public:
                error_t start();
                error_t stop();
                bool active() const;

                void setClockPeriod_us(uint_t);
                uint_t getClockPeriod_us() const;

                //commands
                const Interpreter::CommandList* getCommands();
                static void start_c(const Interpreter::Token*);
                static void stop_c(const Interpreter::Token*);
                static void status_c(const Interpreter::Token*);
                static void setClock_c(const Interpreter::Token*);
            private:
                void initTimer(uint_t);
                void stopTimer();
                static void timerCallback();
                void clearBackdoors();

            public:
                //singleton
                static DataSim* get();
                DataSim(const DataSim&) = delete;
                void operator=(const DataSim&) = delete;
            private:
                DataSim();
            };
        }
    }
}