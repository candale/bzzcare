#ifdef ROLE_MASTER
#ifndef MASTER_H
#define MASTER_H

#include <RFM69.h>
#include "config.h"
#include "utils.h"
#include "comms.h"
#include "commands.h"
#include "serial_commands.h"

void handle(RFM69* radio);
void device_setup(RFM69* radio);

void cmd_pid_conf(NodeCmd* cmd, RFM69* radio);

void cmd_setpoint(NodeCmd* cmd, RFM69* radio);
void cmd_setpoint(RFM69*, byte target, double setpoint);

extern const void* SERIAL_CMD_MAP [];

#endif
#endif
