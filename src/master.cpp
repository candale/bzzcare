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

int last_period = -1;


void serial_pid_conf(RFM69* radio, char* command) {
    Serial.println("Executing serial setpoint command");


}


void serial_setpoint(RFM69* radio, char* command) {
    Serial.println("Executing serial PID conf command");
    char command_details[3][20];
    int num_columns = break_command(command, command_details, 3);
    if(num_columns != 3) {
        Serial.print("ERROR: Expected 3 command columns. Got "); Serial.println(num_columns);
        return;
    }

    int target_ = atoi(command_details[1]);
    if(target_ < 2 || target_ > 255) {
        Serial.println("ERROR: Second argument (target) must an int [2,255]");
        return;
    }
    byte target = target_;

    double setpoint;
    zatof(command_details[2], &setpoint);
    if(setpoint == DOUBLE_ERR) {
        Serial.println("ERROR: Third argument must be a double");
        return;
    }

    Serial.print("Updating setpoint. Target: "); Serial.print(target);
    Serial.print(" Setpoint: "); Serial.println(setpoint);
    cmd_setpoint(radio, target, setpoint);
}


serial_function cmd_serial_setpoint = &serial_setpoint;
serial_function cmd_serial_pid_conf = &serial_pid_conf;

// Commands that get executed from serial
const void* SERIAL_CMD_MAP [] = {
    (void*)SERIAL_SETPOINT,
    (void*)cmd_serial_setpoint,

    (void*)SERIAL_PID_CONF,
    (void*)cmd_serial_pid_conf,
    0
};


void do_specific_work(RFM69* radio, NodeCmd* data) {
    if(Serial.available() > 0) {
        char command[50];
        // TODO: move this logic into anther function
        byte index = 0;
        while(index < 49) {
            byte c = Serial.read();
            if(c == '\n') {
                break;
            }

            command[index++] = c;
            // it appears we need this to get the serial uninterrupted
            delayMicroseconds(100);
        }
        command[index] = 0;
        Serial.print("Got command: "); Serial.println(command);

        serial_function func = get_serial_cmd(SERIAL_CMD_MAP, command);
        if(func != 0) {
            func(radio, command);
        }
    }

    int current_period = millis() / 3000;
    if(current_period != last_period) {
        Serial.println("Sending message ...");
        cmd_setpoint(radio, 99, 60.66);
        last_period = current_period;
    }

}


void handle(RFM69* radio) {
    NodeCmd* data = get_message_with_ack(radio);
    if(data != 0) {
        print_command(data);
        route_cmd(data, radio);
    }

    do_specific_work(radio, data);

    if(data != 0) {
        del_cmd(data);
    }
}


void device_setup(RFM69* radio) {

}


void cmd_pid_conf(NodeCmd* cmd, RFM69* radio) {

}


void cmd_setpoint(NodeCmd* cmd, RFM69* radio) {
    double value;
    zatof(cmd->cmd->payload, &value);
    if(value == DOUBLE_ERR) {
        Serial.println("ERROR: got bad double");
    }

    Serial.print("Got setpoint from someone: ");
    Serial.println(value);
}


void cmd_setpoint(RFM69* radio, byte target, double setpoint) {
    if(send_message(radio, target, CMD_SETPOINT, setpoint) == true) {
        Serial.println("Updated setpoint successfully.");
    } else {
        Serial.println("Failed to update setpoint");
    }
}


#endif
#endif
