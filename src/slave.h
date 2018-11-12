#ifdef ROLE_SLAVE
#ifndef SLAVE_H
#define SLAVE_H

#include <RFM69.h>
#include "config.h"
#include "utils.h"
#include "commands.h"

#define TRANSMITPERIOD 1000
#define PID_DEFAULT_KP

#define PID_DEFAULT    4
#define INIT_KP          2
#define INIT_KI          0.005
#define INIT_KD          2


typedef struct {
    double kp;
    double ki;
    double kd;

    double setpoint;
    double output;
    int window_size;
} SlaveState;


void handle(RFM69* radio);
void device_setup(RFM69* radio);

void cmd_pid_conf(NodeCmd* cmd, RFM69* radio);
void cmd_setpoint(NodeCmd* cmd, RFM69* radio);

#endif
#endif
