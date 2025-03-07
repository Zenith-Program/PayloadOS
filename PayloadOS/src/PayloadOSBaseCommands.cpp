#include "PayloadOSStateMachine.h"
#include <Arduino.h>
#include <cstring>
#include "PayloadOSPeripheralSelector.h"
#include "PayloadOSConsoleInterpreter.h"

using namespace PayloadOS;
using namespace State;

//commands----------------------------------------------------
const Interpreter::CommandList* ProgramState::getBaseCommands(){
    static constexpr auto base = std::array{
        CMD{"echo", "", echo},
        CMD{"state", "", getState},
        CMD{"commands", "", commands},
        CMD{"pause", "", pause_C},
        CMD{"play", "", play},
        CMD{"paused?", "", isPlaying_C},
        CMD{"reset", "", reset_C},
        CMD{"getClock", "w", getClock},
        CMD{"setClock", "uw", setClock},
        CMD{"do", "w", do_C},
        CMD{"mode", "ww", mode},
        CMD{"status", "ww", status},
        CMD{"read", "ww", read},
        CMD{"hardware", "ww", hardware_C},
        CMD{"transmitRF", "s", transmitRF},
        CMD{"units", "w", units}
    };
    static const Interpreter::CommandList baseList(&base.front(), base.size());
    return &baseList; // for now
}

void ProgramState::echo(const Interpreter::Token*){
    Serial.println("echo");
}
void ProgramState::getState(const Interpreter::Token*){
    Serial.println(ProgramState::get()->getCurrentStateName());
}
void ProgramState::commands(const Interpreter::Token*){
    Serial.print("##### ");
    Serial.print(ProgramState::get()->getCurrentStateName());
    Serial.println(" Commands #####");
    ProgramState::get()->getCurrentCommands()->printCommands();
    Serial.println("##### Base Commands #####");
    ProgramState::get()->getBaseCommands()->printCommands();
}

void ProgramState::pause_C(const Interpreter::Token*){
    ProgramState::get()->pause();
    Serial.println("Paused");
}

void ProgramState::play(const Interpreter::Token*){
    ProgramState::get()->resume();
    Serial.println("Playing");
}

void ProgramState::isPlaying_C(const Interpreter::Token*){
    if(ProgramState::get()->paused) Serial.println("Paused");
    else Serial.println("Playing");
}

void ProgramState::reset_C(const Interpreter::Token*){
    Serial.println("Reseting");
    ProgramState::get()->reset();
}

void ProgramState::getClock(const Interpreter::Token* args){
    char unit[5];
    args[0].copyStringData(unit, 5);
    if(std::strcmp(unit, "ns") == 0){
        Serial.print(ProgramState::get()->getPeriod()*1000);
        Serial.println("ns");
        return;
    }
    if(std::strcmp(unit, "us") == 0){
        Serial.print(ProgramState::get()->getPeriod());
        Serial.println("us");
        return;
    }
    if(std::strcmp(unit, "ms") == 0){
        Serial.print(ProgramState::get()->getPeriod()/1000);
        Serial.println("ms");
        return;
    }
    if(std::strcmp(unit, "s") == 0){
        Serial.print(ProgramState::get()->getPeriod()/1000000);
        Serial.println("s");
        return;
    }
    Serial.print("'");
    Serial.print(unit);
    Serial.println("' is not a valid unit. Choose ns, us, ms, or s");
}

void ProgramState::setClock(const Interpreter::Token* args){
    char unit[5];
    uint_t newPeriod = args[0].getUnsignedData();
    args[1].copyStringData(unit, 5);
    if(std::strcmp(unit, "ns") == 0){
        ProgramState::get()->changePeriod(newPeriod/1000);
        return;
    }
    if(std::strcmp(unit, "us") == 0){
        ProgramState::get()->changePeriod(newPeriod);
        return;
    }
    if(std::strcmp(unit, "ms") == 0){
        ProgramState::get()->changePeriod(newPeriod*1000);
        return;
    }
    if(std::strcmp(unit, "s") == 0){
        ProgramState::get()->changePeriod(newPeriod*1000000);
        return;
    }
    Serial.print("'");
    Serial.print(unit);
    Serial.println("' is not a valid unit. Choose ns, us, ms, or s");
}

