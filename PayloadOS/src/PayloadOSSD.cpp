#include "PayloadOSSD.h"
#include "PayloadOSStateMachine.h"
#include <cstring>

using namespace PayloadOS;
using namespace FlightData;

#define SD_CONFIG SdioConfig(FIFO_SDIO)
#define EMPTY_VECTOR {0,0,0}
#define ERROR_TELEMETRY {false, 0, 0, 0, 0, 0, 0, 0, 0, \
    EMPTY_VECTOR, EMPTY_VECTOR, EMPTY_VECTOR, EMPTY_VECTOR,  EMPTY_VECTOR, \
    EMPTY_VECTOR, EMPTY_VECTOR, EMPTY_VECTOR, EMPTY_VECTOR,  EMPTY_VECTOR,\
    EMPTY_VECTOR, EMPTY_VECTOR, EMPTY_VECTOR, EMPTY_VECTOR,  EMPTY_VECTOR,\
    0, {{0,0}, 0, 0, 0}}
#define SPACE file.print(' ')
#define Print_SPACE Serial.print(' ')
#define NEWLINE file.println()
#define Print_NEWLINE Serial.println()
#define CHECK_EOF if(!file.available()) {telemetry.endOfFile = true; return PayloadOS::GOOD;}

TelemetryLog::TelemetryLog() : fileName(""), flushPeriod(1), flushCount(0), state(SDStates::None) {} 

error_t TelemetryLog::init(){
    if(sd.begin(SD_CONFIG)) return PayloadOS::GOOD;
    return PayloadOS::ERROR;
}

error_t TelemetryLog::openForRead(){
    if(std::strcmp(fileName, "") == 0) return PayloadOS::ERROR;
    file.close();
    state = SDStates::None;
    file = sd.open(fileName, FILE_READ);
    if(!file) return PayloadOS::ERROR;
    state = SDStates::Read;
    return PayloadOS::GOOD;
}

error_t TelemetryLog::openForWrite(){
    if(std::strcmp(fileName, "") == 0) return PayloadOS::ERROR;
    file.close();
    state = SDStates::None;
    file = sd.open(fileName, FILE_WRITE);
    if(!file) return PayloadOS::ERROR;
    state = SDStates::Write;
    return PayloadOS::GOOD;
}

error_t TelemetryLog::close(){
    file.close();
    state = SDStates::None;
    return PayloadOS::GOOD;
}

error_t TelemetryLog::setFlushPeriod(uint_t period){
    if(period > 0){
        flushPeriod = period;
        return PayloadOS::GOOD;
    }
    return PayloadOS::ERROR;
}

error_t TelemetryLog::setFileName(const char* newName){
    uint_t i;
    for(i=0; i<PayloadOS_LogFileNameSize && newName[i] != '\0'; i++)
        fileName[i] = newName[i];
    if(i == PayloadOS_LogFileNameSize){
        fileName[i-1] = '\0';
        return PayloadOS::ERROR;
    } 
    fileName[i] = '\0';
    return PayloadOS::GOOD;
}

//read
error_t TelemetryLog::readLine(TelemetryData& telemetry){
    if(!file.isOpen() || !file || state != SDStates::Read) return PayloadOS::ERROR;
    telemetry.endOfFile = false;
    telemetry.time = file.parseInt();
    CHECK_EOF;
    telemetry.state = file.parseInt();
    CHECK_EOF;
    telemetry.altitude1 = file.parseFloat();
    CHECK_EOF;
    telemetry.altitude2 = file.parseFloat();
    CHECK_EOF;
    telemetry.pressure1 = file.parseFloat();
    CHECK_EOF;
    telemetry.pressure2 = file.parseFloat();
    CHECK_EOF;
    telemetry.temperature1 = file.parseFloat();
    CHECK_EOF;
    telemetry.temperature2 = file.parseFloat();
    CHECK_EOF;
    readIMU(telemetry, 0);
    CHECK_EOF;
    readIMU(telemetry, 1);
    CHECK_EOF;
    readIMU(telemetry, 2);
    CHECK_EOF;
    readIMU(telemetry, 3);
    CHECK_EOF;
    readIMU(telemetry, 4);
    CHECK_EOF;
    telemetry.power = file.parseFloat();
    CHECK_EOF;
    readGPS(telemetry);
    CHECK_EOF;
    return PayloadOS::GOOD;
}

