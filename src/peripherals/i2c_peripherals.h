#ifndef I2C_PERIPHERALS_H_INCLUDED
#define I2C_PERIPHERALS_H_INCLUDED

#include <stdint.h>

void i2c_init(void);
void i2c_ack_polling(uint8_t devaddr);
int  i2c_custom_transfer(uint8_t devaddr, uint8_t *write, int wrlen, uint8_t *read, int rdlen);

#endif