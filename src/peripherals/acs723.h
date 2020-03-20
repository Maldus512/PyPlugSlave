#ifndef ACS723_H_INCLUDED
#define ACS723_H_INCLUDED

#include <stdint.h>

void     adc_setup(void);
uint16_t acs723_read();

double   acs723_read_current(uint16_t *calibration);

#endif
