#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define OUT_SIGNAL  GPIO1
#define IN_SIGNAL   GPIO0
#define SIGNAL_PORT GPIOB

void signals_init(void) {
    /* Enable GPIO clock. */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);

    /* set pins to output mode, push pull */
    gpio_mode_setup(SIGNAL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, OUT_SIGNAL);
    gpio_mode_setup(SIGNAL_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, IN_SIGNAL);
}

void signal_toggle_out(void) {
    gpio_toggle(SIGNAL_PORT, OUT_SIGNAL);
}