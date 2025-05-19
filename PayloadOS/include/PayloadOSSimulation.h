#pragma once
#include "PayloadOSGeneral.h"
#include "PayloadOSPeripheralInterfaces.h"
#include "PayloadOSCommandList.h"

namespace PayloadOS{
    namespace Simulation{
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

        enum class Simulations{
            Model, Data
        };
        class SimulationTerminalInterface{
            Simulations current;
        public:
            void chooseSimulation(Simulations);
            const Interpreter::CommandList* getChosenCommands();

            static void setSim_c(const Interpreter::Token*);
            static void getSim_c(const Interpreter::Token*);

            //singleton
            static SimulationTerminalInterface* get();
            SimulationTerminalInterface(const SimulationTerminalInterface&) = delete;
            void operator=(const SimulationTerminalInterface&) = delete;
        private:
            SimulationTerminalInterface();
        };
    }
}