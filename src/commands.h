#ifndef COMMANDS_H
#define COMMANDS_H


#include "utils.h"
#include "config.h"
#include "work.h"


// == SYSTEM RESERVED COMMANDS | FIRST 19 ==

#define SYS_MAX_CMD 19
#define USER_CMD_START 20
#define CMD_REBOOT 1
#define CMD_REPORT_CAPABILITIES 2
#define CMD_TEST_ACK 3

typedef void (*cmd_function)(NodeCmd*, RFM69*);
typedef void (*serial_function)(RFM69*, char*);

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


/*
Both master and slave implement the same exact set of commands. Slave commands
have the responsibility  of changing and reporting the device state.
The master acts only as a proxy. This is why it implements all the commands
of the slave: every command issued to or from a slave must be routed
to the slave or back to the serial command.
Depending on the direction, the master will call the same command but with
different arguments: when it goes from serial to slave we call the command with
the arguments necessary to build the command and the function is responsible of
building and sending the command; when it comes from the slave to the master
the function receives a NodeCmd and the function is responsible of interpreting
the payload and forwarding up through the serial.

example:
void cmd_setpoint(NodeCmd* cmd, RFM69* radio) {
    // this is used to receive the command and forward it upstream, to serial
}

void cmd_setpoint(byte target, float setpoint) {
    // this is used to send the command to a slave
}

We can use function overloading to achieve this goal.
TODO: is this a sane solution?
*/
extern const cmd_function CMD_MAP [];
void route_cmd(NodeCmd* cmd, RFM69* radio);

/*
The argument cmd_app is an array of void pointers. On each even index in the
array there is a char pointer and on each odd pointer there is a pointer to
a serial_function function. The char array on the even index is the name by
which the function from the next position, the odd one, is identified.
The array must terminate in a null pointer.

The second parameter is the actual string received from serial.
*/
serial_function get_serial_cmd(const void** cmd_map, char* payload);

byte break_command(const char* command, char container[][20], int take_only);


#endif
