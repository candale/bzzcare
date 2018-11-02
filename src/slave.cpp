#ifdef ROLE_SLAVE
#ifndef SLAVE_CPP
#define SLAVE_CPP

#include "slave.h"


int last_period = -1;


void do_specific_work(RFM69* radio, NodeCmd* data) {
    int current_period = millis() / TRANSMITPERIOD;
    if(current_period != last_period) {
        Serial.println("Sending message ...");
        send_message(radio, GATEWAYID, (byte)'A', "cool payload man");
        last_period = current_period;
    }
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


void cmd_pid_conf(NodeCmd* cmd) {

}

void cmd_temp(NodeCmd* cmd) {

}


#endif
#endif
