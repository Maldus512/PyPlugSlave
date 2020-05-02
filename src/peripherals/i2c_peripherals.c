#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>

#include "timekeeper.h"
#include "generic/timer/timecheck.h"

#define WAIT4ACK 2

//TODO: libopencm3 code is very confused, I need to refactor this (also using the RELOAD register)
int i2c_write_session(uint8_t devaddr, uint8_t *write, int wrlen) {
    i2c_set_7bit_address(I2C1, devaddr >> 1);
    i2c_set_write_transfer_dir(I2C1);
    i2c_set_bytes_to_transfer(I2C1, wrlen);
    i2c_send_start(I2C1);

    while (wrlen--) {
        bool          wait  = true;
        unsigned long start = get_millis();
        while (wait) {
            if (i2c_transmit_int_status(I2C1)) {
                wait = false;
            }
            while (i2c_nack(I2C1) && !is_expired(start, get_millis(), WAIT4ACK))
                ; /* FIXME Some error */

            if (is_expired(start, get_millis(), WAIT4ACK)) {
                i2c_disable_autoend(I2C1);
                i2c_send_stop(I2C1);
                i2c_reset(I2C1);
                return -3;
            }
        }
        i2c_send_data(I2C1, *write++);
    }
    return 0;
}

int i2c_read_session(uint8_t devaddr, uint8_t *read, int rdlen, int autoend) {
    /* Setting transfer properties */
    i2c_set_7bit_address(I2C1, devaddr >> 1);
    i2c_set_read_transfer_dir(I2C1);
    i2c_set_bytes_to_transfer(I2C1, rdlen);
    /* start transfer */
    i2c_send_start(I2C1);
    /* important to do it afterwards to do a proper repeated start! */
    if (autoend)
        i2c_enable_autoend(I2C1);

    for (int i = 0; i < rdlen; i++) {
        unsigned long start = get_millis();
        while (i2c_received_data(I2C1) == 0 && !is_expired(start, get_millis(), WAIT4ACK))
            ;

        if (is_expired(start, get_millis(), WAIT4ACK)) {
            i2c_disable_autoend(I2C1);
            i2c_send_stop(I2C1);
            i2c_reset(I2C1);
            return -4;
        }

        read[i] = i2c_get_data(I2C1);
    }
    return 0;
}

// FIXME: quando provo a scrivere Piu' di 0x3AE (942) byte non funziona, e non capisco perche'
int i2c_custom_transfer(uint8_t devaddr, uint8_t *write, int wrlen, uint8_t *read, int rdlen) {
    int res;
    /*  waiting for busy is unnecessary. read the RM */
    if (wrlen > 0) {

        if (rdlen) {
            i2c_disable_autoend(I2C1);
        } else {
            i2c_enable_autoend(I2C1);
        }

        if ((res = i2c_write_session(devaddr, write, wrlen)))
            return res;

        /* not entirely sure this is really necessary.
         * RM implies it will stall until it can write out the later bits
         */
        if (rdlen) {
            while (!i2c_transfer_complete(I2C1))
                ;
        }
    }

    while (rdlen > 0xFF) {     // STM32 I2C peripheral is limited to 255 bytes per transaction
        if ((res = i2c_read_session(devaddr, read, 0xFF, 0)))
            return res;

        read += 0xFF;
        rdlen -= 0xFF;
    }
    if (rdlen > 0) {
        if ((res = i2c_read_session(devaddr, read, rdlen, 1)))
            return res;
    }

    return 0;
}

void i2c_ack_polling(uint8_t devaddr) {
    (void)devaddr;
    // delay_us(500); Cosi' non funziona
    delay_ms(3);     // Sto largo
}


void i2c_init(void) {
    rcc_periph_clock_enable(RCC_I2C1);
    rcc_periph_clock_enable(RCC_GPIOB);
    // rcc_set_i2c_clock_hsi(I2C1);

    i2c_reset(I2C1);
    /* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7);
    gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_HIGH, GPIO6 | GPIO7);
    gpio_set_af(GPIOB, GPIO_AF1, GPIO6 | GPIO7);
    i2c_peripheral_disable(I2C1);
    // configure ANFOFF DNF[3:0] in CR1
    // i2c_enable_analog_filter(I2C1);
    // i2c_set_digital_filter(I2C1, 0);
    /* HSI is at 8Mhz */
    rcc_periph_reset_pulse(RST_I2C1);
    i2c_set_speed(I2C1, i2c_speed_sm_100k, rcc_apb1_frequency / 1e6);
    // configure No-Stretch CR1 (only relevant in slave mode)
    // i2c_enable_stretching(I2C1);
    // addressing mode
    i2c_set_7bit_addr_mode(I2C1);

    RCC_CCIPR |= RCC_CCIPR_I2C1SEL_APB;

    i2c_peripheral_enable(I2C1);
}
