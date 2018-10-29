#ifndef COMMANDS_H
#define COMMANDS_H


#include "utils.h"
#include "config.h"


// == SYSTEM RESERVED COMMANDS | FIRST 19 ==
#define CMD_REBOOT 1
#define CMD_REPORT_CAPABILITIES 2
#define CMD_TEST_ACK 3

typedef void cmd_function(NodeCmd*);

// == USER DEFINED COMMANDS ==

/*
All of these commands should have their meaning based on the direction:
- gateway to slave -> request the slave to update its state base on the payload
- gateway to slave (empty payload) -> request the slave to report its state
- slave to gateway -> slave is reporting its state

I guess this could be a model. Update as necessary.
*/

// Temperature PID settings
#define CMD_PID_CONF 20

// Temperature setting
#define CMD_TEMP 21


void cmd_pid_conf(NodeCmd* cmd);
void cmd_temp(NodeCmd* cmd);


extern const cmd_function* CMD_MAP [];


#endif
