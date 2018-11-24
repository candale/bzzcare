#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>

#include "utils.h"
#include "config.h"

// TODO: link slave/master dynamically and have both function named "handle"

#include "commands.h"
#include "work.h"

RFM69* radio;


void setup() {
    Serial.begin(SERIAL_BAUD);
    #ifdef ROLE_MASTER
        Serial.println("ME IS MASTER");
    #elif defined ROLE_SLAVE
        Serial.println("ME IS SLAVE");
    #endif
    radio = make_radio();
    device_setup(radio);
}

void loop() {
    handle(radio);
}
