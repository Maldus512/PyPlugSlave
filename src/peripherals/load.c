#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define LOAD_PIN  GPIO15
#define LOAD_PORT GPIOA


void load_init(void) {
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(LOAD_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LOAD_PIN);
}

void load_on(void) {
    gpio_set(LOAD_PORT, LOAD_PIN);
}

void load_off(void) {
    gpio_clear(LOAD_PORT, LOAD_PIN);
}