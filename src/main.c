
#include <stdio.h>
#include <string.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include "peripherals/timekeeper.h"
#include "peripherals/acs723.h"
#include "peripherals/usart.h"
#include <libopencm3/cm3/systick.h>

#define LOAD_PIN    GPIO15
#define LOAD_PORT   GPIOA
#define OUT_SIGNAL  GPIO1
#define IN_SIGNAL   GPIO0
#define SIGNAL_PORT GPIOB

static void gpio_setup(void) {
    /* Enable GPIO clock. */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);

    /* set pins to output mode, push pull */
    gpio_mode_setup(SIGNAL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, OUT_SIGNAL);
    gpio_mode_setup(SIGNAL_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, IN_SIGNAL);
    gpio_mode_setup(LOAD_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LOAD_PIN);
}


void clock_init(void) {
    // Internal 16mHz oscillator
    rcc_osc_on(RCC_HSI16);
    rcc_wait_for_osc_ready(RCC_HSI16);
    rcc_set_sysclk_source(RCC_HSI16);

    // these are defined globally by libopencm3
    rcc_ahb_frequency  = 16e6;
    rcc_apb1_frequency = 16e6;
    rcc_apb2_frequency = 16e6;
}


/* Set up a timer to create 1mS ticks. */
static void systick_setup(void) {
    /* clock rate / 1000 to get 1mS interrupt rate */
    systick_set_reload(16000);
    // systick_set_clocksource(RCC_HSI16);
    systick_clear();
    systick_counter_enable();
    /* this done last */
    systick_interrupt_enable();
}

int main(void) {
    char          msg[128];
    int           len;
    unsigned long timestamp = 0;
    uint16_t      cal       = 2048;

    clock_init();
    gpio_setup();
    usart_setup();
    adc_setup();
    systick_setup();

    timekeeper_init();

    gpio_clear(LOAD_PORT, LOAD_PIN);

    while (1) {
        if ((len = uart_readline((uint8_t *)msg)) > 0) {
            int res = 0;
            if (strcmp(msg, "ON\n") == 0) {
                gpio_set(LOAD_PORT, LOAD_PIN);
                res = 1;
            } else if (strcmp(msg, "OFF\n") == 0) {
                gpio_clear(LOAD_PORT, LOAD_PIN);
                res = 1;
            } else if (strcmp(msg, "READ\n") == 0) {
                double tmp = acs723_read_current(&cal);
                snprintf(msg, 128, "%4f (%i)\n", tmp, cal);
                send_string(msg);
                res = 1;
            }

            if (res == 1)
                send_string("OK\n");
            else if (res < 0)
                send_string("ERR\n");
        }

        if (get_millis() > timestamp + 1000UL) {
            gpio_toggle(SIGNAL_PORT, OUT_SIGNAL);

            // sprintf(msg, "log [%lu]: %d\n", get_millis(), read_acs723());
            // send_string(msg);
            timestamp = get_millis();
        }

        __asm__("wfi");
    }

    return 0;
}
