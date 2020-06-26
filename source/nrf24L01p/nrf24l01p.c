#include <nrf24l01p.h>
#include <string.h>
#include <define_pins.h>
#include <delay.h>



/**
 * Set chip select pin
 *
 * @param mode HIGH to take this unit off the SPI bus, LOW to put it on
 */
DEFINE_PIN(RADIO, A, 0) // CE Radio Enable and Disable
DEFINE_PIN(CSN, A, 1) // CSN Serial Chip Select
DEFINE_PIN(CLK, A, 2) // Serial Clock
DEFINE_PIN(DOUT, A, 3) // Serial Data OUT to NRF24L01+
DEFINE_PIN(DIN, A, 4) // Serial Data IN from NRF24L01+
DEFINE_PIN(RF_IRQ, A, 5) // IRQ from NRF24L01+

uint8_t rxbuff[5];

// ---------------------------------------------------------------------------
// This one send and receive data from Serial on the selected channel
// ---------------------------------------------------------------------------

// SPI BitBang to send and Get data 8 bits

static UCHAR8 spiIO(UINT8 data)
{
    uint8_t temp, count;
    UCHAR8 data_in = 0;
    CLK_LOW();

    temp = data;
    for (count = 0; count < 8; count++)
    {
        // Write data
        if (temp & 0x80)
            DOUT_HIGH();
        else
            DOUT_LOW();
        temp = temp << 1;

        CLK_HIGH();
        __nop();

        //Read data
        data_in <<= 1; // Rotate the data
        data_in += DIN_READ(); // Read the data bit

        CLK_LOW();
        __nop();
    }
    return data_in;
}

/************************************************************************
 * Function: SpiInit                                                 *
 *                                                                       *
 * Preconditions: SPI module must be configured to operate with          *
 *                 parameters: Master, MODE8 = 0, CKP = 1, SMP = 1.     *
 *                                                                       *
 * Overview: This function setup SPI IOs connected to EEPROM.            *
 *                                                                       *
 * Input: None.                                                          *
 *                                                                       *
 * Output: None.                                                         *
 *                                                                       *
 ************************************************************************/
 static void spiINIT(void)
{
    RADIO_OUTPUT();
    CSN_OUTPUT();
    CLK_OUTPUT();
    RF_IRQ_INPUT();
    DOUT_OUTPUT();
    DIN_INPUT();
}


/****************************************************************
 * RADIO BASIC LOW LEVEL FUNCTIONS - ALL PRIVATE
 ****************************************************************/

/**
 Read a register

 @param Reg Register to read

 @return Registry Value
 */
 char NRF24L01_ReadReg(char Reg)
{
    char Result;

    CSN_DEASSERT();
    spiIO(Reg);
    Result = spiIO(0);
    CSN_ASSERT();
    return Result;
}

/**
 Returns the STATUS register and then modify a register

 @param Reg Register to change
 @param Value New value

 @return STATUS Register
 */
 char NRF24L01_WriteReg(char Reg, char Value)
{
    char status;

    CSN_DEASSERT();
    status = spiIO(Reg);
    spiIO(Value);
    CSN_ASSERT();
    return status;
}

/**
 Returns the STATUS register and then read "n" registers

 @param Reg Register to read
 @param Buf Pointer to a buffer
 @param Size Buffer Size

 @return STATUS Register
 */
 char NRF24L01_ReadRegBuf(char Reg, char *Buf, int Size)
{
    int i;
    char status;

    CSN_DEASSERT();
    status = spiIO(Reg);
    for (i = 0; i < Size; i++)
    {
        Buf[i] = spiIO(0);
    }
    CSN_ASSERT();
    return status;
}

/**
 Returns the STATUS register and then write "n" registers

 @param Reg Registers to change
 @param Buf Pointer to a buffer
 @param Size Buffer Size

 @return STATUS Register
 */
 char NRF24L01_WriteRegBuf(char Reg, char *Buf, int Size)
{
    int i;
    char status;

    CSN_DEASSERT();
    status = spiIO(Reg);
    for (i = 0; i < Size; i++)
    {
        spiIO(Buf[i]);
    }
    CSN_ASSERT();
    return status;
}

