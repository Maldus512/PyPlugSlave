
#include <stdio.h>
#include <string.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/systick.h>

#include "generic/timer/timecheck.h"

#include "peripherals/system.h"
#include "peripherals/timekeeper.h"
#include "peripherals/acs723.h"
#include "peripherals/serial.h"
#include "peripherals/load.h"

#include "model/model.h"

#include "application/commands.h"

#define OUT_SIGNAL  GPIO1
#define IN_SIGNAL   GPIO0
#define SIGNAL_PORT GPIOB

static void gpio_setup(void) {
    /* Enable GPIO clock. */
    rcc_periph_clock_enable(RCC_GPIOB);

    /* set pins to output mode, push pull */
    gpio_mode_setup(SIGNAL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, OUT_SIGNAL);
    gpio_mode_setup(SIGNAL_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, IN_SIGNAL);
}

int main(void) {
    char          msg[128];
    unsigned long timestamp = 0, readts = 0, cycletime;

    model_t model     = {0};
    model.calibration = DEFAULT_CALIBRATION;

    clock_init();
    systick_init();
    gpio_setup();

    serial_init();
    acs723_init();
    load_init();

    timekeeper_init();

    load_off();

    while (1) {
        cycletime = get_millis();

        if (serial_readline((uint8_t *)msg) > 0)
            process_command(msg, &model) ? serial_print("ERR\n") : serial_print("OK\n");

        if (is_expired(readts, cycletime, 200UL)) {
            model.current = acs723_read_current(&model.calibration);
            model.current_sum += model.current;
        }

        if (is_expired(timestamp, cycletime, 1000UL)) {
            gpio_toggle(SIGNAL_PORT, OUT_SIGNAL);
            timestamp = cycletime;
        }

        __asm__("wfi");
    }

    return 0;
}
