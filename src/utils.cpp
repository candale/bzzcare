#ifndef UTILS_CPP
#define UTILS_CPP


#include "utils.h"
#include "common.h"

double DOUBLE_ERR = 1111.11;


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



/*
Transform a double in it's string representation
If the value is 0, the string "zero" will be put in buff.
*/
void zftoa(double value, char* buff) {
    if(value == 0) {
        strcpy(buff, "zero");
    } else {
        dtostrf(value, 3, 2, buff);
    }
}


/*
Try and transform a string into a double number.
When the string "zero" is met, it is interpreted as the number 0.
This is so to not confuse the 0 return value of atof, which is an error.
*/
void zatof(char* buff, double* value) {
    if(strcmp(buff, "zero") == 0) {
        *value = 0;
        return;
    }

    double parsed_value = atof(buff);
    if(parsed_value == 0) {
        // Serial.println("ERROR: payload could not be interpreted");
        *value = DOUBLE_ERR;
        return;
    }

    *value = parsed_value;
}


int read_from_serial(char* buff, int max_length) {
    // TODO: figure out why this doesn't work
    int index = 0;
    if(Serial.available() > 0) {
        while(index < max_length - 1) {
            byte c = Serial.read();
            if(c == '\n') {
                break;
            }
            buff[index++] = c;
        }
        buff[index] = 0;
    }
    return index;
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


/*
Break a semi-colon separated string into an array of char arrays.
We assume that a single command element cannot be more than 20 characters
Maybe we should be a bit more conservative than that.
*/
byte break_command(const char* command, char container[][20], int take_only) {
    int len = strlen(command) + 1;
    char command_cpy[len];
    strcpy(command_cpy, command);
    command_cpy[len - 1] = 0;

    byte num_tokens = 0;
    char* token = strtok(command_cpy, ";");
    while(token != NULL && num_tokens != take_only) {
        strncpy(container[num_tokens++], token, 10);
        token = strtok(NULL, ";");
    }

    return num_tokens;
}

#endif
