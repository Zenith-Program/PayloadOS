#include "PayloadOSPeripheralInterfaces.h"
#include <cmath>

using namespace PayloadOS;
using namespace Peripherals;
#define PI 3.141592654
#define FEET_TO_METER 0.3048
#define METER_TO_FEET 3.28084
#define PSI_TO_BAR 0.0689476
#define BAR_TO_PSI 14.5038
#define RAD_TO_DEG 180.0/PI
#define DEG_TO_RAD PI/180.0
#define KELVIN_TO_CELCIUS_BIAS 273.15
#define CELCIUS_TO_FARENHEIGHT_BIAS 32
#define CELCIUS_TO_FARENHEIGHT_SCALAR 1.8

#define m_SI 0.001
#define u_SI 0.000001
#define n_SI 0.000000001
#define c_SI 0.01
#define k_SI 1000
#define M_SI 1000000
#define G_SI 1000000000

//Altimeter
float_t AltimeterInterface::getAltitude_ft(){
    return getAltitude_m()*METER_TO_FEET;
}

float_t AltimeterInterface::getTemperature_F(){
    return getTemperature_C()*CELCIUS_TO_FARENHEIGHT_SCALAR + CELCIUS_TO_FARENHEIGHT_BIAS;
}

float_t AltimeterInterface::getTemperature_C(){
    return getTemperature_K() + KELVIN_TO_CELCIUS_BIAS;
}

float_t AltimeterInterface::getPressure_psi(){
    return getPressure_mBar() * m_SI * BAR_TO_PSI;
}

//IMU
EulerAngle IMUInterface::getOrientation_eulerRad(){
    EulerAngle deg = getOrientation_eulerDeg();
    deg.pitch *= DEG_TO_RAD;
    deg.roll *= DEG_TO_RAD;
    deg.yaw *= DEG_TO_RAD;
    return deg;
}

Matrix3x3 IMUInterface::getOrientation_mat(){
    EulerAngle e = getOrientation_eulerRad();
    float_t cYaw = std::cos(e.yaw);
    float_t sYaw = std::cos(e.yaw);

    float_t cRoll = std::cos(e.roll);
    float_t sRoll = std::cos(e.roll);

    float_t cPitch = std::cos(e.pitch);
    float_t sPitch = std::cos(e.pitch);

    return {cYaw*cPitch,    cYaw * sPitch * sRoll - sYaw * cRoll,   cYaw * sPitch * cRoll + sYaw * sRoll,
        sYaw * cPitch,      sYaw * sPitch * sRoll + cYaw * cRoll,   sYaw * sPitch * cRoll - cYaw * sRoll,
        -sPitch,            cPitch * sRoll,                         cPitch * cYaw 
    };
}

LinearVector IMUInterface::getAcceleration_ft_s2(){
    LinearVector a = getAcceleration_m_s2();
    a.x *= METER_TO_FEET;
    a.y *= METER_TO_FEET;
    a.z *= METER_TO_FEET;

    return a;
}

RotationVector IMUInterface::getAngularVelocity_rad_s(){
    RotationVector omega = getAngularVelocity_deg_s();
    omega.x_rot *= DEG_TO_RAD;
    omega.y_rot *= DEG_TO_RAD;
    omega.z_rot *= DEG_TO_RAD;

    return omega;
}

float_t IMUInterface::getAccelerationMagnitude_m_s2(){
    LinearVector a = getAcceleration_m_s2();
    return std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}
float_t IMUInterface::getAccelerationMagnitude_ft_s2(){
    return getAccelerationMagnitude_m_s2() * METER_TO_FEET;
}

LinearVector IMUInterface::getDirection(){
    EulerAngle e = getOrientation_eulerRad();
    float_t cYaw = std::cos(e.yaw);
    float_t sYaw = std::cos(e.yaw);

    float_t cRoll = std::cos(e.roll);
    float_t sRoll = std::cos(e.roll);

    float_t cPitch = std::cos(e.pitch);
    float_t sPitch = std::cos(e.pitch);

    return {cYaw * sPitch * cRoll + sYaw * sRoll, 
        sYaw * sPitch * cRoll - cYaw * sRoll,
        cPitch * cRoll
    };
}

float_t IMUInterface::getVerticalAngle_deg(){
    LinearVector g = getGravityVector();
    return std::acos(g.z / std::sqrt(g.z * g.x + g.y * g.y + g.z * g.z)) * RAD_TO_DEG;
}

float_t IMUInterface::getVerticalAngle_rad(){
    LinearVector g = getGravityVector();
    return std::acos(g.z / std::sqrt(g.z * g.x + g.y * g.y + g.z * g.z));
}

float_t IMUInterface::getAngularVelocityMagnitude_deg_s(){
    RotationVector omega = getAngularVelocity_deg_s();
    return std::sqrt(omega.x_rot * omega.x_rot + omega.y_rot * omega.y_rot + omega.z_rot * omega.z_rot);
}

float_t IMUInterface::getAngularVelocityMagnitude_rad_s(){
    return getAngularVelocityMagnitude_deg_s() * DEG_TO_RAD;
}

//GPS
float GPSInterface::getAltitude_ft(){
    GPSData data = getData();
    return data.altitude * METER_TO_FEET;
}

float GPSInterface::getAltitude_m(){
    GPSData data = getData();
    return data.altitude;
}

Coordinate GPSInterface::getPosition(){
    GPSData data = getData();
    return data.position;
}


