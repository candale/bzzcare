#ifndef COMMON_CPP
#define COMMON_CPP

#include "common.h"


bool update_pid_conf_from_str(const char* cmd_data, PIDConf* pid_conf) {
    char command_details[5][20];
    int num_columns = break_command(cmd_data, command_details, 5);
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
                break;
            // Set Integral control
            case 'i':
                pid_conf->ki = value;
                break;
            // Set Derivative control
            case 'd':
                pid_conf->kd = value;
                break;
            // Set window size
            case 'w':
                pid_conf->window_size = (int)value;
                break;
            // Set setpoint
            case 's':
                pid_conf->setpoint = value;
                break;
        }
    }

    return true;
}


bool update_pid_conf_from_obj(PIDConf* dest_pid_conf, PIDConf* source_pid_conf) {
    bool changed = false;
    if(dest_pid_conf->kp != source_pid_conf->kp &&
            source_pid_conf->kp != PID_CONF_IGNORE_VAL) {
        dest_pid_conf->kp = source_pid_conf->kp;
        changed = true;
    }
    if(dest_pid_conf->ki != source_pid_conf->ki &&
            source_pid_conf->ki != PID_CONF_IGNORE_VAL) {
        dest_pid_conf->ki = source_pid_conf->ki;
        changed = true;
    }
    if(dest_pid_conf->kd != source_pid_conf->kd &&
            source_pid_conf->kd != PID_CONF_IGNORE_VAL) {
        dest_pid_conf->kd = source_pid_conf->kd;
        changed = true;
    }
    if(dest_pid_conf->err != source_pid_conf->err &&
            source_pid_conf->err != PID_CONF_IGNORE_VAL) {
        dest_pid_conf->err = source_pid_conf->err;
        changed = true;
    }
    if(dest_pid_conf->setpoint != source_pid_conf->setpoint &&
            source_pid_conf->setpoint != PID_CONF_IGNORE_VAL) {
        dest_pid_conf->setpoint = source_pid_conf->setpoint;
        changed = true;
    }
    if(dest_pid_conf->window_size != (int)source_pid_conf->window_size &&
            source_pid_conf->window_size != (int)PID_CONF_IGNORE_VAL) {
        dest_pid_conf->window_size = source_pid_conf->window_size;
        changed = true;
    }

    return changed;
}


void init_pid_conf_blank(PIDConf* pid_conf) {
    pid_conf->control = PID_CONF_CONTROL;
    pid_conf->kp = PID_CONF_IGNORE_VAL;
    pid_conf->ki = PID_CONF_IGNORE_VAL;
    pid_conf->kd = PID_CONF_IGNORE_VAL;
    pid_conf->err = PID_CONF_IGNORE_VAL;
    pid_conf->window_size = PID_CONF_IGNORE_VAL;
    pid_conf->setpoint = PID_CONF_IGNORE_VAL;
}


void init_pid_conf_default_or_saved(PIDConf* pid_conf) {
    pid_conf->control = PID_CONF_CONTROL;

    pid_conf->kp = PID_CONF_KP_DEFAULT;
    pid_conf->ki = PID_CONF_KI_DEFAULT;
    pid_conf->kd = PID_CONF_KD_DEFAULT;
    pid_conf->setpoint = PID_CONF_SETPONT_DEFAULT;
    pid_conf->window_size = PID_CONF_WINDOW_SIZE_DEFAULT;
    pid_conf->output = 0;
    pid_conf->err = 0;
}

#endif
