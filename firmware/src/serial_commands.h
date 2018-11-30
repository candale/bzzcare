#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

#include <RFM69.h>
#include "config.h"
#include "commands.h"
#include "common.h"

extern const char* SERIAL_PID_CONF;


void serial_pid_conf(RFM69* radio, char* command);
void serial_report_pid_conf(NodeCmd* cmd);
void pid_conf_set(RFM69* radio, byte target, const char* pid_conf_data);
void pid_conf_request(RFM69* radio, byte target);
char* get_after_command_and_target(const char* command);



extern const void* SERIAL_CMD_MAP [];


#endif
