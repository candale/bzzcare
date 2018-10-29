#ifndef COMMANDS_CPP
#define COMMANDS_CPP


#include "commands.h"


const cmd_function* CMD_MAP [] = {
    &cmd_pid_conf,
    &cmd_temp
};


void cmd_pid_conf(NodeCmd* cmd) {

}


void cmd_temp(NodeCmd* cmd) {

}


#endif
