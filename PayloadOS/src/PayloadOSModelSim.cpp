#include "PayloadOSModelSim.h"
#include "PayloadOSModelSimStates.h"
#include "PayloadOSPeripheralSelector.h"
#include "PayloadOSModelSimData.h"
#include <cstring>
#include <Arduino.h>

using namespace PayloadOS;
using namespace Simulation;
using namespace Model;

//operation
void ModelSim::start(){
    stop();
    state = SimStates::Delay;
    getState(state)->init();
    initTimer(period);
}

void ModelSim::stop(){
    stopTimer();
    state = SimStates::Inactive;
}

void ModelSim::clock(){
    UpdateParams data = getState(state)->update();
    setBackdoors(data);
    if(data.change){
        getState(state)->end();
        state = nextState(state);
        getState(state)->init();
    }
    
}

void ModelSim::ClockISR(){
    ModelSim::get()->clock();
}

//preferences
void ModelSim::setClockPeriod(uint_t period){
    this->period = period;
    if(state != SimStates::Inactive){
        timer.end();
        timer.begin(ClockISR, period);
    }
}

//static helpers
void ModelSim::initTimer(uint_t period){
    timer.begin(ClockISR, period);
}

void ModelSim::stopTimer(){
    timer.end();
}

SimState* ModelSim::getState(SimStates state){
    switch(state){
        case SimStates::Inactive:
            return Inactive::get();
        break;
        case SimStates::Delay:
            return Delay::get();
        break;
        case SimStates::Boost:
            return Boost::get();
        break;
        case SimStates::Coast:
            return Coast::get();
        break;
        case SimStates::Apogee:
            return Apogee::get();
        break;
        case SimStates::Drogue:
            return Drogue::get();
        break;
        case SimStates::Deploy:
            return Deploy::get();
        break;
        case SimStates::Main:
            return Main::get();
        break;
        case SimStates::Landing:
            return Landing::get();
        break;
        case SimStates::Landed:
            return Landed::get();
        break;
        default:
            return nullptr;
            //error
        break;
    }
}

SimStates ModelSim::nextState(SimStates state){
    if(state == SimStates::Inactive) return SimStates::Inactive;
    if(state == SimStates::Landed) return SimStates::Inactive;
    return static_cast<SimStates>(static_cast<uint_t>(state)+1);
}

void ModelSim::setBackdoors(const UpdateParams& data){
    //altimeters
    Peripherals::PeripheralSelector::get()->getPayloadAltimeterBackdoor()->setCurrentAltitude_m(data.altitude);
    Peripherals::PeripheralSelector::get()->getPayloadAltimeterBackdoor()->setCurrrentPressure_mBar(data.pressure);
    Peripherals::PeripheralSelector::get()->getPayloadAltimeterBackdoor()->setCurrentTemperature_K(data.temp);

    Peripherals::PeripheralSelector::get()->getLightAPRSAltimeterBackdoor()->setCurrentAltitude_m(data.altitude2);
    Peripherals::PeripheralSelector::get()->getLightAPRSAltimeterBackdoor()->setCurrrentPressure_mBar(data.pressure2);
    Peripherals::PeripheralSelector::get()->getLightAPRSAltimeterBackdoor()->setCurrentTemperature_K(data.temp2);

    //IMU's
    Peripherals::PeripheralSelector::get()->getPayloadIMUBackdoor()->setCurrentAcceleration_m_s2(data.acceleration0);
    Peripherals::PeripheralSelector::get()->getPayloadIMUBackdoor()->setCurrentAngularVelocity_deg_s(data.angularVelocity0);
    Peripherals::PeripheralSelector::get()->getPayloadIMUBackdoor()->setGravity(data.gravity0);

    Peripherals::PeripheralSelector::get()->getSTEMnaut1Backdoor()->setCurrentAcceleration_m_s2(data.acceleration1);
    Peripherals::PeripheralSelector::get()->getSTEMnaut1Backdoor()->setCurrentAngularVelocity_deg_s(data.angularVelocity1);
    Peripherals::PeripheralSelector::get()->getSTEMnaut1Backdoor()->setGravity(data.gravity1);

    Peripherals::PeripheralSelector::get()->getSTEMnaut2Backdoor()->setCurrentAcceleration_m_s2(data.acceleration2);
    Peripherals::PeripheralSelector::get()->getSTEMnaut2Backdoor()->setCurrentAngularVelocity_deg_s(data.angularVelocity2);
    Peripherals::PeripheralSelector::get()->getSTEMnaut2Backdoor()->setGravity(data.gravity2);

    Peripherals::PeripheralSelector::get()->getSTEMnaut3Backdoor()->setCurrentAcceleration_m_s2(data.acceleration3);
    Peripherals::PeripheralSelector::get()->getSTEMnaut3Backdoor()->setCurrentAngularVelocity_deg_s(data.angularVelocity3);
    Peripherals::PeripheralSelector::get()->getSTEMnaut3Backdoor()->setGravity(data.gravity3);

    Peripherals::PeripheralSelector::get()->getSTEMnaut4Backdoor()->setCurrentAcceleration_m_s2(data.acceleration4);
    Peripherals::PeripheralSelector::get()->getSTEMnaut4Backdoor()->setCurrentAngularVelocity_deg_s(data.angularVelocity4);
    Peripherals::PeripheralSelector::get()->getSTEMnaut4Backdoor()->setGravity(data.gravity4);

    //GPS
    Peripherals::PeripheralSelector::get()->getGPSBackdoor()->setData(data.gps);

    //Power
    Peripherals::PeripheralSelector::get()->getPowerCheckBackdoor()->setPower(data.voltage);

    //Switch
    Peripherals::PeripheralSelector::get()->getArmSwitchBackdoor()->setState(data.arm);
}

