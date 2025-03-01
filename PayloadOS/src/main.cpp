#include <Arduino.h>
#include "PayloadOSConsoleIO.h"
#include "PayloadOSConsoleInterpreter.h"
#include "PayloadOSPeripheralSelector.h"
#include "PayloadOSSDLog.h"

using namespace PayloadOS;
char buffer[64] = "original";

void setup() {
  PayloadOS::SerialIO::get()->init();
  if(Peripherals::Log::get()->initSD()){
    Serial.println("SD Init Error");
  }
  else{
    Serial.println("Init");
  }
  Peripherals::Log::get()->setDestination(Peripherals::LogDestinations::SDCard);
  Peripherals::Log::get()->setFileName("TestLog.txt");
}

void loop() {
  PayloadOS::Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter();
  PayloadOS::SerialIO* serial = PayloadOS::SerialIO::get();
  PayloadOS::Interpreter::ConsoleInterpreter* interpreter = PayloadOS::Interpreter::ConsoleInterpreter::get();
  serial->update();
  if(serial->hasData()){
    interpreter->readLine();
    serial->clear();
  }
  delay(1000);
  float_t altitude = PayloadOS::Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getAltitude_m();
  Serial.println(buffer);
  if(Serial.available()){
    if(Serial.parseInt() == 1) Peripherals::Log::get()->getLine(buffer, 64);
  }
 
}

