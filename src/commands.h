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
extern char CMD_REQ [];

// Temperature PID settings
#define CMD_PID_CONF 20


/*
Both master and slave implement the same exact set of commands.
Slave commands have the responsibility  of changing and reporting the device
state.
Master commands are used only for reporting what a slave sent up to the
serial interface. So the master acts only as a proxy between the serial comms
and the slaves.

Data flow:

SERIAL -> GATEWAY -> SLAVE
SLAVE -> GATEWAY -> SERIAL

There are two types of commands: radio and serial.
Serial commands are only between a device connected to the gateway and the
gateway.
Radio commands are between the gateway and slaves.


Serial commands
===============

Serial commands get executed when there is input from the serial. In this
case the gateway acts as a proxy: it interprets the data from the serial, turns
it into a domain-specific command and sends it over the radio to the specified
target(s).

Incoming serial commands must have the following form:
<cmd>;<target>;<payload>

The <payload> is interpreted by the serial command responsible for the
send <cmd>.

Radio commands
==============

Radio commands get executed when data is received by one of the slave devices
or when data is received by the gateway. When a slave receives a command it
has the responsibility to execute it. When the gateway receives a command from
a slave, it has the responsibility to act as a proxy and forward the command,
in the appropriate format, to the serial interface.

It is preferred to use structs for sending data such that the parsing overhead
is as small as possible.

The gateway radio commands get executed when a slave send data its way. These
commands have the responsibility to make up the text representation of the
command received and send to the serial.



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
byte parse_target(const char* target_str);


#endif
