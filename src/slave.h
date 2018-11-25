#ifdef ROLE_SLAVE
#ifndef SLAVE_H
#define SLAVE_H

#include <RFM69.h>
#include <Wire.h>

#include "config.h"
#include "utils.h"
#include "comms.h"
#include "commands.h"
#include "common.h"
#include "Adafruit_MCP9808.h"

#define TRANSMITPERIOD 1000


typedef struct {
    PIDConf* pid_conf;
} SlaveState;


void handle(RFM69* radio);
void device_setup(RFM69* radio);

void cmd_pid_conf(NodeCmd* cmd, RFM69* radio);
// void cmd_setpoint(NodeCmd* cmd, RFM69* radio);

#endif
#endif
