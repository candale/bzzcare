#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>

#include "utils.h"
#include "config.h"


#define ROLE_MASTER

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
        Payload* data = get_message(radio);
        if (data == 0) {
            return;
        }

        Serial.print(" nodeId=");
        Serial.print(data->nodeId);
        Serial.print(" uptime=");
        Serial.print(data->uptime);
        Serial.print(" temp=");
        Serial.print(data->temp);
        Serial.println("");
    #endif

    #ifdef ROLE_SLAVE
        get_message(radio);
        int currPeriod = millis() / TRANSMITPERIOD;
        if (currPeriod != lastPeriod)
        {
            Payload theData;
             //fill in the struct with new values
            theData.nodeId = NODEID;
            theData.uptime = millis();
            theData.temp = 91.23; //it's hot!

            Serial.println("Sending ...");
            send_message(radio, GATEWAYID, &theData);
            lastPeriod=currPeriod;
        }
    #endif

}
