#include "PayloadOSPersistent.h"
#include <cstring>

#include "PayloadOSVariance.h"
#include "PayloadOSStateMachine.h"
#include "PayloadOSPeripheralSelector.h"
#include "PayloadOSFlightParameters.h"

using namespace PayloadOS;
using namespace Reset;

Persistent::Persistent(SdFat* card) : sd(card){}

error_t Persistent::update(PersistentData& data){
    //open file
    file.close();
    oflag_t openFlag = O_WRITE | O_CREAT | O_TRUNC;
    if(sd == nullptr) return PayloadOS::ERROR;
    file = sd->open("Persistent.txt", openFlag);
    if(!file) return PayloadOS::ERROR;
    //write persistent data
    file.println(data.active);
    for(uint_t i=0; i<PayloadOS_NumberOfPeripherals; i++)
        file.println(data.hardware[i]);
    for(uint_t i=0; i<PayloadOS_NumberOfPeripherals; i++)
        file.println(data.mode[i]);
    file.println(data.altimeterZero1);
    file.println(data.altimeterZero2);
    file.println(data.clockPeriod);
    file.println(data.analysisFile);
    file.println(data.blackboxFile);
    file.println(data.eventFile);
    for(uint_t i=0; i<PayloadOS_NumberOfFlightParameters; i++)
        file.println(data.flightParams[i]);
    file.println(data.state);
    file.println(data.step);
    file.close();
    return PayloadOS::GOOD;
}

error_t Persistent::read(PersistentData& data){
    //open file
    file.close();
    oflag_t openFlag = FILE_READ;
    if(sd == nullptr) return PayloadOS::ERROR;
    file = sd->open("Persistent.txt", openFlag);
    if(!file) return PayloadOS::ERROR;
    //read data
    getline();
    data.active = readUnsigned() != 0;
    Serial.print("active: ");
    Serial.println(data.active); //debug
    for(uint_t i=0; i<PayloadOS_NumberOfPeripherals; i++){
        getline();
        data.hardware[i] = readUnsigned() != 0;
        Serial.print("hardware: ");
        Serial.println(data.hardware[i]); //debug
    }
    for(uint_t i=0; i<PayloadOS_NumberOfPeripherals; i++){
        getline();
        data.mode[i] = readUnsigned() != 0;
        Serial.print("mode: ");
        Serial.println(data.mode[i]); //debug
    }
    getline();
    data.altimeterZero1 = readFloat();
    getline();
    data.altimeterZero2 = readFloat();
    getline();
    data.clockPeriod = readUnsigned();
    Serial.print("pd: ");
    Serial.println(data.clockPeriod); //debug
    //files
    getline();
    readString(analysisFileName);
    data.analysisFile = analysisFileName;
    Serial.print("file: ");
    Serial.println(data.analysisFile); //debug
    getline();
    readString(blackBoxFileName);
    data.blackboxFile = blackBoxFileName;
    Serial.print("file: ");
    Serial.println(data.blackboxFile); //debug
    getline();
    readString(eventFileName);
    data.eventFile = eventFileName;
    Serial.print("file: ");
    Serial.println(data.eventFile); //debug
    //params
    for(uint_t i=0; i<PayloadOS_NumberOfFlightParameters; i++){
        getline();
        data.flightParams[i] = readFloat();
        Serial.print("param: ");
        Serial.println(data.flightParams[i]); //debug
    }
    getline();
    data.state = readUnsigned();
    Serial.print("state: ");
    Serial.println(data.state); //debug
    getline();
    data.step = readUnsigned();
    Serial.print("step: ");
    Serial.println(data.step); //debug
    Serial.println("done");
    return PayloadOS::GOOD;
}

//get and apply data---------------------------
void Persistent::fillData(PersistentData& data){
    //hardware & mode
    for(uint_t i = 0; i<PayloadOS_NumberOfPeripherals; i++)
        data.hardware[i] = Peripherals::PeripheralSelector::get()->getHardwareAvailibility(Peripherals::PeripheralSelector::getEnumFromIndex(i));
    for(uint_t i = 0; i<PayloadOS_NumberOfPeripherals; i++)
        data.mode[i] = Peripherals::PeripheralSelector::get()->isBackdoor(Peripherals::PeripheralSelector::getEnumFromIndex(i));
    data.altimeterZero1 = FlightData::AltimeterVariances::getAltimeter1Zero();
    data.altimeterZero2 = FlightData::AltimeterVariances::getAltimeter2Zero();
    data.clockPeriod = State::ProgramState::get()->getPeriod();
    data.analysisFile = FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->getFileName();
    data.blackboxFile = FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->getFileName();
    data.eventFile = FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->getFileName();
    for(uint_t i=0; i<PayloadOS_NumberOfFlightParameters; i++)
        data.flightParams[i] = FlightData::FlightParameters::get()->getData(FlightData::FlightParameters::getEnumFromIndex(i))->value;
    data.state = static_cast<uint_t>(State::ProgramState::get()->getCurrentState());
}