/**
 Returns the STATUS register

 @return STATUS Register
 */
 char NRF24L01_Get_Status(void)
{
    char status;
    CSN_DEASSERT();
    status = spiIO(_NOP_);
    CSN_ASSERT();
    return status;
}

/**
 Returns the carrier signal in RX mode (high when detected)

 @return CD
 */
 char NRF24L01_Get_CD(void)
{
    NRF24L01_Set_Device_Mode(_RX_MODE);
    RADIO_ON();
    dly_us(1500);
    RADIO_OFF();
    if (NRF24L01_ReadReg(CD) & 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 Select power mode

 @param Mode = _POWER_DOWN, _POWER_UP

 @see _POWER_DOWN
 @see _POWER_UP

 */
 char NRF24L01_Set_Power(char Mode)
{
    char Result;
    char status;

    Result = NRF24L01_ReadReg(CONFIG) & 0b00001101; // Read Conf. Reg. AND Clear bit 1 (PWR_UP) and 7 (Reserved)
    NRF24L01_WriteReg(W_REGISTER | CONFIG, Result | Mode);
    return status;
}

/**
 Select the radio channel

 @param CH = 0..125

 */
 void NRF24L01_Set_CH(char CH)
{
    NRF24L01_WriteReg(W_REGISTER | RF_CH, CH) ; // Clear bit 8
}

/**
 Select Enhanced ShockBurst ON/OFF

 Disable this functionality to be compatible with nRF2401

 @param Mode = _ShockBurst_ON, _ShockBurst_OFF

 @see _ShockBurst_ON
 @see _ShockBurst_OFF

 */
 void NRF24L01_Set_ShockBurst(char Mode)
{
    NRF24L01_WriteReg(W_REGISTER | SETUP_RETR, Mode);
    NRF24L01_WriteReg(W_REGISTER | EN_AA, Mode);
}

/**
 Select the address width

 @param Width = 3..5
 */
 void NRF24L01_Set_Address_Width(char Width)
{
    NRF24L01_WriteReg(W_REGISTER | SETUP_AW, (Width & 3) - 2);
}

/**
 Select mode receiver or transmitter

 @param Device_Mode = _TX_MODE, _RX_MODE

 @see _TX_MODE
 @see _RX_MODE
 */
 void NRF24L01_Set_Device_Mode(char Device_Mode)
{
    char Result;

    Result = NRF24L01_ReadReg(CONFIG) & 0b00001110; // Read Conf. Reg. AND Clear bit 0 (PRIM_RX) and 7 (Reserved)
    NRF24L01_WriteReg(W_REGISTER | CONFIG, Result | Device_Mode);
}

/**
 Enables and configures the pipe receiving the data

 @param PipeNum Number of pipe
 @param Address Address
 @param AddressSize Address size
 @param PayloadSize Buffer size, data receiver

 */
  void NRF24L01_Set_RX_Pipe(UCHAR8 PipeNum, UCHAR8* addr, int addr_size, int pld_size)
{
    NRF24L01_WriteReg(W_REGISTER | EN_RXADDR, NRF24L01_ReadReg(EN_RXADDR) | (1 << PipeNum)); // enable the Pipe number
    NRF24L01_WriteRegBuf(W_REGISTER | (RX_ADDR_P0 + PipeNum), addr, addr_size); // Set address according to pipe number
    NRF24L01_WriteReg(W_REGISTER | (RX_PW_P0 + PipeNum), pld_size); //Set payload size according to pipe number
}

/**
 Disable all pipes
 */
 void NRF24L01_Disable_All_Pipes(void)
{
    NRF24L01_WriteReg(W_REGISTER | EN_RXADDR, 0);
}

/** Returns the STATUS register and then clear all interrupts
 *
 * @return STATUS Register
 */
 char NRF24L01_Clear_Interrupts(void)
{
    return NRF24L01_WriteReg(W_REGISTER | STATUS, _RX_DR | _TX_DS | _MAX_RT);
}

/**
 Sets the direction of transmission

 @param Address Address
 @param Size Address size 3..5

 */
 void NRF24L01_Set_TX_Address(char *Address, int Size)
{
    NRF24L01_WriteRegBuf(W_REGISTER | TX_ADDR, Address, Size);
}

/**
 Empty the transmit buffer

 */
 void NRF24L01_Flush_TX(void)
{
    CSN_DEASSERT();
    spiIO(FLUSH_TX);
    CSN_ASSERT();
}

/**
 Empty the receive buffer
 */
 void NRF24L01_Flush_RX(void)
{
    CSN_DEASSERT();
    spiIO(FLUSH_RX);
    CSN_ASSERT();
}

 
 
/*
 * Configure On the Air Data Rate and RF Wave Power
 */

 Bool NRF24L01_Set_DataRate(RF_DATA_RATE_e data_rate)
{
    uint8_t rf_reg;
    rf_reg = NRF24L01_ReadReg(RF_SETUP) & 0b00000110;
    
    NRF24L01_WriteReg(W_REGISTER | RF_SETUP , rf_reg | data_rate);
    
    if (rf_reg != NRF24L01_ReadReg(RF_SETUP)) return TRUE;
    else return FALSE;
}

// return status register
char NRF24L01_Set_RFPwr(RF_PWR_DBM_e pwr)
{
    uint8_t rf_pwr;
    rf_pwr = NRF24L01_ReadReg(RF_SETUP) & 0b00101000;

    return NRF24L01_WriteReg(W_REGISTER | RF_SETUP , rf_pwr | pwr);
}

 // return status register
char NRF24L01_TX_Reuse(void)
{
    RADIO_ON();
    dly_us(10);
    return NRF24L01_WriteReg(REUSE_TX_PL,0);
}

 char NRF24L01_Get_FIFO_Status(char *fifo_stat)
 {
    char status;
    CSN_LOW();
    status = spiIO(FIFO_STATUS);
    *fifo_stat = spiIO(0);
    CSN_HIGH();
    return status;
 }

char NRF24L01_Get_Rx_Payload_W(char* pldw)
{
    char status;
    CSN_LOW();
    status = spiIO(R_RX_PL_WID);
    *pldw = spiIO(0);
    CSN_HIGH();
    return status;
}

/*
 * See what happened with the last action in total
 * for TX and RX packets
 */
char NRF24L01_See_What_Happened(STATUS_REG_s* status_reg)
{
    status_reg->STATUS_REG = NRF24L01_WriteReg(W_REGISTER|CONFIG,_TX_DS|_RX_DR|_MAX_RT);
    status_reg->RX_PIPE_NO = status_reg->STATUS_REG & (7<<RX_P_NO) ? TRUE:FALSE;
    status_reg->RX_RDY = status_reg->STATUS_REG & _RX_DR ? TRUE:FALSE;
    status_reg->TX_FIFO_FULL = status_reg->STATUS_REG & (1<<TX_FULL) ? TRUE:FALSE;
    status_reg->TX_OK = status_reg->STATUS_REG & _TX_DS ? TRUE:FALSE;
    status_reg->TX_RT_FAIL = status_reg->STATUS_REG & _MAX_RT ? TRUE:FALSE;
    return status_reg->STATUS_REG;
}

/**
 read all register from nrf24l01+
 @param NRF24_REGS_t data structure

 @see typedef struct all_reg_s
{
    UCHAR8 vCONFIG;		//0x00
    UCHAR8 vEN_AA;		//0x01
    UCHAR8 vEN_RXADDR;	//0x02
    UCHAR8 vSETUP_AW;     //0x03
    UCHAR8 vSETUP_RETR;	//0x04
    UCHAR8 vRF_CH;		//0x05
    UCHAR8 vRF_SETUP;     //0x06
    UCHAR8 vSTATUS;		//0x07
    UCHAR8 vOBSERVE_TX;	//0x08
    UCHAR8 vCD;			//0x09
    UCHAR8 vRX_ADDR_P0[5];	//0x0A
    UCHAR8 vRX_ADDR_P1[5];	//0x0B
    UCHAR8 vRX_ADDR_P2;	//0x0C
    UCHAR8 vRX_ADDR_P3;	//0x0D
    UCHAR8 vRX_ADDR_P4;	//0x0E
    UCHAR8 vRX_ADDR_P5;	//0x0F
    UCHAR8 vTX_ADDR[5];		//0x10
    UCHAR8 vRX_PW_P0;	//0x11
    UCHAR8 vRX_PW_P1;	//0x12
    UCHAR8 vRX_PW_P2;	//0x13
    UCHAR8 vRX_PW_P3;	//0x14
    UCHAR8 vRX_PW_P4;	//0x15
    UCHAR8 vRX_PW_P5;	//0x16
    UCHAR8 vFIFO_STATUS;	//0x17
    UCHAR8 vDYNPD;		//0x1C
    UCHAR8 vFEATURE;		//0x1D
}NRF24_REGS_t;
 * Must declare a 38 byte data buffer in order to hold all the data then 
 * cast the data buffer as the NRF24_REGS_t data structure.
 * Good Luck!!! DS
 */
 void NRF24L01_ReadAllData(UCHAR8 *regs)
{
    unsigned int outer;
    unsigned int inner;
    unsigned int dataloc = 0;
    char buffer[5];

    for (outer = 0; outer <= 0x1E; outer++)
    {
        if (outer >= 0x18 && outer <= 0x1B) continue;
        NRF24L01_ReadRegBuf(outer, buffer, 5);

        for (inner = 0; inner < 5; inner++)
        {
            if (inner >= 1 && (outer != 0x0A && outer != 0x0B && outer != 0x10))
                break;

            regs[dataloc] = buffer[inner];
            dataloc++;
        }
    }
}

/**
 Initializes the device
 @param Device_Mode = _TX_MODE, _RX_MODE
 @param CH = 0..125
 @param DataRate = _1Mbps, _2Mbps
 @param Address Address
 @param Address_Width Width direction: 3..5
 @param Size_Payload Data buffer size

 @see _TX_MODE
 @see _RX_MODE
 @see _1Mbps
 @see _2Mbps
 */
 void NRF24L01_Init(NRF24_t *nrf24)
{

    spiINIT();
    // Disable Enhanced ShockBurst
    NRF24L01_Set_ShockBurst(_ShockBurst_OFF);

    // RF output power in TX mode = 0dBm (Max.)
    // Set LNA gain
    NRF24L01_Set_DataRate(nrf24->data_rate);
    NRF24L01_Set_RFPwr(nrf24->power_db);
    NRF24L01_Set_Address_Width(nrf24->addr_size);
    NRF24L01_Set_RX_Pipe(0, nrf24->rx_addr_P0,nrf24->addr_size,nrf24->payload_size);
    NRF24L01_Set_CH(nrf24->channel);
    NRF24L01_Set_TX_Address(nrf24->tx_addr, nrf24->addr_size); // Set Transmit address
    NRF24L01_Set_Device_Mode(nrf24->mode);
    NRF24L01_Set_Power(_POWER_UP);

    dly_us(1500);
}

/**
 Turn on transmitter, and transmits the data loaded into the buffer
 */
void NRF24L01_RF_TX(void)
{
    RADIO_OFF(); 
    RADIO_ON();
    dly_us(10);
    RADIO_OFF();
}

/**
 Writes the buffer of data transmission

 @param Buf Buffer with data to send
 @param Size Buffer size

 */
 void NRF24L01_Write_TX_Buf(char *Buf, int Size)
{
    NRF24L01_WriteRegBuf(W_TX_PAYLOAD, Buf, Size);
}

/**
 Read the data reception buffer

 @param Buf Buffer with data received
 @param Size Buffer size

 */
 void NRF24L01_Read_RX_Buf(char *Buf, int Size)
{
    NRF24L01_ReadRegBuf(R_RX_PAYLOAD, Buf, Size);
}

 void NRF24L01_StartListening(void){
    NRF24L01_Clear_Interrupts();
    NRF24L01_Flush_RX();
    NRF24L01_Flush_TX();
    NRF24L01_Set_Power(_POWER_UP);
    NRF24L01_Set_Device_Mode(_RX_MODE);
    RADIO_ON;
    dly_us(150);
}

 void NRF24L01_StopListening(void)
{
    RADIO_OFF();
    NRF24L01_Flush_RX();
    NRF24L01_Flush_TX();
    NRF24L01_Clear_Interrupts();
    NRF24L01_Set_Power(_POWER_DOWN);
}
