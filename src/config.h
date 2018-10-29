#ifndef CONFIG_H
#define CONFIG_H

#include <RFM69.h>

// #define ROLE_SLAVE
#define ROLE_MASTER

// The id of the master
#define GATEWAYID 1

#define NODEID 1

#define PROMISCUOS_MODE false

#define FREQUENCY RF69_868MHZ

//has to be same 16 characters/bytes on all nodes, not more not less!
#define KEY "sampleEncryptKey"

#define SERIAL_BAUD 115200

// # of ms to wait for an ack
#define ACK_TIME 30

//uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!
#define IS_RFM69HW_HCW

#define PAYLOAD_SIZE 44

#define SEND_NUM_RETRIES 4

/*
This is supposed to encapsulate the command, from 0 to 255, and a payload
that will be the string representation of whatever is sent.

This representation is used for transport.

Everybody has the responsibility to interpret the payload as necessary.
*/
typedef struct {
    // 1 byte
    byte command;
    // 44 bytes for payload
    char payload[PAYLOAD_SIZE];
} TransportCmd;

/*
This encapsulates the a TransportCmd and adds additional information about
the node and strength of the signal
*/
typedef struct {
    TransportCmd* cmd;
    int node_id;
    int rssi;
} NodeCmd;


// == EVERYTHING THAT IS BELOW YOU DON'T USUALLY TOUCH ==

#ifdef ROLE_MASTER
#ifdef ROLE_SLAVE
#error only master or slave, not both
#endif
#endif

// When this is a master, NODEID must always be 1
#ifdef ROLE_MASTER

    #define NODEID 1

#endif

// When this is a slave, must always be 1
#ifdef ROLE_SLAVE

    #define NODEID 99
    #define GATEWAYID 1

#endif

#define NETWORKID 100

#endif
