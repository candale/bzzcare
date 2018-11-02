#ifdef ROLE_MASTER
#ifndef MASTER_CPP
#define MASTER_CPP

#include "master.h"


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