void TelemetryLog::readIMU(TelemetryData& telemetry, uint_t IMU){
    Peripherals::LinearVector* accel, *grav;
    Peripherals::RotationVector* angular;
    switch(IMU){
    case 1:
        accel = &telemetry.accel1;
        angular = &telemetry.angularVelocity1;
        grav = &telemetry.grav1;
        break;
    case 2:
        accel = &telemetry.accel2;
        angular = &telemetry.angularVelocity2;
        grav = &telemetry.grav2;
        break;
    case 3:
        accel = &telemetry.accel3;
        angular = &telemetry.angularVelocity3;
        grav = &telemetry.grav3;
        break;
    case 4:
        accel = &telemetry.accel4;
        angular = &telemetry.angularVelocity4;
        grav = &telemetry.grav4;
        break;
    case 0:
    default:
        accel = &telemetry.accel0;
        angular = &telemetry.angularVelocity0;
        grav = &telemetry.grav0;
        break;
    }
    accel->x = file.parseFloat();
    accel->y = file.parseFloat();
    accel->z = file.parseFloat();

    angular->x_rot = file.parseFloat();
    angular->y_rot = file.parseFloat();
    angular->z_rot = file.parseFloat();

    grav->x = file.parseFloat();
    grav->y = file.parseFloat();
    grav->z = file.parseFloat();
    
}

void TelemetryLog::readGPS(TelemetryData& telemetry){
    telemetry.gps.position.x = file.parseFloat();
    telemetry.gps.position.y = file.parseFloat();
    telemetry.gps.altitude = file.parseFloat();
    telemetry.gps.satalites = file.parseInt();
    telemetry.gps.fixAge = file.parseInt();
}
//write
error_t TelemetryLog::logLine(){
    if(!file.isOpen() || !file || state != SDStates::Write) return PayloadOS::ERROR;
    file.print(millis());
    SPACE;
    file.print(static_cast<uint_t>(State::ProgramState::get()->getCurrentState()));
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getAltitude_ft());
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getAltitude_ft());
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getPressure_psi());
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getPressure_psi());
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getTemperature_F());
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getTemperature_F());
    SPACE;
    logIMU(Peripherals::PeripheralSelector::get()->getPayloadIMU());
    SPACE;
    logIMU(Peripherals::PeripheralSelector::get()->getSTEMnaut1());
    SPACE;
    logIMU(Peripherals::PeripheralSelector::get()->getSTEMnaut2());
    SPACE;
    logIMU(Peripherals::PeripheralSelector::get()->getSTEMnaut3());
    SPACE;
    logIMU(Peripherals::PeripheralSelector::get()->getSTEMnaut4());
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getPowerCheck()->getVoltage());
    SPACE;
    logGPS();
    NEWLINE;
    if(flushCount == flushPeriod){
        file.flush();
        flushCount = 0;
    }
    else{
        flushCount++;
    }
    if(file) return PayloadOS::GOOD;
    return PayloadOS::ERROR;
}

void TelemetryLog::logIMU(Peripherals::IMUInterface* imu){
    Peripherals::LinearVector v = imu->getAcceleration_ft_s2();
    file.print(v.x);
    SPACE;
    file.print(v.y);
    SPACE;
    file.print(v.z);
    SPACE;
    Peripherals::RotationVector u = imu->getAngularVelocity_deg_s();
    file.print(u.x_rot);
    SPACE;
    file.print(u.y_rot);
    SPACE;
    file.print(u.z_rot);
    SPACE;
    v = imu->getGravityVector();
    file.print(v.x);
    SPACE;
    file.print(v.y);
    SPACE;
    file.print(v.z);
}

void TelemetryLog::logGPS(){
    Peripherals::GPSData data = Peripherals::PeripheralSelector::get()->getGPS()->getData();
    file.print(data.position.x);
    SPACE;
    file.print(data.position.y);
    SPACE;
    file.print(data.altitude);
    SPACE;
    file.print(data.satalites);
    SPACE;
    file.print(data.fixAge);
}

