#ifndef COMMON_H
#define COMMON_H

#include "commands.h"
#include "utils.h"


typedef struct {
    double kp;
    double ki;
    double kd;
} PIDConf;


bool update_pid_conf_from_str(const char* cmd_data, PIDConf* pid_conf);


#endif
