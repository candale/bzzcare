#ifndef SERIAL_COMMANDS_CPP
#define SERIAL_COMMANDS_CPP

#include "serial_commands.h"


const char* SERIAL_SETPOINT = "setpoint";
const char* SERIAL_PID_CONF = "pid";


void serial_pid_conf(RFM69* radio, char* command);
void serial_setpoint(RFM69* radio, char* command);


// Commands that get executed from serial
const void* SERIAL_CMD_MAP [] = {
    (void*)SERIAL_SETPOINT,
    (void*)serial_setpoint,

    (void*)SERIAL_PID_CONF,
    (void*)serial_pid_conf,
    0
};



void serial_pid_conf(RFM69* radio, char* command) {
    // Serial.println("Executing serial setpoint command");


}


void set_setpoint(RFM69* radio, byte target, double setpoint) {
    if(send_message(radio, target, CMD_SETPOINT, setpoint) == true) {
        // Serial.println("Updated setpoint successfully.");
    } else {
        // Serial.println("Failed to update setpoint");
    }
}


void request_setpoint(RFM69* radio, byte target) {
    send_message(radio, target, CMD_SETPOINT, CMD_REQ);
}


/*
Interpret the serial command for setting/requesting the setpoint to/from
a node.
The command is as follows:
- set: "setpoint;<target:byte>;<value:double>"
- request: "setpoint;<target:byte>;req"
*/
void serial_setpoint(RFM69* radio, char* command) {
    // Serial.println("Executing serial PID conf command");
    char command_details[3][20];
    int num_columns = break_command(command, command_details, 3);
    if(num_columns != 3) {
        // Serial.print("ERROR: Expected 3 command columns. Got "); Serial.println(num_columns);
        return;
    }

    int target_ = atoi(command_details[1]);
    if(target_ < 2 || target_ > 255) {
        // Serial.println("ERROR: Second argument (target) must an int [2,255]");
        return;
    }
    byte target = target_;

    if(strcmp(command_details[1], CMD_REQ) == 0) {
        request_setpoint(radio, target);
        return;
    }

    double setpoint;
    zatof(command_details[2], &setpoint);
    if(setpoint == DOUBLE_ERR) {
        // Serial.println("ERROR: Third argument must be a double");
        return;
    }

    // Serial.print("Updating setpoint. Target: "); Serial.print(target);
    // Serial.print(" Setpoint: "); Serial.println(setpoint);
    set_setpoint(radio, target, setpoint);
}



#endif
