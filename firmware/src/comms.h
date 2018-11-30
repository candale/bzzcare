#ifndef COMMS_H
#define COMMS_H

#include <RFM69.h>
#include "config.h"
#include "utils.h"

/*
This is supposed to encapsulate the command, from 0 to 255, and a payload
that will be the string representation of whatever is sent.

This representation is used for transport.

Everybody has the responsibility to interpret the payload as necessary.
*/
typedef struct {
    // 1 byte
    byte command;
    byte payload[PAYLOAD_SIZE];
} TransportCmd;

/*
This encapsulates the a TransportCmd and adds additional information about
the node and strength of the signal
*/
typedef struct {
    TransportCmd* cmd;

    bool ack_requested;
    int node_id;
    int rssi;
} NodeCmd;

void del_cmd(TransportCmd* cmd);
void del_cmd(NodeCmd* cmd);
void print_command(NodeCmd* cmd);

RFM69* make_radio();

NodeCmd* get_message(RFM69* radio, bool reply_to_ack_request);
NodeCmd* get_message_without_ack(RFM69* radio);
NodeCmd* get_message_with_ack(RFM69* radio);

bool send_message(RFM69* radio, int target, byte cmd, const char* payload);
bool send_message(RFM69* radio, int target, byte cmd_code, double payload);
bool send_message(RFM69* radio, int target, byte cmd_code, byte* payload, byte size);

#ifdef ROLE_MASTER
bool report_err_to_gateway(RFM69* radio, const char* payload);
#endif


#endif