void Persistent::applyData(PersistentData& data){
    for(uint_t i=0; i<PayloadOS_NumberOfPeripherals; i++)
        Peripherals::PeripheralSelector::get()->changeHardwareAvailability(Peripherals::PeripheralSelector::getEnumFromIndex(i), data.hardware[i]);
    for(uint_t i = 0; i<PayloadOS_NumberOfPeripherals; i++){
        if(data.mode[i] == true) Peripherals::PeripheralSelector::get()->chooseBackdoor(Peripherals::PeripheralSelector::getEnumFromIndex(i));
        else Peripherals::PeripheralSelector::get()->chooseHardware(Peripherals::PeripheralSelector::getEnumFromIndex(i));
    }
    FlightData::AltimeterVariances::setAltimeter1Zero(data.altimeterZero1);
    FlightData::AltimeterVariances::setAltimeter2Zero(data.altimeterZero2);
    State::ProgramState::get()->changePeriod(data.clockPeriod);
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Analysis)->setFileName(data.analysisFile);
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::BlackBox)->setFileName(data.blackboxFile);
    FlightData::SDFiles::get()->getLog(FlightData::TelemetryLogs::Message)->setFileName(data.eventFile);
    for(uint_t i=0; i<PayloadOS_NumberOfFlightParameters; i++)
        FlightData::FlightParameters::get()->getData(FlightData::FlightParameters::getEnumFromIndex(i))->value = data.flightParams[i];
}

void Persistent::enable(){
    enabled = true;
    WDT_timings_t timing;
    timing.timeout = PayloadOS_WatchdogTimerPeriod_ms/1000;
    Serial.println("test");//h
    watchdog.begin(timing);
    Serial.println("test");//h
}
void Persistent::disable(){
    enabled = false;
    watchdog.reset();
}

bool Persistent::isEnabled(){
    return enabled;
}

void Persistent::clearWatchdog(){
    watchdog.feed();
}

void Persistent::persistentMode_c(const Interpreter::Token* args){
    char buffer[16];
    Serial.println("test2");
    args[0].copyStringData(buffer, 16);
    Serial.println("test1");
    if(std::strcmp(buffer, "do") != 0){
        enable();
        return;
    }
    if(std::strcmp(buffer, "undo") != 0){
        disable();
        return;
    }
    if(std::strcmp(buffer, "get") != 0){
        if(isEnabled()) Serial.println("Enabled");
        else Serial.println("Disabled");
        return;
    }
    Serial.println("Invalid argument. Choose 'do', 'undo', or 'get'");
}

bool Persistent::enabled;
WDT_T4<WDT1> Persistent::watchdog;



//helpers----------------------------------------------------------------------
bool Persistent::getline(char delim){
    char curr = file.read();
    lineBuffer[0] = curr;
    uint_t i = 1;
    while(i<PayloadOS_LogParseBufferSize && curr != delim && file.available()){
        curr = file.read();
        lineBuffer[i] = curr;
        i++;
    }
    return !file.available();
}
error_t Persistent::readString(char* location, char delim){
    uint_t index;
    for(index = 0; index<PayloadOS_LogParseBufferSize && lineBuffer[index] != delim; index++){
        location[index] = lineBuffer[index];
    }
    if(index != PayloadOS_LogParseBufferSize){
        location[index] = '\0';
        return PayloadOS::GOOD;
    } 
    location[PayloadOS_LogParseBufferSize - 1] = '\0';
    return PayloadOS::ERROR;
}

uint_t Persistent::readUnsigned(){
    uint_t value = 0;
    const char* pos = skipWhitespace(lineBuffer);
    while(isNumeric(*pos)){
        value*=10;
        value += getDigit(*pos);
        pos++;
    }
    return value;
}

float_t Persistent::readFloat(){
    float_t value = 0;
    const char* pos = skipWhitespace(lineBuffer);
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

//parsing--------------------------------------
const char* Persistent::skipWhitespace(const char* location){
    for(; isWhiteSpace(*location); location++);
    return location;
}

uint_t Persistent::getDigit(char c){
    return c - '0';
}

bool Persistent::isWhiteSpace(char c){
    if (c == ' ' || c == '\t' || c == '\r' || c == '\v' || c == '\f' || c == '\n') return true;
	return false;
}

bool Persistent::isNumeric(char c){
    if (c >= '0' && c <= '9') return true;
	return false;
}

float_t Persistent::negPow10(uint_t n){
    float_t value = 1;
    for(uint_t i = 0; i<n; i++)
        value /= 10;
    return value;
}