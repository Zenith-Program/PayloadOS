#include "PayloadOSPeripheralSelector.h"
#include "PayloadOSHardwareImplementations.h"
#include "PayloadOSBackdoorImplementations.h"

using namespace PayloadOS;
using namespace Peripherals;


//Selection getters--------------------------------------------
AltimeterInterface* PeripheralSelector::getPayloadAltimeter(){
    if(selections[getIndex(PeripheralNames::PayloadAltimeter)].isBackdoor) return &altimeterBackdoor;
    return &altimeterHardware;
}

IMUInterface* PeripheralSelector::getPayloadIMU(){
    if(selections[getIndex(PeripheralNames::PayloadIMU)].isBackdoor) return &imuBackdoor;
    return &imuHardware;
}

IMUInterface* PeripheralSelector::getSTEMnaut1(){
    if(selections[getIndex(PeripheralNames::STEMnaut1)].isBackdoor) return &stemnaut1Backdoor;
    return &stemnaut1Hardware;
}

IMUInterface* PeripheralSelector::getSTEMnaut2(){
    if(selections[getIndex(PeripheralNames::STEMnaut2)].isBackdoor) return &stemnaut2Backdoor;
    return &stemnaut2Hardware;
}

IMUInterface* PeripheralSelector::getSTEMnaut3(){
    if(selections[getIndex(PeripheralNames::STEMnaut3)].isBackdoor) return &stemnaut3Backdoor;
    return &stemnaut3Hardware;
}

IMUInterface* PeripheralSelector::getSTEMnaut4(){
    if(selections[getIndex(PeripheralNames::STEMnaut4)].isBackdoor) return &stemnaut4Backdoor;
    return &stemnaut4Hardware;
}

GPSInterface* PeripheralSelector::getGPS(){
    if(selections[getIndex(PeripheralNames::GPS)].isBackdoor) return &gpsBackdoor;
    return &gpsHardware;
}

AltimeterInterface* PeripheralSelector::getLightAPRSAltimeter(){
    if(selections[getIndex(PeripheralNames::LightAPRSAltimeter)].isBackdoor) return &altimeter2Backdoor;
    return &altimeter2Hardware;
}

TransmitterInterface* PeripheralSelector::getTransmitter(){
    if(selections[getIndex(PeripheralNames::Transmitter)].isBackdoor) return &transmitterBackdoor;
    return &transmitterHardware;
}

PowerCheckInterface* PeripheralSelector::getPowerCheck(){
    if(selections[getIndex(PeripheralNames::PowerCheck)].isBackdoor) return &powerCheckBackdoor;
    return &powerCheckHardware;
}

//Hardware getters---------------------------------------------
Hardware::AltimeterHardware* PeripheralSelector::getPayloadAltimeterHardware(){
    if(selections[getIndex(PeripheralNames::PayloadAltimeter)].hasHardware) return &altimeterHardware;
    return nullptr;
}

Hardware::IMUHardware* PeripheralSelector::getPayloadIMUHardware(){
    if(selections[getIndex(PeripheralNames::PayloadIMU)].hasHardware) return &imuHardware;
    return nullptr;
}

Hardware::STEMnaut1Hardware* PeripheralSelector::getSTEMnaut1Hardware(){
    if(selections[getIndex(PeripheralNames::STEMnaut1)].hasHardware) return &stemnaut1Hardware;
    return nullptr;
}

Hardware::STEMnaut2Hardware* PeripheralSelector::getSTEMnaut2Hardware(){
    if(selections[getIndex(PeripheralNames::STEMnaut2)].hasHardware) return &stemnaut2Hardware;
    return nullptr;
}

Hardware::STEMnaut3Hardware* PeripheralSelector::getSTEMnaut3Hardware(){
    if(selections[getIndex(PeripheralNames::STEMnaut3)].hasHardware) return &stemnaut3Hardware;
    return nullptr;
}

Hardware::STEMnaut4Hardware* PeripheralSelector::getSTEMnaut4Hardware(){
    if(selections[getIndex(PeripheralNames::STEMnaut4)].hasHardware) return &stemnaut4Hardware;
    return nullptr;
}

Hardware::GPSHardware* PeripheralSelector::getGPSHardware(){
    if(selections[getIndex(PeripheralNames::GPS)].hasHardware) return &gpsHardware;
    return nullptr;
}

Hardware::Altimeter2Hardware* PeripheralSelector::getLightAPRSAltimeterHardware(){
    if(selections[getIndex(PeripheralNames::LightAPRSAltimeter)].hasHardware) return &altimeter2Hardware;
    return nullptr;
}

Hardware::TransmitterHardware* PeripheralSelector::getTransmitterHardware(){
    if(selections[getIndex(PeripheralNames::Transmitter)].hasHardware) return &transmitterHardware;
    return nullptr;
}

Hardware::PowerCheckHardware* PeripheralSelector::getPowerCheckHardware(){
    if(selections[getIndex(PeripheralNames::PowerCheck)].hasHardware) return &powerCheckHardware;
    return nullptr;
}



//Backdoor getters---------------------------------------------
Simulation::AltimeterBackdoor* PeripheralSelector::getPayloadAltimeterBackdoor(){
    return &altimeterBackdoor;
}

