#include <Arduino.h>
#include "PayloadOSConsoleIO.h"
#include "PayloadOSConsoleInterpreter.h"
#include "PayloadOSPeripheralSelector.h"

//#define SPIN_CHECK

#define INPUT_DELAY 200
using namespace PayloadOS;




void setup() {
  PayloadOS::SerialIO::get()->init();
  State::ProgramState::get()->reset();
}

void loop() {
  SerialIO::get()->update();
  if(SerialIO::get()->hasData()){
    if(Interpreter::ConsoleInterpreter::get()->readLine() == PayloadOS::ERROR){
      State::ProgramState::get()->initiateFailure();
    } 
  }
  delay(INPUT_DELAY);
  #ifdef SPIN_CHECK
  Serial.println("Spin");
  #endif
}

