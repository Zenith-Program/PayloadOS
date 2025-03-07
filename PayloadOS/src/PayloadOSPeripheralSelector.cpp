#include "PayloadOSPeripheralSelector.h"
#include "PayloadOSHardwareImplementations.h"
#include "PayloadOSBackdoorImplementations.h"
#include <cstring>

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

ArmSwitchInterface* PeripheralSelector::getArmSwitch(){
    if(selections[getIndex(PeripheralNames::ArmSwitch)].isBackdoor) return &armSwitchBackdoor;
    return &armSwitchHardware;
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

Hardware::ArmSwitchHardware* PeripheralSelector::getArmSwitchHardware(){
    if(selections[getIndex(PeripheralNames::ArmSwitch)].hasHardware) return &armSwitchHardware;
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

Simulation::ArmSwitchBackdoor* PeripheralSelector::getArmSwitchBackdoor(){
    return &armSwitchBackdoor;
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

bool PeripheralSelector::isBackdoor(PeripheralNames name) const{
    int_t index = getIndex(name);
    if(index == -1) return true;
    else return selections[index].isBackdoor;
}

int_t PeripheralSelector::getIndex(PeripheralNames name){
    if(name == PeripheralNames::SENTINAL_COUNT) return -1;
    return static_cast<int_t>(name);
}
//interface functions------------------------------------------
PeripheralNames PeripheralSelector::getEnumFromName(const char* peripheralName){
    if(std::strcmp(peripheralName, getNameFromEnum(PeripheralNames::PayloadAltimeter)) == 0) return Peripherals::PeripheralNames::PayloadAltimeter;
    if(std::strcmp(peripheralName, getNameFromEnum(PeripheralNames::LightAPRSAltimeter)) == 0) return Peripherals::PeripheralNames::LightAPRSAltimeter;
    if(std::strcmp(peripheralName, getNameFromEnum(PeripheralNames::PayloadIMU)) == 0) return Peripherals::PeripheralNames::PayloadIMU;
    if(std::strcmp(peripheralName, getNameFromEnum(PeripheralNames::STEMnaut1)) == 0) return Peripherals::PeripheralNames::STEMnaut1;
    if(std::strcmp(peripheralName, getNameFromEnum(PeripheralNames::STEMnaut2)) == 0) return Peripherals::PeripheralNames::STEMnaut2;
    if(std::strcmp(peripheralName, getNameFromEnum(PeripheralNames::STEMnaut3)) == 0) return Peripherals::PeripheralNames::STEMnaut3;
    if(std::strcmp(peripheralName, getNameFromEnum(PeripheralNames::STEMnaut4)) == 0) return Peripherals::PeripheralNames::STEMnaut4;
    if(std::strcmp(peripheralName, getNameFromEnum(PeripheralNames::Transmitter)) == 0) return Peripherals::PeripheralNames::Transmitter;
    if(std::strcmp(peripheralName, getNameFromEnum(PeripheralNames::GPS)) == 0) return Peripherals::PeripheralNames::GPS;
    if(std::strcmp(peripheralName, getNameFromEnum(PeripheralNames::ArmSwitch)) == 0) return Peripherals::PeripheralNames::ArmSwitch;
    if(std::strcmp(peripheralName, getNameFromEnum(PeripheralNames::PowerCheck)) == 0) return Peripherals::PeripheralNames::PowerCheck;
    return PeripheralNames::SENTINAL_COUNT;
}

const char* PeripheralSelector::getNameFromEnum(PeripheralNames name){
    switch(name){
        case PeripheralNames::ArmSwitch:
        return "armSwitch";
        break;
        case PeripheralNames::PayloadAltimeter:
        return "altimeter";
        break;
        case PeripheralNames::LightAPRSAltimeter:
        return "altimeter2";
        case PeripheralNames::PayloadIMU:
        return "IMU";
        break;
        case PeripheralNames::STEMnaut1:
        return "STEMnaut1";
        break;
        case PeripheralNames::STEMnaut2:
        return "STEMnaut2";
        break;
    case PeripheralNames::STEMnaut3:
        return "STEMnaut3";
        break;
    case PeripheralNames::STEMnaut4:
        return "STEMnaut4";
        break;
    case PeripheralNames::Transmitter:
        return "transmitter";
        break;
    case PeripheralNames::GPS:
        return "GPS";
        break;
    case PeripheralNames::PowerCheck:
        return "ADC";
        break;
    default:
        return "";
    }
}

error_t PeripheralSelector::initAll(){
    if(Peripherals::PeripheralSelector::get()->getArmSwitch()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getPowerCheck()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getPayloadIMU()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getTransmitter()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getGPS()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getSTEMnaut1()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getSTEMnaut2()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getSTEMnaut3()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getSTEMnaut4()->init() == PayloadOS::ERROR) return PayloadOS::ERROR;
    return PayloadOS::GOOD;
}

error_t PeripheralSelector::init(PeripheralNames name){
    switch(name){
        case PeripheralNames::ArmSwitch:
        return PeripheralSelector::get()->getArmSwitch()->init();
        break;
        case PeripheralNames::PayloadAltimeter:
        return PeripheralSelector::get()->getPayloadAltimeter()->init();
        break;
        case PeripheralNames::LightAPRSAltimeter:
        return PeripheralSelector::get()->getLightAPRSAltimeter()->init();
        break;
        case PeripheralNames::PayloadIMU:
        return PeripheralSelector::get()->getPayloadIMU()->init();
        break;
        case PeripheralNames::STEMnaut1:
        return PeripheralSelector::get()->getSTEMnaut1()->init();
        break;
        case PeripheralNames::STEMnaut2:
        return PeripheralSelector::get()->getSTEMnaut2()->init();
        break;
    case PeripheralNames::STEMnaut3:
        return PeripheralSelector::get()->getSTEMnaut3()->init();
        break;
    case PeripheralNames::STEMnaut4:
        return PeripheralSelector::get()->getSTEMnaut4()->init();
        break;
    case PeripheralNames::Transmitter:
        return PeripheralSelector::get()->getTransmitter()->init();
        break;
    case PeripheralNames::GPS:
        return PeripheralSelector::get()->getGPS()->init();
        break;
    case PeripheralNames::PowerCheck:
        return PeripheralSelector::get()->getPowerCheck()->init();
        break;
    default:
        return PayloadOS::ERROR;
    }
}

error_t PeripheralSelector::deInitAll(){
    if(Peripherals::PeripheralSelector::get()->getArmSwitch()->deInit() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getPowerCheck()->deInit() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->deInit() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getPayloadIMU()->deInit() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getTransmitter()->deInit() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getGPS()->deInit() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->deInit() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getSTEMnaut1()->deInit() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getSTEMnaut2()->deInit() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getSTEMnaut3()->deInit() == PayloadOS::ERROR) return PayloadOS::ERROR;
    if(Peripherals::PeripheralSelector::get()->getSTEMnaut4()->deInit() == PayloadOS::ERROR) return PayloadOS::ERROR;
    return PayloadOS::GOOD;
}

error_t PeripheralSelector::deInit(PeripheralNames name){
    switch(name){
        case PeripheralNames::ArmSwitch:
        return PeripheralSelector::get()->getArmSwitch()->deInit();
        break;
        case PeripheralNames::PayloadAltimeter:
        return PeripheralSelector::get()->getPayloadAltimeter()->deInit();
        break;
        case PeripheralNames::LightAPRSAltimeter:
        return PeripheralSelector::get()->getLightAPRSAltimeter()->deInit();
        break;
        case PeripheralNames::PayloadIMU:
        return PeripheralSelector::get()->getPayloadIMU()->deInit();
        break;
        case PeripheralNames::STEMnaut1:
        return PeripheralSelector::get()->getSTEMnaut1()->deInit();
        break;
        case PeripheralNames::STEMnaut2:
        return PeripheralSelector::get()->getSTEMnaut2()->deInit();
        break;
    case PeripheralNames::STEMnaut3:
        return PeripheralSelector::get()->getSTEMnaut3()->deInit();
        break;
    case PeripheralNames::STEMnaut4:
        return PeripheralSelector::get()->getSTEMnaut4()->deInit();
        break;
    case PeripheralNames::Transmitter:
        return PeripheralSelector::get()->getTransmitter()->deInit();
        break;
    case PeripheralNames::GPS:
        return PeripheralSelector::get()->getGPS()->deInit();
        break;
    case PeripheralNames::PowerCheck:
        return PeripheralSelector::get()->getPowerCheck()->init();
        break;
    default:
        return PayloadOS::ERROR;
    }
}

error_t PeripheralSelector::getStatus(PeripheralNames name) const{
    switch(name){
        case PeripheralNames::ArmSwitch:
        return PeripheralSelector::get()->getArmSwitch()->status();
        break;
        case PeripheralNames::PayloadAltimeter:
        return PeripheralSelector::get()->getPayloadAltimeter()->status();
        break;
        case PeripheralNames::LightAPRSAltimeter:
        return PeripheralSelector::get()->getLightAPRSAltimeter()->status();
        break;
        case PeripheralNames::PayloadIMU:
        return PeripheralSelector::get()->getPayloadIMU()->status();
        break;
        case PeripheralNames::STEMnaut1:
        return PeripheralSelector::get()->getSTEMnaut1()->status();
        break;
        case PeripheralNames::STEMnaut2:
        return PeripheralSelector::get()->getSTEMnaut2()->status();
        break;
    case PeripheralNames::STEMnaut3:
        return PeripheralSelector::get()->getSTEMnaut3()->status();
        break;
    case PeripheralNames::STEMnaut4:
        return PeripheralSelector::get()->getSTEMnaut4()->status();
        break;
    case PeripheralNames::Transmitter:
        return PeripheralSelector::get()->getTransmitter()->status();
        break;
    case PeripheralNames::GPS:
        return PeripheralSelector::get()->getGPS()->status();
        break;
    case PeripheralNames::PowerCheck:
        return PeripheralSelector::get()->getPowerCheck()->status();
        break;
    default:
        return PayloadOS::ERROR;
    }
}

void PeripheralSelector::changeHardwareAvailability(PeripheralNames name, bool connect){
    int_t index = getIndex(name);
    if(index == -1) return;
    if(!connect){
        chooseBackdoor(name);
    }
    selections[index].hasHardware = connect;
}

bool PeripheralSelector::getHardwareAvailibility(PeripheralNames name) const{
    int_t index = getIndex(name);
    if(index != -1)  return selections[index].hasHardware;
    return false;
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

    //Arm Switch
    #ifdef PayloadOS_armSwitch
        selections[getIndex(PeripheralNames::ArmSwitch)] = {true, false};
    #else
        selections[getIndex(PeripheralNames::ArmSwitch)] = {false, true};
    #endif

}