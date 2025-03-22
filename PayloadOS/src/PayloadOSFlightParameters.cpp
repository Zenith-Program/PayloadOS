#include "PayloadOSFlightParameters.h"
#include <Arduino.h>
#include <cstring>

using namespace PayloadOS;
using namespace FlightData;

void FlightParameters::makeDefault(){
    for(uint_t i=0; i<data.size(); i++)
        data[i].value = data[i].defaultValue;
}

void FlightParameters::printData() const{
    Serial.println("##### Flight Parameters #####");
    for(uint_t i=0; i<data.size(); i++)
        Serial.printf("%s: %.2f%s\n", data[i].name, data[i].value, data[i].unit);
}

FlightParameter* FlightParameters::getDataWithName(const char* name){
    for(uint_t i = 0; i<data.size(); i++)
        if(std::strcmp(name, data[i].name) == 0) return &data[i];
    return nullptr;
}

const FlightParameter* FlightParameters::getDataWithName(const char* name) const{
    for(uint_t i = 0; i<data.size(); i++)
        if(std::strcmp(name, data[i].name) == 0) return &data[i];
    return nullptr;
}

FlightParameter* FlightParameters::getData(FlightParameterNames name){
    int_t index = getIndex(name);
    if(index == -1) return nullptr;
    return &data[index];
}

const FlightParameter* FlightParameters::getData(FlightParameterNames name) const{
    int_t index = getIndex(name);
    if(index == -1) return nullptr;
    return &data[index];
}

int_t FlightParameters::getIndex(FlightParameterNames name){
    uint_t index = static_cast<uint_t>(name);
    if(index >= PayloadOS_NumberOfFlightParameters) return -1;
    return static_cast<int_t>(index);
}

FlightParameters::FlightParameters() : data(defaultInit()){}

//default values----------------------------------------------
#define FlightParameter_CovarianceWindowSize 16
#define FlightParameter_UpwardMotionThreshold_ft_s 5
#define FlightParameter_DownwardMotionThreshold_ft_s -1
#define FlightParameter_LaunchResetTime_s 8
#define FlightParameter_MinimumDescentTime_s 25
#define FlightParameter_MinimumLandingTime_s 1
#define FlightParameter_OutlierCount 2
//------------------------------------------------------------

constexpr ParameterData FlightParameters::defaultInit(){
    return {{
        {"covariance window size", FlightParameter_CovarianceWindowSize,FlightParameter_CovarianceWindowSize,""},
        {"upward motion threshold", FlightParameter_UpwardMotionThreshold_ft_s, FlightParameter_UpwardMotionThreshold_ft_s, "ft/s"},
        {"downward motion threshold", FlightParameter_DownwardMotionThreshold_ft_s, FlightParameter_DownwardMotionThreshold_ft_s, "ft/s"},
        {"minimum ascent time", FlightParameter_LaunchResetTime_s, FlightParameter_LaunchResetTime_s, "s"},
        {"minimum descent time", FlightParameter_MinimumDescentTime_s, FlightParameter_MinimumDescentTime_s, "s"},
        {"minimum landing time", FlightParameter_MinimumLandingTime_s, FlightParameter_MinimumLandingTime_s, "s"},
        {"covariance outlier count", FlightParameter_OutlierCount, FlightParameter_OutlierCount, ""}
    }};
}

FlightParameters* FlightParameters::get(){
    static FlightParameters instance;
    return &instance;
}

//commands-------------------------------------------
void FlightParameters::print_c(const Interpreter::Token*){
    get()->printData();
}

void FlightParameters::setParam_c(const Interpreter::Token* args){
    char nameBuffer[64];
    args[0].copyStringData(nameBuffer, 64);
    float_t newValue = args[1].getFloatData();
    FlightParameter* param = get()->getDataWithName(nameBuffer);
    if(param == nullptr){
        Serial.println("Invalid flight parameter name");
        return;
    }
    param->value = newValue;
}