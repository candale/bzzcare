#ifndef COMMON_CPP
#define COMMON_CPP

#include "common.h"


bool update_pid_conf_from_str(const char* cmd_data, PIDConf* pid_conf) {
    char command_details[3][20];
    int num_columns = break_command(cmd_data, command_details, 3);
    if(num_columns == 0) {
        // Serial.println("Got PID conf data with no columns");
        return false;
    }

    for(byte i = 0; i < num_columns; i++) {
        char* cmd = command_details[i];

        // should be something like 'kp=34.5'
        if(cmd[0] != 'k' || cmd[2] != '=') {
            // Serial.println("All pid commands must start with 'k' and contain '='");
            return false;
        }

        double value;
        zatof(cmd + 3, &value);

        if(value == DOUBLE_ERR) {
            // Serial.println("Got invalid error at pid conf");
            return false;
        }

        switch(cmd[1]) {
            // Set Proportional control
            case 'p':
                pid_conf->kp = value;
            // Set Integral control
            case 'i':
                pid_conf->ki = value;
            // Set Derivative control
            case 'd':
                pid_conf->kd = value;
            // Set window size
            case 'w':
                pid_conf->window_size = (int)value;
            // Set setpoint
            case 's':
                pid_conf->setpoint = value;
        }
    }

    return true;
}


bool update_pid_conf_from_obj(PIDConf* dest_pid_conf, PIDConf* source_pid_conf) {
    bool changed = false;
    if(dest_pid_conf->kp != source_pid_conf->kp) {
        dest_pid_conf->kp = source_pid_conf->kp;
        changed = true;
    }
    if(dest_pid_conf->ki != source_pid_conf->ki) {
        dest_pid_conf->ki = source_pid_conf->ki;
        changed = true;
    }
    if(dest_pid_conf->kd != source_pid_conf->kd) {
        dest_pid_conf->kd = source_pid_conf->kd;
        changed = true;
    }
    if(dest_pid_conf->err != source_pid_conf->err) {
        dest_pid_conf->err = source_pid_conf->err;
        changed = true;
    }
    if(dest_pid_conf->setpoint != source_pid_conf->setpoint) {
        dest_pid_conf->setpoint = source_pid_conf->setpoint;
        changed = true;
    }
    if(dest_pid_conf->window_size != source_pid_conf->window_size) {
        dest_pid_conf->window_size = source_pid_conf->window_size;
        changed = true;
    }

    return changed;
}


#endif
