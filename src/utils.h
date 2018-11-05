#ifndef UTILS_H
#define UTILS_H

#include "config.h"


RFM69* make_radio();
NodeCmd* get_message(RFM69* radio);

bool send_message(RFM69* radio, int target, byte cmd, const char* payload);
bool send_message(RFM69* radio, int target, byte cmd_code, float payload);

void del_cmd(TransportCmd* cmd);
void del_cmd(NodeCmd* cmd);

void print_command(NodeCmd* cmd);


extern float FLOAT_ERR;
void zftoa(float value, char* buff);
void zatof(char* buff, float* value);

int read_from_serial(char* buff, int max_length);

#endif
