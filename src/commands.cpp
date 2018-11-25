#ifndef COMMANDS_CPP
#define COMMANDS_CPP

#include "commands.h"

void cmd_reboot(NodeCmd* cmd, RFM69* radio);
void cmd_report_capabilities(NodeCmd* cmd, RFM69* radio);
void cmd_test_ack(NodeCmd* cmd, RFM69* radio);

/*
We define this command only for the master since only slaves send errors tO
the gateway; there's not obvious reason why it should happen vice-versa.


*/
#ifdef ROLE_MASTER
    void cmd_report_err(NodeCmd* cmd, RFM69* radio);
#endif


const cmd_function SYS_CMD_MAP [] = {
    cmd_reboot,
    cmd_report_capabilities,
    cmd_test_ack,
    #ifdef ROLE_MASTER
        cmd_report_err,
    #endif
    0
};


const cmd_function CMD_MAP [] = {
    cmd_pid_conf,
    0
};

char CMD_REQ [] = "req";


void route_cmd(NodeCmd* cmd, RFM69* radio) {
    const cmd_function* commands;
    // Serial.print("Routing command: "); Serial.println(cmd->cmd->command);
    if(cmd->cmd->command <= SYS_MAX_CMD) {
        // Serial.println("Choosing sys CMD map");
        commands = SYS_CMD_MAP;
    } else {
        // Serial.println("Choosing user CMD map");
        commands = CMD_MAP;
    }

    cmd_function func = 0;
    byte index = 0;
    do{
        func = commands[index];
        index += 1;
    } while (func != 0 && index - 1 != cmd->cmd->command - USER_CMD_START);

    if(func == 0) {
        // Serial.print("ERROR: Cannot find USER function with code: ");
        Serial.println(cmd->cmd->command, DEC);
        return;
    }

    // Serial.print("Calling found function for cmd "); Serial.println(cmd->cmd->command);
    func(cmd, radio);
}


serial_function get_serial_cmd(const void** cmd_map, char* command) {
    char command_id_[1][20];
    char* command_id = 0;
    if(break_command(command, command_id_, 1) != 1) {
        // Serial.println("ERROR: Expected one command at the beginning but none was found.");
    }

    command_id = (char*)command_id_[0];

    serial_function func = 0;
    byte index = 0;
    while(cmd_map[index] != 0) {
        if(strcmp((char*)cmd_map[index], command_id) == 0) {
            func = (serial_function)cmd_map[index + 1];
            // Serial.println("Found function to execute.");
            break;
        }
        index += 2;
    }

    if(func == 0) {
        // Serial.println("Could not find function.");
        return 0;
    }

    return func;
}

byte parse_target(const char* target_str) {
    int target = atoi(target_str);
    if(target > 255 || target == NODEID) {
        // Serial.println("Target must be a value in interval [1,255] but not the self nodeid");
        return 0;
    }

    return (byte)target;
}


void cmd_reboot(NodeCmd* cmd, RFM69* radio) {
    // Serial.println("Rebooting device ...");
}


void cmd_report_capabilities(NodeCmd* cmd, RFM69* radio) {
    // Serial.println("Reporting capabilities ...");
}


void cmd_test_ack(NodeCmd* cmd, RFM69* radio) {
    // Serial.println("Responding to ACK request ...");
}


#endif
