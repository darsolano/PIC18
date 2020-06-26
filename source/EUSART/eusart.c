
#include <eusart.h>
#define _XTAL_FREQ 64000000


// Handle UART Buffer full
void interrupt high_priority UART_ISR(void)
{
    if (PIR1bits.RCIF)
    {
        // Handle interrupt from UART RX buffer full
    }
}

char UART_Init(int baudrate, Bool Interrupt_State)
{
    unsigned int x;
    x = (_XTAL_FREQ - baudrate * 64) / (baudrate * 64); //SPBRG for Low Baud Rate
    if (x > 255) //If High Baud Rage Required
    {
        x = (_XTAL_FREQ - baudrate * 16) / (baudrate * 16); //SPBRG for High Baud Rate
        BRGH = 1; //Setting High Baud Rate
    }
    if (x < 256)
    {
        SPBRG = x; //Writing SPBRG Register
        SYNC = 0; //Setting Asynchronous Mode, ie UART
        SPEN = 1; //Enables Serial Port
        TRISC7 = 1; //As Prescribed in Datasheet
        TRISC6 = 1; //As Prescribed in Datasheet
        CREN = 1; //Enables Continuous Reception
        TXEN = 1; //Enables Transmission

        if (Interrupt_State)
        {
            RCONbits.IPEN = 1; // Interrupt Priority Enable. Is Disable to avoid complexity
            PIE1bits.RCIE = 1; // Peripheral Interrupt for Receive Enable
            IPR1bits.RCIP = 1; // Interrupt Priority For Receive is High
            INTCONbits.GIEH = 1; // Enable all Interrupts Unmasked High Priority
            INTCONbits.GIEL = 1; // Enable all Low Priority Interrupt and Peripheral
        }
        else
        {
            RCONbits.IPEN = 0;
            PIE1bits.RCIE = 0;
            IPR1bits.RCIP = 0;
            INTCONbits.GIEH = 0;
            INTCONbits.GIEL = 0;
        }
        return 1; //Returns 1 to indicate Successful Completion
    }
    return 0; //Returns 0 to indicate UART initialization failed
}

char UART_TX_Empty(void)
{
    return TRMT;
}

void UART_Write(char data)
{
    while (!TRMT);
    TXREG = data;
}

void UART_Write_Text(char *text)
{
    int i;
    for (i = 0; text[i] != '\0'; i++)
        UART_Write(text[i]);
}

char UART_Data_Ready(void)
{
    return RCIF;
}

char UART_Read(void)
{
    while (!RCIF);
    return RCREG;
}

void UART_Read_Text(char *Output, unsigned int length)
{
    unsigned int i;
    for (i = 0; i < length; i++)
        Output[i] = UART_Read();
}