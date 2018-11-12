#ifdef ROLE_SLAVE
#ifndef SLAVE_CPP
#define SLAVE_CPP

#include "slave.h"


SlaveState current_state;
int last_period = -1;


void do_specific_work(RFM69* radio, NodeCmd* data) {

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
    // respond with current state
    Serial.println("Setting setpoint");
    if(strcmp(cmd->cmd->payload, "REQ") == 0) {
        send_message(radio, GATEWAYID, CMD_SETPOINT, current_state.setpoint);
        return;
    }

    double value;
    zatof(cmd->cmd->payload, &value);
    if(value == DOUBLE_ERR) {
        Serial.println("Error interpreting setpoint");
        return;
    }
    current_state.setpoint = value;
    Serial.print("Set setpoint to: "); Serial.println(value);

    send_message(radio, GATEWAYID, CMD_SETPOINT, current_state.setpoint);
}


#endif
#endif
