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
    radio = make_radio();
}

void loop() {
    handle(radio);
}
