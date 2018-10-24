#ifndef UTILS_H
#define UTILS_H

#include "config.h"



RFM69* make_radio();
Command* get_message(RFM69* radio);
void send_message(RFM69* radio, int target, Command* data);

#endif
