#ifndef SLAVE_H
#define SLAVE_H

#include <RFM69.h>
#include "config.h"
#include "utils.h"

#define TRANSMITPERIOD 1000


void handle(RFM69* radio);

#endif
