#include "PayloadOSSD.h"
#include "PayloadOSStateMachine.h"
#include <cstring>

using namespace PayloadOS;
using namespace FlightData;

#define SD_CONFIG SdioConfig(FIFO_SDIO)
//implementation of SDFiles----------------------------------------------------
SDFiles::SDFiles() : analysisLog(&sd), blackBoxLog(&sd), messageLog(&sd), HILLog(&sd){
    analysisLog.setFileName(PayloadOS_DefaultAnalysisFileName);
    blackBoxLog.setFileName(PayloadOS_DefaultBlackBoxFileName);
    messageLog.setFileName(PayloadOS_DefaultEventFileName);
    HILLog.setFileName(PayloadOS_DataSimDefaultFileName);
}

error_t SDFiles::init(){
    if(sd.begin(SD_CONFIG) == false) return PayloadOS::ERROR;
    return PayloadOS::GOOD;
}

TelemetryLog* SDFiles::getLog(TelemetryLogs log){
    switch(log){
    case TelemetryLogs::Analysis:
        return &analysisLog;
        break;
    case TelemetryLogs::BlackBox:
        return &blackBoxLog;
        break;
    case TelemetryLogs::Message:
        return &messageLog;
        break;
    case TelemetryLogs::HIL:
        return &HILLog;
        break;
    default:
        return nullptr;
    }
}

SdFat* SDFiles::getCard(){
    return &sd;
}

//singleton--------------------
SDFiles* SDFiles::get(){
    static SDFiles instance;
    return &instance;
}

//commands---------------------
void SDFiles::init_c(const Interpreter::Token*){
    if(get()->init() == PayloadOS::ERROR) Serial.println("Failed to initialize the SD card");
}
void SDFiles::getFilename_c(const Interpreter::Token* args){
    char nameBuffer[128];
    args[0].copyStringData(nameBuffer, 128);
    if(std::strcmp(nameBuffer, "analysis") == 0){
        Serial.println(get()->getLog(TelemetryLogs::Analysis)->getFileName());
        return;
    }
    if(std::strcmp(nameBuffer, "blackBox") == 0){
        Serial.println(get()->getLog(TelemetryLogs::BlackBox)->getFileName());
        return;
    }
    if(std::strcmp(nameBuffer, "events") == 0){
        Serial.println(get()->getLog(TelemetryLogs::Message)->getFileName());
        return;
    }
    if(std::strcmp(nameBuffer, "dataSim") == 0){
        Serial.println(get()->getLog(TelemetryLogs::HIL)->getFileName());
        return;
    }
    Serial.print("PayloadOS does not contain the file '");
    Serial.print(nameBuffer);
    Serial.println("'");
}

void SDFiles::setFileName_C(const Interpreter::Token* args){
    char nameBuffer[128];
    char fileNameBuffer[PayloadOS_LogFileNameSize];
    args[0].copyStringData(nameBuffer, 128);
    args[1].copyStringData(fileNameBuffer, PayloadOS_LogFileNameSize);
    if(std::strcmp(nameBuffer, "analysis") == 0){
        get()->getLog(TelemetryLogs::Analysis)->setFileName(fileNameBuffer);
        return;
    }
    if(std::strcmp(nameBuffer, "blackBox") == 0){
        get()->getLog(TelemetryLogs::BlackBox)->setFileName(fileNameBuffer);
        return;
    }
    if(std::strcmp(nameBuffer, "events") == 0){
        get()->getLog(TelemetryLogs::Message)->setFileName(fileNameBuffer);
        return;
    }
    if(std::strcmp(nameBuffer, "dataSim") == 0){
        get()->getLog(TelemetryLogs::HIL)->setFileName(fileNameBuffer);
        return;
    }
    Serial.print("PayloadOS does not contain the file '");
    Serial.print(nameBuffer);
    Serial.println("'");
}

void SDFiles::displayFile_c(const Interpreter::Token* args){
    char nameBuffer[128];
    args[0].copyStringData(nameBuffer, 128);
    if(std::strcmp(nameBuffer, "analysis") == 0){
        get()->getLog(TelemetryLogs::Analysis)->displayFile();
        return;
    }
    if(std::strcmp(nameBuffer, "blackBox") == 0){
        get()->getLog(TelemetryLogs::BlackBox)->displayFile();
        return;
    }
    if(std::strcmp(nameBuffer, "events") == 0){
        get()->getLog(TelemetryLogs::Message)->displayFile();
        return;
    }
    if(std::strcmp(nameBuffer, "dataSim") == 0){
        get()->getLog(TelemetryLogs::HIL)->displayFile();
        return;
    }
    Serial.print("PayloadOS does not contain the file '");
    Serial.print(nameBuffer);
    Serial.println("'");
}

