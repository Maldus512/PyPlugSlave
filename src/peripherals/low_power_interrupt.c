#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/syscfg.h>



void low_power_interrupt_init(void) {
    /* Enable GPIOA clock. */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_SYSCFG);

    int i;
    for (i = 0; i < 800000; i++) { /* Wait a bit. */
        __asm__("nop");
    }

    /* Enable EXTI0 interrupt. */
    nvic_enable_irq(NVIC_EXTI0_1_IRQ);
    nvic_enable_irq(NVIC_EXTI4_15_IRQ);

    /* Set GPIO0 (in GPIO port A) to 'input float'. */
    gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO8);
    gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);

    SYSCFG_EXTICR3 = 0;

    /* Configure the EXTI subsystem. */
    // exti_select_source(EXTI0, GPIOA);
    exti_select_source(EXTI0, GPIOB);
    exti_set_trigger(EXTI0, EXTI_TRIGGER_BOTH);
    exti_enable_request(EXTI0);

    //uint32_t x = SYSCFG_EXTICR1;
    //SYSCFG_EXTICR3 = 1;
    //uint32_t z = SYSCFG_EXTICR3;
    //uint32_t y = EXTI_IMR;
    exti_reset_request(EXTI0);
}

void exti0_1_isr(void) {
    exti_reset_request(EXTI0);
}

void exti4_15_isr(void) {
    exti_reset_request(EXTI4);
}