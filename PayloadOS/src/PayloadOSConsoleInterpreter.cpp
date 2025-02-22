#include "PayloadOSConsoleInterpreter.h"
#include "PayloadOSConsoleIO.h"
#include "PayloadOSCommandList.h"
#include <cstring>

using namespace PayloadOS;
using namespace PayloadOS::Interpreter;

ConsoleInterpreter::ConsoleInterpreter() : focus(-1) {}

error_t ConsoleInterpreter::readLine() {
	SerialIO* buffer = SerialIO::get();
	Token::setTokenBuffer(buffer);
    //check if there was a command at all
    if(buffer->at(0) != '>'){
        return PayloadOS::GOOD;
        //ignored
    }
	//find newline at the end of the transmission
	int_t currentCharacter;
	for (currentCharacter = 0; currentCharacter < buffer->size() && buffer->at(currentCharacter) != '\0'; currentCharacter++);
	if (currentCharacter >= buffer->size()) {
		return PayloadOS::ERROR;
		//log message (no newline)
	}
	currentCharacter--;
	//extract data into tokens
	int_t currentToken;
	for (currentToken = 0; currentToken < PayloadOS_TokenBufferSize && !tokens[currentToken].extract(currentCharacter); currentToken++);
	if (currentToken == PayloadOS_TokenBufferSize) { 
		SerialIO::get()->printf("<[Cmd:error] Command has too many arguments for interpretation\n");
		return PayloadOS::GOOD;
	}
	//check for command
	if (buffer->at(currentCharacter) != '>') return PayloadOS::GOOD; //possible message
	//reorder tokens start to finish
	int_t numTokens = currentToken;
	reorderTokens(numTokens);
	//check that there is at least one token
	if (numTokens < 1) { 
		SerialIO::get()->printf("<[Cmd:error] You must provide a command name\n");
		return PayloadOS::GOOD;
	}
	//check that first token is a word
	if (tokens[0].setInterpretation(TokenTypes::WordData) == PayloadOS::ERROR) {
		SerialIO::get()->printf("<[Cmd:error] Command name must be interpretable as a word\n");
		return PayloadOS::GOOD;
	}
	//copy command name from first token
	if (tokens->copyStringData(commandBuffer, PayloadOS_CommandBufferSize) == PayloadOS::ERROR){ 
		SerialIO::get()->printf("<[Cmd:error] Command name is too long for interpretation\n");
		return PayloadOS::GOOD;
	}
	//search joint command list
	const Interpreter::CommandList* jointList = State::ProgramState::get()->getBaseCommands();
	const Interpreter::Command* command = nullptr;
    if(jointList != nullptr) command = jointList->getCommandWithName(commandBuffer);
	//search state-specific command list if not found
	if (command == nullptr) {
		const Interpreter::CommandList* stateList = State::ProgramState::get()->getCurrentCommands();
        if(stateList != nullptr) command = stateList->getCommandWithName(commandBuffer);
	}
	//check that command was found 
	if (command == nullptr) {
		SerialIO::get()->printf("<[Cmd:error] Command '%s' is not available in this focus\n", commandBuffer);
		return PayloadOS::GOOD;
	}
	//check that args match
	if (numTokens - 1 != (int_t)std::strlen(command->args)) {
		SerialIO::get()->printf("<[Cmd:error] Invalid number of arguments for '%s'. Expected %d, actual was %d\n", commandBuffer, std::strlen(command->args), numTokens-1);
		return PayloadOS::GOOD;
	}
	//check args and store data
	int_t i;
	for (i = 0; i < numTokens-1; i++) {
		TokenTypes expected = getTokenTypeFromArgCharacter(command->args[i]);
		if (expected == TokenTypes::Invalid) {
			SerialIO::get()->printf("<[Cmd:error] Argument types specifier for '%s' contains invalid character '%c'\n", commandBuffer, command->args[i]);
			return PayloadOS::GOOD;
		}
		if (tokens[i + 1].setInterpretation(expected) == PayloadOS::ERROR) {
			SerialIO::get()->printf("<[Cmd:error] Invalid argument type for '%s'. Expected type '%c' for argument %d\n",commandBuffer, command->args[i], i+1);
			return PayloadOS::GOOD;
		}
	}
	if (command->execution == nullptr) {
		SerialIO::get()->printf("<[Cmd:error] Implementation for '%s' not found\n", commandBuffer);
		return PayloadOS::GOOD;
	}
	//register control change with scheduler!!
	command->execution(tokens + 1);
	//move control change back!!
	printEOC();
	return PayloadOS::GOOD;
}
//-----------------------------------------------------------------------------

void ConsoleInterpreter::printAvailableCommands() const {
	//print state-specific first
	SerialIO::get()->printf("--%s--\n", State::ProgramState::get()->getCurrentStateName());
	const Interpreter::CommandList* stateList = State::ProgramState::get()->getCurrentCommands();
	stateList->printCommands();
	//print general
	const Interpreter::CommandList* generalList = State::ProgramState::get()->getBaseCommands();
	SerialIO::get()->printf("--General--\n");
	generalList->printCommands();
}

//helper functions-------------------------------------------------------------

void ConsoleInterpreter::reorderTokens(int_t numTokens) {
	Token temp;
	for (int_t i = 0; i < numTokens / 2; i++) {
		temp = tokens[i];
		tokens[i] = tokens[numTokens - 1 - i];
		tokens[numTokens - 1 - i] = temp;
	}
}

TokenTypes ConsoleInterpreter::getTokenTypeFromArgCharacter(char arg) {
	switch (arg) {
	case 'u':
		return TokenTypes::unsignedData;
		break;
	case 'i':
		return TokenTypes::signedData;
		break;
	case 's':
		return TokenTypes::StringData;
		break;
	case 'w':
		return TokenTypes::WordData;
		break;
	case 'f':
		return TokenTypes::FloatData;
		break;
	default:
		return TokenTypes::Invalid;
		break;
	}
}

void ConsoleInterpreter::printEOC() {
	SerialIO::get()->printf("<\n");
}

ConsoleInterpreter* ConsoleInterpreter::get() {
	static ConsoleInterpreter instance;
	return &instance;
}