#ifdef ROLE_SLAVE
#ifndef SLAVE_H
#define SLAVE_H

#include <RFM69.h>
#include "config.h"
#include "utils.h"

#define TRANSMITPERIOD 1000


typedef struct {
    float temp;
    float setpoint;
} SlaveState;

SlaveState current_state;


void handle(RFM69* radio);
void cmd_pid_conf(NodeCmd* cmd, RFM69* radio)
void cmd_setpoint(NodeCmd* cmd, RFM69* radio);

#endif
#endif
