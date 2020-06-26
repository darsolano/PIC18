



#include <xc.h>
#include "m_i2c.h"
#include <stdio.h>
#include <delay.h>
/*************************************************************************************************
 *                                      Private Variables and Prototypes                                       *
 *************************************************************************************************/
PRIVATE persistent uint8_t i2c_errstat; // Which error occurred most recently?
/*************************************************************************************************/

PRIVATE inline I2C_Errors_Type i2c_start(void);
PRIVATE inline I2C_Errors_Type i2c_rstart(void);
PRIVATE inline I2C_Errors_Type i2c_stop(void);
PRIVATE inline I2C_Errors_Type i2c_ack(I2C_RX_ACK_Type ackbit);
PRIVATE inline I2C_Errors_Type i2c_put(uint8_t byte);
PRIVATE inline I2C_Errors_Type i2c_putbyte(uint8_t byte);
PRIVATE inline I2C_Errors_Type i2c_idle(void);
PRIVATE inline uint8_t get_ack_status(uint8_t address);
PRIVATE inline uint8_t i2c_get(I2C_RX_ACK_Type ackbit);
PRIVATE inline uint8_t i2c_getbyte(I2C_RX_ACK_Type ackbit, uint8_t *data);

/*************************************************************************************************
 *                                      Private Funtions                                        *
 *************************************************************************************************/

/*
 * Block until the I2C interface is idle.
 */
PRIVATE inline I2C_Errors_Type i2c_idle()
{
    unsigned char byte1; // R/W status: Is a transfer in progress?
    unsigned char byte2; // Lower 5 bits: Acknowledge Sequence, Receive, STOP, Repeated START, START
    CLRWDT();
    i2c_errstat = I2C_IDLE_ERR;
    do {
        byte1 = SSPSTAT & 0x04;
        byte2 = SSPCON2 & 0x1F;
    } while (byte1 | byte2);
    CLRWDT();
    i2c_errstat = I2C_NO_ERR;
    return i2c_errstat;
}

/*
 * Send the start message and wait start to finish.
 */
PRIVATE inline I2C_Errors_Type i2c_start()
{
    i2c_idle();
    i2c_errstat = I2C_START_ERR;
    SSPCON2bits.SEN = 1; // Initiate start
    while (SSPCON2bits.SEN);
    CLRWDT();
    i2c_errstat = I2C_NO_ERR;
    return i2c_errstat;
}

/*
 * Send the repeated start message and wait repeated start to finish.
 */
PRIVATE inline I2C_Errors_Type i2c_rstart()
{
    i2c_idle();
    i2c_errstat = I2C_RSTART_ERR;
    SSPCON2bits.RSEN = 1; // Reinitiate start
    while (SSPCON2bits.RSEN);
    CLRWDT();
    i2c_errstat = I2C_NO_ERR;
    return i2c_errstat;
}

/*
 * Sends the stop message and wait stop to finish.
 */
PRIVATE inline I2C_Errors_Type i2c_stop()
{
    i2c_idle();
    i2c_errstat = I2C_STOP_ERR;
    SSPCON2bits.PEN = 1; // Initiate stop
    while (SSPCON2bits.PEN);
    CLRWDT();
    i2c_errstat = I2C_NO_ERR;
    return i2c_errstat;
}

/*
 * Send an acknowledgement.
 */
PRIVATE inline I2C_Errors_Type i2c_ack(I2C_RX_ACK_Type ackbit)
{
    CLRWDT();
    SSPCON2bits.ACKDT = ackbit;
    if (ackbit == NACK) i2c_errstat = I2C_NAK_ERR;
    else i2c_errstat = I2C_ACK_ERR;
    SSPCON2bits.ACKEN = 1; // Initiate acknowlege cycle to send ackbit to slave 1 = NACK, 0 = ACK
    while (SSPCON2bits.ACKEN);
    CLRWDT();
    i2c_errstat = I2C_NO_ERR;
    return i2c_errstat;
}

/*
 * Send a single byte on the I2C bus.
 */
PRIVATE inline I2C_Errors_Type i2c_put(uint8_t byte)
{
    i2c_errstat = I2C_PUT_ERR;
    PIR1bits.SSPIF = 0;
    SSPBUF = byte;
    while (!PIR1bits.SSPIF); // Wait for the ACK to finish
    CLRWDT();
    if (SSPCON2bits.ACKSTAT) i2c_errstat = I2C_MISSACK_ERR;
    else i2c_errstat = I2C_NO_ERR;
    return i2c_errstat;
}

/*
 * Send a single byte on the I2C bus, checking idle first.
 */
PRIVATE inline I2C_Errors_Type i2c_putbyte(uint8_t byte)
{
    I2C_Errors_Type status;
    i2c_idle();
    status = i2c_put(byte);
    return status;
}

// This function writes the slave address to the i2c bus.
// If a slave chip is at that address, it should respond to
// this with an "ACK".   This function returns TRUE if an
// ACK was found.  Otherwise it returns FALSE.