//singleton

TelemetryLog* TelemetryLog::get(){
    static TelemetryLog instance;
    return &instance;
}

//commands
void TelemetryLog::setName_CMD(const Interpreter::Token* args){
    if(args[0].copyStringData(get()->fileName, PayloadOS_LogFileNameSize) == PayloadOS::ERROR) Serial.println("File name was truncated");
}

void TelemetryLog::displayFile_CMD(const Interpreter::Token*){
    if(get()->openForRead() == PayloadOS::ERROR) Serial.printf("Could not open the file '%s'\n", get()->fileName);
    Serial.printf("##### %s #####\n", get()->fileName);
    TelemetryData telemetry;

    while(get()->readLine(telemetry) != PayloadOS::ERROR && !telemetry.endOfFile){
        Serial.print(telemetry.time);
        Print_SPACE;
        Serial.print(telemetry.state);
        Print_SPACE;
        Serial.print(telemetry.altitude1);
        Print_SPACE;
        Serial.print(telemetry.altitude2);
        Print_SPACE;
        Serial.print(telemetry.pressure1);
        Print_SPACE;
        Serial.print(telemetry.pressure2);
        Print_SPACE;
        Serial.print(telemetry.temperature1);
        Print_SPACE;
        Serial.print(telemetry.temperature2);
        Print_SPACE;
        printIMU(telemetry, 0);
        Print_SPACE;
        printIMU(telemetry, 1);
        Print_SPACE;
        printIMU(telemetry, 2);
        Print_SPACE;
        printIMU(telemetry, 3);
        Print_SPACE;
        printIMU(telemetry, 4);
        Print_SPACE;
        Serial.print(telemetry.power);
        Print_SPACE;
        printGPS(telemetry);
        Print_NEWLINE;
    }
}

void TelemetryLog::printIMU(const TelemetryData& telemetry, uint_t IMU){
    const Peripherals::LinearVector* accel, *grav;
    const Peripherals::RotationVector* angular;
    switch(IMU){
    case 1:
        accel = &telemetry.accel1;
        angular = &telemetry.angularVelocity1;
        grav = &telemetry.grav1;
        break;
    case 2:
        accel = &telemetry.accel2;
        angular = &telemetry.angularVelocity2;
        grav = &telemetry.grav2;
        break;
    case 3:
        accel = &telemetry.accel3;
        angular = &telemetry.angularVelocity3;
        grav = &telemetry.grav3;
        break;
    case 4:
        accel = &telemetry.accel4;
        angular = &telemetry.angularVelocity4;
        grav = &telemetry.grav4;
        break;
    case 0:
    default:
        accel = &telemetry.accel0;
        angular = &telemetry.angularVelocity0;
        grav = &telemetry.grav0;
        break;
    }
    Serial.print(accel->x);
    Print_SPACE;
    Serial.print(accel->y);
    Print_SPACE;
    Serial.print(accel->z);
    Print_SPACE;

    Serial.print(angular->x_rot);
    Print_SPACE;
    Serial.print(angular->y_rot);
    Print_SPACE;
    Serial.print(angular->z_rot);
    Print_SPACE;

    Serial.print(grav->x);
    Print_SPACE;
    Serial.print(grav->x);
    Print_SPACE;
    Serial.print(grav->x);
}

void TelemetryLog::printGPS(const TelemetryData& telemetry){
    Serial.print(telemetry.gps.position.x);
    Print_SPACE;
    Serial.print(telemetry.gps.position.y);
    Print_SPACE;
    Serial.print(telemetry.gps.altitude);
    Print_SPACE;
    Serial.print(telemetry.gps.satalites);
    Print_SPACE;
    Serial.print(telemetry.gps.fixAge);
}

void TelemetryLog::setFlush_CMD(const Interpreter::Token*args){
    uint_t newFlush = args[0].getUnsignedData();
    if(get()->setFlushPeriod(newFlush) == PayloadOS::ERROR) Serial.println("Invalid Flush Period");
}

void TelemetryLog::init_CMD(const Interpreter::Token*){
    get()->init();
}