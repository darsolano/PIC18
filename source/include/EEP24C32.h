/* 
 * File:   24C32.h
 * Author: dsolano
 *
 * Created on June 26, 2014, 12:36 AM
 */
#ifndef EEP24C32_H
#define	EEP24C32_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "m_i2c.h"
//#define DBGPRINT
// Peripheral ID for EEPROM 24C32 4KBytes from ATMEL
#define I2C_24C32                           0x50
#define I2C_24C32_ADDR                      (I2C_24C32 << 1)
#define I2C_24C32_BUS                       I2C1
#define EE_READ                             1
#define EE_WRITE                            0
#define DS1307_I2C_CLOCK_FREQ               400000      // 100KHz

#define PAGE_BYTES_SIZE                     0x001F
#define MAX_8BYTE_PAGES                     0xEF
#define FIRST_PAGE                          0x000
#define LAST_PAGE                           0xFE0
#define PAGE_INCREMENT                      

// bitwise and for those mask
#define LOW_ADDRESS_MASK                    0b0000000000011111
#define PAGE_ADDRESS_MASK                   0b0000111111100000
#define PAGE(p)                             (p << 5)

#define EEP_ACK                             1
#define EEP_NACK                            0
    
    
typedef union{
    struct EE_Address{
        unsigned    Low     :   5;
        unsigned    High    :   7;
        unsigned            :   4;
    }EE_Address_t;
    struct EE_WordAddr{
        unsigned    Addr    :   12;
        unsigned            :   4;
    }EE_WordAddr_t;
    struct WordAddr{
        UINT16 w_addr;
    }WordAddr_t;
}EE24C32_Addr_t;


/*
 * Function Prototypes
 */
UCHAR8  EEPReadByte(UINT16 address);
UCHAR8  EEPWriteByte(UINT16 addr , UCHAR8 data);
void    EEPWriteString(UINT16 addr , UCHAR8* buff ,UINT8 size);
UCHAR8* EEPReadString(UINT16 addr , UINT8 size, UCHAR8* ptr);


#ifdef	__cplusplus
}
#endif

#endif	/* EEP24C32_H */

