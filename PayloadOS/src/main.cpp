#include <Arduino.h>
#include "PayloadOSConsoleIO.h"
#include "PayloadOSConsoleInterpreter.h"


void setup() {
  PayloadOS::SerialIO::get()->init();
}

void loop() {
  PayloadOS::SerialIO* serial = PayloadOS::SerialIO::get();
  PayloadOS::Interpreter::ConsoleInterpreter* interpreter = PayloadOS::Interpreter::ConsoleInterpreter::get();
  serial->update();
  if(serial->hasData()){
    interpreter->readLine();
    serial->clear();
  }
  delay(1000);
  Serial.println("Good");
}

