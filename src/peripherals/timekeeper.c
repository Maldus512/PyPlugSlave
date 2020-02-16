#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include "generic/timer/timecheck.h"

static unsigned long millis = 0;

void timekeeper_init() {
    millis = 0;
    rcc_periph_clock_enable(RCC_TIM2);
    timer_disable_counter(TIM2);
    timer_set_counter(TIM2, 0);
    timer_continuous_mode(TIM2);
    timer_direction_up(TIM2);
    timer_set_prescaler(TIM2, rcc_ahb_frequency / 1000);
    timer_enable_counter(TIM2);
    timer_clear_flag(TIM2, TIM_SR_UIF);
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
        ;
}