void ProgramState::do_C(const Interpreter::Token* args){
    char type[16];
    args[0].copyStringData(type, 16);
    if(std::strcmp(type, "process") == 0){
        ProgramState::get()->doProcess(true);
        return;
    }
    if(std::strcmp(type, "update") == 0){
        ProgramState::get()->nextState(true);
        return;
    }
    Serial.print("'");
    Serial.print(type);
    Serial.println("' is not a valid process type. Choose 'process' or 'update'");
}

//peripherals
void ProgramState::mode(const Interpreter::Token* args){
    char peripheralName[32];
    char version[16];
    args[0].copyStringData(peripheralName, 32);
    args[1].copyStringData(version, 16);
    bool all = false;
    //interpret name
    Peripherals::PeripheralNames peripheral = Peripherals::PeripheralSelector::getEnumFromName(peripheralName);
    if(peripheral == Peripherals::PeripheralNames::SENTINAL_COUNT){
        if(std::strcmp(peripheralName, "all") == 0){
            all = true;
        }
        else{
            Serial.print("'");
            Serial.print(peripheralName);
            Serial.println("' is not a valid peripheral name");
            return;
        }
    }
    //read case
    if(std::strcmp(version, "get") == 0){
        if(all){
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::PayloadAltimeter), (Peripherals::PeripheralSelector::get()->isBackdoor(Peripherals::PeripheralNames::PayloadAltimeter))? "backdoor" : "hardware");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::LightAPRSAltimeter), (Peripherals::PeripheralSelector::get()->isBackdoor(Peripherals::PeripheralNames::LightAPRSAltimeter))? "backdoor" : "hardware");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::PayloadIMU), (Peripherals::PeripheralSelector::get()->isBackdoor(Peripherals::PeripheralNames::PayloadIMU))? "backdoor" : "hardware");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut1), (Peripherals::PeripheralSelector::get()->isBackdoor(Peripherals::PeripheralNames::STEMnaut1))? "backdoor" : "hardware");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut2), (Peripherals::PeripheralSelector::get()->isBackdoor(Peripherals::PeripheralNames::STEMnaut2))? "backdoor" : "hardware");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut3), (Peripherals::PeripheralSelector::get()->isBackdoor(Peripherals::PeripheralNames::STEMnaut3))? "backdoor" : "hardware");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut4), (Peripherals::PeripheralSelector::get()->isBackdoor(Peripherals::PeripheralNames::STEMnaut4))? "backdoor" : "hardware");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::GPS), (Peripherals::PeripheralSelector::get()->isBackdoor(Peripherals::PeripheralNames::GPS))? "backdoor" : "hardware");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::Transmitter), (Peripherals::PeripheralSelector::get()->isBackdoor(Peripherals::PeripheralNames::Transmitter))? "backdoor" : "hardware");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::PowerCheck), (Peripherals::PeripheralSelector::get()->isBackdoor(Peripherals::PeripheralNames::PowerCheck))? "backdoor" : "hardware");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::ArmSwitch), (Peripherals::PeripheralSelector::get()->isBackdoor(Peripherals::PeripheralNames::ArmSwitch))? "backdoor" : "hardware");
        }
        else{
            if(Peripherals::PeripheralSelector::get()->isBackdoor(peripheral)) Serial.println("Backdoor");
            else Serial.println("Hardware");
        }
        return;
    }
    //set case
    bool back;
    if(std::strcmp(version, "backdoor") == 0) back = true;
    else if(std::strcmp(version, "hardware") == 0) back = false;
    else{
        Serial.print("'");
        Serial.print(version);
        Serial.println("' is not a valid argument. Choose 'hardware', 'backdoor', or 'get'");
        return;
    }
    if(all){
        if(back) Peripherals::PeripheralSelector::get()->chooseAllBackdoor();
        else if(Peripherals::PeripheralSelector::get()->chooseAllHardware() == PayloadOS::ERROR){
            Serial.println("Hardware is not connected for some peripherals");
        }
        return;
    }
    if(back) Peripherals::PeripheralSelector::get()->chooseBackdoor(peripheral);
        else if(Peripherals::PeripheralSelector::get()->chooseHardware(peripheral) == PayloadOS::ERROR){
            Serial.print("Hardware is not connected for ");
            Serial.println(peripheralName);
        } 
}

