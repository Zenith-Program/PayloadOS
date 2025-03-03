#include "PayloadOSModelSimData.h"

using namespace PayloadOS;
using namespace Simulation;
using namespace Model;

const InternalData& ModelSimData::getInternal() const{
    return internal;
}

ModifiableData& ModelSimData::getModifiable(){
    return external;
}

void ModelSimData::makeDefault(){
    external = defaultExternal();
}

//singleton implementation
ModelSimData::ModelSimData() : internal(defaultInternal()), external(defaultExternal()){

}

ModelSimData* ModelSimData::get(){
    static ModelSimData instance;
    return &instance;
}

//default values
#define Data_LaunchSiteTemp 295             //kelvin
#define Data_InitialPower 8.4               //volts
#define GPSFixPeriod 1000                   //ms
#define Data_PowerConsumptionRate 0.00001   //volts/s
#define Data_MotorStrength 100              //m/s^2
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
#define DefaultData_InitialAltitude 20      //meters
#define DefaultData_TimeToLaunchAfterArm 15 //seconds until sim lanches the rocket
#define DefaultData_LaunchRailAngle 4.5     //degrees
#define DefaultData_DragCoefficient 0.0001  //u for airbrakes (not actually drag coef)       
#define DefaultData_EjectionPower 10        //m/s^2
#define DefaultData_EjectionDuration 0.1    //sec  
#define DefaultData_EjectionDelay 0.5       //sec   
#define DefaultData_LandingDeltaT 0.1       //seconds (time impulse is spread acros)
#define DefaultData_DrogueSpeed 20          //m/s
#define DefaultData_MainSpeed 5             //m/s
#define DefaultData_DriftSpeed 8            //m/s (assuming similar to wind)
#define DefaultData_OscilationPeriod 0.5    //seconds    
#define DefaultData_OscilationAmplitude 10  //degrees (thought of as a pendulum)

constexpr InternalData ModelSimData::defaultInternal(){
    return {
        Data_LaunchSiteTemp,
        Data_InitialPower,
        GPSFixPeriod,
        Data_PowerConsumptionRate,
        Data_MotorStrength,
        Data_Satalites,
        Data_InitialLatitude,
        Data_InitialLongitude
    };
}

constexpr ModifiableData ModelSimData::defaultExternal(){
    return {
        DefaultData_Altimeter1Noise,
        DefaultData_Altimeter2Noise,
        DefaultData_IMU0Noise,
        DefaultData_IMU1Noise,
        DefaultData_IMU2Noise,
        DefaultData_IMU3Noise,
        DefaultData_IMU4Noise,
        DefaultData_PowerNoise,
        DefaultData_InitialAltitude,
        DefaultData_TimeToLaunchAfterArm,
        DefaultData_LaunchRailAngle,
        DefaultData_DragCoefficient,
        DefaultData_EjectionPower,
        DefaultData_EjectionDuration,
        DefaultData_EjectionDelay,
        DefaultData_LandingDeltaT,
        DefaultData_DrogueSpeed,
        DefaultData_MainSpeed,
        DefaultData_DriftSpeed,
        DefaultData_OscilationPeriod,
        DefaultData_OscilationAmplitude
    };
}

