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
            SdFat* sd;
            char fileName[PayloadOS_LogFileNameSize];
            char buffer[PayloadOS_LogParseBufferSize];
            uint_t flushPeriod;
            uint_t flushCount;
            SDStates state;
            FsFile file;
            char* pos;
        public:
            //general
            void setSDCard(SdFat*);
            error_t openForRead();
            error_t openForWrite();
            error_t close();
            SDStates currentMode() const;
            //read
            error_t readLine(TelemetryData&);
            //write
            error_t logLine();
            error_t logMessage(const char*);
            //interface
            error_t setFlushPeriod(uint_t);
            error_t setFileName(const char*);
            const char* getFileName() const;
            uint_t getFlushPeriod() const;
            void displayFile();
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
        public:
        //public constructor
            TelemetryLog();
            TelemetryLog(SdFat*);
        };

        enum class TelemetryLogs{
            Analysis, BlackBox, Message, HIL, SENTINAL_COUNT
        };

        class SDFiles{
            SdFat sd;
            TelemetryLog analysisLog;
            TelemetryLog blackBoxLog;
            TelemetryLog messageLog;
            TelemetryLog HILLog;
        public:
            error_t init();
            TelemetryLog* getLog(TelemetryLogs);

            //commands------------------
            static void init_c(const Interpreter::Token*);
            static void getFilename_c(const Interpreter::Token*);
            static void setFileName_C(const Interpreter::Token*);
            static void displayFile_c(const Interpreter::Token*);
            static void logCustomEvent_c(const Interpreter::Token*);
            static void eventLogCntrl_c(const Interpreter::Token*);
            static void getFlushPeriod_c(const Interpreter::Token*);
            static void setFlushPeriod_c(const Interpreter::Token*);
            //--------------------------

            //singleton
            static SDFiles* get();
            SDFiles(const SDFiles&) = delete;
            void operator=(const SDFiles&) = delete;
        private:
            SDFiles();
        };
    }
}