#ifdef ROLE_MASTER
#ifndef MASTER_CPP
#define MASTER_CPP

#include "master.h"


void handle(RFM69* radio) {
    NodeCmd* data = get_message(radio);
    if(data != 0) {
        print_command(data);
        del_cmd(data);
    }
}


void cmd_pid_conf(NodeCmd* cmd) {

}

void cmd_temp(NodeCmd* cmd) {

}


#endif
#endif
