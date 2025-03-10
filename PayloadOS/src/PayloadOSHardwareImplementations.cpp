#include "PayloadOSHardwareImplementations.h"
#include "PayloadOSConsoleInterpreter.h"
#include <Wire.h>

using namespace PayloadOS;
using namespace PayloadOS::Hardware;

#define AltimeterAdress 0x77
#define LightAPRSAdress 0x38
#define STEMnaut4Adress 0x4B

#define FalseStatus {0,0,0,0}
#define EmptyVector {0,0,0}
#define EmptyGPS {{0,0}, 0, 0, 0}

//Altimeter1---------------------------------------------------
AltimeterHardware::AltimeterHardware() : init_m(false){}

float_t AltimeterHardware::getAltitude_m(){
    return 0;
}

float_t AltimeterHardware::getPressure_mBar(){
    return 0; //for now
}

float_t AltimeterHardware::getTemperature_K(){
    return 0; // for now
}

error_t AltimeterHardware::init(){
    Wire.begin();
    init_m = true;
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus AltimeterHardware::status(){
    bool responsive = false;
    Wire.begin();
    Wire.beginTransmission(AltimeterAdress);
    if(Wire.endTransmission() == 0) responsive = true;
    return {init_m, responsive, init_m && responsive, init_m && responsive};
}

error_t AltimeterHardware::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

void AltimeterHardware::printReport(){

}

//IMU----------------------------------------------------------
#define IMU_MinimumCalibration 3
#define IMU_UpdateInterval_ms 10
IMUHardware::IMUHardware() : init_m(false), imu(Adafruit_BNO055()){}

Peripherals::LinearVector IMUHardware::getAcceleration_m_s2(){
    imu::Vector<3> acceleration = imu.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    return {static_cast<float_t>(acceleration.x()), static_cast<float_t>(acceleration.y()), static_cast<float_t>(acceleration.z())};
}

Peripherals::RotationVector IMUHardware::getAngularVelocity_deg_s(){
    imu::Vector<3> velocity = imu.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    return {static_cast<float_t>(velocity.x()), static_cast<float_t>(velocity.y()), static_cast<float_t>(velocity.z())};
}

Peripherals::LinearVector IMUHardware::getGravityVector(){
    imu::Vector<3> gravity = imu.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
    return {static_cast<float_t>(gravity.x()), static_cast<float_t>(gravity.y()), static_cast<float_t>(gravity.z())};
}

error_t IMUHardware::init(){
    Wire1.begin();
    imu = Adafruit_BNO055(55, 0x28, &Wire1);
    if(imu.begin()){
        init_m = true;
        return PayloadOS::GOOD;
        imu.setMode(OPERATION_MODE_NDOF);
    } 
    init_m = false;
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus IMUHardware::status(){
    uint8_t systemStatus, systemTest, systemError;
    imu.getSystemStatus(&systemStatus, &systemTest, &systemError);
    updateInitStatus(systemStatus);
    bool ready = systemStatus == 5 && systemTest == 0xF && systemError == 0;
    uint8_t system, gyro, accel, mag;
    imu.getCalibration(&system, &gyro, &accel, &mag);
    bool readyForLaunch = system >= IMU_MinimumCalibration && gyro >= IMU_MinimumCalibration && accel >= IMU_MinimumCalibration && mag >= IMU_MinimumCalibration;
    return {init_m, systemStatus!=1, init_m && ready, init_m && readyForLaunch};
}

error_t IMUHardware::deInit(){
    init_m = false;
    imu.setMode(OPERATION_MODE_CONFIG);
    return PayloadOS::GOOD;
}

#define IMU_AccelerometerTest 0b0001
#define IMU_MagnetometerTest 0b0010
#define IMU_GyroscopeTest 0b0100
#define IMU_MCUTest 0b1000

void IMUHardware::printReport(){
    Serial.println("Version: Hardware");
    Serial.print("Initialized: ");
    //calibration & tests
    uint8_t systemStatus, systemTest, systemError;
    imu.getSystemStatus(&systemStatus, &systemTest, &systemError);
    uint8_t system, gyro, accel, mag;
    imu.getCalibration(&system, &gyro, &accel, &mag);
    updateInitStatus(systemStatus);
    Serial.println(init_m);
    const char* status = getStatusMeaning(systemStatus);
    const char* error = getErrorMeaning(systemError);
    Serial.printf("Status code: %s\n", status);
    Serial.printf("Error code: %s\n", error);
    Serial.printf("MCU - Test: %s, Calibration: %d\n", (systemTest & IMU_MCUTest)? "passed" : "fail", system);
    Serial.printf("Accelerometer - Test: %s, Calibration: %d\n", (systemTest & IMU_AccelerometerTest)? "passed" : "fail", system);
    Serial.printf("Gyroscope - Test: %s, Calibration: %d\n", (systemTest & IMU_GyroscopeTest)? "passed" : "fail", system);
    Serial.printf("Magnetometer - Test: %s, Calibration: %d\n", (systemTest & IMU_MagnetometerTest)? "passed" : "fail", system);
    //readings
    Serial.print("Acceleration: ");
    if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
        printLinear(getAcceleration_ft_s2());
        Serial.println("ft/s^2");
    }
    else{
        printLinear(getAcceleration_m_s2());
        Serial.println("m/s^2");
    }
    Serial.print("Angular Velocity: ");
    if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
        printRotatation(getAngularVelocity_deg_s());
        Serial.println("deg/s");
    }
    else{
        printRotatation(getAngularVelocity_rad_s());
        Serial.println("rad/s");
    }
    Serial.print("Direction: ");
    printLinear(getDirection());
    Serial.println();
}

