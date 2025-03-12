#include "PayloadOSModelSimData.h"
#include <Arduino.h>
#include <cstring>

using namespace PayloadOS;
using namespace Simulation;
using namespace Model;

void ModelSimData::makeDefault(){
    for(uint_t i = 0; i<external.size(); i++)
        external[i].value = external[i].defaultValue;
}

void ModelSimData::printData() const{
    Serial.println("##### ModelSim Parameters #####");
    for(uint_t i=0; i<external.size(); i++)
        Serial.printf("%s: %.2f%s\n", external[i].name, external[i].value, external[i].units);
}
Parameter* ModelSimData::getDataWithName(const char* name){
    for(uint_t i = 0; i<external.size(); i++)
        if(std::strcmp(name, external[i].name) == 0) return &external[i];
    return nullptr;
}
const Parameter* ModelSimData::getDataWithName(const char* name) const{
    for(uint_t i = 0; i<external.size(); i++)
        if(std::strcmp(name, external[i].name) == 0) return &external[i];
    return nullptr;
}

Parameter* ModelSimData::getData(ModifiableDataNames name){
    int_t index = getIndex(name);
    if(index == -1) return nullptr;
    return &external[index];
}

const Parameter* ModelSimData::getData(ModifiableDataNames name) const{
    int_t index = getIndex(name);
    if(index == -1) return nullptr;
    return &external[index];
}

int_t ModelSimData::getIndex(ModifiableDataNames name){
    uint_t index = static_cast<uint_t>(name);
    if(index >= PayloadOS_NumberOfModifiables) return -1;
    return static_cast<int_t>(index);
}



//singleton implementation
ModelSimData::ModelSimData() :  external(defaultExternal()){}

ModelSimData* ModelSimData::get(){
    static ModelSimData instance;
    return &instance;
}

//default values
#define Data_LaunchSiteTemp 72              //F
#define Data_InitialPower 8.4               //volts
#define GPSFixPeriod 1                      //s
#define Data_PowerConsumptionRate 0.00001   //volts/s
#define Data_MotorStrength 200              //ft/s^2
#define Data_Satalites 10                   //normal num of sats
#define Data_InitialLatitude 27.93298       //coordinates at palm bay
#define Data_InitialLongitude 80.70833      //coordinates at palm bay

#define DefaultData_Altimeter1Noise 0.05    //standard deviation
#define DefaultData_Altimeter2Noise 0.05    //standard deviation
#define DefaultData_IMU0Noise 0.05          //standard deviation
#define DefaultData_IMU1Noise 0.05          //standard deviation
#define DefaultData_IMU2Noise 0.05          //standard deviation
#define DefaultData_IMU3Noise 0.05          //standard deviation
#define DefaultData_IMU4Noise 0.05          //standard deviation
#define DefaultData_PowerNoise 0.05         //standard deviation
#define DefaultData_InitialAltitude 100      //ft
#define DefaultData_TimeToLaunchAfterArm 5  //seconds until sim lanches the rocket
#define DefaultData_LaunchRailAngle 4.5     //degrees
#define DefaultData_DragCoefficient 0.0001  //u for airbrakes (not actually drag coef)       
#define DefaultData_EjectionPower 10        //scalar for model
#define DefaultData_EjectionDuration 1      //sec  
#define DefaultData_EjectionDelay 0.5       //sec   
#define DefaultData_LandingDeltaT 0.1       //seconds (time impulse is spread acros)
#define DefaultData_DrogueSpeed 50          //ft/s
#define DefaultData_MainSpeed 20            //ft/s
#define DefaultData_DriftSpeed 15           //ft/s (assuming similar to wind)
#define DefaultData_OscilationPeriod 0.5    //seconds    
#define DefaultData_OscilationAmplitude 10  //degrees (thought of as a pendulum)
#define DefaultData_GPSAltNoise 0.05        
#define DefaultData_GPSPosNoise 0.05
#define DefaultData_Temperature1Noise 0.05        
#define DefaultData_Temperature2Noise 0.05
#define DefaultData_LinearPowerRatio 3.28   //ft/s
#define DefaultData_AngularPowerRatio 90    //degrees
#define DefaultData_GravityPowerRatio 9.8   //move around pointing vector
#define DefaultData_AltitudePowerRatio 1    //ft
#define DefaultData_MotorBurnTime 3         //seconds

#define Domain_Positive true
#define Domain_All false

