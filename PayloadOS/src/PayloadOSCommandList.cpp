#include "PayloadOSCommandList.h"
#include "PayloadOSConsoleIO.h"
#include <algorithm>
#include <cstring>

using namespace PayloadOS;
using namespace Interpreter;

//CommandList::CommandList() {}
CommandList::CommandList(const Command* cmds, uint_t size) : commands(cmds), actualSize(size) {}

const Command* CommandList::getCommandWithName(char* name) const {
	int_t i;
	for (i = 0; i < actualSize && std::strcmp(name, commands[i].name) != 0; i++);
	if (i == actualSize) return nullptr;
	return commands + i;
}

void CommandList::printCommands() const {
	for (int_t i = 0; i< actualSize; i++)
		SerialIO::get()->printf("%s {%s}\n", commands[i].name, commands[i].args);
}
