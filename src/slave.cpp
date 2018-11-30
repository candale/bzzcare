#ifdef ROLE_SLAVE
#ifndef SLAVE_CPP
#define SLAVE_CPP

#include "slave.h"


SlaveState current_state;
Adafruit_MCP9808 temp_sensor = Adafruit_MCP9808();
PID* pid_controller;

unsigned int pid_window_start;

// TODO: have a separate "service" that handles the period tasks
// TODO: make sure you deal with overflow for the variable read_temp_last
//       with the unsigned long we can go for almost 50 days
unsigned long read_temp_last;
unsigned long report_pid_conf_last;


void report_err(RFM69* radio, const char* message) {
    send_message(radio, GATEWAYID, CMD_REPORT_ERR, message);
}


void report_pid_conf(RFM69* radio) {
    send_message(
        radio, GATEWAYID, CMD_PID_CONF,
        (byte*)current_state.pid_conf, sizeof(PIDConf)
    );
}


void relay_on() {
    if(current_state.pid_conf->relay_state == 1) {
        return;
    }

    Serial.println("relay on");
    digitalWrite(RELAY_PIN, HIGH);
    current_state.pid_conf->relay_state = 1;
}


void relay_off() {
    if(current_state.pid_conf->relay_state == 0) {
        return;
    }

    Serial.println("relay off");
    digitalWrite(RELAY_PIN, LOW);
    current_state.pid_conf->relay_state = 0;
}


void device_setup(RFM69* radio) {
    Serial.print("SETUP SLAVE. NODEID: "); Serial.println(NODEID);
    current_state.pid_conf = (PIDConf*)malloc(sizeof(PIDConf));
    init_pid_conf_default_or_saved(current_state.pid_conf);

    if (!temp_sensor.begin()) {
        Serial.println("Couldn't find MCP9808!");
        report_err(radio, "no temp sensor");
        while (1);
    }

    pid_controller = new PID(
        &current_state.pid_conf->current_temp,
        &current_state.pid_conf->output,
        &current_state.pid_conf->setpoint,
        current_state.pid_conf->kp,
        current_state.pid_conf->ki,
        current_state.pid_conf->kd,
        DIRECT
    );
    pid_controller->SetTunings(
        current_state.pid_conf->kp,
        current_state.pid_conf->ki,
        current_state.pid_conf->kd
    );

    //tell the PID to range between 0 and the full window size
    pid_controller->SetOutputLimits(0, current_state.pid_conf->window_size);
    pid_controller->SetMode(AUTOMATIC);

    pid_window_start = millis();
    read_temp_last = millis();
    report_pid_conf_last = millis();
}


void handle_pid() {
    if(pid_controller->Compute()) {
        // Serial.println(current_state.pid_conf->output);
    }
    if(millis() - pid_window_start > current_state.pid_conf->window_size) {
        pid_window_start += current_state.pid_conf->window_size;
    }

    if(current_state.pid_conf->output < millis() - pid_window_start) {
        relay_on();
    } else {
        relay_off();
    }
}


void handle_temp_sensor() {
    if(read_temp_last + READ_TEMP_PERIOD <= millis()) {
        current_state.pid_conf->current_temp = temp_sensor.readTempC();
        read_temp_last = millis();
    }
}


void handle_pid_conf_report(RFM69* radio) {
    if(report_pid_conf_last + REPORT_PID_CONF_PERIOD <= millis()) {
        report_pid_conf(radio);
        report_pid_conf_last = millis();
    }
}


void do_specific_work(RFM69* radio, NodeCmd* data) {
    handle_temp_sensor();
    handle_pid();
    handle_pid_conf_report(radio);
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
        Serial.println("Reporting PID state to master. ");
        report_pid_conf(radio);
    } else {
        PIDConf pid_conf;
        memcpy(&pid_conf, cmd->cmd->payload, sizeof(pid_conf));

        if(pid_conf.control != PID_CONF_CONTROL) {
            Serial.println("Got pid conf with bad control");
            Serial.println(pid_conf.control);
            return;
        }
        update_pid_conf_from_obj(current_state.pid_conf, &pid_conf);
        pid_controller->SetTunings(
            current_state.pid_conf->kp,
            current_state.pid_conf->ki,
            current_state.pid_conf->kd
        );
    }
}


#endif
#endif
