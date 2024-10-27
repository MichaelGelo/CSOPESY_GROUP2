#include "ICommand.h"

// Constructor definition
ICommand::ICommand(int pid, CommandType commandType)
    : pid(pid), commandType(commandType) {}

// Getter for command type
ICommand::CommandType ICommand::getCommandType(){
    return commandType;
}

void ICommand::execute() {
    int a = 0;
    a++;
    a--;
}

