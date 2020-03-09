#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

void usart_setup(void) {
    nvic_enable_irq(NVIC_USART2_IRQ);

    /* Setup GPIO pin GPIO_USART2_TX. */
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO3);
    gpio_set_af(GPIOA, GPIO_AF4, GPIO2);
    gpio_set_af(GPIOA, GPIO_AF4, GPIO3);

    rcc_periph_clock_enable(RCC_USART2);
    rcc_set_usart2_sel(1);

    /* Setup UART parameters. */
    usart_set_baudrate(USART2, 115200);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX_RX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    // USART_CR1(USART2) |= USART_CR1_RXNEIE;

    /* Finally enable the USART. */
    usart_enable(USART2);
}

void send_string(char *str) {
    while (*str != 0) {
        usart_send_blocking(USART2, *(str++));
    }
}

void usart2_isr(void) {
    /* Check if we were called because of RXNE. */
    if (((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0) && ((USART_ISR(USART2) & USART_ISR_RXNE) != 0)) {
        /* Retrieve the data from the peripheral. */
        usart_recv(USART2);
    }
    __asm__("nop");
}