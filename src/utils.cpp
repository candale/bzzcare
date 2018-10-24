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

Payload* get_message(RFM69* radio) {
    if(radio->receiveDone() == false) {
        return 0;
    }

    if (radio->DATALEN != sizeof(Payload)) {
        Serial.print("Invalid payload received, not matching Payload struct!");
        return 0;
    }

    Serial.print('[');Serial.print(radio->SENDERID, DEC);Serial.print("] ");
    for (byte i = 0; i < radio->DATALEN; i++) {
      Serial.print((char)radio->DATA[i]);
    }
    Serial.print("   [RX_RSSI:");Serial.print(radio->readRSSI());Serial.print("]");

    Payload* data = (Payload*)malloc(sizeof(Payload));
    memcpy(data, (const void*)radio->DATA, sizeof(Payload));

    if (radio->ACKRequested())
    {
        radio->sendACK();
        Serial.print(" - ACK sent.");
    }

    return data;
}


void send_message(RFM69* radio, int target, Payload* data) {
    if (radio->sendWithRetry(target, (const void*)(data), sizeof(*data))) {
        return true;
    }

    return false;
}

#endif
