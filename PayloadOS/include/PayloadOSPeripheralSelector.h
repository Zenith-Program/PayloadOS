#pragma once
#include "PayloadOSHardwareImplementations.h"
#include "PayloadOSBackdoorImplementations.h"

namespace PayloadOS{
    namespace Peripherals{

        struct SelectorData{
            bool isBackdoor;
            bool hasHardware;
        };

        class PeripheralSelector{
            SelectorData selections[PayloadOS_NumberOfPeripherals];
            //Altimeter Instance
            Hardware::AltimeterHardware altimeterHardware;
            Simulation::AltimeterBackdoor altimeterBackdoor;
            //IMU Instance
            Hardware::IMUHardware imuHardware;
            Simulation::IMUBackdoor imuBackdoor;
            //STEMnaut1 Instance
            Hardware::STEMnaut1Hardware stemnaut1Hardware;
            Simulation::STEMnaut1Backdoor stemnaut1Backdoor;
            //STEMnaut2 Instance
            Hardware::STEMnaut2Hardware stemnaut2Hardware;
            Simulation::STEMnaut2Backdoor stemnaut2Backdoor;
            //STEMnaut3 Instance
            Hardware::STEMnaut3Hardware stemnaut3Hardware;
            Simulation::STEMnaut3Backdoor stemnaut3Backdoor;
            //STEMnaut4 Instance
            Hardware::STEMnaut4Hardware stemnaut4Hardware;
            Simulation::STEMnaut4Backdoor stemnaut4Backdoor;
            //GPS Instance
            Hardware::GPSHardware gpsHardware;
            Simulation::GPSBackdoor gpsBackdoor;
            //Altimeter2 Instance
            Hardware::Altimeter2Hardware altimeter2Hardware;
            Simulation::Altimeter2Backdoor altimeter2Backdoor;
            //Transmitter Instance
            Hardware::TransmitterHardware transmitterHardware;
            Simulation::TransmitterBackdoor transmitterBackdoor;
            //Power Check Instance
            Hardware::PowerCheckHardware powerCheckHardware;
            Simulation::PowerCheckBackdoor powerCheckBackdoor;
            //Arm Switch Instance
            Hardware::ArmSwitchHardware armSwitchHardware;
            Simulation::ArmSwitchBackdoor armSwitchBackdoor;


        public:
            //generalSelection
            PeripheralInterface* getPeripheral(PeripheralNames);
            PeripheralInterface* getPeripheralHardware(PeripheralNames);
            PeripheralInterface* getPeripheralBackdoor(PeripheralNames);

            //selection interface
            AltimeterInterface* getPayloadAltimeter();
            IMUInterface* getPayloadIMU();
            IMUInterface* getSTEMnaut1();
            IMUInterface* getSTEMnaut2();
            IMUInterface* getSTEMnaut3();
            IMUInterface* getSTEMnaut4();
            GPSInterface* getGPS();
            AltimeterInterface* getLightAPRSAltimeter();
            TransmitterInterface* getTransmitter();
            PowerCheckInterface* getPowerCheck();
            ArmSwitchInterface* getArmSwitch();

            //Hardware interface
            Hardware::AltimeterHardware* getPayloadAltimeterHardware();
            Hardware::IMUHardware* getPayloadIMUHardware();
            Hardware::STEMnaut1Hardware* getSTEMnaut1Hardware();
            Hardware::STEMnaut2Hardware* getSTEMnaut2Hardware();
            Hardware::STEMnaut3Hardware* getSTEMnaut3Hardware();
            Hardware::STEMnaut4Hardware* getSTEMnaut4Hardware();
            Hardware::GPSHardware* getGPSHardware();
            Hardware::Altimeter2Hardware* getLightAPRSAltimeterHardware();
            Hardware::TransmitterHardware* getTransmitterHardware();
            Hardware::PowerCheckHardware* getPowerCheckHardware();
            Hardware::ArmSwitchHardware* getArmSwitchHardware();

            //Simulation interface
            Simulation::AltimeterBackdoor* getPayloadAltimeterBackdoor();
            Simulation::IMUBackdoor* getPayloadIMUBackdoor();
            Simulation::STEMnaut1Backdoor* getSTEMnaut1Backdoor();
            Simulation::STEMnaut2Backdoor* getSTEMnaut2Backdoor();
            Simulation::STEMnaut3Backdoor* getSTEMnaut3Backdoor();
            Simulation::STEMnaut4Backdoor* getSTEMnaut4Backdoor();
            Simulation::GPSBackdoor* getGPSBackdoor();
            Simulation::Altimeter2Backdoor* getLightAPRSAltimeterBackdoor();
            Simulation::TransmitterBackdoor* getTransmitterBackdoor();
            Simulation::PowerCheckBackdoor* getPowerCheckBackdoor();
            Simulation::ArmSwitchBackdoor* getArmSwitchBackdoor();

            error_t chooseBackdoor(PeripheralNames);
            error_t chooseHardware(PeripheralNames);
            error_t chooseAllHardware();
            void chooseAllBackdoor();
            bool isBackdoor(PeripheralNames) const;
            error_t initAll();
            error_t init(PeripheralNames);
            error_t deInitAll();
            error_t deInit(PeripheralNames);
            PeripheralStatus getStatus(PeripheralNames) const;
            void changeHardwareAvailability(PeripheralNames, bool);
            bool getHardwareAvailibility(PeripheralNames) const;
            void setAllHardwareAvalibility(bool);
            void printAllReports();

            //interface functions
            static PeripheralNames getEnumFromName(const char*);
            static const char* getNameFromEnum(PeripheralNames);
        private:
            static int_t getIndex(PeripheralNames);
            void setHardwareBits();
        //singleton implementation
        public:
            static PeripheralSelector* get();
            PeripheralSelector(const PeripheralSelector&) = delete;
            void operator=(const PeripheralSelector&) = delete;
        private:
            PeripheralSelector();
        };
    }
}