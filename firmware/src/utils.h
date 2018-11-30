#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include "config.h"

byte break_command(const char* command, char container[][20], int take_only);

extern double DOUBLE_ERR;
void zftoa(double value, char* buff);
void zatof(char* buff, double* value);

int read_from_serial(char* buff, int max_length);

#endif