void ProgramState::status(const Interpreter::Token* args){
    char peripheralName[32];
    char version[8];
    args[0].copyStringData(peripheralName, 32);
    args[1].copyStringData(version, 8);
    bool all = false;
    Peripherals::PeripheralNames peripheral = Peripherals::PeripheralSelector::getEnumFromName(peripheralName);
    if(peripheral == Peripherals::PeripheralNames::SENTINAL_COUNT){
        if(std::strcmp(peripheralName, "all") == 0){
            all = true;
        }
        else{
            Serial.print("'");
            Serial.print(peripheralName);
            Serial.println("' is not a valid peripheral name");
            return;
        }
    }
    if(std::strcmp(version, "init") == 0){
        if(all){
            if(Peripherals::PeripheralSelector::get()->initAll() == PayloadOS::GOOD) return;
            Serial.print("Failed to initialize at least one peripheral");
            return;
        }
        if(Peripherals::PeripheralSelector::get()->init(peripheral) == PayloadOS::GOOD) return;
        Serial.print("Failed to initialize ");
        Serial.println(peripheralName);
        return;
    }
    if(std::strcmp(version, "deInit") == 0){
        if(all){
            if(Peripherals::PeripheralSelector::get()->deInitAll() == PayloadOS::GOOD) return;
            Serial.print("Failed to de-initialize at least one peripheral");
            return;
        }
        if(Peripherals::PeripheralSelector::get()->deInit(peripheral) == PayloadOS::GOOD) return;
        Serial.print("Failed to de-initialize ");
        Serial.println(peripheralName);
        return;
    }
    if(std::strcmp(version, "get") == 0){
        if(all){
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::PayloadAltimeter), (Peripherals::PeripheralSelector::get()->getStatus(Peripherals::PeripheralNames::PayloadAltimeter) == PayloadOS::GOOD)? "ready" : "not ready");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::LightAPRSAltimeter), (Peripherals::PeripheralSelector::get()->getStatus(Peripherals::PeripheralNames::LightAPRSAltimeter) == PayloadOS::GOOD)? "ready" : "not ready");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::PayloadIMU), (Peripherals::PeripheralSelector::get()->getStatus(Peripherals::PeripheralNames::PayloadIMU) == PayloadOS::GOOD)? "ready" : "not ready");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut1), (Peripherals::PeripheralSelector::get()->getStatus(Peripherals::PeripheralNames::STEMnaut1) == PayloadOS::GOOD)? "ready" : "not ready");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut2), (Peripherals::PeripheralSelector::get()->getStatus(Peripherals::PeripheralNames::STEMnaut2) == PayloadOS::GOOD)? "ready" : "not ready");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut3), (Peripherals::PeripheralSelector::get()->getStatus(Peripherals::PeripheralNames::STEMnaut3) == PayloadOS::GOOD)? "ready" : "not ready");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut4), (Peripherals::PeripheralSelector::get()->getStatus(Peripherals::PeripheralNames::STEMnaut4) == PayloadOS::GOOD)? "ready" : "not ready");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::GPS), (Peripherals::PeripheralSelector::get()->getStatus(Peripherals::PeripheralNames::GPS) == PayloadOS::GOOD)? "ready" : "not ready");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::Transmitter), (Peripherals::PeripheralSelector::get()->getStatus(Peripherals::PeripheralNames::Transmitter) == PayloadOS::GOOD)? "ready" : "not ready");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::PowerCheck), (Peripherals::PeripheralSelector::get()->getStatus(Peripherals::PeripheralNames::PowerCheck) == PayloadOS::GOOD)? "ready" : "not ready");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::ArmSwitch), (Peripherals::PeripheralSelector::get()->getStatus(Peripherals::PeripheralNames::ArmSwitch) == PayloadOS::GOOD)? "ready" : "not ready");
            return;
        }
        Serial.print(peripheralName);
        if(Peripherals::PeripheralSelector::get()->getStatus(peripheral) == PayloadOS::GOOD) Serial.println(" is ready");
        else Serial.println(" is not ready");
        return;
    }
    Serial.print("'");
    Serial.print(version);
    Serial.println("' is not a valid peripheral argument. Choose 'init', 'deInit', or 'get'");
}

