#pragma once

#include "PayloadOSGeneral.h"
#include "PayloadOSToken.h"
#include <array>

#define CMD PayloadOS::Interpreter::Command

namespace PayloadOS{
    namespace Interpreter{
        struct Command{
            const char* name;
            const char* args;
            void (*execution) (const Token*);
        };

        class CommandList{
            const Command* commands;
            int_t actualSize;
        public:
            CommandList(const Command*, uint_t);
		    const Command* getCommandWithName(char*) const;
		    void printCommands() const;
        };

    }
}