void SDFiles::logCustomEvent_c(const Interpreter::Token* args){
    char messageBuffer[128];
    args[0].copyStringData(messageBuffer, 128);
    if(get()->getLog(TelemetryLogs::Message)->logMessage(messageBuffer) == PayloadOS::ERROR)
        Serial.println("Failed to log the message");
}

void SDFiles::eventLogCntrl_c(const Interpreter::Token* args){
    char type[32];
    args[0].copyStringData(type, 32);
    if(std::strcmp(type, "get") == 0){
        switch(get()->getLog(TelemetryLogs::Message)->currentMode()){
        default:
        case SDStates::None:
            Serial.println("closed");
            break;
        case SDStates::Write:
            Serial.println("Write");
            break;
        case SDStates::Read:
            Serial.println("Read");
            break;
        }
        return;
    }
    if(std::strcmp(type, "open") == 0){
        get()->getLog(TelemetryLogs::Message)->openForWrite(OpenTypes::End);
        return;
    }
    if(std::strcmp(type, "new") == 0){
        get()->getLog(TelemetryLogs::Message)->openForWrite(OpenTypes::New);
        return;
    }
    if(std::strcmp(type, "close") == 0){
        get()->getLog(TelemetryLogs::Message)->close();
        return;
    }
    Serial.print("'");
    Serial.print(type);
    Serial.println("' is not a valid argument. Choose 'get', 'open', 'new', or 'close'");
}

void SDFiles::getFlushPeriod_c(const Interpreter::Token* args){
    char nameBuffer[128];
    args[0].copyStringData(nameBuffer, 128);
    if(std::strcmp(nameBuffer, "analysis") == 0){
        Serial.print(get()->getLog(TelemetryLogs::Analysis)->getFlushPeriod());
        Serial.println(" logs");
        return;
    }
    if(std::strcmp(nameBuffer, "blackBox") == 0){
        Serial.print(get()->getLog(TelemetryLogs::BlackBox)->getFlushPeriod());
        Serial.println(" logs");
        return;
    }
    if(std::strcmp(nameBuffer, "events") == 0){
        Serial.print(get()->getLog(TelemetryLogs::Message)->getFlushPeriod());
        Serial.println(" logs");
        return;
    }
    if(std::strcmp(nameBuffer, "dataSim") == 0){
        Serial.print(get()->getLog(TelemetryLogs::HIL)->getFlushPeriod());
        Serial.println(" logs");
        return;
    }
    Serial.print("PayloadOS does not contain the file '");
    Serial.print(nameBuffer);
    Serial.println("'");
}

void SDFiles::setFlushPeriod_c(const Interpreter::Token* args){
    char nameBuffer[128];
    uint_t newPeriod = args[1].getUnsignedData();
    args[0].copyStringData(nameBuffer, 128);
    if(std::strcmp(nameBuffer, "analysis") == 0){
        get()->getLog(TelemetryLogs::Analysis)->setFlushPeriod(newPeriod);
        return;
    }
    if(std::strcmp(nameBuffer, "blackBox") == 0){
        get()->getLog(TelemetryLogs::BlackBox)->setFlushPeriod(newPeriod);
        return;
    }
    if(std::strcmp(nameBuffer, "events") == 0){
        get()->getLog(TelemetryLogs::Message)->setFlushPeriod(newPeriod);
        return;
    }
    if(std::strcmp(nameBuffer, "dataSim") == 0){
        get()->getLog(TelemetryLogs::HIL)->setFlushPeriod(newPeriod);
        return;
    }
    Serial.print("PayloadOS does not contain the file '");
    Serial.print(nameBuffer);
    Serial.println("'");
}


//implementation of TelemetryFile----------------------------------------------
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

TelemetryLog::TelemetryLog() : sd(nullptr), fileName(""), flushPeriod(4), flushCount(0), state(SDStates::None) {} 
TelemetryLog::TelemetryLog(SdFat* sdInstance) : sd(sdInstance), fileName(""), flushPeriod(4), flushCount(0), state(SDStates::None) {} 

void TelemetryLog::setSDCard(SdFat* sdInstance){
    sd = sdInstance;
}

error_t TelemetryLog::openForRead(){
    if(std::strcmp(fileName, "") == 0) return PayloadOS::ERROR;
    file.close();
    state = SDStates::None;
    if(sd == nullptr) return PayloadOS::ERROR;
    file = sd->open(fileName, FILE_READ);
    if(!file) return PayloadOS::ERROR;
    state = SDStates::Read;
    return PayloadOS::GOOD;
}

