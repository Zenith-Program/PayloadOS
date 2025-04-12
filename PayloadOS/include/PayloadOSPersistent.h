#pragma once
#include "PayloadOSPeripheralInterfaces.h"
#include "PayloadOSFlightParameters.h"
#include "PayloadOSSD.h"
#include "Watchdog_t4.h"

namespace PayloadOS{
    namespace Reset{
        struct PersistentData{
            bool active;
            bool hardware[PayloadOS_NumberOfPeripherals];
            bool mode[PayloadOS_NumberOfPeripherals];
            float_t altimeterZero1, altimeterZero2;
            uint_t clockPeriod;
            const char* analysisFile;
            const char* blackboxFile;
            const char* eventFile;
            float_t flightParams[PayloadOS_NumberOfFlightParameters];
            uint_t state, step;
        };

        class Persistent{
            SdFat* sd;
            FsFile file;
            char lineBuffer[PayloadOS_LogParseBufferSize];
            char analysisFileName[PayloadOS_LogFileNameSize];
            char blackBoxFileName[PayloadOS_LogFileNameSize];
            char eventFileName[PayloadOS_LogFileNameSize];
            static bool enabled;
            static WDT_T4<WDT1> watchdog;
        public:
            Persistent(SdFat*);
            error_t update(PersistentData&);
            error_t read(PersistentData&);

            static void fillData(PersistentData&);
            static void applyData(PersistentData&);

            static void enable();
            static void disable();
            static bool isEnabled();
            static void clearWatchdog();
            static void persistentMode_c(const Interpreter::Token*);

        private:
            bool getline(char='\n');
            error_t readString(char*, char='\n');
            uint_t readUnsigned();
            float_t readFloat();

            const char* skipWhitespace(const char*);
            static uint_t getDigit(char);
            static bool isWhiteSpace(char);
		    static bool isNumeric(char);
            static float_t negPow10(uint_t);
            
        };
    }
}