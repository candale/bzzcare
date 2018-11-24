#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

#include <RFM69.h>
#include "config.h"
#include "commands.h"
#include "common.h"

extern const char* SERIAL_SETPOINT;
extern const char* SERIAL_PID_CONF;

void set_setpoint(RFM69* radio, byte target, double setpoint);
void request_setpoint(RFM69* radio, byte target);

void serial_pid_conf(RFM69* radio, char* command);
void serial_report_pid_conf(NodeCmd* cmd);

void serial_setpoint(RFM69* radio, char* command);
void serial_report_setpoint(NodeCmd* cmd);


extern const void* SERIAL_CMD_MAP [];


#endif
