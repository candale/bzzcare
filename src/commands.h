#ifndef COMMANDS_H
#define COMMANDS_H


#include "utils.h"
#include "config.h"
#include "work.h"


// == SYSTEM RESERVED COMMANDS | FIRST 19 ==

#define SYS_MAX_CMD 19
#define CMD_REBOOT 1
#define CMD_REPORT_CAPABILITIES 2
#define CMD_TEST_ACK 3

typedef void cmd_function(NodeCmd*, RFM69*);

// == USER DEFINED COMMANDS ==

/*
All of these commands should have their meaning based on the direction:
- gateway to slave -> request the slave to update its state base on the payload
- gateway to slave with 'REQ' payload -> request the slave to report its state
- slave to gateway -> slave is reporting its state

I guess this could be a model. Update as necessary.
*/

// Temperature PID settings
#define CMD_PID_CONF 20

// Temperature setting
#define CMD_SETPOINT 21


extern const cmd_function* CMD_MAP [];
void route_cmd(NodeCmd* cmd, RFM69* radio);


#endif
