#ifndef COMMANDS_CPP
#define COMMANDS_CPP

#include "commands.h"

void cmd_reboot(NodeCmd* cmd, RFM69* radio);
void cmd_report_capabilities(NodeCmd* cmd, RFM69* radio);
void cmd_test_ack(NodeCmd* cmd, RFM69* radio);


const cmd_function SYS_CMD_MAP [] = {
    cmd_reboot,
    cmd_report_capabilities,
    cmd_test_ack,
    0
};


const cmd_function CMD_MAP [] = {
    cmd_pid_conf,
    cmd_setpoint,
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

/*
Break a semi-colon separated string into an array of char arrays.
We assume that a single command element cannot be more than 20 characters
Maybe we should be a bit more conservative than that.
*/
byte break_command(const char* command, char container[][20], int take_only) {
    int len = strlen(command) + 1;
    char command_cpy[len];
    strcpy(command_cpy, command);
    command_cpy[len - 1] = 0;

    byte num_tokens = 0;
    char* token = strtok(command_cpy, ";");
    while(token != NULL && num_tokens != take_only) {
        strncpy(container[num_tokens++], token, 10);
        token = strtok(NULL, ";");
    }

    return num_tokens;
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
    if(target < 2 || target > 255) {
        // Serial.println("ERROR: Second argument (target) must an int [2,255]");
        return;
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
