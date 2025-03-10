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

#define KELVIN_TO_CELCIUS_BIAS 273.15

//Altimeter1---------------------------------------------------
AltimeterHardware::AltimeterHardware() : altimeter(MS5607(AltimeterAdress)), init_m(false), lastAltitude(0), lastTemp(0), lastPressure(0){}

float_t AltimeterHardware::getAltitude_m(){
    updateReadings();
    return lastAltitude;
}

float_t AltimeterHardware::getPressure_mBar(){
    updateReadings();
    return lastPressure;
}

float_t AltimeterHardware::getTemperature_K(){
    updateReadings();
    return lastTemp + KELVIN_TO_CELCIUS_BIAS;
}

error_t AltimeterHardware::init(){
    int8_t error = altimeter.begin();
    if(error == 1){
        init_m = true;
        return PayloadOS::GOOD;
    } 
    init_m = false;
    return PayloadOS::ERROR;
}

Peripherals::PeripheralStatus AltimeterHardware::status(){
    bool responsive = false;
    Wire.beginTransmission(AltimeterAdress);
    if(Wire.endTransmission() == 0) responsive = true;
    bool reading = true;
    if(updateReadings() == PayloadOS::ERROR) reading = false;
    return {init_m, responsive, init_m && responsive && reading, init_m && responsive && reading};//maybe add zero
}

error_t AltimeterHardware::deInit(){
    lastAltitude = 0;
    lastPressure = 0; 
    lastTemp = 0;
    init_m = false;
    return PayloadOS::GOOD;
}

void AltimeterHardware::printReport(){
    Serial.println("Version: Hardware");
    Serial.print("Initialized: ");
    Serial.println(init_m? "yes" : "no");
    Serial.print("Altitude: ");
    if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
        Serial.print(getAltitude_ft());
        Serial.println("ft");
    }
    else{
        Serial.print(getAltitude_m());
        Serial.println("m");
    }
    Serial.print("Temperature: ");
    if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
        Serial.print(getTemperature_F());
        Serial.println("F");
    }
    else{
        Serial.print(getTemperature_C());
        Serial.println("C");
    }
    Serial.print("Pressure: ");
    if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
        Serial.print(getPressure_psi());
        Serial.println("psi");
    }
    else{
        Serial.print(getPressure_mBar());
        Serial.println("mBar");
    }
}

error_t AltimeterHardware::updateReadings(){
    if(init_m){
        if(millis() - lastUpdateTime > PayloadOS_AltimeterSamplePeriod){
            lastUpdateTime = millis();
            if(altimeter.readDigitalValue()){
                lastAltitude = altimeter.getAltitude();
                lastTemp = altimeter.getTemperature();
                lastPressure = altimeter.getPressure();
                return PayloadOS::GOOD;
            }
            deInit();
            return PayloadOS::ERROR;
        }
        return PayloadOS::GOOD;
    }
    return PayloadOS::GOOD;
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
    Serial.println(init_m? "yes" : "no");
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
STEMnaut4Hardware::STEMnaut4Hardware() : init_m(false), imu(BNO080()), acceleration(EmptyVector), angularVelocity(EmptyVector), gravity(EmptyVector){}

Peripherals::LinearVector STEMnaut4Hardware::getAcceleration_m_s2(){
    updateReadings();
    return acceleration;
}

Peripherals::RotationVector STEMnaut4Hardware::getAngularVelocity_deg_s(){
    updateReadings();
    return angularVelocity;
}

Peripherals::LinearVector STEMnaut4Hardware::getGravityVector(){
    updateReadings();
    return gravity;
}

error_t STEMnaut4Hardware::init(){
    Wire1.begin();
    if(!imu.begin(STEMnaut4Adress, Wire1)){
        return PayloadOS::ERROR;
    }
    imu.modeOn();
    init_m = true;
    imu.enableLinearAccelerometer(PayloadOS_STEMnautAccelerometerSamplePeriod);
    imu.enableGyro(PayloadOS_STEMnautGyroscopeSamplePeriod);
    imu.enableGravity(PayloadOS_STEMnautMagnetometerSamplePeriod);
    imu.calibrateAll();
    delay(10);
    imu.hasReset();
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
    imu.modeSleep();
    return PayloadOS::GOOD;
}

void STEMnaut4Hardware::printReport(){
    Serial.println("Version: Hardware");
    Serial.print("Initialized: ");
    uint_t resetCode = updateInitStatus();
    Serial.println(init_m? "yes" : "no");
    //status
    Serial.print("Reset: ");
    Serial.println(getResetMeaning(resetCode));
    Serial.print("Incoming data: ");
    Serial.println(imu.dataAvailable()? "yes" : "no");
    Serial.print("Calibration: ");
    Serial.println(imu.calibrationComplete()? "yes" : "no");
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

bool STEMnaut4Hardware::updateReadings(){
    if(imu.dataAvailable()){
        uint8_t accuracy; //discard for now
        imu.getLinAccel(acceleration.x, acceleration.y, acceleration.z, accuracy);
        Serial.println(accuracy);
        imu.getGyro(angularVelocity.x_rot, angularVelocity.y_rot, angularVelocity.z_rot, accuracy);
        Serial.println(accuracy);
        imu.getGravity(gravity.x, gravity.y, gravity.z, accuracy);
        Serial.println(accuracy);
        Peripherals::IMUInterface::printLinear(gravity);
        return true;
    }
    return false;
}

uint_t STEMnaut4Hardware::updateInitStatus(){
    if(imu.hasReset()){
        init_m = false;
        return imu.resetReason();
    }
    return 0;
}

const char* STEMnaut4Hardware::getResetMeaning(uint_t code){
    switch(code){
    case 0:
        return "None";
        break;
    case 1:
        return "POR";
        break;
    case 2:
        return "Internal";
        break;
    case 3:
        return "Watchdog";
        break;
    case 4:
        return "External";
        break;
    case 5:
        return "Other";
        break;
    default:
        return "Undefined";
        break;

    }
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
