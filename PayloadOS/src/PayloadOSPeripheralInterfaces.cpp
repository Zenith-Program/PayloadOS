#include "PayloadOSPeripheralInterfaces.h"
#include <cmath>
#include <Arduino.h>

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
    return getTemperature_K() - KELVIN_TO_CELCIUS_BIAS;
}

float_t AltimeterInterface::getPressure_psi(){
    return getPressure_mBar() * m_SI * BAR_TO_PSI;
}

//IMU

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
    return magnitude(a);
}
float_t IMUInterface::getAccelerationMagnitude_ft_s2(){
    return getAccelerationMagnitude_m_s2() * METER_TO_FEET;
}

LinearVector IMUInterface::getDirection(){
    LinearVector g = getGravityVector();
    return norm(LinearVector{-g.x, -g.y, g.z});
}

float_t IMUInterface::getVerticalAngle_deg(){
    return getVerticalAngle_rad() * RAD_TO_DEG;
}

float_t IMUInterface::getVerticalAngle_rad(){
    LinearVector g = getGravityVector();
    return std::acos(g.z / magnitude(g));
}

float_t IMUInterface::getAngularVelocityMagnitude_deg_s(){
    RotationVector omega = getAngularVelocity_deg_s();
    return magnitude(omega);
}

float_t IMUInterface::getAngularVelocityMagnitude_rad_s(){
    return getAngularVelocityMagnitude_deg_s() * DEG_TO_RAD;
}

void IMUInterface::printLinear(LinearVector v){
    Serial.printf("<%f, %f, %f>", v.x, v.y, v.z);
}

void IMUInterface::printRotatation(RotationVector v){
    Serial.printf("<%f, %f, %f>", v.x_rot, v.y_rot, v.z_rot);
}

Peripherals::LinearVector IMUInterface::norm(Peripherals::LinearVector v){
    float_t mag = magnitude(v);
    return {v.x/mag, v.y/mag, v.z/mag};
}

float_t IMUInterface::magnitude(Peripherals::LinearVector v){
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Peripherals::RotationVector IMUInterface::norm(Peripherals::RotationVector v){
    float_t mag = magnitude(v);
    return {v.x_rot/mag, v.y_rot/mag, v.z_rot/mag};
}

float_t IMUInterface::magnitude(Peripherals::RotationVector v){
    return std::sqrt(v.x_rot * v.x_rot + v.y_rot * v.y_rot + v.z_rot * v.z_rot);
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

void GPSInterface::printGPSData(const GPSData& data){
    Serial.print("Location: ");
    printGPSCoordinate(data.position);
    Serial.print("Altitude: ");
    Serial.println(data.altitude);
    Serial.print("Satalites: ");
    Serial.println(data.satalites);
    Serial.print("Time since fix: ");
    Serial.println(data.fixAge);
}

void GPSInterface::printGPSCoordinate(Coordinate c){
    Serial.print(std::abs(c.x));
    if(c.x < 0) Serial.print("S");
    else Serial.print("N");
    Serial.print(", ");
    Serial.print(c.y);
    if(c.x < 0) Serial.println("W");
    else Serial.println("E");
}




