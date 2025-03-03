#pragma once

#include "PayloadOSToken.h"
#include "PayloadOSStateMachine.h"

namespace PayloadOS{
    namespace Interpreter{
        class ConsoleInterpreter {
			int_t focus;
			Token tokens[PayloadOS_TokenBufferSize];
			char commandBuffer[PayloadOS_CommandBufferSize];
		public:
			//parsing interface 
			error_t readLine();

			//callbacks for internal commands
			void printAvailableCommands() const;

			//singleton implementation
		private:
			ConsoleInterpreter();
		public:
			static ConsoleInterpreter* get();
			ConsoleInterpreter(const ConsoleInterpreter&) = delete;
			ConsoleInterpreter operator=(const ConsoleInterpreter&) = delete;

			//helper functions
		private:
			void reorderTokens(int_t);
			static TokenTypes getTokenTypeFromArgCharacter(char);
			static void printEOC();
		};
    }
}