Simulation::IMUBackdoor* PeripheralSelector::getPayloadIMUBackdoor(){
    return &imuBackdoor;
}

Simulation::STEMnaut1Backdoor* PeripheralSelector::getSTEMnaut1Backdoor(){
    return &stemnaut1Backdoor;
}

Simulation::STEMnaut2Backdoor* PeripheralSelector::getSTEMnaut2Backdoor(){
    return &stemnaut2Backdoor;
}

Simulation::STEMnaut3Backdoor* PeripheralSelector::getSTEMnaut3Backdoor(){
    return &stemnaut3Backdoor;
}

Simulation::STEMnaut4Backdoor* PeripheralSelector::getSTEMnaut4Backdoor(){
    return &stemnaut4Backdoor;
}

Simulation::GPSBackdoor* PeripheralSelector::getGPSBackdoor(){
    return &gpsBackdoor;
}

Simulation::Altimeter2Backdoor* PeripheralSelector::getLightAPRSAltimeterBackdoor(){
    return &altimeter2Backdoor;
}

Simulation::TransmitterBackdoor* PeripheralSelector::getTransmitterBackdoor(){
    return &transmitterBackdoor;
}

Simulation::PowerCheckBackdoor* PeripheralSelector::getPowerCheckBackdoor(){
    return &powerCheckBackdoor;
}

//implementation Selection-------------------------------------
error_t PeripheralSelector::chooseBackdoor(PeripheralNames name){
    int_t index = getIndex(name);
    if(index == -1) return PayloadOS::ERROR;
    selections[index].isBackdoor = true;
    return PayloadOS::GOOD;
}

error_t PeripheralSelector::chooseHardware(PeripheralNames name){
    int_t index = getIndex(name);
    if(index == -1) return PayloadOS::ERROR;
    if(!selections[index].hasHardware) return PayloadOS::ERROR;
    selections[index].isBackdoor = false;
    return PayloadOS::GOOD;
}

int_t PeripheralSelector::getIndex(PeripheralNames name){
    if(name == PeripheralNames::SENTINAL_COUNT) return -1;
    return static_cast<int_t>(name);
}
//singleton implementation-------------------------------------
PeripheralSelector::PeripheralSelector(){
    setHardwareBits();
}

PeripheralSelector* PeripheralSelector::get(){
    static PeripheralSelector instance;
    return &instance;
}

//initialization from Configuration----------------------------
void PeripheralSelector::setHardwareBits(){
    //Altimeter
    #ifdef PayloadOS_altimeter
        selections[getIndex(PeripheralNames::PayloadAltimeter)] = {true, false};
    #else
        selections[getIndex(PeripheralNames::PayloadAltimeter)] = {false, true};
    #endif

    //IMU
    #ifdef PayloadOS_IMU
        selections[getIndex(PeripheralNames::PayloadIMU)] = {true, false};
    #else
        selections[getIndex(PeripheralNames::PayloadIMU)] = {false, true};
    #endif

    //Transmitter
    #ifdef PayloadOS_lightAPRS_Tx
        selections[getIndex(PeripheralNames::Transmitter)] = {true, false};
    #else
        selections[getIndex(PeripheralNames::Transmitter)] = {false, true};
    #endif

    //GPS
    #ifdef PayloadOS_lightAPRS_GPS
        selections[getIndex(PeripheralNames::GPS)] = {true, false};
    #else
        selections[getIndex(PeripheralNames::GPS)] = {false, true};
    #endif

    //Altimeter 2
    #ifdef PayloadOS_lightAPRS_altimeter
        selections[getIndex(PeripheralNames::LightAPRSAltimeter)] = {true, false};
    #else
        selections[getIndex(PeripheralNames::LightAPRSAltimeter)] = {false, true};
    #endif

    //STEMnaut 1
    #ifdef PayloadOS_STEMnaut1
        selections[getIndex(PeripheralNames::STEMnaut1)] = {true, false};
    #else
        selections[getIndex(PeripheralNames::STEMnaut1)] = {false, true};
    #endif

    //STEMnaut 2
    #ifdef PayloadOS_STEMnaut2
        selections[getIndex(PeripheralNames::STEMnaut2)] = {true, false};
    #else
        selections[getIndex(PeripheralNames::STEMnaut2)] = {false, true};
    #endif

    //STEMnaut 3
    #ifdef PayloadOS_STEMnaut3
        selections[getIndex(PeripheralNames::STEMnaut3)] = {true, false};
    #else
        selections[getIndex(PeripheralNames::STEMnaut3)] = {false, true};
    #endif

    //STEMnaut 4
    #ifdef PayloadOS_STEMnaut4
        selections[getIndex(PeripheralNames::STEMnaut4)] = {true, false};
    #else
        selections[getIndex(PeripheralNames::STEMnaut4)] = {false, true};
    #endif

    //Power Check
    #ifdef PayloadOS_powerCheck
        selections[getIndex(PeripheralNames::PowerCheck)] = {true, false};
    #else
        selections[getIndex(PeripheralNames::PowerCheck)] = {false, true};
    #endif

}