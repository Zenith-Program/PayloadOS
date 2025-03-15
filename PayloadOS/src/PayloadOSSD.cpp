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

TelemetryLog::TelemetryLog() : fileName(""), flushPeriod(4), flushCount(0), state(SDStates::None) {} 

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
    //Serial.print("z");
    telemetry.endOfFile = getline();
    //Serial.print("a");
    telemetry.time = getUnsigned();
    //Serial.print("b");
    telemetry.state = getUnsigned();
    telemetry.altitude1 = getFloat();
    //Serial.print("c");
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

//parsing

bool TelemetryLog::getline(char delim){
    char curr = file.read();
    buffer[0] = curr;
    uint_t i = 1;
    while(i<PayloadOS_LogParseBufferSize && curr != delim && file.available()){
        curr = file.read();
        buffer[i] = curr;
        i++;
    }
    pos = buffer;
    return !file.available();
}

float_t TelemetryLog::getFloat(){
    float_t value = 0;
    removeWhitespace();
    bool neg = false;
    if(*pos == '-') {
        neg = true;
        pos++;
    }
    while(isNumeric(*pos)){
        value*=10;
        value += getDigit(*pos);
        pos++;
    }
    if(*pos != '.') return (neg)? -value : value;
    uint_t place = 1;
    pos++;
    while(isNumeric(*pos)){
        value += negPow10(place) * getDigit(*pos);
        pos++;
    }
    return (neg)? -value : value;
}

uint_t TelemetryLog::getUnsigned(){
    uint_t value = 0;
    removeWhitespace();
    while(isNumeric(*pos)){
        value*=10;
        value += getDigit(*pos);
        pos++;
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
    while(isWhiteSpace(*pos)) pos++;
}

float_t TelemetryLog::negPow10(uint_t n){
    float_t value = 1;
    for(uint_t i = 0; i<n; i++)
        value /= 10;
    return value;
}

uint_t TelemetryLog::getDigit(char c){
    return static_cast<uint_t>(c) - 0x30;
}
 