#ifdef ROLE_SLAVE
#ifndef SLAVE_H
#define SLAVE_H

#include <RFM69.h>
#include <Wire.h>
#include <PID_v1.h>

#include "config.h"
#include "utils.h"
#include "comms.h"
#include "commands.h"
#include "common.h"
#include "Adafruit_MCP9808.h"


#define READ_TEMP_PERIOD 1000
#define REPORT_PID_CONF_PERIOD 2000

#define RELAY_PIN 8


typedef struct {
    PIDConf* pid_conf;
} SlaveState;


void handle(RFM69* radio);
void device_setup(RFM69* radio);

void cmd_pid_conf(NodeCmd* cmd, RFM69* radio);
// void cmd_setpoint(NodeCmd* cmd, RFM69* radio);

#endif
#endif
