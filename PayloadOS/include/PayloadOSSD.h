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

        enum class SDFiles{
            Synthesis, Telemetry, Event, SENTINAL_COUNT
        };
        
        class TelemetryLog{
            //general---------------
            SdFat sd;
            uint_t flushPeriod;
            //----------------------

            //Synthesis log---------
            char synthesisFileName[PayloadOS_LogFileNameSize];
            char synthesisLineBuffer[PayloadOS_LogParseBufferSize];
            uint_t synthesisFlushCount;
            SDStates synthesisState;
            FsFile synthesisFile;
            char* synthesisPos;
            //----------------------

            //Telemetry log----------
            char telemetryFileName[PayloadOS_LogFileNameSize];
            uint_t telemetryFlushCount;
            FsFile telemetryFile;
            //----------------------

            //Event log-------------
            char eventFileName[PayloadOS_LogFileNameSize];
            uint_t eventFlushCount;
            FsFile eventFlushCount;
            //----------------------
            
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

            bool getline(char='\n');
            float_t getFloat();
            uint_t getUnsigned();
            void removeWhitespace();
            static uint_t getDigit(char);
            static bool isWhiteSpace(char);
		    static bool isNumeric(char);
            static float_t negPow10(uint_t);
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