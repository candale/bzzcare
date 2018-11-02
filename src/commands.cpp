#ifndef COMMANDS_CPP
#define COMMANDS_CPP


#include "commands.h"


void cmd_reboot(NodeCmd* cmd, RFM69* radio) {
    Serial.println("Rebooting device ...");
}


void cmd_report_capabilities(NodeCmd* cmd, RFM69* radio) {
    Serial.println("Reporting capabilities ...");
}


void cmd_test_ack(NodeCmd* cmd, RFM69* radio) {
    Serial.println("Responding to ACK request ...");
}


const sys_cmd_function* SYS_CMD_MAP [] = {
    &cmd_reboot,
    &cmd_report_capabilities,
    &cmd_test_ack,
    0
};


const cmd_function* CMD_MAP [] = {
    &cmd_pid_conf,
    &cmd_temp,
    0
};


void route_cmd(NodeCmd* cmd, RFM69* radio) {
    if(cmd->cmd->command <= SYS_MAX_CMD) {
        byte index = 0;
        sys_cmd_function* func = 0;
        do {
            func = SYS_CMD_MAP[index];
            index += 1;
        } while(func != 0 && index != cmd->cmd->command);

        if(func == 0) {
            Serial.print("ERROR: Cannot find SYS function with code: ");
            Serial.println(cmd->cmd->command, DEC);
            return;
        }
        func(cmd, radio);
    }

    cmd_function* func = 0;
    byte index = 0;
    do{
        func = CMD_MAP[index];
        index += 1;
    } while (func != 0 && index != cmd->cmd->command);

    if(func == 0) {
        Serial.print("ERROR: Cannot find USER function with code: ");
        Serial.println(cmd->cmd->command, DEC);
        return;
    }

    func(cmd);
}

#endif
