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

int last_period = -1;


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
        // Serial.print("Got command: "); Serial.println(command);

        serial_function func = get_serial_cmd(SERIAL_CMD_MAP, command);
        if(func != 0) {
            func(radio, command);
        }
    }

    int current_period = millis() / 3000;
    if(current_period != last_period) {
        Serial.println("Sending message ...");
        set_setpoint(radio, 99, 60.66);
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
        // Serial.println("ERROR: got bad double");
    }

    // Serial.print("Got setpoint from someone: ");
    Serial.println(value);
}


#endif
#endif