const char* IMUHardware::getStatusMeaning(uint8_t code){
    switch(code){
    case 0:
        return "Idle";
        break;
    case 1:
        return "System Error";
        break;
    case 2:
        return "Initializing Peripherals";
        break;
    case 3:
        return "System Initialization";
        break;
    case 4:
        return "Executing Self Test";
        break;
    case 5:
        return "Fully Operational";
        break;
    default:
        return "Undefined";
        break;
    }
}
const char* IMUHardware::getErrorMeaning(uint8_t code){
    switch(code){
        case 0:
    return "None";
        break;
        case 1:
    return "Peripheral initialization";
        break;
        case 2:
    return "System initialization";
        break;
        case 3:
    return "Self-test fail";
        break;
        case 4:
    return "Register map value range";
        break;
        case 5:
    return "Register map adress range";
        break;
        case 6:
    return "Register map write";
        break;
        case 7:
    return "Low power not available";
        break;
        case 8:
    return "Accelerometer power mode not available";
        break;
        case 9:
    return "Fusion configuration";
        break;
        case 10:
    return "Sensor configuration";
        break;
    default:
        return "Undefined";
    }
}

error_t IMUHardware::updateInitStatus(int_t code){
    uint8_t system, o1, o2;
    if(code == -1){
        imu.getSystemStatus(&system, &o1, &o2);
    }
    else{
        system = code;
    }
    if(system != 5){
        init_m = false;
        return PayloadOS::ERROR;
    } 
    init_m = true;
    return PayloadOS::GOOD;
}


//STEMnaut1----------------------------------------------------

Peripherals::LinearVector STEMnaut1Hardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}

Peripherals::RotationVector STEMnaut1Hardware::getAngularVelocity_deg_s(){
    return {0,0,0}; //for now
}
Peripherals::LinearVector STEMnaut1Hardware::getGravityVector(){
    return {0,0,0}; //for now
}

error_t STEMnaut1Hardware::init(){
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus STEMnaut1Hardware::status(){
    return FalseStatus;
}

error_t STEMnaut1Hardware::deInit(){
    return PayloadOS::ERROR;
}

void STEMnaut1Hardware::printReport(){
    
}

//STEMnuat2----------------------------------------------------

Peripherals::LinearVector STEMnaut2Hardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}

Peripherals::RotationVector STEMnaut2Hardware::getAngularVelocity_deg_s(){
    return {0,0,0}; //for now
}

Peripherals::LinearVector STEMnaut2Hardware::getGravityVector(){
    return {0,0,0}; //for now
}

error_t STEMnaut2Hardware::init(){
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus STEMnaut2Hardware::status(){
    return FalseStatus;
}

error_t STEMnaut2Hardware::deInit(){
    return PayloadOS::ERROR;
}

void STEMnaut2Hardware::printReport(){
    
}

//STEMnaut3----------------------------------------------------

Peripherals::LinearVector STEMnaut3Hardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}

