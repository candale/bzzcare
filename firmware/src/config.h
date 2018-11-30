#ifndef CONFIG_H
#define CONFIG_H

// #define ROLE_SLAVE
// #define ROLE_MASTER

// The id of the master
// #define GATEWAYID 1

// #define NODEID 1

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


// == EVERYTHING THAT IS BELOW YOU DON'T USUALLY TOUCH ==

#ifdef ROLE_MASTER
#ifdef ROLE_SLAVE
#error only master or slave, not both
#endif
#endif

// When this is a master, NODEID must always be 1
#ifdef ROLE_MASTER
    // this is for test only
    #define GATEWAYID 1
    #define NODEID 1

    #if NODEID != 1
        #error "When role is master, NODEID must be 1"
    #endif
#elif defined ROLE_SLAVE
    // this is for test only
    #define GATEWAYID 1
    #define NODEID 99
    // When this is a slave, must always be 1
    #if NODEID == 1
        #error "When role is slave, NODEID must not be 1"
    #endif

    #if GATEWAYID != 1
        #error "When role is slave, GATEWAYID must be 1"
    #endif
#else
    #error "You must define this as either ROLE_MASTER or ROLE_SLAVE"
#endif

#define NETWORKID 100

#endif
