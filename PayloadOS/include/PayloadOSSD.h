#pragma once
#include "PayloadOSGeneral.h"
#include "PayloadOSPeripheralSelector.h"
#include "PayloadOSConsoleInterpreter.h"
#include <SdFat.h>

namespace PayloadOS{
    namespace FlightData{

        struct TelemetryData{
            bool endOfFile;
            uint_t time;
            uint_t state;
            float_t altitude1, altitude2;
            float_t pressure1, pressure2;
            float_t temperature1, temperature2;
            Peripherals::LinearVector accel0, accel1, accel2, accel3, accel4;
            Peripherals::RotationVector angularVelocity0, angularVelocity1, angularVelocity2, angularVelocity3, angularVelocity4;
            Peripherals::LinearVector grav0, grav1, grav2, grav3, grav4;
            float_t power;
            Peripherals::GPSData gps;
        };

        enum class SDStates{
            None, Read, Write
        };
        
        class TelemetryLog{
            char fileName[PayloadOS_LogFileNameSize];
            uint_t flushPeriod;
            uint_t flushCount;
            SDStates state;
            SdFat sd;
            FsFile file;
        public:
            //general
            error_t init();
            error_t openForRead();
            error_t openForWrite();
            error_t close();
            error_t setFlushPeriod(uint_t);
            error_t setFileName(const char*);
            //read
            error_t readLine(TelemetryData&);
            //write
            error_t logLine();
            //commands
            static void setName_CMD(const Interpreter::Token*);
            static void displayFile_CMD(const Interpreter::Token*);
            static void setFlush_CMD(const Interpreter::Token*);
            static void init_CMD(const Interpreter::Token*);
        private:
            void logIMU(Peripherals::IMUInterface*);
            void logGPS();
            void readIMU(TelemetryData&, uint_t);
            void readGPS(TelemetryData&);
            static void printIMU(const TelemetryData&, uint_t);
            static void printGPS(const TelemetryData&);
        //singleton implementation
        public:
            static TelemetryLog* get();
            TelemetryLog(const TelemetryLog&) = delete;
            void operator=(const TelemetryLog&) = delete;
        private:
            TelemetryLog();
        };

    }
}