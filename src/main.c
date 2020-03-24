
#include <stdio.h>
#include <string.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/systick.h>

#include "common/i2c_common.h"
#include "MCP9800/MCP9800.h"
#include "EEPROM/24LC16.h"
#include "generic/timer/timecheck.h"
#include "peripherals/system.h"
#include "peripherals/timekeeper.h"
#include "peripherals/acs723.h"
#include "peripherals/serial.h"
#include "peripherals/load.h"
#include "peripherals/signals.h"
#include "peripherals/i2c_peripherals.h"
#include "peripherals/low_power_interrupt.h"
#include "model/model.h"
#include "application/commands.h"

int main(void) {
    char          msg[128];
    unsigned long sumts = 0, timestamp = 0, readts = 0, cycletime;

    model_t model     = {0};
    model.calibration = DEFAULT_CALIBRATION;

    clock_init();
    systick_init();
    signals_init();
    i2c_init();
    serial_init();
    acs723_init();
    load_init();
    low_power_interrupt_init();

    timekeeper_init();

    load_off();

    while (1) {
        cycletime = get_millis();

        if (serial_readline((uint8_t *)msg) > 0)
            process_command(msg, &model) ? printf("ERR\n") : printf("OK\n");

        if (is_expired(readts, cycletime, 200UL)) {
            model.current = acs723_read_current(&model.calibration);
            readts        = cycletime;
        }

        if (is_expired(timestamp, cycletime, 1000UL)) {
            signal_toggle_out();
            timestamp = cycletime;
        }

        if (is_expired(sumts, cycletime, 60000UL)) {
            model.current_sum += model.current;
            sumts = cycletime;
        }

        __asm__("wfi");
    }

    return 0;
}
