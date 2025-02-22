#include "PayloadOSConsoleIO.h"
#include <Arduino.h>
#include <cstdarg>
#include <cstdio>

using namespace PayloadOS;

SerialIO::SerialIO() : hasData_m(false) {
    //nothing to do
}

int_t SerialIO::size() const{
    return PayloadOS_ConsoleRxBufferSize;
}

char& SerialIO::operator[](int_t index){
    int_t m = (index % size() + size()) % size();
	return rxBuffer[m];
}

char& SerialIO::at(int_t index){
    int_t m = (index % size() + size()) % size();
	return rxBuffer[m];
}

char SerialIO::operator[](int_t index) const{
    int_t m = (index % size() + size()) % size();
	return rxBuffer[m];
}

char SerialIO::at(int_t index) const{
    int_t m = (index % size() + size()) % size();
	return rxBuffer[m];
}

void SerialIO::copy(char* newBuffer, int_t start, int_t stop) const{
    for (int_t i = start; i < stop; i++)
		newBuffer[i - start] = at(i);
}

void SerialIO::printf(const char* fmt, ...){
    va_list args;
	va_start(args, fmt);
	vsnprintf(txBuffer, PayloadOS_ConsoleTxBufferSize, fmt, args);
	va_end(args);

	transmit(txBuffer);
}

error_t SerialIO::init(){
    Serial.begin(PayloadOS_ConsoleBaudRate);
    while(!Serial) {
        delay(10);
    }
    return PayloadOS::GOOD;
}

error_t SerialIO::update(){
    if(Serial.available()){
        hasData_m = true;
        int_t i;
        for(i = 0; i < PayloadOS_ConsoleRxBufferSize && Serial.available() > 0 && Serial.peek() != '\n'; i++){
            rxBuffer[i] = Serial.read();
        }
        if(i==PayloadOS_ConsoleRxBufferSize) i--;
        rxBuffer[i] = '\0';
    }
    return PayloadOS::GOOD;
}

error_t SerialIO::shutdown(){
    hasData_m = false;
    Serial.end();
    return PayloadOS::GOOD;
}

bool SerialIO::hasData() const{
    return hasData_m;
}

void SerialIO::clear(){
    hasData_m = false;
}

void SerialIO::transmit(const char* buffer){
    Serial.print(buffer);
}

SerialIO* SerialIO::get(){
    static SerialIO instance;
    return &instance;
}
