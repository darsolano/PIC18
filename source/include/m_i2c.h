/* 
 * File:   m_i2c.h
 * Author: dsolano
 *
 * Created on June 14, 2014, 10:53 PM
 */

#ifndef M_I2C_H
#define	M_I2C_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <xc.h>
#include <types.h>

#define I2C_SPEED               400000   //  400KHz
// #define I2C_BRG                 0x27    // = 400KHz


    //SSPSTAT: MSSP STATUS REGISTER (I2C MODE)
#define I2C_TX_IN_PROGRESS          0x02
#define I2C_RX_BUFF_FULL            0x01
#define I2C_TX_BUFF_FULL            0x01

    //SSPCON1: MSSP CONTROL 1 REGISTER (I2C MODE)
#define I2C_WRITE_COLLISION         0x80
#define I2C_RX_BUFF_OVERFLOW        0x40
#define I2C_ENABLE_SDA_SCL          0x20

    /*SSPM<3:0>: Synchronous Serial Port Mode Select bits
    1111 = I2C Slave mode, 10-bit address with Start and Stop bit interrupts enabled
    1110 = I2C Slave mode, 7-bit address with Start and Stop bit interrupts enabled
    1011 = I2C Firmware Controlled Master mode (Slave Idle)
    1000 = I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
    0111 = I2C Slave mode, 10-bit address
    0110 = I2C Slave mode, 7-bit address
    Bit combinations not specifically listed here are either reserved or implemented in SPI mode only.*/
#define I2C_SLAVE_MODE_10_bit_W_INT_ENB           0x0F
#define I2C_SLAVE_MODE_7_bit_W_INT_ENB            0x0E
#define I2C_FIRMWARE_CONTROL_MASTER_MODE          0x0B
#define I2C_MASTER_MODE                         0x08
#define I2C_SLAVE_MODE_10_bit                    0x07
#define I2C_SLAVE_MODE_7_bit                     0x06

    //SSPCON2: MSSP CONTROL REGISTER (I2C MODE)
#define I2C_ACK_NOT_REC        0x40
#define I2C_RXBYTE_NACK        0x20
#define I2C_START_ACK_SEQ      0x10
#define I2C_RX_ENABLE          0x80
#define I2C_STOP_ENABLE        0x04
#define I2C_RESTART_ENABLE     0x02
#define I2C_START_ENABLE       0x01

#define null                 ((void*) 0)
    //ACKDT: Acknowledge Data bit (Master Receive mode only)

    typedef enum {
        ACK,
        NACK
    } I2C_RX_ACK_Type;
    /* Error conditions which may be set by any public I2C library function */
    // Clasified errors for I2C

    typedef enum {
        I2C_NO_ERR,
        I2C_IDLE_ERR,
        I2C_START_ERR,
        I2C_RSTART_ERR,
        I2C_STOP_ERR,
        I2C_GET_ERR,
        I2C_PUT_ERR,
        I2C_MISSACK_ERR,
        I2C_ACK_ERR,
        I2C_NAK_ERR
    } I2C_Errors_Type;

    // Read and Write LSB for Address Byte

    typedef enum {
        I2C_WRITE,
        I2C_READ
    } I2C_RW_Type;

    // Action to be performed by the master trasnfer routine

    typedef enum {
        i2c_to_send,
        i2c_to_receive,
        i2c_tx_rx
    } I2C_Action_Type;

    // Result for action on I2C interface

    typedef enum {
        I2C_Success,
        I2C_Errors
    } I2C_Results_Type;

    // to be used as a Structure for sending and recceinving data from I2C

    typedef struct {
        uint8_t slave_addr;
        uint8_t *txdata;
        int txdatalen;
        uint8_t txcount;
        uint8_t *rxdata;
        int rxdatalen;
        uint8_t rxcount;
        I2C_Errors_Type error;
        I2C_Action_Type action;
    } I2C_Tranfer_Cfg_Type;

    /*
     * I2C Library Functions
     *
     * These functions assume the WatchDogTimer is in operation. If it timed our during normal
     * operation, call i2c_get_err() or i2c_print_err() to determine what went wrong.
     */

    void i2c_init(int bitrate);
    void i2c_put_err(unsigned char byte);
    void i2c_print_err(void);
    uint8_t i2c_get_err(void);
    void i2c_dtect(void);
    I2C_Results_Type I2C_MasterDataTransfer(I2C_Tranfer_Cfg_Type *TrfCfg);
    I2C_Errors_Type i2c_send_data(uint8_t addr , uint8_t* buffer , uint8_t len);
    I2C_Errors_Type i2c_send_byte(uint8_t byte , uint8_t i2caddr);


#ifdef	__cplusplus
}
#endif

#endif	/* M_I2C_H */