void ProgramState::read(const Interpreter::Token* args){
    char peripheralName[32];
    char parameter[32];
    args[0].copyStringData(peripheralName, 32);
    args[1].copyStringData(parameter, 32);
    bool perHit = false, nameHit = false;
    //altimeter case
    Peripherals::AltimeterInterface* altimeter_p = nullptr;
    if(std::strcmp(Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::PayloadAltimeter), peripheralName) == 0) 
        altimeter_p = Peripherals::PeripheralSelector::get()->getPayloadAltimeter();
    if(std::strcmp(Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::LightAPRSAltimeter), peripheralName) == 0) 
        altimeter_p = Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter();
    if(altimeter_p != nullptr){
        perHit = true;
        if(std::strcmp(parameter, "altitude") == 0){
            if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
                Serial.print(altimeter_p->getAltitude_ft());
                Serial.println("ft");
            }
            else{
                Serial.print(altimeter_p->getAltitude_m());
                Serial.println("m");
            }
            nameHit = true;
        }
        if(std::strcmp(parameter, "temperature") == 0){
            if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
                Serial.print(altimeter_p->getTemperature_F());
                Serial.println("F");
            }
            else{
                Serial.print(altimeter_p->getTemperature_C());
                Serial.println("C");
            }
            nameHit = true;
        }
        if(std::strcmp(parameter, "pressure") == 0){
            if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
                Serial.print(altimeter_p->getPressure_psi());
                Serial.println("psi");
            }
            else{
                Serial.print(altimeter_p->getPressure_mBar());
                Serial.println("mBar");
            }
            nameHit = true;
        }
    }
    //imu case
    Peripherals::IMUInterface* imu_p = nullptr;
    if(std::strcmp(Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::PayloadIMU), peripheralName) == 0) 
        imu_p = Peripherals::PeripheralSelector::get()->getPayloadIMU();
    if(std::strcmp(Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut1), peripheralName) == 0) 
        imu_p = Peripherals::PeripheralSelector::get()->getSTEMnaut1();
    if(std::strcmp(Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut2), peripheralName) == 0) 
        imu_p = Peripherals::PeripheralSelector::get()->getSTEMnaut2();
    if(std::strcmp(Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut3), peripheralName) == 0) 
        imu_p = Peripherals::PeripheralSelector::get()->getSTEMnaut3();
    if(std::strcmp(Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut4), peripheralName) == 0) 
        imu_p = Peripherals::PeripheralSelector::get()->getSTEMnaut4();
    if(imu_p != nullptr){
        if(std::strcmp(parameter, "accelerationVector") == 0){
            if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
                Peripherals::LinearVector vec = imu_p->getAcceleration_ft_s2();
                Serial.printf("<%d, %d, %d> ft/s^2\n", vec.x, vec.y, vec.z);
            }
            else{
                Peripherals::LinearVector vec = imu_p->getAcceleration_m_s2();
                Serial.printf("<%d, %d, %d> m/s^2\n", vec.x, vec.y, vec.z);
            }
            nameHit = true;
        }
        if(std::strcmp(parameter, "angularVelocityVector") == 0){
            if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
                Peripherals::RotationVector vec = imu_p->getAngularVelocity_deg_s();
                Serial.printf("<%d, %d, %d> deg/s\n", vec.x_rot, vec.y_rot, vec.z_rot);
            }
            else{
                Peripherals::RotationVector vec = imu_p->getAngularVelocity_rad_s();
                Serial.printf("<%d, %d, %d> rad/s\n", vec.x_rot, vec.y_rot, vec.z_rot);
            }
            nameHit = true;
        }
        if(std::strcmp(parameter, "directionVector") == 0){
            Peripherals::LinearVector vec = imu_p->getDirection();
            Serial.printf("<%d, %d, %d>", vec.x, vec.y, vec.z);
            nameHit = true;
        }
        if(std::strcmp(parameter, "angle") == 0){
            if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
                Serial.print(imu_p->getVerticalAngle_deg());
                Serial.println("deg");
            }
            else{
                Serial.print(imu_p->getVerticalAngle_rad());
                Serial.println("rad");
            }
            nameHit = true;
        }
        if(std::strcmp(parameter, "acceleration") == 0){
            if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
                Serial.print(imu_p->getAccelerationMagnitude_ft_s2());
                Serial.println("ft/s^2");
            }
            else{
                Serial.print(imu_p->getAccelerationMagnitude_m_s2());
                Serial.println("m/s^2");
            }
            nameHit = true;
        }
        if(std::strcmp(parameter, "angularVelocity") == 0){
            if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
                Serial.print(imu_p->getAngularVelocityMagnitude_deg_s());
                Serial.println("deg/s");
            }
            else{
                Serial.print(imu_p->getAngularVelocityMagnitude_rad_s());
                Serial.println("rad/s");
            }
            nameHit = true;
        }
        perHit = true;
    }
    //gps case
    if(std::strcmp(Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::GPS), peripheralName) == 0){
        perHit = true;
        if(std::strcmp(parameter, "fix") == 0){
            Peripherals::GPSData data = Peripherals::PeripheralSelector::get()->getGPS()->getData();
            Serial.print("Satalites: ");
            Serial.println(data.satalites);
            Serial.print("Since last fix: ");
            Serial.println(data.fixAge);
            nameHit = true;
        }
        if(std::strcmp(parameter, "altitude") == 0){
            if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()){
                Serial.print(Peripherals::PeripheralSelector::get()->getGPS()->getAltitude_ft());
                Serial.println("ft");
            }
            else{
                Serial.print(Peripherals::PeripheralSelector::get()->getGPS()->getAltitude_m());
                Serial.println("m");
            }
            nameHit = true;
        }
        if(std::strcmp(parameter, "location") == 0){
            Peripherals::Coordinate c = Peripherals::PeripheralSelector::get()->getGPS()->getPosition();
            Serial.print(std::abs(c.x));
            if(c.x < 0) Serial.print("S");
            else Serial.print("N");
            Serial.print(", ");
            Serial.print(c.y);
            if(c.x < 0) Serial.println("W");
            else Serial.println("E");
            nameHit = true;
        }
    }
    //switch case
    if(std::strcmp(Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::ArmSwitch), peripheralName) == 0){
        perHit = true;
        if(std::strcmp(parameter, "state") == 0){
            if(Peripherals::PeripheralSelector::get()->getArmSwitch()->isOn()) Serial.println("On");
            else Serial.println("Off");
            nameHit = true;
        }
    }
    //adc case
    if(std::strcmp(Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::PowerCheck), peripheralName) == 0){
        perHit = true;
        if(std::strcmp(parameter, "voltage") == 0){
            Serial.print(Peripherals::PeripheralSelector::get()->getPowerCheck()->getVoltage());
            Serial.println("V");
            nameHit = true;
        }
    }
    if(!perHit){
        Serial.print("'");
        Serial.print(peripheralName);
        Serial.println("' is not a valid peripheral name");
        return;
    }
    if(!nameHit){
        Serial.print("'");
        Serial.print(parameter);
        Serial.print("' is not measured by ");
        Serial.println(peripheralName);
    }
}