constexpr ModifiableData ModelSimData::defaultExternal(){
    return {{
        {"launch site temperature", Domain_All, Data_LaunchSiteTemp, Data_LaunchSiteTemp, "F"},
        {"initial power", Domain_Positive, Data_InitialPower, Data_InitialPower, "V"},
        {"GPS fix period", Domain_Positive, GPSFixPeriod, GPSFixPeriod, "s"},
        {"power consumption rate", Domain_Positive, Data_PowerConsumptionRate, Data_PowerConsumptionRate, "V/s"},
        {"motor strength", Domain_Positive, Data_MotorStrength, Data_MotorStrength, "ft/s^2"},
        {"visible satalites", Domain_Positive, Data_Satalites, Data_Satalites, ""},
        {"launch site latitude", Domain_All, Data_InitialLatitude, Data_InitialLatitude, "degN"},
        {"launch site longitude", Domain_All, Data_InitialLongitude, Data_InitialLongitude, "degE"},
        {"altimeter1 noise", Domain_Positive, DefaultData_Altimeter1Noise, DefaultData_Altimeter1Noise, ""},
        {"altimeter2 noise", Domain_Positive, DefaultData_Altimeter2Noise, DefaultData_Altimeter2Noise, ""},
        {"IMU noise", Domain_Positive, DefaultData_IMU0Noise, DefaultData_IMU0Noise, ""},
        {"STEMnaut1 noise", Domain_Positive, DefaultData_IMU1Noise, DefaultData_IMU1Noise, ""},
        {"STEMnaut2 noise", Domain_Positive, DefaultData_IMU2Noise, DefaultData_IMU2Noise, ""},
        {"STEMnaut3 noise", Domain_Positive, DefaultData_IMU3Noise, DefaultData_IMU3Noise, ""},
        {"STEMnaut4 noise", Domain_Positive, DefaultData_IMU4Noise, DefaultData_IMU4Noise, ""},
        {"power check noise", Domain_Positive, DefaultData_PowerNoise, DefaultData_PowerNoise, ""},
        {"launch site elevation", Domain_All, DefaultData_InitialAltitude, DefaultData_InitialAltitude, "ft"},
        {"launch time after arm", Domain_Positive, DefaultData_TimeToLaunchAfterArm, DefaultData_TimeToLaunchAfterArm, "s"},
        {"launch rail angle", Domain_Positive, DefaultData_LaunchRailAngle, DefaultData_LaunchRailAngle, "deg"},
        {"drag coefficient", Domain_Positive, DefaultData_DragCoefficient, DefaultData_DragCoefficient, ""},
        {"ejection power", Domain_Positive, DefaultData_EjectionPower, DefaultData_EjectionPower, ""},
        {"ejection durration", Domain_Positive, DefaultData_EjectionDuration, DefaultData_EjectionDuration, "s"},
        {"ejection delay", Domain_Positive, DefaultData_EjectionDelay, DefaultData_EjectionDelay, "s"},
        {"landing impulse time", Domain_Positive, DefaultData_LandingDeltaT, DefaultData_LandingDeltaT, "s"},
        {"drogue descent speed", Domain_Positive, DefaultData_DrogueSpeed, DefaultData_DrogueSpeed, "ft/s"},
        {"main descent speed", Domain_Positive, DefaultData_MainSpeed, DefaultData_MainSpeed, "ft/s"},
        {"wind drift speed", Domain_Positive, DefaultData_DriftSpeed, DefaultData_DriftSpeed, "ft/s"},
        {"descent oscilation period", Domain_Positive, DefaultData_OscilationPeriod, DefaultData_OscilationPeriod, "s"},
        {"descent oscilation amplitude", Domain_Positive, DefaultData_OscilationAmplitude, DefaultData_OscilationAmplitude, "deg"},
        {"GPS altitude noise", Domain_Positive, DefaultData_GPSAltNoise, DefaultData_GPSAltNoise, ""},
        {"GPS position noise", Domain_Positive, DefaultData_GPSPosNoise, DefaultData_GPSPosNoise, ""},
        {"thermometer1 noise", Domain_Positive, DefaultData_Temperature1Noise, DefaultData_Temperature1Noise, ""},
        {"thermometer2 noise", Domain_Positive, DefaultData_Temperature2Noise, DefaultData_Temperature2Noise, ""},
        {"linear acceleration power", Domain_Positive, DefaultData_LinearPowerRatio, DefaultData_LinearPowerRatio, "ft/s^2"},
        {"angular velocity power", Domain_Positive, DefaultData_AngularPowerRatio, DefaultData_AngularPowerRatio, "deg/s"},
        {"orientation power", Domain_Positive, DefaultData_GravityPowerRatio, DefaultData_GravityPowerRatio, "g"},
        {"altitude power", Domain_Positive, DefaultData_AltitudePowerRatio, DefaultData_AltitudePowerRatio, "ft"},
        {"motor burn time", Domain_Positive, DefaultData_MotorBurnTime, DefaultData_MotorBurnTime, "s"}
    }};
}

//LinearPowerRatio, AngularPowerRatio, GravityPowerRatio, AltitudePowerRatio