//singleton
ModelSim* ModelSim::get(){
    static ModelSim instance;
    return &instance;
}


ModelSim::ModelSim() : state(SimStates::Inactive), period(PayloadOS_ModelSimDefaultClockPeriod_us){}
//helpers----------------------------------------------------------------------
const char* ModelSim::getStateName(SimStates state){
    if(state == SimStates::Inactive) return "inactive";
    if(state == SimStates::Delay) return "delay";
    if(state == SimStates::Boost) return "boost";
    if(state == SimStates::Coast) return "coast";
    if(state == SimStates::Apogee) return "apogee";
    if(state == SimStates::Drogue) return "drogue";
    if(state == SimStates::Deploy) return "deploy";
    if(state == SimStates::Main) return "main";
    if(state == SimStates::Landing) return "landing";
    if(state == SimStates::Landed) return "landed";
    return "undefined";
}

//commands---------------------------------------------------------------------
const Interpreter::CommandList* ModelSim::getCommands(){
    static constexpr auto cmds = std::array{
        CMD{"startSim", "", startSim},
        CMD{"stopSim", "", stopSim},
        CMD{"simStatus", "", simStatus},
        CMD{"simClock", "uw", setSimClock},
        CMD{"simParameters", "", getSimData},
        CMD{"setSimParameter", "sf", setSimData},
        CMD{"seedSimRNG", "u", seedRNG}
    };
    static const Interpreter::CommandList simList(cmds.data(), cmds.size());
    return &simList;
}

void ModelSim::startSim(const Interpreter::Token*){
    get()->start();
}

void ModelSim::stopSim(const Interpreter::Token*){
    get()->stop();
}

void ModelSim::simStatus(const Interpreter::Token*){
    Serial.print("Simulation state: ");
    Serial.println(getStateName(get()->state));
    Serial.print("Clock period: ");
    Serial.print(get()->period/1000.0);
    Serial.println("ms");
}

void ModelSim::setSimClock(const Interpreter::Token* args){
    uint_t period = args[0].getUnsignedData();
    char unit[5];
    args[1].copyStringData(unit, 5);
    if(std::strcmp(unit, "ns") == 0){
        get()->setClockPeriod(period/1000);
        return;
    }
    if(std::strcmp(unit, "us") == 0){
        get()->setClockPeriod(period);
        return;
    }
    if(std::strcmp(unit, "ms") == 0){
        get()->setClockPeriod(period*1000);
        return;
    }
    if(std::strcmp(unit, "s") == 0){
        get()->setClockPeriod(period*1000000);
        return;
    }
    Serial.print("'");
    Serial.print(unit);
    Serial.println("' is not a valid unit. Choose ns, us, ms, or s");
    get()->setClockPeriod(period);
}
void ModelSim::getSimData(const Interpreter::Token*){
    ModelSimData::get()->printData();
}
void ModelSim::setSimData(const Interpreter::Token* args){
    char dataName[64];
    args[0].copyStringData(dataName, 64);
    float_t value = args[1].getFloatData();
    Parameter* target = ModelSimData::get()->getDataWithName(dataName);
    if(target == nullptr){
        Serial.print("ModelSim does not have a parameter with the name '");
        Serial.print(dataName);
        Serial.println("'");
        return;
    }
    if(target->positive && value < 0){
        Serial.printf("'%s' cannot be assigned a negative value\n", dataName);
        return;
    }
    target->value = value;
}

void ModelSim::seedRNG(const Interpreter::Token* args){
    uint_t newSeed = args[0].getUnsignedData();
    randomSeed(newSeed);
}