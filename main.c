
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define LOAD_PIN  GPIO1
#define LOAD_PORT GPIOB
#define OUT_SIGNAL    GPIO15
#define OUT_SIGNAL_PORT   GPIOA

static void gpio_setup(void) {
    /* Enable GPIO clock. */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);

    /* set pins to output mode, push pull */
    gpio_mode_setup(OUT_SIGNAL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, OUT_SIGNAL);
    gpio_mode_setup(LOAD_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LOAD_PIN);
}

int main(void) {
    int i;

    gpio_setup();

    while (1) {
        /* toggle each led in turn */
        gpio_toggle(LOAD_PORT, LOAD_PIN);
        gpio_toggle(OUT_SIGNAL_PORT, OUT_SIGNAL);
        for (i = 0; i < 100000; i++) { /* Wait a bit. */
            __asm__("nop");
        }
    }

    return 0;
}
