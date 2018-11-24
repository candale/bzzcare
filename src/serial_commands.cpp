#ifndef SERIAL_COMMANDS_CPP
#define SERIAL_COMMANDS_CPP

#include "serial_commands.h"


const char* SERIAL_SETPOINT = "setpoint";
const char* SERIAL_PID_CONF = "pid";


/*
For each serial command, define a command name and a command function.
Assign the command function to another name: prepend the original name
with cmd_. We do this to ensure that we catch any function type errors
at compile time rather than runtime. Well...warnings, not errors. Maybe
play with -fpermissive.
*/
void serial_pid_conf(RFM69* radio, char* command);
void serial_setpoint(RFM69* radio, char* command);


serial_function cmd_serial_setpoint = serial_setpoint;
serial_function cmd_serial_pid_conf = serial_pid_conf;


// Commands that get executed from serial input
const void* SERIAL_CMD_MAP [] = {
    (void*)SERIAL_SETPOINT,
    (void*)cmd_serial_setpoint,

    (void*)SERIAL_PID_CONF,
    (void*)cmd_serial_pid_conf,
    0
};


void set_setpoint(RFM69* radio, byte target, double setpoint) {
    if(send_message(radio, target, CMD_SETPOINT, setpoint) == true) {
        Serial.println("Updated setpoint successfully.");
    } else {
        Serial.println("Failed to update setpoint");
    }
}


void request_setpoint(RFM69* radio, byte target) {
    send_message(radio, target, CMD_SETPOINT, CMD_REQ);
}


/*
Interpret the serial command for setting/requesting the PID conf to/from
a node.
The command is as follows:
- set: pid;<target:bytes>;{[kp|ki|kd]=<value:double>}+
  for example: pid;99;kp=10;kd=4 or pid;99;ki=9.8
- request: pid;<target:bytes>;req
*/
void serial_pid_conf(RFM69* radio, char* command) {
    Serial.println("Executing serial pid command");
    char command_details[3][20];
    int num_columns = break_command(command, command_details, 3);

    if(num_columns != 3) {
        Serial.println("At least three columns requires for pid conf");
        return;
    }

    byte target = parse_target(command_details[1]);
    if(target == 0) {
        Serial.println("Got bad target");
        return;
    }

    if(strcmp(command_details[2], CMD_REQ) == 0) {
        send_message(radio, target, CMD_PID_CONF, CMD_REQ);
        return;
    }

    // Skip over the command name and target and send the rest to the  slave device
    char* start_of_payload = command;
    byte number_of_smicolons = 0;
    do {
        if(*start_of_payload == ';') {
            number_of_smicolons += 1;
        }
        start_of_payload += 1;
    } while(number_of_smicolons != 2 && *start_of_payload != 0);

    if(*start_of_payload == 0) {
        Serial.println("Could not parse anything of value from serial");
        return;
    }

    if(strlen(start_of_payload) > PAYLOAD_SIZE - 1) {
        Serial.println("err");
        return;
    }

    PIDConf pid_conf;
    init_pid_conf(&pid_conf);

    update_pid_conf_from_str(start_of_payload, &pid_conf);
    send_message(radio, target, CMD_PID_CONF, (byte *)&pid_conf, sizeof(pid_conf));
}


void serial_report_pid_conf(NodeCmd* cmd) {
    PIDConf reported_pid_conf;
    memcpy(&reported_pid_conf, cmd->cmd->payload, sizeof(reported_pid_conf));
    if(reported_pid_conf.control != PID_CONF_CONTROL) {
        Serial.println("Got PID conf from slave with bad control number");
        return;
    }

    char data[120];
    char kp[10], ki[10], kd[10], ks[10], err[10];
    dtostrf(reported_pid_conf.kp, 3, 2, kp);
    dtostrf(reported_pid_conf.ki, 3, 2, ki);
    dtostrf(reported_pid_conf.kd, 3, 2, kd);
    dtostrf(reported_pid_conf.err, 3, 2, err);
    dtostrf(reported_pid_conf.setpoint, 3, 2, ks);
    sprintf(
        data, "%s;%u;%d;kp=%s;ki=%s;kd=%s;kw=%d;ks=%s;err=%s;ko=%s",
        SERIAL_PID_CONF, cmd->node_id, cmd->rssi,
        kp, ki, kd, reported_pid_conf.window_size, ks, err,
        reported_pid_conf.output == 1 ? "on" : "off"
    );
    Serial.println(data);
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
    if(target == 0) {
        // Serial.println("Got bad target");
        return;
    }

    if(strcmp(command_details[2], CMD_REQ) == 0) {
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


// void serial_report_setpoint(NodeCmd* cmd) {
//     double value;
//     zatof(cmd->cmd->payload, &value);
//     if(value == DOUBLE_ERR) {
//         // Serial.println("ERROR: got bad double");
//         return;
//     }

//     // Serial.print("Got setpoint from someone: ");
//     char data[20];
//     sprintf(data, "%s;%u;%s;%d", SERIAL_SETPOINT, cmd->node_id, cmd->cmd->payload, cmd->rssi);
//     Serial.println(data);
// }



#endif
