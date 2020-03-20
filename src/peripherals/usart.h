#ifndef USART_H_INCLUDED
#define USART_H_INCLUDED


void usart_setup(void);
void send_string(char *str);
int  uart_readline(uint8_t *buffer);

#endif