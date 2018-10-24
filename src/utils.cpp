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

Command* get_message(RFM69* radio) {
    if(radio->receiveDone() == false) {
        return 0;
    }

    // TODO: add both ways ACK checks here as in examples

    if (radio->DATALEN != sizeof(Command)) {
        Serial.print("Invalid Command received, not matching Command struct!");
        return 0;
    }

    Serial.print('[');Serial.print(radio->SENDERID, DEC);Serial.print("] ");
    Serial.print("   [RX_RSSI:");Serial.print(radio->readRSSI());Serial.print("]");

    Command* data = (Command*)malloc(sizeof(Command));
    memcpy(data, (const void*)radio->DATA, sizeof(Command));

    if (radio->ACKRequested())
    {
        radio->sendACK();
        Serial.print(" - ACK sent.");
    }

    return data;
}


void send_message(RFM69* radio, int target, Command* data) {
    if (radio->sendWithRetry(target, (const void*)(data), sizeof(*data))) {
        return true;
    }

    return false;
}

#endif
