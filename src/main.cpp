#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>

#include "utils.h"
#include "config.h"

RFM69* radio;

#ifdef ROLE_SLAVE
int lastPeriod = -1;
#endif

void setup() {
    Serial.begin(SERIAL_BAUD);
    radio = make_radio();
}

void loop() {

    #ifdef ROLE_MASTER
        Command* data = get_message(radio);
        if (data == 0) {
            return;
        }

        Serial.print(" nodeId=");
        Serial.print(data->node_id);
        Serial.print(" command=");
        Serial.print((char)data->command);
        Serial.print(" payload=");
        Serial.print(data->payload);
        Serial.println("");
        delete(data);
    #endif

    #ifdef ROLE_SLAVE
        Command* data = get_message(radio);
        if(data != 0) {
            delete(data);
        }

        int currPeriod = millis() / TRANSMITPERIOD;
        if (currPeriod != lastPeriod)
        {
            Command cmd;
             //fill in the struct with new values
            cmd.node_id = NODEID;
            cmd.command = (byte)'A';
            strcpy(cmd.payload, "cool payload");

            Serial.println("Sending ...");
            send_message(radio, GATEWAYID, &cmd);
            lastPeriod=currPeriod;
        }
    #endif

}
