#ifndef UTILS_CPP
#define UTILS_CPP


#include "utils.h"


RFM69* make_radio() {
    RFM69 * radio = new RFM69();
    radio->initialize(FREQUENCY, NODEID, NETWORKID);

    #ifdef IS_RFM69HW_HCW
      radio->setHighPower(); //must include this only for RFM69HW/HCW!
    #endif

    radio->encrypt(KEY);
    radio->promiscuous(PROMISCUOS_MODE);

    char buff[50];

    sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(buff);

    return radio;
}

NodeCmd* get_message(RFM69* radio) {
    if(radio->receiveDone() == false) {
        return 0;
    }

    // TODO: add both ways ACK checks here as in examples

    if (radio->DATALEN != sizeof(TransportCmd)) {
        Serial.print("Invalid Command received, not matching TransportCmd struct!");
        return 0;
    }

    TransportCmd* data = (TransportCmd*)malloc(sizeof(TransportCmd));
    NodeCmd* node_cmd = (NodeCmd*)malloc(sizeof(NodeCmd));
    memcpy(data, (const void*)radio->DATA, sizeof(TransportCmd));

    node_cmd->cmd = data;
    node_cmd->node_id = radio->SENDERID;
    node_cmd->rssi = radio->readRSSI();

    if (radio->ACKRequested())
    {
        radio->sendACK();
        Serial.print(" - ACK sent.");
    }

    return node_cmd;
}

void del_cmd(TransportCmd* cmd) {
    if(cmd != 0) {
        delete(cmd);
    }
}

void del_cmd(NodeCmd* cmd) {
    if(cmd == 0) {
        return;
    }

    if(cmd->cmd != 0) {
        delete(cmd->cmd);
    }

    delete(cmd);
}


bool send_message(RFM69* radio, int target, byte cmd_code, const char* payload) {
    TransportCmd cmd;
    cmd.command = cmd_code;
    strcpy(cmd.payload, payload);

    if (radio->sendWithRetry(target, (const void*)(&cmd), sizeof(cmd)), SEND_NUM_RETRIES) {
        return true;
    }

    return false;
}


void print_command(NodeCmd* cmd) {
    Serial.print(" nodeId=");
    Serial.print(cmd->node_id);

    Serial.print(" rssi=");
    Serial.print(cmd->rssi);

    Serial.print(" command=");
    Serial.print((char)cmd->cmd->command);

    Serial.print(" payload=");
    Serial.println(cmd->cmd->payload);
}

#endif