void ProgramState::hardware_C(const Interpreter::Token* args){
    char peripheralName[32];
    char version[16];
    args[0].copyStringData(peripheralName, 32);
    args[1].copyStringData(version, 16);
    bool all = false;
    Peripherals::PeripheralNames peripheral = Peripherals::PeripheralSelector::getEnumFromName(peripheralName);
    if(peripheral == Peripherals::PeripheralNames::SENTINAL_COUNT){
        if(std::strcmp(peripheralName, "all") == 0){
            all = true;
        }
        else{
            Serial.print("'");
            Serial.print(peripheralName);
            Serial.println("' is not a valid peripheral name");
            return;
        }
    }
    if(std::strcmp(version, "connect") == 0){
        if(all){
            Peripherals::PeripheralSelector::get()->setAllHardwareAvalibility(true);
            return;
        }
        Peripherals::PeripheralSelector::get()->changeHardwareAvailability(peripheral, true);
        return;
    }
    if(std::strcmp(version, "disconnect") == 0){
        if(all){
            Peripherals::PeripheralSelector::get()->setAllHardwareAvalibility(false);
            return;
        }
        Peripherals::PeripheralSelector::get()->changeHardwareAvailability(peripheral, false);
        return;
    }
    if(std::strcmp(version, "get") == 0){
        if(all){
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::PayloadAltimeter), (Peripherals::PeripheralSelector::get()->getHardwareAvailibility(Peripherals::PeripheralNames::PayloadAltimeter))? "connected" : "not connected");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::LightAPRSAltimeter), (Peripherals::PeripheralSelector::get()->getHardwareAvailibility(Peripherals::PeripheralNames::LightAPRSAltimeter))? "connected" : "not connected");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::PayloadIMU), (Peripherals::PeripheralSelector::get()->getHardwareAvailibility(Peripherals::PeripheralNames::PayloadIMU))? "connected" : "not connected");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut1), (Peripherals::PeripheralSelector::get()->getHardwareAvailibility(Peripherals::PeripheralNames::STEMnaut1))? "connected" : "not connected");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut2), (Peripherals::PeripheralSelector::get()->getHardwareAvailibility(Peripherals::PeripheralNames::STEMnaut2))? "connected" : "not connected");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut3), (Peripherals::PeripheralSelector::get()->getHardwareAvailibility(Peripherals::PeripheralNames::STEMnaut3))? "connected" : "not connected");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::STEMnaut4), (Peripherals::PeripheralSelector::get()->getHardwareAvailibility(Peripherals::PeripheralNames::STEMnaut4))? "connected" : "not connected");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::GPS), (Peripherals::PeripheralSelector::get()->getHardwareAvailibility(Peripherals::PeripheralNames::GPS))? "connected" : "not connected");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::Transmitter), (Peripherals::PeripheralSelector::get()->getHardwareAvailibility(Peripherals::PeripheralNames::Transmitter))? "connected" : "not connected");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::PowerCheck), (Peripherals::PeripheralSelector::get()->getHardwareAvailibility(Peripherals::PeripheralNames::PowerCheck))? "connected" : "not connected");
            Serial.printf("%s: %s\n", Peripherals::PeripheralSelector::getNameFromEnum(Peripherals::PeripheralNames::ArmSwitch), (Peripherals::PeripheralSelector::get()->getHardwareAvailibility(Peripherals::PeripheralNames::ArmSwitch))? "connected" : "not connected");
            return;
        }
        Serial.print(peripheralName);
        if(Peripherals::PeripheralSelector::get()->getHardwareAvailibility(peripheral)) Serial.println(" is connected");
        else Serial.println(" is not connected");
        return;
    }
    Serial.print("'");
    Serial.print(version);
    Serial.println("' is not a valid peripheral argument. Choose 'get', 'connect', or 'disconnect'");
}

