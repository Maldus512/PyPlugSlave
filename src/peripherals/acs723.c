#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdint.h>

uint8_t channel_array[] = {1, 1, ADC_CHANNEL_TEMP};

void adc_setup(void) {
    rcc_periph_clock_enable(RCC_ADC1);
    rcc_periph_clock_enable(RCC_GPIOA);

    //gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);
    gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);

    adc_power_off(ADC1);

    ADC_CHSELR(ADC1) |= 0x2;

    // adc_set_clk_source(ADC1, ADC_CLKSOURCE_ADC);
    adc_calibrate(ADC1);
    // adc_set_operation_mode(ADC1, ADC_MODE_SCAN);
    adc_set_single_conversion_mode(ADC1);
    // adc_disable_external_trigger_regular(ADC1);
    adc_set_right_aligned(ADC1);

    ADC_SMPR1(ADC1) |= 6;

    //adc_set_sample_time_on_all_channels(ADC1, 6);
    //adc_set_regular_sequence(ADC1, 1, channel_array);
    adc_set_resolution(ADC1, 0b00); // 12 bit
    //adc_disable_analog_watchdog(ADC1);
    adc_power_on(ADC1);

    /* Wait for ADC starting up. */
    int i;
    for (i = 0; i < 800000; i++) { /* Wait a bit. */
        __asm__("nop");
    }
}

uint16_t read_acs723() {
    adc_start_conversion_regular(ADC1);
    while((ADC_ISR(ADC1) & ADC_ISR_EOC) == 0)
        __asm__("nop");
    return (uint16_t) adc_read_regular(ADC1);
}
