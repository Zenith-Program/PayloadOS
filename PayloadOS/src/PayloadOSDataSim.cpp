#include <cstring>
#include "PayloadOSDataSim.h"
#include "PayloadOSSD.h"


using namespace PayloadOS;
using namespace Simulation;
using namespace Data;

#define FEET_TO_METERS 0.3048

error_t DataSim::start(){
    stop();
    if(FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::HIL)->openForRead() == PayloadOS::ERROR) return PayloadOS::ERROR;
    isActive = true;
    initTimer(period);
    return PayloadOS::GOOD;
}

error_t DataSim::stop(){
    stopTimer();
    isActive = false;
    clearBackdoors();
    return FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::HIL)->close();
}

bool DataSim::active() const{
    return isActive;
}

void DataSim::setClockPeriod_us(uint_t newValue){
    period = newValue;
}

uint_t DataSim::getClockPeriod_us() const{
    return period;
}

//helpers------------------------------------------------------
void DataSim::timerCallback(){
    FlightData::TelemetryData data;
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::HIL)->readLine(data);
    //update flight data
    //altimeters
    Peripherals::PeripheralSelector::get()->getPayloadAltimeterBackdoor()->setCurrentAltitude_m(FEET_TO_METERS * data.altitude1); //other units may be a problem in the future
    Peripherals::PeripheralSelector::get()->getPayloadAltimeterBackdoor()->setCurrrentPressure_mBar(data.pressure1);
    Peripherals::PeripheralSelector::get()->getPayloadAltimeterBackdoor()->setCurrentTemperature_K(data.temperature1);

    Peripherals::PeripheralSelector::get()->getLightAPRSAltimeterBackdoor()->setCurrentAltitude_m(FEET_TO_METERS* data.altitude2);
    Peripherals::PeripheralSelector::get()->getLightAPRSAltimeterBackdoor()->setCurrrentPressure_mBar(data.pressure2);
    Peripherals::PeripheralSelector::get()->getLightAPRSAltimeterBackdoor()->setCurrentTemperature_K(data.temperature2);

    //IMU's
    Peripherals::PeripheralSelector::get()->getPayloadIMUBackdoor()->setCurrentAcceleration_m_s2(data.accel0);
    Peripherals::PeripheralSelector::get()->getPayloadIMUBackdoor()->setCurrentAngularVelocity_deg_s(data.angularVelocity0);
    Peripherals::PeripheralSelector::get()->getPayloadIMUBackdoor()->setGravity(data.grav0);

    Peripherals::PeripheralSelector::get()->getSTEMnaut1Backdoor()->setCurrentAcceleration_m_s2(data.accel1);
    Peripherals::PeripheralSelector::get()->getSTEMnaut1Backdoor()->setCurrentAngularVelocity_deg_s(data.angularVelocity1);
    Peripherals::PeripheralSelector::get()->getSTEMnaut1Backdoor()->setGravity(data.grav1);

    Peripherals::PeripheralSelector::get()->getSTEMnaut2Backdoor()->setCurrentAcceleration_m_s2(data.accel2);
    Peripherals::PeripheralSelector::get()->getSTEMnaut2Backdoor()->setCurrentAngularVelocity_deg_s(data.angularVelocity2);
    Peripherals::PeripheralSelector::get()->getSTEMnaut2Backdoor()->setGravity(data.grav2);

    Peripherals::PeripheralSelector::get()->getSTEMnaut3Backdoor()->setCurrentAcceleration_m_s2(data.accel3);
    Peripherals::PeripheralSelector::get()->getSTEMnaut3Backdoor()->setCurrentAngularVelocity_deg_s(data.angularVelocity3);
    Peripherals::PeripheralSelector::get()->getSTEMnaut3Backdoor()->setGravity(data.grav3);

    Peripherals::PeripheralSelector::get()->getSTEMnaut4Backdoor()->setCurrentAcceleration_m_s2(data.accel4);
    Peripherals::PeripheralSelector::get()->getSTEMnaut4Backdoor()->setCurrentAngularVelocity_deg_s(data.angularVelocity4);
    Peripherals::PeripheralSelector::get()->getSTEMnaut4Backdoor()->setGravity(data.grav4);

    //GPS
    Peripherals::PeripheralSelector::get()->getGPSBackdoor()->setData(data.gps);

    //Power
    Peripherals::PeripheralSelector::get()->getPowerCheckBackdoor()->setPower(data.power);

    //Switch
    Peripherals::PeripheralSelector::get()->getArmSwitchBackdoor()->setState(true);
    //stop if eof
    if(data.endOfFile){
        get()->stop();
    } 
}

void DataSim::initTimer(uint_t){
    timer.begin(timerCallback, period);
}

void DataSim::stopTimer(){
    timer.end();
}

