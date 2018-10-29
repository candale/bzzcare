#ifndef UTILS_H
#define UTILS_H

#include "config.h"


RFM69* make_radio();
NodeCmd* get_message(RFM69* radio);
bool send_message(RFM69* radio, int target, byte cmd, const char* payload);

void del_cmd(TransportCmd* cmd);
void del_cmd(NodeCmd* cmd);

void print_command(NodeCmd* cmd);


#endif
