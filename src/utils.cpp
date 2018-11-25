#ifndef UTILS_CPP
#define UTILS_CPP


#include "utils.h"

double DOUBLE_ERR = 1111.11;

/*
Transform a double in it's string representation
If the value is 0, the string "zero" will be put in buff.
*/
void zftoa(double value, char* buff) {
    if(value == 0) {
        strcpy(buff, "zero");
    } else {
        dtostrf(value, 3, 2, buff);
    }
}


/*
Try and transform a string into a double number.
When the string "zero" is met, it is interpreted as the number 0.
This is so to not confuse the 0 return value of atof, which is an error.
*/
void zatof(char* buff, double* value) {
    if(strcmp(buff, "zero") == 0) {
        *value = 0;
        return;
    }

    double parsed_value = atof(buff);
    if(parsed_value == 0) {
        // Serial.println("ERROR: payload could not be interpreted");
        *value = DOUBLE_ERR;
        return;
    }

    *value = parsed_value;
}


int read_from_serial(char* buff, int max_length) {
    // TODO: figure out why this doesn't work
    int index = 0;
    if(Serial.available() > 0) {
        while(index < max_length - 1) {
            byte c = Serial.read();
            if(c == '\n') {
                break;
            }
            buff[index++] = c;
        }
        buff[index] = 0;
    }
    return index;
}


/*
Break a semi-colon separated string into an array of char arrays.
We assume that a single command element cannot be more than 20 characters
Maybe we should be a bit more conservative than that.
*/
byte break_command(const char* command, char container[][20], int take_only) {
    int len = strlen(command) + 1;
    char command_cpy[len];
    strcpy(command_cpy, command);
    command_cpy[len - 1] = 0;

    byte num_tokens = 0;
    char* token = strtok(command_cpy, ";");
    while(token != NULL && num_tokens != take_only) {
        strncpy(container[num_tokens++], token, 10);
        token = strtok(NULL, ";");
    }

    return num_tokens;
}

#endif
