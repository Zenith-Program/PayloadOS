#pragma once

#include "PayloadOSGeneral.h"
#include "PayloadOSToken.h"
#include <initializer_list>

namespace PayloadOS{
    namespace Interpreter{
        struct Command{
            const char* name;
            const char* args;
            void (*execution) (const Token*);
        };

        class CommandList{
            Command commands[PayloadOS_CommandListSize];
            int_t actualSize;
        public:
            CommandList();
		    CommandList(std::initializer_list<Command> list);
		    const Command* getCommandWithName(char*) const;
		    void printCommands() const;
        };

    }
}