error_t TelemetryLog::openForWrite(OpenTypes type){
    oflag_t openFlag = O_WRITE | O_CREAT | O_TRUNC;
    if(type == OpenTypes::End) openFlag = O_WRITE | O_CREAT | O_AT_END;
    if(std::strcmp(fileName, "") == 0) return PayloadOS::ERROR;
    file.close();
    state = SDStates::None;
    if(sd == nullptr) return PayloadOS::ERROR;
    file = sd->open(fileName, openFlag);
    if(!file) return PayloadOS::ERROR;
    state = SDStates::Write;
    return PayloadOS::GOOD;
}

error_t TelemetryLog::close(){
    file.close();
    state = SDStates::None;
    return PayloadOS::GOOD;
}

SDStates TelemetryLog::currentMode() const{
    return state;
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

const char* TelemetryLog::getFileName() const{
    return fileName;
}

uint_t TelemetryLog::getFlushPeriod() const{
    return flushPeriod;
}

//read
error_t TelemetryLog::readLine(TelemetryData& telemetry){
    if(!file.isOpen() || !file || state != SDStates::Read) return PayloadOS::ERROR;
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
#define POLL_TIME_US 10
error_t TelemetryLog::logLine(){
    if(!file.isOpen() || !file || state != SDStates::Write) return PayloadOS::ERROR;
    file.print(millis());
    SPACE;
    file.print(static_cast<uint_t>(State::ProgramState::get()->getCurrentState()));
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getAltitude_ft());
    //delayMicroseconds(POLL_TIME_US);
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getAltitude_ft());
    //delayMicroseconds(POLL_TIME_US);
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getPressure_psi());
    //delayMicroseconds(POLL_TIME_US);
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getPressure_psi());
    //delayMicroseconds(POLL_TIME_US);
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getTemperature_F());
    //delayMicroseconds(POLL_TIME_US);
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getTemperature_F());
    //delayMicroseconds(POLL_TIME_US);
    SPACE;
    logIMU(Peripherals::PeripheralSelector::get()->getPayloadIMU());
    //delayMicroseconds(POLL_TIME_US);
    SPACE;
    logIMU(Peripherals::PeripheralSelector::get()->getSTEMnaut1());
    //delayMicroseconds(POLL_TIME_US);
    SPACE;
    logIMU(Peripherals::PeripheralSelector::get()->getSTEMnaut2());
    //delayMicroseconds(POLL_TIME_US);
    SPACE;
    logIMU(Peripherals::PeripheralSelector::get()->getSTEMnaut3());
    //delayMicroseconds(POLL_TIME_US);
    SPACE;
    logIMU(Peripherals::PeripheralSelector::get()->getSTEMnaut4());
    //delayMicroseconds(POLL_TIME_US);
    SPACE;
    file.print(Peripherals::PeripheralSelector::get()->getPowerCheck()->getVoltage());
    //delayMicroseconds(POLL_TIME_US);
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
    //delayMicroseconds(POLL_TIME_US);
    file.print(v.x);
    SPACE;
    file.print(v.y);
    SPACE;
    file.print(v.z);
    SPACE;
    Peripherals::RotationVector u = imu->getAngularVelocity_deg_s();
    //delayMicroseconds(POLL_TIME_US);
    file.print(u.x_rot);
    SPACE;
    file.print(u.y_rot);
    SPACE;
    file.print(u.z_rot);
    SPACE;
    v = imu->getGravityVector();
    //delayMicroseconds(POLL_TIME_US);
    file.print(v.x);
    SPACE;
    file.print(v.y);
    SPACE;
    file.print(v.z);
}

void TelemetryLog::logGPS(){
    Peripherals::GPSData data = Peripherals::PeripheralSelector::get()->getGPS()->getData();
    //delayMicroseconds(POLL_TIME_US);
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

//commands

void TelemetryLog::displayFile(){
    if(openForRead() == PayloadOS::ERROR) Serial.printf("Could not open the file '%s'\n", fileName);
    Serial.printf("##### %s #####\n", fileName);
    TelemetryData telemetry;

    while(readLine(telemetry) != PayloadOS::ERROR && !telemetry.endOfFile){
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

error_t TelemetryLog::logMessage(const char* message){
    if(!file.isOpen() || !file || state != SDStates::Write) return PayloadOS::ERROR;
    file.print("[");
    file.print(millis());
    file.print("] ");
    file.println(message);
    if(flushCount == flushPeriod){
        file.flush();
        flushCount = 0;
    }
    else{
        flushCount++;
    }
    return PayloadOS::GOOD;
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
    else if(!isNumeric(*pos)){
        while(!isWhiteSpace(*pos)) pos++;
        return 0;
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
        place++;
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
    return c - '0';
}
 