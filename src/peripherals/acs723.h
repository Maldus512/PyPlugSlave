#ifndef ACS723_H_INCLUDED
#define ACS723_H_INCLUDED

#include <stdint.h>

void     acs723_init(void);
double   acs723_read_current(uint16_t *calibration);

#endif
