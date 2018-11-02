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


const cmd_function* SYS_CMD_MAP [] = {
    &cmd_reboot,
    &cmd_report_capabilities,
    &cmd_test_ack,
    0
};


const cmd_function* CMD_MAP [] = {
    &cmd_pid_conf,
    &cmd_setpoint,
    0
};


void route_cmd(NodeCmd* cmd, RFM69* radio) {
    cmd_function** commands;
    if(cmd->cmd->command <= SYS_MAX_CMD) {
        commands = SYS_CMD_MAP;
    } else {
        commands = CMD_MAP;
    }

    cmd_function* func = 0;
    byte index = 0;
    do{
        func = commands[index];
        index += 1;
    } while (func != 0 && index != cmd->cmd->command);

    if(func == 0) {
        Serial.print("ERROR: Cannot find USER function with code: ");
        Serial.println(cmd->cmd->command, DEC);
        return;
    }

    func(cmd, radio);
}

#endif
