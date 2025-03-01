#pragma once
#include "PayloadOSGeneral.h"
#include <SdFat.h>

namespace PayloadOS{
    namespace Peripherals{
        enum class LogDestinations{
            SDCard, Console, Discard
        };
        enum class FileStates{
            Closed, Read, Write
        };

        struct LogErrors{
            bool wrongMode;
            bool unableToAcessFile;
            bool notInitialized;
            bool bufferLength;
            bool didntClose;
            bool didntOpen;
        };

        class Log{
            SdFat sdCard;
            bool initialized;

            FsFile currentFile;
            FileStates state;
            
            LogDestinations destination;
            LogErrors flags;

            char fileName[PayloadOS_LogFileNameSize];

        public:

            //configuration
            error_t initSD();
            void deInitSD();
            error_t setFileName(const char*);
            error_t close();
            error_t setDestination(LogDestinations);
            LogDestinations currentDestination() const;
            LogErrors getError() const;
            void clearError();

            //write functions
            error_t log(const char*);
            error_t logln(const char*);

            //read functions
            error_t getLine(char*, uint_t, char='\n');
            error_t getInt(int_t&);
            error_t getFloat(float&);

        private:
            error_t openForRead();
            error_t openForWrite();
            
        //singleton implementation
        public:
            static Log* get();
            Log(const Log&) = delete;
            void operator=(const Log&) = delete;
        private:
            Log();
        };
    }
}