PRIVATE inline uint8_t get_ack_status(uint8_t address)
{
    uint8_t status;

    i2c_start();
    status = i2c_put(address); // Status = 0 if got an ACK
    if (status) {
        i2c_stop();
        return (FALSE);
    } else {
        i2c_stop();
        return (TRUE);
    }
}

/*
 * Read a byte from the I2C bus and send an acknowledgement.
 */
PRIVATE inline uint8_t i2c_get(I2C_RX_ACK_Type ackbit)
{
    uint8_t byte; // Byte read from the bus

    i2c_errstat = I2C_GET_ERR;
    SSPCON2bits.RCEN = 1; // Initiate read event
    while (SSPCON2bits.RCEN);
    CLRWDT();
    while (!SSPSTATbits.BF);
    CLRWDT();
    byte = SSPBUF;
    i2c_errstat = I2C_NO_ERR;
    i2c_ack(ackbit);
    return byte;
}

/*
 * Read a byte from the I2C bus, checking idle first, and send an acknowledgement.
 */
PRIVATE inline uint8_t i2c_getbyte(I2C_RX_ACK_Type ackbit, uint8_t *data)
{
    i2c_idle();
    *data = i2c_get(ackbit);
    return I2C_NO_ERR;
}


//=================================================================================================

/*************************************************************************************************
 *                                       Public Functions                                        *
 *************************************************************************************************/

/*
 * Initialize the I2C library in Master Mode.
 */
void i2c_init(int bitrate)
{
    SSPCON1 = 0x08; // Lower 4 bits of SSPCON1 represent the SSPM bits, I2C Master
    SSPADD = (_XTAL_FREQ / (4 * bitrate)) - 1; // In Master Mode the lower seven bits of SSPADD act as the baud rate generator reload value.

    // SDA and SCL pins must initially be inputs to use the PIC18's I2C hardware in Master Mode
    TRISCbits.RC3 = 1; // Serial clock (SCL) - RC3/SCK/SCL
    TRISCbits.RC4 = 1; // Serial data (SDA) - RC4/SDI/SDA

    SSPIF = 0; // Clear SPIF bit
    i2c_errstat = I2C_NO_ERR; // Clear error status

    SSPCON1bits.SSPEN = 1; // Enable SDA / SCL port bits for I2C
}

/*********************************************************************//**
 * @brief 		Transmit and Receive data in master mode
 * @param[in]	I2Cx			I2C peripheral selected, should be:
 * @param[in]	txcfg   		Pointer to a I2C_Tranfer_Cfg_Type structure that
 * 								contains specified information about the
 * 								configuration for master transfer.
 * @return 		SUCCESS or ERROR
 *
 * Note:
 * - In case of using I2C to transmit data only, either transmit length set to 0
 * or transmit data pointer set to NULL.
 * - In case of using I2C to receive data only, either receive length set to 0
 * or receive data pointer set to NULL.
 * - In case of using I2C to transmit followed by receive data, transmit length,
 * transmit data pointer, receive length and receive data pointer should be set
 * corresponding.
 **********************************************************************/

I2C_Results_Type I2C_MasterDataTransfer(I2C_Tranfer_Cfg_Type *i2ccfg)
{
    uint8_t *ptxdat;
    uint8_t *prxdat;
    uint8_t rxtmp;
    I2C_Errors_Type status;

    // reset all variables to initial must be states
    i2ccfg->error = I2C_NO_ERR;
    i2ccfg->txcount = 0;
    i2ccfg->rxcount = 0;
    ptxdat = i2ccfg->txdata;
    prxdat = i2ccfg->rxdata;

    // First pay attention to send Start Command
    // Send Start Command
    status = i2c_start();
    // review start error
    if (status) {
        i2ccfg->error = status;
        goto error;
    }
    // + Slave Address + Bunch of data bytes
    if ((i2ccfg->txdatalen != 0) && (i2ccfg->txdata != null)) {
        status = i2c_putbyte(i2ccfg->slave_addr << 1);
        if (status) {
            i2ccfg->error = status;
            goto error;
        }
        // Send pack of bytes
        while (i2ccfg->txcount < i2ccfg->txdatalen) {
            status = i2c_putbyte(*ptxdat);
            if (status) {
                i2ccfg->error = status;
                goto error;
            }
            i2ccfg->txcount++;
            ptxdat++;
        }
    }// End of sending data with Write Enable as a LSB os Slv_Addr

    // After Sending Data we need to check if it is neccesary to get some data from I2C device
    // or no data need to be send just get some from the device
    if ((i2ccfg->rxdatalen != 0) && (i2ccfg->rxdata != null)
            && (i2ccfg->txdatalen != 0) && (i2ccfg->txdata != null)) {
        status = i2c_rstart();
        if (status) {
            i2ccfg->error = status;
            goto error;
        }
    }

    // If we only need to recieve some data from I2C device, this is the function
    if ((i2ccfg->rxdatalen != 0) && (i2ccfg->rxdata != null)) {
        // Send Slave Address again at restart
        status = i2c_putbyte((i2ccfg->slave_addr << 1) | I2C_READ);
        if (status) {
            i2ccfg->error = status;
            goto error;
        }
        while (i2ccfg->rxcount < i2ccfg->rxdatalen) {
            if (i2ccfg->rxcount < (i2ccfg->rxdatalen - 1)) { // if data lenth is greater than 1 getbyte and ACK
                status = i2c_getbyte(ACK, &rxtmp);
                if (status) {
                    i2ccfg->error = status;
                    goto error;
                }
            } else { // if data is only 1 lenght or the last one to get send a NACK
                status = i2c_getbyte(NACK, &rxtmp);
                if (status) {
                    i2ccfg->error = status;
                    goto error;
                }
            }
            *prxdat++ = rxtmp;
            //printf("status: %u data read: %02X ",status, *prxdat);
            i2ccfg->rxcount++;
        }
    }
    i2c_stop();
    return (I2C_Success);

error:
    i2c_stop();
    return (I2C_Errors);
}//End I2C_MasterDataTransfer**********************************************

