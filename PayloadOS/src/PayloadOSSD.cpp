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
#define SPACE synthesisFile.print(' ')
#define Print_SPACE Serial.print(' ')
#define NEWLINE synthesisFile.println()
#define Print_NEWLINE Serial.println()
#define CHECK_EOF if(!file.available()) {telemetry.endOfFile = true; return PayloadOS::GOOD;}

TelemetryLog::TelemetryLog() : flushPeriod(4), synthesisFileName(""), synthesisFlushCount(0), synthesisState(SDStates::None) {} 

error_t TelemetryLog::init(){
    if(sd.begin(SD_CONFIG)) return PayloadOS::GOOD;
    return PayloadOS::ERROR;
}

error_t TelemetryLog::openForRead(){
    if(std::strcmp(synthesisFileName, "") == 0) return PayloadOS::ERROR;
    synthesisFile.close();
    synthesisState = SDStates::None;
    synthesisFile = sd.open(synthesisFileName, FILE_READ);
    if(!synthesisFile) return PayloadOS::ERROR;
    synthesisState = SDStates::Read;
    return PayloadOS::GOOD;
}

error_t TelemetryLog::openForWrite(){
    if(std::strcmp(synthesisFileName, "") == 0) return PayloadOS::ERROR;
    synthesisFile.close();
    synthesisState = SDStates::None;
    synthesisFile = sd.open(synthesisFileName, FILE_WRITE);
    if(!synthesisFile) return PayloadOS::ERROR;
    synthesisState = SDStates::Write;
    return PayloadOS::GOOD;
}

error_t TelemetryLog::close(){
    synthesisFile.close();
    synthesisState = SDStates::None;
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
        synthesisFileName[i] = newName[i];
    if(i == PayloadOS_LogFileNameSize){
        synthesisFileName[i-1] = '\0';
        return PayloadOS::ERROR;
    } 
    synthesisFileName[i] = '\0';
    return PayloadOS::GOOD;
}

//read
error_t TelemetryLog::readLine(TelemetryData& telemetry){
    if(!synthesisFile.isOpen() || !synthesisFile || synthesisState != SDStates::Read) return PayloadOS::ERROR;
    telemetry.endOfFile = getline();
    telemetry.time = getUnsigned();
    telemetry.state = getUnsigned();
    telemetry.altitude1 = getFloat();
    telemetry.altitude2 = getFloat();
    telemetry.pressure1 = getFloat();
    telemetry.pressure2 = getFloat();
    telemetry.temperature1 = getFloat();
    telemetry.temperature2 = getFloat();
    readIMU(telemetry, 0);
    readIMU(telemetry, 1);
    readIMU(telemetry, 2);
    readIMU(telemetry, 3);
    readIMU(telemetry, 4);
    telemetry.power = getFloat();
    readGPS(telemetry);
    //if(!file.available()) telemetry.endOfFile = true;
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
    accel->x = getFloat();
    accel->y = getFloat();
    accel->z = getFloat();

    angular->x_rot = getFloat();
    angular->y_rot = getFloat();
    angular->z_rot = getFloat();

    grav->x = getFloat();
    grav->y = getFloat();
    grav->z = getFloat();
    
}

void TelemetryLog::readGPS(TelemetryData& telemetry){
    telemetry.gps.position.x = getFloat();
    telemetry.gps.position.y = getFloat();
    telemetry.gps.altitude = getFloat();
    telemetry.gps.satalites = getUnsigned();
    telemetry.gps.fixAge = getUnsigned();
}
//write
error_t TelemetryLog::logLine(){
    if(!synthesisFile.isOpen() || !synthesisFile || synthesisState != SDStates::Write) return PayloadOS::ERROR;
    synthesisFile.print(millis());
    SPACE;
    synthesisFile.print(static_cast<uint_t>(State::ProgramState::get()->getCurrentState()));
    SPACE;
    synthesisFile.print(Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getAltitude_ft());
    SPACE;
    synthesisFile.print(Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getAltitude_ft());
    SPACE;
    synthesisFile.print(Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getPressure_psi());
    SPACE;
    synthesisFile.print(Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getPressure_psi());
    SPACE;
    synthesisFile.print(Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getTemperature_F());
    SPACE;
    synthesisFile.print(Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getTemperature_F());
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
    synthesisFile.print(Peripherals::PeripheralSelector::get()->getPowerCheck()->getVoltage());
    SPACE;
    logGPS();
    NEWLINE;
    if(synthesisFlushCount == flushPeriod){
        synthesisFile.flush();
        synthesisFlushCount = 0;
    }
    else{
        synthesisFlushCount++;
    }
    if(synthesisFile) return PayloadOS::GOOD;
    return PayloadOS::ERROR;
}

