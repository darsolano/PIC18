/*
 * Protocol handlers
 */

#include <nrf24l01p.h>
#include <nrf24_protocol.h>
#include <i2clcd.h>
#include <xprintf.h>
#include <string.h>

#include "delay.h"


#define CHANNELS		128
uint8_t available_channels[CHANNELS];

NRF24_t r;      // Radio nrf24l01+ data structure
UCHAR8 regs[38];
int i=0;

void nrf24_start_protocol(void)
{
    r.mode = _TX_MODE;
    r.channel = _CH;
    r.data_rate = RF_RATE_1MBPS;
    r.power_db = RF_PWR_0dBm;
    r.tx_addr = _TX_ADDR;
    r.addr_size =_Address_Width;
    r.payload_size = _TX_PLOAD_WIDTH;
    r.rx_addr_P0 = _TX_ADDR;
    r.rx_pipe_PW[0] = _TX_PLOAD_WIDTH;
            
    NRF24L01_Init(&r);
    LCDI2C_Init();
    xdev_out(LCDI2C_Putc);
}

void nrf24_protocol_Print_Details(void)
{
    NRF24L01_ReadAllData(regs);
    NRF24_REGS_t *db_regs = (NRF24_REGS_t*) &regs;

    xputc('\f');    // Clear screen and set the cursor home
    
    // Prints Transmit Pipe Address
    xprintf("TX :%02X%02X%02X%02X%02X C:%03d\n",   db_regs->vTX_ADDR[0],
                                            db_regs->vTX_ADDR[1],
                                            db_regs->vTX_ADDR[2],
                                            db_regs->vTX_ADDR[3],
                                            db_regs->vTX_ADDR[4],
                                            db_regs->vRF_CH);
    
    // Prints Receive Pipe 0 Address
    xprintf("RX0:%02X%02X%02X%02X%02X ",   db_regs->vRX_ADDR_P0[0],
                                            db_regs->vRX_ADDR_P0[1],
                                            db_regs->vRX_ADDR_P0[2],
                                            db_regs->vRX_ADDR_P0[3],
                                            db_regs->vRX_ADDR_P0[4]);
    // Prints Config register
    if (db_regs->vCONFIG & 0x01) xputs("PRx\n");
        else xputs("PTx\n");

    // Prints Receive Pipe 1 Address
    xprintf("RX1:%02X%02X%02X%02X%02X Cf:%02X\n",   db_regs->vRX_ADDR_P1[0],
                                            db_regs->vRX_ADDR_P1[1],
                                            db_regs->vRX_ADDR_P1[2],
                                            db_regs->vRX_ADDR_P1[3],
                                            db_regs->vRX_ADDR_P1[4],
                                            db_regs->vCONFIG);
    
    // Prints Data Rate for radio
    if (db_regs->vRF_SETUP & _BIT(RF_DR_HIGH))  xputs("2Mbps");
    else 
        if (db_regs->vRF_SETUP & _BIT(RF_DR_LOW)) xputs("250kb");
        else 
            if (!(db_regs->vRF_SETUP & _BIT(RF_DR_HIGH))) xputs("1Mbps");

    xputc(' '); // insert a space
    
    // Prints Rf power setup for radio
    if (((db_regs->vRF_SETUP&0x06)>>1) == 0) xputs("-18dBm");
    else
        if (((db_regs->vRF_SETUP&0x06)>>1) == 1) xputs("-12dBm");
        else
            if (((db_regs->vRF_SETUP&0x06)>>1) == 2) xputs("-6dBm");
            else
                if (((db_regs->vRF_SETUP&0x06)>>1) == 3) xputs("0dBm");
    xputc(' ');
    xprintf("RF:%02X",db_regs->vRF_SETUP);
}

void nrf24_SendData( uint8_t* buf , int len)
{
    NRF24L01_Write_TX_Buf(buf, len);
    NRF24L01_RF_TX();
    while ((NRF24L01_Get_Status() & _TX_DS) != _TX_DS);
    
    NRF24L01_Clear_Interrupts();
    NRF24L01_Flush_TX();
    NRF24L01_Flush_RX();
}


void nrf24_GetData( uint8_t *buf, int len)
{
    NRF24L01_StartListening();
    while ((NRF24L01_Get_Status() & _RX_DR) != _RX_DR);
    NRF24L01_Read_RX_Buf(buf,len);
    NRF24L01_StopListening();
}

#define NUMB_OF_REPS    100
uint8_t values[_NUMBER_OF_CHANNELS];
void nrf24_ChannelScanner(void)
{
    // Go to Standby mode
    NRF24L01_StartListening();
    NRF24L01_StopListening();

// Print out header, high then low digit 
    int i = 0;
    while ( i < _NUMBER_OF_CHANNELS )
    {
      xprintf("%x",i>>4);
      ++i;
    }
    xprintf("\n\r");
    i = 0;
    while ( i < _NUMBER_OF_CHANNELS )
    {
      xprintf("%x",i&0xf);
      ++i;
    }
    xprintf("\n\r");
    
  // Clear measurement values 
    memset(values,0,_NUMBER_OF_CHANNELS);

    // Scan all channels num_reps times
    int rep_counter = NUMB_OF_REPS;
    while (rep_counter--)
    {
      int i = _NUMBER_OF_CHANNELS;
      while (i--)
      {
        // Select this channel
        NRF24L01_Set_CH(i);

          // Listen for a little
        NRF24L01_StartListening();
        dly_us(128);
        NRF24L01_StopListening();

        // Did we get a carrier?
        if ( NRF24L01_Get_CD() )
          ++values[i];
      }
    }

    // Print out channel measurements, clamped to a single hex digit 
    i = 0;
    while ( i < _NUMBER_OF_CHANNELS )
    {
      xprintf("%x",MIN(0xf,values[i]&0xf));
      ++i;
    }
    xprintf("\n\r");
}
