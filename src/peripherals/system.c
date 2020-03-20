#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>


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
void systick_init(void) {
    /* clock rate / 1000 to get 1mS interrupt rate */
    systick_set_reload(16000);
    // systick_set_clocksource(RCC_HSI16);
    systick_clear();
    systick_counter_enable();
    /* this done last */
    systick_interrupt_enable();
}
