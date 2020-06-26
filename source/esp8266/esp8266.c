
#include <esp8266.h>
#include <xprintf.h>
#include <eusart.h>

void esp8266_init(void)
{
    if (UART_Init(57600, ENABLE)){
        xdev_out(UART_Write);
        xdev_in(UART_Read);
    }
}



