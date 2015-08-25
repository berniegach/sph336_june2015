/*
 * infra.c
 *
 * Copyright (c) 2015 Bernie Gach
 * see the LICENCE file
 */
#include"infra.h"
void infra_init(void)
{
        /*set clock to uart0 module*/
        SIM_SCGC4|=SIM_SCGC4_UART0_MASK;

         //PTC6 as receiver and PTD7 as transmitter
        portC->PCR[6].mux=0x00;
        portD->PCR[7].mux=0x00;
        portD->PCR[7].mux=0x02;
        SIM_SOPT5|= SIM_SOPT5_UART0RXSRC(0x01);   	
        PTC->PDDR.bit_reg.bit6=in;
        PTD->PDDR.bit_reg.bit7=out;
}

void uart0_init (UART_MemMapPtr uartch, int sysclk, int baud)
{ 
 	uint16_t ubd, temp, brfa;
	/* Make sure that the transmitter and receiver are disabled while we
	* change settings.
	*/
	UART_C2_REG(uartch) &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );
	/* Configure the UART for 8-bit mode, no parity */
	/* We need all default settings, so entire register is cleared */
	UART_C1_REG(uartch) = 0;
	/* Calculate baud settings */
	ubd = (uint16_t)((sysclk*2000)/(baud * 16));
	/* Save off the current value of the UARTx_BDH except for the SBR */
	temp = UART_BDH_REG(uartch) & ~(UART_BDH_SBR(0x1F));
	UART_BDH_REG(uartch) = temp | UART_BDH_SBR(((ubd & 0x1F00) >> 8));
	UART_BDL_REG(uartch) = (uint8_t)(ubd & UART_BDL_SBR_MASK);
	/* Determine if a fractional divider is needed to get closer to the baud rate */
	brfa = (((sysclk*32000)/(baud * 16)) - (ubd * 32));
	/* Save off the current value of the UARTx_C4 register except for the BRFA */
	temp = UART_C4_REG(uartch) & ~(UART_C4_BRFA(0x1F));
	UART_C4_REG(uartch) = temp | UART_C4_BRFA(brfa);
	/* Enable receiver and transmitter */
	UART_C2_REG(uartch) |= (UART_C2_TE_MASK | UART_C2_RE_MASK );
	UART_S1_REG(uartch) &= ~UART_S1_TDRE_MASK;
    /*infrared enable and set transmitter narrow pulse*/
        UART0_IR|= UART_IR_IREN_MASK;
        UART0_IR|= UART_IR_TNP(0x00);  
 
}


void infra_transmit(uint8_t ch)
{
        /* Wait until space is available in the FIFO */
	while(!(UART_S1_REG(UART0_BASE_PTR) & UART_S1_TDRE_MASK));

	/* Send the character */
	UART_D_REG(UART0_BASE_PTR) = (uint8_t)ch;

}

/*send a string*/
void infra_transmit_string(uint8_t *s)
{
	while(*s!='\0')
        {
		infra_transmit(*s++);
	}
}



uint8_t infra_read(void)
{
      	/* Wait until character has been received */
	while (!(UART_S1_REG(UART0_BASE_PTR) & UART_S1_RDRF_MASK));

	/* Return the 8-bit data from the receiver */
	return (uint8_t)UART_D_REG(UART0_BASE_PTR);
}





/*check if there is data to read*/
uint16_t data_present(void)
{
      return (((uint16_t)(UART_S1_REG(UART0_BASE_PTR) & UART_S1_RDRF_MASK)) == UART_S1_RDRF_MASK);

}

