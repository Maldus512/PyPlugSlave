
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include "peripherals/timekeeper.h"
#include "peripherals/acs723.h"

#define LOAD_PIN GPIO1
#define LOAD_PORT GPIOB
#define OUT_SIGNAL GPIO15
#define OUT_SIGNAL_PORT GPIOA

static void gpio_setup(void) {
    /* Enable GPIO clock. */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);

    /* set pins to output mode, push pull */
    gpio_mode_setup(OUT_SIGNAL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, OUT_SIGNAL);
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

static void usart_setup(void) {
    /* Setup GPIO pin GPIO_USART2_TX. */
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
    gpio_set_af(GPIOA, GPIO_AF4, GPIO2);

    rcc_periph_clock_enable(RCC_USART2);
    rcc_set_usart2_sel(1);

    /* Setup UART parameters. */
    usart_set_baudrate(USART2, 115200);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    /* Finally enable the USART. */
    usart_enable(USART2);
}

void send_string(char *str) {
    while (*str != 0) {
        usart_send_blocking(USART2, *(str++));
    }
}

int main(void) {
    char msg[128];
    int  c = 0;

    clock_init();
    gpio_setup();
    usart_setup();
    adc_setup();

    timekeeper_init();

    while (1) {
        /* toggle each led in turn */
        gpio_toggle(LOAD_PORT, LOAD_PIN);
        gpio_toggle(OUT_SIGNAL_PORT, OUT_SIGNAL);

        delay_ms(1000);

        c = (c + 1) % 10;

        sprintf(msg, "log [%lu]: %i - %d\n", get_millis(), c, read_acs723());
        send_string(msg);
    }

    return 0;
}
