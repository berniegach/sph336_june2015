#ifndef INFRA_H
#define INFRA_H
#include"MK60DZ10.h"
#include"gpio.h"
 extern int periph_clk_khz;
 void infra_init(void);
 void uart0_init (UART_MemMapPtr uartch, int sysclk, int baud);
 void infra_transmit(uint8_t ch);
 void infra_transmit_string(uint8_t *s);
 uint8_t infra_read(void); 
 uint16_t data_present(void);

#endif

