#ifndef COMMON_H
#define COMMON_H

#include "utils.h"


// Any of the kp, ki or kd should be within [-9999.99,9999.99] so keep this
// in mind when you send it over the wire, the buffer for any of them must be 9
#define PID_CONF_CONTROL 0xAA
#define PID_CONF_IGNORE_VAL 12345.12345
typedef struct {
    // Make sure you always initialize control with PID_CONF_CONTROL
    byte control;
    double kp;
    double ki;
    double kd;
    double err;
    double setpoint;
    int window_size;
    byte output;
} PIDConf;


bool update_pid_conf_from_str(const char* cmd_data, PIDConf* pid_conf);
bool update_pid_conf_from_obj(PIDConf* dest_pid_conf, PIDConf* source_pid_conf);
void init_pid_conf(PIDConf*);


#endif
