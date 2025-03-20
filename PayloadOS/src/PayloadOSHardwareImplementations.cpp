#include "PayloadOSHardwareImplementations.h"
#include "PayloadOSConsoleInterpreter.h"
#include <Wire.h>
#include <cstring>

using namespace PayloadOS;
using namespace PayloadOS::Hardware;

#define AltimeterAdress 0x77
#define LightAPRSAdress 0x38
#define STEMnaut4Adress 0x4B
#define STEMnaut3Adress 0x4A
#define STEMnaut2Adress 0x4B
#define STEMnaut1Adress 0x4A

#define LightAPRSSendSectionCommand 0x01
#define LightAPRSADCCommand 0x02
#define LightAPRSAltimeterCommand 0x03
#define LightAPRSGPSCommand 0x04
#define LightAPRSTransmitCommand 0x05
#define LightAPRSReadyCommand 0x06
#define LightAPRSBeginTransmissionCommand 0x07
#define LightAPRSResetCommand 0x08

#define I2C_DelayTime_us 5

#define LightAPRS_TX_MaxSize 256
#define LightAPRS_ChunckSize 32

#define FalseStatus {0,0,0,0}
#define EmptyVector {0,0,0}
#define EmptyGPS {{0,0}, 0, 0, 0}

#define KELVIN_TO_CELCIUS_BIAS 273.15
#define PASCALS_TO_mBAR_SCALE 0.01

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
    updateReadings();
    return acceleration;
}

Peripherals::RotationVector STEMnaut1Hardware::getAngularVelocity_deg_s(){
    updateReadings();
    return angularVelocity;
}
Peripherals::LinearVector STEMnaut1Hardware::getGravityVector(){
    updateReadings();
    return gravity;
}

