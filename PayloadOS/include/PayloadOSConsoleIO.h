#pragma once

#include "PayloadOSGeneral.h"

namespace PayloadOS{
    class SerialIO{
        char rxBuffer[PayloadOS_ConsoleRxBufferSize];
        char txBuffer[PayloadOS_ConsoleTxBufferSize];
        bool hasData_m;
    public:
        int_t size() const;
        char& operator[](int_t);
        char& at(int_t);
        char operator[](int_t) const;
        char at(int_t) const;
		void copy(char*, int_t, int_t) const;
		void printf(const char*, ...);

		error_t init();
		error_t update();
		error_t shutdown();
		bool hasData() const;
		void clear();
		void transmit(const char*);

    //singleton implementation
    private:
        SerialIO();
    public:
        static SerialIO* get();
        SerialIO(const SerialIO&) = delete;
        void operator=(const SerialIO&) = delete;
    };
}