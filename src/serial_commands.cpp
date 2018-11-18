#ifndef SERIAL_COMMANDS_CPP
#define SERIAL_COMMANDS_CPP

#include "serial_commands.h"


const char* SERIAL_SETPOINT = "setpoint";
const char* SERIAL_PID_CONF = "pid";


void serial_pid_conf(RFM69* radio, char* command);
void serial_setpoint(RFM69* radio, char* command);


// Commands that get executed from serial input
const void* SERIAL_CMD_MAP [] = {
    (void*)SERIAL_SETPOINT,
    (void*)serial_setpoint,

    (void*)SERIAL_PID_CONF,
    (void*)serial_pid_conf,
    0
};


/*
Interpret the serial command for setting/requesting the PID conf to/from
a node.
The command is as follows:
- set: pid;<target:bytes>;{[kp|ki|kd]=<value:double>}+
  for example: pid;99;kp=10;kd=4 or pid;99;ki=9.8
- request: pid;<target:bytes>;req
*/
void serial_pid_conf(RFM69* radio, char* command) {
    // Serial.println("Executing serial setpoint command");
    char command_details[2][20];
    int num_columns = break_command(command, command_details, 2);

    if(num_columns != 2) {
        // Serial.println("At least two columns requires for pid conf");
        return;
    }

    byte target = parse_target(command_details[1]);

    char* start_of_payload = command;
    byte number_of_smicolons = 0;
    do {
        if(*start_of_payload == ';') {
            number_of_smicolons += 1;
        }
        start_of_payload += 1;
    } while(number_of_smicolons != 2 && *start_of_payload != 0);

    if(*start_of_payload == 0) {
        // Serial.println("Could not parse anything of value from serial");
        return;
    }

    send_message(radio, target, CMD_PID_CONF, start_of_payload);
}

void serial_report_pid_conf(NodeCmd* cmd) {
    char data[50];
    sprintf(data, "%s;%u;%s", SERIAL_PID_CONF, cmd->node_id, cmd->cmd->payload);
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

    byte target = parse_target(command_details[1]);

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


void serial_report_setpoint(NodeCmd* cmd) {
    double value;
    zatof(cmd->cmd->payload, &value);
    if(value == DOUBLE_ERR) {
        // Serial.println("ERROR: got bad double");
        return;
    }

    // Serial.print("Got setpoint from someone: ");
    char data[20];
    sprintf(data, "%s;%u;%s;%d", SERIAL_SETPOINT, cmd->node_id, cmd->cmd->payload, cmd->rssi);
    Serial.println(data);
}



#endif
