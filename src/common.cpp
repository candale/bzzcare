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
            case 'p':
                pid_conf->kp = value;
            case 'i':
                pid_conf->ki = value;
            case 'd':
                pid_conf->kd = value;
        }
    }

    return true;
}


#endif