void DataSim::clearBackdoors(){
    //altimeters
    Peripherals::PeripheralSelector::get()->getPayloadAltimeterBackdoor()->setCurrentAltitude_m(0);
    Peripherals::PeripheralSelector::get()->getPayloadAltimeterBackdoor()->setCurrrentPressure_mBar(0);
    Peripherals::PeripheralSelector::get()->getPayloadAltimeterBackdoor()->setCurrentTemperature_K(0);

    Peripherals::PeripheralSelector::get()->getLightAPRSAltimeterBackdoor()->setCurrentAltitude_m(0);
    Peripherals::PeripheralSelector::get()->getLightAPRSAltimeterBackdoor()->setCurrrentPressure_mBar(0);
    Peripherals::PeripheralSelector::get()->getLightAPRSAltimeterBackdoor()->setCurrentTemperature_K(0);

    //IMU's
    Peripherals::PeripheralSelector::get()->getPayloadIMUBackdoor()->setCurrentAcceleration_m_s2({0,0,0});
    Peripherals::PeripheralSelector::get()->getPayloadIMUBackdoor()->setCurrentAngularVelocity_deg_s({0,0,0});
    Peripherals::PeripheralSelector::get()->getPayloadIMUBackdoor()->setGravity({0,0,0});

    Peripherals::PeripheralSelector::get()->getSTEMnaut1Backdoor()->setCurrentAcceleration_m_s2({0,0,0});
    Peripherals::PeripheralSelector::get()->getSTEMnaut1Backdoor()->setCurrentAngularVelocity_deg_s({0,0,0});
    Peripherals::PeripheralSelector::get()->getSTEMnaut1Backdoor()->setGravity({0,0,0});

    Peripherals::PeripheralSelector::get()->getSTEMnaut2Backdoor()->setCurrentAcceleration_m_s2({0,0,0});
    Peripherals::PeripheralSelector::get()->getSTEMnaut2Backdoor()->setCurrentAngularVelocity_deg_s({0,0,0});
    Peripherals::PeripheralSelector::get()->getSTEMnaut2Backdoor()->setGravity({0,0,0});

    Peripherals::PeripheralSelector::get()->getSTEMnaut3Backdoor()->setCurrentAcceleration_m_s2({0,0,0});
    Peripherals::PeripheralSelector::get()->getSTEMnaut3Backdoor()->setCurrentAngularVelocity_deg_s({0,0,0});
    Peripherals::PeripheralSelector::get()->getSTEMnaut3Backdoor()->setGravity({0,0,0});

    Peripherals::PeripheralSelector::get()->getSTEMnaut4Backdoor()->setCurrentAcceleration_m_s2({0,0,0});
    Peripherals::PeripheralSelector::get()->getSTEMnaut4Backdoor()->setCurrentAngularVelocity_deg_s({0,0,0});
    Peripherals::PeripheralSelector::get()->getSTEMnaut4Backdoor()->setGravity({0,0,0});

    //GPS
    Peripherals::PeripheralSelector::get()->getGPSBackdoor()->setData({{0,0},0,0,0});

    //Power
    Peripherals::PeripheralSelector::get()->getPowerCheckBackdoor()->setPower(0);

    //Switch
    Peripherals::PeripheralSelector::get()->getArmSwitchBackdoor()->setState(false);
}

//commands-----------------------------------------------------
const Interpreter::CommandList* DataSim::getCommands(){
    static constexpr auto cmds = std::array{
        CMD{"startSim", "", start_c},
        CMD{"stopSim", "", stop_c},
        CMD{"simStatus", "", status_c},
        CMD{"simClock", "uw", setClock_c},
    };
    static const Interpreter::CommandList simList(cmds.data(), cmds.size());
    return &simList;
}
void DataSim::start_c(const Interpreter::Token*){
    if(get()->start() == PayloadOS::ERROR) Serial.println("Error starting dataSim");
}

void DataSim::stop_c(const Interpreter::Token*){
    if(get()->stop() == PayloadOS::ERROR) Serial.println("Error closing dataSim file. Check for corruption");
}

void DataSim::status_c(const Interpreter::Token*){
    if(get()->active()) Serial.println("running");
    else Serial.println("idle");
}

void DataSim::setClock_c(const Interpreter::Token* args){
    uint_t newPeriod = args[0].getUnsignedData();
    char unit[8];
    args[1].copyStringData(unit, 8);
    if(std::strcmp(unit, "ns") == 0){
        get()->setClockPeriod_us(newPeriod/1000);
        return;
    }
    if(std::strcmp(unit, "us") == 0){
        get()->setClockPeriod_us(newPeriod);
        return;
    }
    if(std::strcmp(unit, "ms") == 0){
        get()->setClockPeriod_us(newPeriod*1000);
        return;
    }
    if(std::strcmp(unit, "s") == 0){
        get()->setClockPeriod_us(newPeriod*1000000);
        return;
    }
    Serial.print("'");
    Serial.print(unit);
    Serial.println("' is not a valid unit. Choose ns, us, ms, or s");
    get()->setClockPeriod_us(newPeriod);
}
                

            
//singleton----------------------------------------------------
 DataSim* DataSim::get(){
    static DataSim instance;
    return &instance;
 }

DataSim::DataSim() : period(PayloadOS_DataSimDefaultClockPeriod_us), isActive(false) {}