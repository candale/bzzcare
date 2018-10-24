#ifndef UTILS_H
#define UTILS_H

#include "config.h"



RFM69* make_radio();
Payload* get_message(RFM69* radio);
void send_message(RFM69* radio, int target, Payload* data);

#endif