void TelemetryLog::logIMU(Peripherals::IMUInterface* imu){
    Peripherals::LinearVector v = imu->getAcceleration_ft_s2();
    synthesisFile.print(v.x);
    SPACE;
    synthesisFile.print(v.y);
    SPACE;
    synthesisFile.print(v.z);
    SPACE;
    Peripherals::RotationVector u = imu->getAngularVelocity_deg_s();
    synthesisFile.print(u.x_rot);
    SPACE;
    synthesisFile.print(u.y_rot);
    SPACE;
    synthesisFile.print(u.z_rot);
    SPACE;
    v = imu->getGravityVector();
    synthesisFile.print(v.x);
    SPACE;
    synthesisFile.print(v.y);
    SPACE;
    synthesisFile.print(v.z);
}

void TelemetryLog::logGPS(){
    Peripherals::GPSData data = Peripherals::PeripheralSelector::get()->getGPS()->getData();
    synthesisFile.print(data.position.x);
    SPACE;
    synthesisFile.print(data.position.y);
    SPACE;
    synthesisFile.print(data.altitude);
    SPACE;
    synthesisFile.print(data.satalites);
    SPACE;
    synthesisFile.print(data.fixAge);
}

//singleton

TelemetryLog* TelemetryLog::get(){
    static TelemetryLog instance;
    return &instance;
}

//commands
void TelemetryLog::setName_CMD(const Interpreter::Token* args){
    if(args[0].copyStringData(get()->synthesisFileName, PayloadOS_LogFileNameSize) == PayloadOS::ERROR) Serial.println("File name was truncated");
}

void TelemetryLog::displayFile_CMD(const Interpreter::Token*){
    if(get()->openForRead() == PayloadOS::ERROR) Serial.printf("Could not open the file '%s'\n", get()->synthesisFileName);
    Serial.printf("##### %s #####\n", get()->synthesisFileName);
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
        Print_SPACE;
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
    Serial.print(grav->y);
    Print_SPACE;
    Serial.print(grav->z);
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

//parsing

bool TelemetryLog::getline(char delim){
    char curr = synthesisFile.read();
    synthesisLineBuffer[0] = curr;
    uint_t i = 1;
    while(i<PayloadOS_LogParseBufferSize && curr != delim && synthesisFile.available()){
        curr = synthesisFile.read();
        synthesisLineBuffer[i] = curr;
        i++;
    }
    synthesisPos = synthesisLineBuffer;
    return !synthesisFile.available();
}

float_t TelemetryLog::getFloat(){
    float_t value = 0;
    removeWhitespace();
    bool neg = false;
    if(*synthesisPos == '-') {
        neg = true;
        synthesisPos++;
    }
    else if(!isNumeric(*synthesisPos)){
        while(!isWhiteSpace(*synthesisPos)) synthesisPos++;
        return 0;
    }
    while(isNumeric(*synthesisPos)){
        value*=10;
        value += getDigit(*synthesisPos);
        synthesisPos++;
    }
    if(*synthesisPos != '.') return (neg)? -value : value;
    uint_t place = 1;
    synthesisPos++;
    while(isNumeric(*synthesisPos)){
        value += negPow10(place) * getDigit(*synthesisPos);
        synthesisPos++;
        place++;
    }
    return (neg)? -value : value;
}

uint_t TelemetryLog::getUnsigned(){
    uint_t value = 0;
    removeWhitespace();
    while(isNumeric(*synthesisPos)){
        value*=10;
        value += getDigit(*synthesisPos);
        synthesisPos++;
    }
    return value;
}

bool TelemetryLog::isWhiteSpace(char c) {
	if (c == ' ' || c == '\t' || c == '\r' || c == '\v' || c == '\f' || c == '\n') return true;
	return false;
}

bool TelemetryLog::isNumeric(char c) {
	if (c >= '0' && c <= '9') return true;
	return false;
}

void TelemetryLog::removeWhitespace(){
    while(isWhiteSpace(*synthesisPos)) synthesisPos++;
}

float_t TelemetryLog::negPow10(uint_t n){
    float_t value = 1;
    for(uint_t i = 0; i<n; i++)
        value /= 10;
    return value;
}

uint_t TelemetryLog::getDigit(char c){
    return c - '0';
}
 