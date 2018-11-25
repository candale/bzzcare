#ifdef ROLE_SLAVE
#ifndef SLAVE_CPP
#define SLAVE_CPP

#include "slave.h"


SlaveState current_state;
Adafruit_MCP9808 temp_sensor = Adafruit_MCP9808();
int LAST_PERIOD = -1;


void report_err(RFM69* radio, const char* message) {
    send_message(radio, GATEWAYID, CMD_REPORT_ERR, message);
}


void device_setup(RFM69* radio) {
    current_state.pid_conf = (PIDConf*)malloc(sizeof(PIDConf));
    init_pid_conf_default_or_saved(current_state.pid_conf);

    if (!temp_sensor.begin()) {
        Serial.println("Couldn't find MCP9808!");
        report_err(radio, "no temp sensor");
        while (1);
    }
}


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
