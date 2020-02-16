#ifndef TIMEKEEPER_H_INCLUDED
#define TIMEKEEPER_H_INCLUDED

void          timekeeper_init();
void          timekeeper_update();
unsigned long get_millis();
void          delay_ms(unsigned long ms);

#endif