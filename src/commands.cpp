#ifndef COMMANDS_CPP
#define COMMANDS_CPP


#include "commands.h"
#include "work.h"


const cmd_function* CMD_MAP [] = {
    &cmd_pid_conf,
    &cmd_temp
};


#endif