void ProgramState::transmitRF(const Interpreter::Token* args){
    char transmission[256];
    args[0].copyStringData(transmission, 256);
    if(Peripherals::PeripheralSelector::get()->getTransmitter()->available()){
        if(Peripherals::PeripheralSelector::get()->getTransmitter()->transmitString(transmission) == PayloadOS::ERROR){
             Serial.println("Transmission error");
        }
    }
    else {
        Serial.println("Transmitter is not available");
    }
}

void ProgramState::units(const Interpreter::Token* args){
    char type[16];
    args[0].copyStringData(type, 16);
    if(std::strcmp(type, "freedom") == 0){
        Interpreter::ConsoleInterpreter::get()->setUnits(true);
        return;
    }
    if(std::strcmp(type, "metric") == 0){
        Interpreter::ConsoleInterpreter::get()->setUnits(false);
        return;
    }
    if(std::strcmp(type, "get") == 0){
        if(Interpreter::ConsoleInterpreter::get()->getCurrentUnits()) Serial.println("Freedom");
        else Serial.println("Metric");
        return;
    }
    Serial.print("'");
    Serial.print(type);
    Serial.println("' is not a valid argument. Choose 'freedom', 'metric' or 'get'");
}


//Internal command interface-----------------------------------
uint_t ProgramState::getPeriod(){
    return period;
}

void ProgramState::changePeriod(uint_t newPeriod){
    if(paused){
        period = newPeriod;
        return;
    }
    pause();
    period = newPeriod;
    resume();
}