#include <cstring>
#include <Arduino.h>
#include "PayloadOSSimulation.h"
#include "PayloadOSModelSim.h"
#include "PayloadOSDataSim.h"

using namespace PayloadOS;
using namespace Simulation;

void SimulationTerminalInterface::chooseSimulation(Simulations version){
    if(current == Simulations::Model && version != current){
        Model::ModelSim::get()->stop();
    }
    if(current == Simulations::Data && version != current){
        Data::DataSim::get()->stop();
    }
    current = version;
}

const Interpreter::CommandList* SimulationTerminalInterface::getChosenCommands(){
    if(current == Simulations::Model)
        return Model::ModelSim::get()->getCommands();
    else
        return Data::DataSim::get()->getCommands();
}

void SimulationTerminalInterface::setSim_c(const Interpreter::Token* args){
    char type[16];
    args[0].copyStringData(type, 16);
    if(std::strcmp(type, "modelSim") == 0){
        get()->chooseSimulation(Simulations::Model);
        return;
    }
    if(std::strcmp(type, "dataSim") == 0){
        get()->chooseSimulation(Simulations::Data);
        return;
    }
    Serial.print("'");
    Serial.print(type);
    Serial.println("' is not a simulation. Choose 'dataSim' or 'modelSim'");
}

void SimulationTerminalInterface::getSim_c(const Interpreter::Token*){
    if(get()->current == Simulations::Data) Serial.println("dataSim");
    else Serial.println("modelSim");
}
            
//singleton
SimulationTerminalInterface* SimulationTerminalInterface::get(){
    static SimulationTerminalInterface instance;
    return &instance;
}

SimulationTerminalInterface::SimulationTerminalInterface() : current((PayloadOS_DefaultSimulation == PayloadOS_ChooseDataSim)? Simulations::Data : Simulations::Model){}
