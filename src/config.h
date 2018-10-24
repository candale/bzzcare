#ifndef CONFIG_H
#define CONFIG_H

#include <RFM69.h>

// The id of the master
#define GATEWAYID 1

#define NODEID 1

#define NETWORKID 100

#define PROMISCUOS_MODE false

//Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define FREQUENCY RF69_868MHZ

//has to be same 16 characters/bytes on all nodes, not more not less!
#define KEY "sampleEncryptKey"

#define LED 9

#define SERIAL_BAUD 115200

// # of ms to wait for an ack
#define ACK_TIME 30

//uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!
#define IS_RFM69HW_HCW

#define TRANSMITPERIOD 1000


typedef struct {
  int           nodeId; //store this nodeId
  unsigned long uptime; //uptime in ms
  float         temp;   //temperature maybe?
} Payload;


#endif