error_t STEMnaut1Hardware::init(){
    Wire.begin();
    if(!imu.begin(STEMnaut1Adress, Wire)){
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

Peripherals::PeripheralStatus STEMnaut1Hardware::status(){
    bool responsive = false;
    Wire.beginTransmission(STEMnaut1Adress);
    if(Wire.endTransmission() == 0) responsive = true;
    return {init_m, responsive, init_m && responsive, init_m && responsive}; //for now
}

error_t STEMnaut1Hardware::deInit(){
    init_m = false;
    imu.modeSleep();
    return PayloadOS::GOOD;
}

void STEMnaut1Hardware::printReport(){
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

bool STEMnaut1Hardware::updateReadings(){
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

uint_t STEMnaut1Hardware::updateInitStatus(){
    if(imu.hasReset()){
        init_m = false;
        return imu.resetReason();
    }
    return 0;
}

const char* STEMnaut1Hardware::getResetMeaning(uint_t code){
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

//STEMnuat2----------------------------------------------------

Peripherals::LinearVector STEMnaut2Hardware::getAcceleration_m_s2(){
    updateReadings();
    return acceleration;
}

Peripherals::RotationVector STEMnaut2Hardware::getAngularVelocity_deg_s(){
    updateReadings();
    return angularVelocity;
}

Peripherals::LinearVector STEMnaut2Hardware::getGravityVector(){
    updateReadings();
    return gravity;
}

error_t STEMnaut2Hardware::init(){
    Wire.begin();
    if(!imu.begin(STEMnaut2Adress, Wire)){
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

Peripherals::PeripheralStatus STEMnaut2Hardware::status(){
    bool responsive = false;
    Wire.beginTransmission(STEMnaut2Adress);
    if(Wire.endTransmission() == 0) responsive = true;
    return {init_m, responsive, init_m && responsive, init_m && responsive}; //for now
}

error_t STEMnaut2Hardware::deInit(){
    init_m = false;
    imu.modeSleep();
    return PayloadOS::GOOD;
}

void STEMnaut2Hardware::printReport(){
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

bool STEMnaut2Hardware::updateReadings(){
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

uint_t STEMnaut2Hardware::updateInitStatus(){
    if(imu.hasReset()){
        init_m = false;
        return imu.resetReason();
    }
    return 0;
}

const char* STEMnaut2Hardware::getResetMeaning(uint_t code){
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

//STEMnaut3----------------------------------------------------

Peripherals::LinearVector STEMnaut3Hardware::getAcceleration_m_s2(){
    updateReadings();
    return acceleration;
}

Peripherals::RotationVector STEMnaut3Hardware::getAngularVelocity_deg_s(){
    updateReadings();
    return angularVelocity;
}

Peripherals::LinearVector STEMnaut3Hardware::getGravityVector(){
    updateReadings();
    return gravity;
}

error_t STEMnaut3Hardware::init(){
    Wire1.begin();
    if(!imu.begin(STEMnaut3Adress, Wire1)){
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

Peripherals::PeripheralStatus STEMnaut3Hardware::status(){
    bool responsive = false;
    Wire1.beginTransmission(STEMnaut3Adress);
    if(Wire1.endTransmission() == 0) responsive = true;
    return {init_m, responsive, init_m && responsive, init_m && responsive}; //for now
}

error_t STEMnaut3Hardware::deInit(){
    init_m = false;
    imu.modeSleep();
    return PayloadOS::GOOD;
}

void STEMnaut3Hardware::printReport(){
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

bool STEMnaut3Hardware::updateReadings(){
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

uint_t STEMnaut3Hardware::updateInitStatus(){
    if(imu.hasReset()){
        init_m = false;
        return imu.resetReason();
    }
    return 0;
}

const char* STEMnaut3Hardware::getResetMeaning(uint_t code){
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
    return {init_m, responsive, init_m && responsive, init_m && responsive}; //for now
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
Altimeter2Hardware::Altimeter2Hardware() : init_m(false), lastAltitude_m(0), lastPressure_pa(0), lastTemperature_C(0), lastReadingTime(millis()){}
float_t Altimeter2Hardware::getAltitude_m(){
   if(!init_m) return 0;
   updateReadings();
   return lastAltitude_m;
}

float_t Altimeter2Hardware::getPressure_mBar(){
    if(!init_m) return 0;
    updateReadings();
    return lastPressure_pa * PASCALS_TO_mBAR_SCALE;
}

float_t Altimeter2Hardware::getTemperature_K(){
    if(!init_m) return 0;
    updateReadings();
    return lastTemperature_C + KELVIN_TO_CELCIUS_BIAS;
}

error_t Altimeter2Hardware::init(){
    Wire2.begin();
    init_m = true;
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus Altimeter2Hardware::status(){
    bool available = false;
    Wire2.begin();
    Wire2.beginTransmission(LightAPRSAdress);
    if(Wire2.endTransmission() == 0) available = true;
    return {init_m, available, init_m && available, init_m && available};
}

error_t Altimeter2Hardware::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

void Altimeter2Hardware::printReport(){
    Serial.println("Version: Hardware");
    Serial.print("Initialized: ");
    Serial.println(init_m? "yes" : "no");
    if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
        Serial.printf("Altitude - %.2fft\n", getAltitude_ft());
        Serial.printf("Pressure - %.2fpsi\n", getPressure_psi());
        Serial.printf("Temperature - %.2fF\n", getTemperature_F());
    }
    else{
        Serial.printf("Altitude - %.2fm\n", getAltitude_m());
        Serial.printf("Pressure - %.2fmBar\n", getPressure_mBar());
        Serial.printf("Temperature - %.2fC\n", getTemperature_C());
    }
}

error_t Altimeter2Hardware::updateReadings(){
    constexpr uint_t MAX_SAMPLE_PERIOD_ms = 50;
    if(millis() - lastReadingTime > MAX_SAMPLE_PERIOD_ms){
        Wire2.beginTransmission(LightAPRSAdress);
        Wire2.write(LightAPRSAltimeterCommand);
        if(Wire2.endTransmission()!=0) return PayloadOS::ERROR;
        delayMicroseconds(I2C_DelayTime_us);
        Wire2.requestFrom(LightAPRSAdress, 3*sizeof(float));
        delayMicroseconds(I2C_DelayTime_us);
        float recived1, recived2, recived3;
        Wire2.readBytes((char*)&recived1, sizeof(recived1));
        Wire2.readBytes((char*)&recived2, sizeof(recived2));
        Wire2.readBytes((char*)&recived3, sizeof(recived3));
        lastAltitude_m = static_cast<float_t>(recived1);
        lastPressure_pa = static_cast<float_t>(recived2);
        lastTemperature_C = static_cast<float_t>(recived3);
        lastReadingTime = millis();
        return PayloadOS::GOOD;
    }
    return PayloadOS::GOOD;
}

//Transmitter--------------------------------------------------
TransmitterHardware::TransmitterHardware() : init_m(false){}

error_t TransmitterHardware::transmitString(const char* message){
    constexpr uint_t MAX_RETRIES = 8;
    //initiaite coms
    Wire2.beginTransmission(LightAPRSAdress);
    Wire2.write(LightAPRSBeginTransmissionCommand);
    if(Wire2.endTransmission() != 0) return PayloadOS::ERROR;
    //transmit chunks
    const char* current;
    const char* startOfChunck;
    uint_t retries = 0;
    for(current = message; current < message + LightAPRS_TX_MaxSize - 1 && *current != '\0';){
        delayMicroseconds(I2C_DelayTime_us);
        Wire2.beginTransmission(LightAPRSAdress);
        Wire2.write(LightAPRSSendSectionCommand);
        for(startOfChunck = current; current < message + LightAPRS_TX_MaxSize - 1 && current < startOfChunck + LightAPRS_ChunckSize && *current != '\0'; current++){
            Wire2.write(*current);
        }
        uint8_t code = Wire2.endTransmission();
        if(code == 2 && retries < MAX_RETRIES){
            current = startOfChunck;
            retries++;
        } 
        else if(code != 0) return PayloadOS::ERROR;
        else retries = 0;
    }
    //null terminate
    delayMicroseconds(I2C_DelayTime_us);
    Wire2.beginTransmission(LightAPRSAdress);
    Wire2.write(LightAPRSSendSectionCommand);
    Wire2.write('\0');
    if(Wire2.endTransmission() != 0) return PayloadOS::ERROR;
    //end coms
    delayMicroseconds(I2C_DelayTime_us);
    Wire2.beginTransmission(LightAPRSAdress);
    Wire2.write(LightAPRSTransmitCommand);
    if(Wire2.endTransmission() != 0) return PayloadOS::ERROR;
    return PayloadOS::GOOD;
}

bool TransmitterHardware::available(){
    Wire2.beginTransmission(LightAPRSAdress);
    Wire2.write(LightAPRSReadyCommand);
    if(Wire2.endTransmission() != 0) return PayloadOS::ERROR;
    delayMicroseconds(I2C_DelayTime_us);
    Wire2.requestFrom(LightAPRSAdress, 1);
    char code = Wire2.read();
    return code == 0x00;
}

error_t TransmitterHardware::init(){
    Wire2.begin();
    Wire2.beginTransmission(LightAPRSAdress);
    Wire2.write(LightAPRSResetCommand);
    if(Wire2.endTransmission() != 0){
        init_m = false;
        return PayloadOS::ERROR;
    } 
    init_m = true;
    return PayloadOS::GOOD;
}

Peripherals::PeripheralStatus TransmitterHardware::status(){
    bool read = false;
    Wire2.begin();
    Wire2.beginTransmission(LightAPRSAdress);
    if(Wire2.endTransmission() == 0) read = true;
    return {init_m, read, init_m && read && available(), init_m && read && available()};
}

error_t TransmitterHardware::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

void TransmitterHardware::printReport(){
    Serial.println("Version: Hardware");
    Serial.print("Initialized: ");
    Serial.println(init_m? "yes" : "no");
    Serial.print("Available: ");
    Serial.println(available()? "yes" : "no");
}

const char* TransmitterHardware::transmitChunck(const char* message){
    Wire2.beginTransmission(LightAPRSAdress);
    Wire2.write(LightAPRSSendSectionCommand);
    if(Wire2.endTransmission() != 0) return nullptr;
    delayMicroseconds(I2C_DelayTime_us);
    for(uint_t i=0; i<32 && *message != '\0'; i++)
        Wire2.write(*message++);
    if(Wire2.endTransmission() != 0) return nullptr;
    return message;
}

//Power check--------------------------------------------------
PowerCheckHardware::PowerCheckHardware() : init_m(false), lastPower(0){}

float_t PowerCheckHardware::getVoltage(){
    if(!init_m) return 0;
    updateReadings();
    return lastPower;
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
    return {init_m, available, init_m && available, init_m && available}; //for now
}

error_t PowerCheckHardware::deInit(){
    init_m = false;
    return PayloadOS::GOOD;
}

void PowerCheckHardware::printReport(){
    Serial.println("Version: Hardware");
    Serial.print("Initialized: ");
    Serial.println(init_m? "yes" : "no");
    Serial.print("Volatge - ");
    Serial.print(getVoltage());
    Serial.println("V");
}

error_t PowerCheckHardware::updateReadings(){
    Wire2.beginTransmission(LightAPRSAdress);
    Wire2.write(LightAPRSADCCommand);
    if(Wire2.endTransmission()!=0) return PayloadOS::ERROR;
    delayMicroseconds(I2C_DelayTime_us);
    Wire2.requestFrom(LightAPRSAdress, sizeof(float));
    delayMicroseconds(I2C_DelayTime_us);
    float recived;
    Wire2.readBytes((char*)&recived, sizeof(recived));
    lastPower = static_cast<float_t>(recived);
    return PayloadOS::GOOD;
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
