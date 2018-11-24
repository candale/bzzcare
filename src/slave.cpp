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
    Serial.print("Size of PIDConf: ");Serial.println(sizeof(PIDConf));
    current_state.pid_conf = (PIDConf*)malloc(sizeof(PIDConf));
    current_state.pid_conf->control = PID_CONF_CONTROL;

    current_state.pid_conf->kp = INIT_KP;
    current_state.pid_conf->ki = INIT_KI;
    current_state.pid_conf->kd = INIT_KD;
    current_state.pid_conf->setpoint = 5;
    current_state.pid_conf->output = 0;
    current_state.pid_conf->window_size = 0;
}


void cmd_pid_conf(NodeCmd* cmd, RFM69* radio) {
    if(strcmp((char*)cmd->cmd->payload, CMD_REQ) == 0) {
        send_message(
            radio, GATEWAYID, CMD_PID_CONF,
            (byte*)current_state.pid_conf, sizeof(PIDConf)
        );
    } else {
        PIDConf pid_conf;
        memcpy(&pid_conf, cmd->cmd->payload, sizeof(pid_conf));

        if(pid_conf.control != PID_CONF_CONTROL) {
            Serial.println("Got pid conf with bad control");
            Serial.println(pid_conf.control);
            return;
        }
        update_pid_conf_from_obj(current_state.pid_conf, &pid_conf);
    }
}


#endif
#endif
