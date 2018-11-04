#ifdef ROLE_MASTER
#ifndef MASTER_CPP
#define MASTER_CPP

#include "master.h"


/*
For each serial command, define a command name and a command function.
Assign the command function to another name: prepend the original name
with cmd_. We do this to ensure that we catch any function type errors
at compile time rather than runtime. Well...warnings, not errors. Maybe
play with -fpermissive.
*/
const char* SERIAL_SETPOINT = "setpoint";
const char* SERIAL_PID_CONF = "pid";

void serial_setpoint(char** payload) {

}

void serial_pid_conf(char** payload) {

}


serial_function cmd_serial_setpoint = &serial_setpoint;
serial_function cmd_serial_pid_conf = &serial_pid_conf;

const void* SERIAL_CMD_MAP [] = {
    (void*)SERIAL_SETPOINT,
    (void*)cmd_serial_setpoint,

    (void*)SERIAL_PID_CONF,
    (void*)cmd_serial_pid_conf,
    0
};


void do_specific_work(RFM69* radio, NodeCmd* data) {

}


void handle(RFM69* radio) {
    NodeCmd* data = get_message(radio);
    if(data != 0) {
        print_command(data);
        route_cmd(data, radio);
    }

    do_specific_work(radio, data);

    if(data != 0) {
        del_cmd(data);
    }
}


void cmd_pid_conf(NodeCmd* cmd, RFM69* radio) {

}


void cmd_setpoint(NodeCmd* cmd, RFM69* radio) {
    float value;
    zatof(cmd->cmd->payload, &value);
    if(value == FLOAT_ERR) {
        Serial.println("ERROR: got bad float");
    }

    Serial.print("Got setpoint from someone: ");
    Serial.println(value);
}


#endif
#endif
