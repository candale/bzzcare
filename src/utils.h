#ifndef UTILS_H
#define UTILS_H

#include "config.h"


RFM69* make_radio();

NodeCmd* get_message(RFM69* radio, bool reply_to_ack_request);
NodeCmd* get_message_without_ack(RFM69* radio);
NodeCmd* get_message_with_ack(RFM69* radio);

bool send_message(RFM69* radio, int target, byte cmd, const char* payload);
bool send_message(RFM69* radio, int target, byte cmd_code, double payload);
bool send_message(RFM69* radio, int target, byte cmd_code, byte* payload, byte size);

void del_cmd(TransportCmd* cmd);
void del_cmd(NodeCmd* cmd);

void print_command(NodeCmd* cmd);
byte break_command(const char* command, char container[][20], int take_only);

extern double DOUBLE_ERR;
void zftoa(double value, char* buff);
void zatof(char* buff, double* value);

int read_from_serial(char* buff, int max_length);

#endif
