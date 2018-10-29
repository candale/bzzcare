#ifndef MASTER_CPP
#define MASTER_CPP

#include "master.h"


void master_handle(RFM69* radio) {
    NodeCmd* data = get_message(radio);
    if(data != 0) {
        print_command(data);
        del_cmd(data);
    }
}


#endif
