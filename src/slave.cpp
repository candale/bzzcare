#ifdef ROLE_SLAVE
#ifndef SLAVE_CPP
#define SLAVE_CPP

#include "slave.h"


int last_period = -1;


void handle(RFM69* radio) {
    int current_period = millis() / TRANSMITPERIOD;
    if(current_period != last_period) {
        Serial.println("Sending message ...");
        send_message(radio, GATEWAYID, (byte)'A', "cool payload man");
        last_period = current_period;
    }
}


void cmd_pid_conf(NodeCmd* cmd) {

}

void cmd_temp(NodeCmd* cmd) {

}


#endif
#endif
