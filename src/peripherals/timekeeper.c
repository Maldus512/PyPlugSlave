#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dbgmcu.h>
#include "generic/timer/timecheck.h"

static unsigned long millis = 0;

void delay_setup(void) {
    /* set up a microsecond free running timer for ... things... */
    rcc_periph_clock_enable(RCC_TIM21);
    /* microsecond counter */
    timer_set_prescaler(TIM21, rcc_apb1_frequency / 1000000 - 1);
    timer_set_period(TIM21, 0xffff);
    timer_one_shot_mode(TIM21);
}

void timekeeper_init() {
    millis = 0;
    rcc_periph_clock_enable(RCC_TIM2);
    rcc_periph_clock_enable(RCC_DBG);
    timer_disable_counter(TIM2);
    MMIO32(0x40015808) |= 1;     // Stopping timer2 during debug

    timer_set_counter(TIM2, 0);
    timer_continuous_mode(TIM2);
    timer_direction_up(TIM2);
    timer_set_prescaler(TIM2, rcc_ahb_frequency / 1000 - 1);
    timer_enable_counter(TIM2);

    while (!timer_get_flag(TIM2, TIM_SR_UIF))     // One empty round (not sure why it's needed)
        ;

    timer_clear_flag(TIM2, TIM_SR_UIF);

    delay_setup();
}

void timekeeper_update() {
    if (timer_get_flag(TIM2, TIM_SR_UIF)) {
        millis += 0xFFFF;
        timer_clear_flag(TIM2, TIM_SR_UIF);
    }
}

unsigned long get_millis() {
    timekeeper_update();
    return millis + timer_get_counter(TIM2);
}

void delay_ms(unsigned long ms) {
    unsigned long start = get_millis();
    while (!is_expired(start, get_millis(), ms - 1))
        __asm__("wfi");
}

void delay_us(unsigned int us) {
    TIM_ARR(TIM21) = us;
    TIM_EGR(TIM21) = TIM_EGR_UG;
    TIM_CR1(TIM21) |= TIM_CR1_CEN;
    timer_enable_counter(TIM21);
    while (TIM_CR1(TIM21) & TIM_CR1_CEN)
        __asm__("nop");
}
