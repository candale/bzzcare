#ifndef SLAVE_H
#define SLAVE_H

#include <RFM69.h>
#include "config.h"
#include "utils.h"


#define TRANSMITPERIOD 1000


void slave_handle(RFM69* radio);

#endif
