#include "PayloadOSSDLog.h"

using namespace PayloadOS;
using namespace Peripherals;


#define SD_CONFIG SdioConfig(FIFO_SDIO)

//configuration
error_t Log::initSD(){
    if(sdCard.begin(SD_CONFIG)){
        initialized = true;
        return PayloadOS::GOOD;
    }  
    initialized = false;
    return PayloadOS::ERROR;
}

void Log::deInitSD(){
    initialized= false;
    sdCard.end();
}

error_t Log::setFileName(const char* name){
    int i;
    for(i=0; i<PayloadOS_LogFileNameSize && name[i]!='\0'; i++){
        fileName[i] = name[i];
    }
    if(i == PayloadOS_LogFileNameSize){
        fileName[PayloadOS_LogFileNameSize - 1] = '\0';
        return PayloadOS::ERROR;
    }
    fileName[i] = '\0';
    return PayloadOS::GOOD;
}

error_t Log::close(){
    if(currentFile.close()) return PayloadOS::GOOD;
    return PayloadOS::ERROR;
}

error_t Log::setDestination(LogDestinations d){
    destination = d;
    if(state != FileStates::Write) return PayloadOS::GOOD;
    return close();
}

LogDestinations Log::currentDestination() const{
    return destination;
}

LogErrors Log::getError() const{
    return flags;
}

void Log::clearError(){
    flags = {0};
}

//write functions
error_t Log::log(const char* message){
    clearError();
    switch(destination){
    case LogDestinations::SDCard:
    {
        if(openForWrite() == PayloadOS::ERROR) return PayloadOS::ERROR;
        bool outcome = currentFile.print(message);
        if(outcome) return PayloadOS::GOOD;
        flags.unableToAcessFile = true;
        return PayloadOS::ERROR;
    }
    break;
    case LogDestinations::Console:
    {
        Serial.print("[Log]");
        Serial.print(message);
        return PayloadOS::GOOD;
    }
    break;
    case LogDestinations::Discard:
    default:
        return PayloadOS::GOOD;
        break;
    }
}

error_t Log::logln(const char* message){
    clearError();
    switch(destination){
    case LogDestinations::SDCard:
    {
        if(openForWrite() == PayloadOS::ERROR) return PayloadOS::ERROR;
        bool outcome = currentFile.println(message);
        if(outcome) return PayloadOS::GOOD;
        flags.unableToAcessFile = true;
        return PayloadOS::ERROR;
    }
    break;
    case LogDestinations::Console:
    {
        Serial.print("[Log]");
        Serial.println(message);
        return PayloadOS::GOOD;
    }
    break;
    case LogDestinations::Discard:
    default:
        return PayloadOS::GOOD;
        break;
    }
}

//read functions
error_t Log::getLine(char* buffer, uint_t size, char delim){
    clearError();
    if(openForRead() == PayloadOS::ERROR) return PayloadOS::ERROR;
    uint_t read = currentFile.readBytesUntil(delim, buffer, size);
    if(read == size){
        flags.bufferLength = true;
        return PayloadOS::ERROR;
    }
    return PayloadOS::GOOD;
}

error_t Log::getInt(int_t& dest){
    clearError();
    if(openForRead() == PayloadOS::ERROR) return PayloadOS::ERROR;
    dest = currentFile.parseInt(SKIP_WHITESPACE);
    return PayloadOS::GOOD;
}

error_t Log::getFloat(float& dest){
    clearError();
    if(openForRead() == PayloadOS::ERROR) return PayloadOS::ERROR;
    dest = currentFile.parseFloat(SKIP_WHITESPACE);
    return PayloadOS::GOOD;
}

error_t Log::openForRead(){
    if(!initialized){
        flags.notInitialized = true;
        return PayloadOS::ERROR;
    }
    if(state == FileStates::Read) return PayloadOS::GOOD;
    if(state == FileStates::Write){
        bool outcome = currentFile.close();
        state = FileStates::Closed;
        if(!outcome){
            flags.didntClose = true;
            return PayloadOS::ERROR;
        }
    }
    bool outcome = currentFile.open(fileName, FILE_READ);
    if(outcome){
        state = FileStates::Read;
        return PayloadOS::GOOD;
    }
    state = FileStates::Closed;
    flags.didntOpen = true;
    return PayloadOS::ERROR;
}

error_t Log::openForWrite(){
    if(!initialized){
        flags.notInitialized = true;
        return PayloadOS::ERROR;
    }
    if(state == FileStates::Write) return PayloadOS::GOOD;
    if(state == FileStates::Read){
        bool outcome = currentFile.close();
        state = FileStates::Closed;
        if(!outcome){
            flags.didntClose = true;
            return PayloadOS::ERROR;
        }
    }
    bool outcome = currentFile.open(fileName, FILE_WRITE);
    if(outcome){
        state = FileStates::Write;
        return PayloadOS::GOOD;
    }
    state = FileStates::Closed;
    flags.didntOpen = true;
    return PayloadOS::ERROR;
}

//singleton implementation
Log* Log::get(){
    static Log instance;
    return &instance;
}

Log::Log() : state(FileStates::Closed), initialized(false),  destination(LogDestinations::Discard){
    clearError();
}