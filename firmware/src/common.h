#ifndef COMMON_H
#define COMMON_H

#include "utils.h"


// Any of the kp, ki or kd should be within [-9999.99,9999.99] so keep this
// in mind when you send it over the wire, the buffer for any of them must be 9
#define PID_CONF_CONTROL 0xAA
#define PID_CONF_IGNORE_VAL 12345.12345
#define PID_CONF_KP_DEFAULT 1400
#define PID_CONF_KI_DEFAULT 10
#define PID_CONF_KD_DEFAULT 0
#define PID_CONF_SETPONT_DEFAULT 7
// In milliseconds
#define PID_CONF_WINDOW_SIZE_DEFAULT 1000


// TODO: we need to split this up in multiple structs: one that deals with PID
//       conf, other with relay state and maybe other with current_temp?
//       it is easier to leave it like this for now because I don't have to
//       create a bunch o commands
typedef struct {
    // Make sure you always initialize control with PID_CONF_CONTROL
    byte control;
    double kp;
    double ki;
    double kd;
    double err;
    double setpoint;
    unsigned int window_size;
    double output;
    byte relay_state;
    double current_temp;
} PIDConf;


bool update_pid_conf_from_str(const char* cmd_data, PIDConf* pid_conf);
bool update_pid_conf_from_obj(PIDConf* dest_pid_conf, PIDConf* source_pid_conf);

void init_pid_conf_default_or_saved(PIDConf* pid_conf);
void init_pid_conf_blank(PIDConf*);


#endif
