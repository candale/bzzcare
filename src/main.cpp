#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>

#include "utils.h"
#include "config.h"

#include "slave.h"
#include "master.h"

RFM69* radio;


void setup() {
    Serial.begin(SERIAL_BAUD);
    radio = make_radio();
}

void loop() {
    #ifdef ROLE_MASTER
        master_handle(radio);
    #endif

    #ifdef ROLE_SLAVE
        slave_handle(radio);
    #endif

}
