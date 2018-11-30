#ifdef ROLE_MASTER
#ifndef MASTER_CPP
#define MASTER_CPP

#include "master.h"


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
            if(c == '\r') {
                Serial.println("err: must be new line");
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
    serial_report_pid_conf(cmd);
}


void cmd_report_err(NodeCmd* cmd, RFM69* radio) {

}



#endif
#endif
