#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include <stdint.h>

void serial_init(void);
void serial_print(char *str);
int  serial_readline(uint8_t *buffer);

#endif