#ifdef ROLE_MASTER
#ifndef MASTER_H
#define MASTER_H

#include <RFM69.h>
#include "config.h"
#include "utils.h"

void handle(RFM69* radio);
void cmd_pid_conf(NodeCmd* cmd);
void cmd_temp(NodeCmd* cmd);

#endif
#endif
