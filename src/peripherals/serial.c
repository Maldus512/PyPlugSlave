#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <string.h>
#include <errno.h>
#include "generic/circularbuffer/circular_buffer.h"

#define BUFSIZE 256

static unsigned char   buffer[BUFSIZE];
static circular_buf_t  _cbuf;
static circular_buf_t *cbuf = &_cbuf;

void serial_init(void) {
    circular_buf_init(cbuf, buffer, BUFSIZE);

    nvic_enable_irq(NVIC_USART2_IRQ);

    rcc_periph_clock_enable(RCC_GPIOA);
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

    USART_CR1(USART2) |= USART_CR1_RXNEIE;

    /* Finally enable the USART. */
    usart_enable(USART2);
}

void serial_print(char *str) {
    while (*str != 0) {
        usart_send_blocking(USART2, *(str++));
    }
}

void usart2_isr(void) {
    uint32_t reg = USART_ISR(USART2);
    if (reg & USART_ISR_FE)
        USART_ICR(USART2) |= USART_ICR_FECF;

    if (reg & USART_ISR_ORE)
        USART_ICR(USART2) |= USART_ICR_ORECF;

    /* Check if we were called because of RXNE. */
    while ((USART_ISR(USART2) & USART_ISR_RXNE) != 0) {
        /* Retrieve the data from the peripheral. */
        circular_buf_putc(cbuf, usart_recv(USART2));
    }
    __asm__("nop");
}

int serial_readline(uint8_t *buffer) {
    uint8_t intermediate[BUFSIZE];

    int len = circular_buf_peek(cbuf, intermediate, BUFSIZE);

    for (int i = 0; i < len; i++) {
        if (intermediate[i] == '\n') {
            memcpy(buffer, intermediate, i + 1);
            buffer[i + 1] = '\0';
            circular_buf_drop(cbuf, i + 1);
            return i + 1;
        }
    }
    return 0;
}

int _write(int file, char *ptr, int len) {
    int i;

    if (file == 1) {
        for (i = 0; i < len; i++) {
            if (ptr[i] == '\n') {
                usart_send_blocking(USART2, '\r');
            }
            usart_send_blocking(USART2, ptr[i]);
        }
        return i;
    }
    errno = EIO;
    return -1;
}