/*
 * Send a single byte on the I2C bus, and reset on ACK error.
 */
void i2c_put_err(unsigned char byte)
{
    // TODO: Fix any _potential_ bugs with this implementation.
    //
    // Originally, i2c_errstat was marked as 'persistent' (not a valid ANSI C keyword), which
    // I assume means it would survive a device reset based on this context. If we reset here,
    // the current value of i2c_errstat will probably be lost. This is likely bug #1 with this
    // implementation.
    //
    // Any program calling this function must take care NOT to call i2c_init() after it restarts!
    // This side-effect should probably be prominently mentioned somewhere because it would be very
    // easy to do. Originally, this reset was enabled by default in i2c_put(), but since that
    // implementation is so error-prone, I removed it from i2c_put() and replaced i2c_put_noerr()
    // with this function to replace the missing functionality. This is potential bug #2.
    i2c_put(byte);
    if (i2c_errstat == I2C_MISSACK_ERR) RESET();
}

/*
 * Send Data thru I2C interface
 */
I2C_Errors_Type i2c_send_data(uint8_t addr, uint8_t* buffer, uint8_t len)
{
    I2C_Errors_Type status;
    /* Sets data to be send to RTC to init*/
    I2C_Tranfer_Cfg_Type i2ctx; //Data structure to be used to send byte thru I2C Master Data Transfer

    // Fill Data Structure with proper data
    i2ctx.rxdata = 0;
    i2ctx.rxdatalen = 0;
    i2ctx.slave_addr = addr;
    i2ctx.txdata = buffer;
    i2ctx.txdatalen = len;
    // Send data to I2C
    status = I2C_MasterDataTransfer(&i2ctx);
    return status;
}

I2C_Errors_Type i2c_send_byte(uint8_t byte, uint8_t i2caddr)
{
    I2C_Errors_Type err = I2C_NO_ERR;
    err = i2c_start();
    if (err != I2C_NO_ERR) return err;
    err = i2c_putbyte(i2caddr << 1);
    if (err != I2C_NO_ERR) return err;
    err = i2c_putbyte(byte);
    if (err != I2C_NO_ERR) return err;
    i2c_stop();
    return err;
}

/*
 * Print an error message based on the current error status.
 */
void
i2c_print_err()
{
    printf("\r\nI2C bus error is ");
    switch (i2c_errstat) {
    case I2C_NO_ERR:
        printf("None");
        break;
    case I2C_IDLE_ERR:
        printf("Idle");
        break;
    case I2C_START_ERR:
        printf("Start");
        break;
    case I2C_STOP_ERR:
        printf("Stop");
        break;
    case I2C_GET_ERR:
        printf("Get");
        break;
    case I2C_PUT_ERR:
        printf("Put");
        break;
    case I2C_MISSACK_ERR:
        printf("Missing Ack");
        break;
    case I2C_ACK_ERR:
        printf("Ack");
        break;
    case I2C_NAK_ERR:
        printf("Nak");
        break;
    default:
        printf("Unknown");
        break;
    };
    printf("\r\n");
}

/*
 * Return the current error status.
 */
uint8_t
i2c_get_err()
{
    return i2c_errstat;
}

/*To use it you will need to call i2c_init()
with the integer representing your desired frequency
(as defined by your datasheet) as the first argument.
Of course you also need the watchdog timer and interrupts setup first.
Then you may invoke the i2c_*() functions like the following:

void update_dac( unsigned char c )
{
    i2c_start();
    i2c_put_err( DAC_ADDR );
    i2c_put_err( 0x00 );
    i2c_put_err( c );
    i2c_stop();
}*/




//=================================

void
i2c_dtect(void)
{
    uint8_t i;
    uint8_t status;
    uint8_t count = 0;

    printf("\nStart:\n");

    dly_ms(250);
    // Try all slave addresses from 0x10 to 0xEF.
    // See if we get a response from any slaves
    // that may be on the i2c bus.
    for (i = 0x10; i < 0xF0; i += 2) {
        status = get_ack_status(i);
        if (status == TRUE) {
            printf("ACK addr: 0x%02X\n", (i >> 1));
            count++;
            dly_ms(1000);
        }
    }
    if (count == 0)
        printf("Nothing Found \n");
    else
        printf("Number of i2c chips found: %u\n", count);
}