Peripherals::RotationVector STEMnaut3Hardware::getAngularVelocity_deg_s(){
    return {0,0,0}; //for now
}

Peripherals::LinearVector STEMnaut3Hardware::getGravityVector(){
    return {0,0,0}; //for now
}

error_t STEMnaut3Hardware::init(){
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus STEMnaut3Hardware::status(){
    return FalseStatus;
}

error_t STEMnaut3Hardware::deInit(){
    return PayloadOS::ERROR;
}

void STEMnaut3Hardware::printReport(){
    
}


//STEMnaut4----------------------------------------------------
STEMnaut4Hardware::STEMnaut4Hardware() : init_m(false){}

Peripherals::LinearVector STEMnaut4Hardware::getAcceleration_m_s2(){
    return {0,0,0}; //for now
}

Peripherals::RotationVector STEMnaut4Hardware::getAngularVelocity_deg_s(){
    return {0,0,0}; //for now
}

Peripherals::LinearVector STEMnaut4Hardware::getGravityVector(){
    return {0,0,0}; //for now
}

error_t STEMnaut4Hardware::init(){
    if(!imu.begin(STEMnaut4Adress, Wire1)){
        return PayloadOS::ERROR;
    }
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus STEMnaut4Hardware::status(){
    bool responsive = false;
    Wire1.beginTransmission(STEMnaut4Adress);
    if(Wire1.endTransmission() == 0) responsive = true;
    return {init_m, responsive, false, false}; //for now
}

error_t STEMnaut4Hardware::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

void STEMnaut4Hardware::printReport(){
    
}

//GPS----------------------------------------------------------
Peripherals::GPSData GPSHardware::getData(){
    return {{0,0},0,0,0};
}

error_t GPSHardware::init(){
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus GPSHardware::status(){
    return FalseStatus;
}

error_t GPSHardware::deInit(){
    return PayloadOS::ERROR;
}

void GPSHardware::printReport(){
    
}
//Altimeter2---------------------------------------------------
float_t Altimeter2Hardware::getAltitude_m(){
    return 0; //for now
}

float_t Altimeter2Hardware::getPressure_mBar(){
    return 0; //for now
}

float_t Altimeter2Hardware::getTemperature_K(){
    return 0; // for now
}

error_t Altimeter2Hardware::init(){
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus Altimeter2Hardware::status(){
    return FalseStatus;
}

error_t Altimeter2Hardware::deInit(){
    return PayloadOS::ERROR;
}

void Altimeter2Hardware::printReport(){
    
}

//Transmitter--------------------------------------------------
error_t TransmitterHardware::transmitString(const char*){
    return PayloadOS::ERROR;
}

bool TransmitterHardware::available(){
    return false;
}

error_t TransmitterHardware::init(){
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus TransmitterHardware::status(){
    return FalseStatus;
}

error_t TransmitterHardware::deInit(){
    return PayloadOS::ERROR;
}

void TransmitterHardware::printReport(){
    
}

//Power check--------------------------------------------------
PowerCheckHardware::PowerCheckHardware() : init_m(false){}

float_t PowerCheckHardware::getVoltage(){
    if(!init_m) return 0;
    return 5;
}

error_t PowerCheckHardware::init(){
    Wire2.begin();
    init_m = true;
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus PowerCheckHardware::status(){
    bool available = false;
    Wire2.begin();
    Wire2.beginTransmission(LightAPRSAdress);
    if(Wire2.endTransmission() == 0) available = true;
    return {init_m, available, false, false}; //for now
}

error_t PowerCheckHardware::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

void PowerCheckHardware::printReport(){
    
}

//arm switch---------------------------------------------------
ArmSwitchHardware::ArmSwitchHardware() : init_m(false) {}

bool ArmSwitchHardware::isOn(){
    if(!init_m) return false;
    return digitalRead(0) == HIGH;
}
error_t ArmSwitchHardware::init(){
    pinMode(0, INPUT);
    init_m = true;
    return PayloadOS::GOOD;
}
Peripherals::PeripheralStatus ArmSwitchHardware::status(){
    return {init_m, init_m, init_m, init_m};
}
error_t ArmSwitchHardware::deInit(){
    pinMode(0, INPUT_DISABLE);
    init_m = false;
    return PayloadOS::GOOD;
}

void ArmSwitchHardware::printReport(){
    
}
