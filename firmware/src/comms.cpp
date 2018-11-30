#ifndef COMMS_CPP
#define COMMS_CPP

#include "comms.h"


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


NodeCmd* get_message(RFM69* radio, bool reply_to_ack_request) {
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
    node_cmd->ack_requested = radio->ACKRequested();

    if (node_cmd->ack_requested && reply_to_ack_request)
    {
        radio->sendACK();
    }

    return node_cmd;
}


NodeCmd* get_message_without_ack(RFM69* radio) {
    return get_message(radio, false);
}


NodeCmd* get_message_with_ack(RFM69* radio) {
    return get_message(radio, true);
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


bool send_message(RFM69* radio, int target, byte cmd_code, byte* payload, byte size) {
    TransportCmd cmd;
    cmd.command = cmd_code;
    memset(cmd.payload, 0, PAYLOAD_SIZE);
    memcpy(cmd.payload, payload, size);

    if (radio->sendWithRetry(target, (const void*)(&cmd), sizeof(cmd), SEND_NUM_RETRIES)) {
        // Serial.println("Send with success");
        return true;
    }

    // Serial.println("Failure in sending.");
    return false;
}


bool send_message(RFM69* radio, int target, byte cmd_code, const char* payload) {
    return send_message(radio, target, cmd_code, (byte*)payload, strlen(payload));
}


bool send_message(RFM69* radio, int target, byte cmd_code, double payload) {
    char buff[8];
    zftoa(payload, buff);

    return send_message(radio, target, cmd_code, buff);
}


void print_command(NodeCmd* cmd) {
    Serial.print("rssi= ");
    Serial.print(cmd->rssi);

    Serial.print(" nodeId=");
    Serial.print(cmd->node_id);

    Serial.print(" rssi=");
    Serial.print(cmd->rssi);

    Serial.print(" command=");
    Serial.print((char)cmd->cmd->command);

    Serial.print(" payload=");
    Serial.println((char*)cmd->cmd->payload);
}


#endif
