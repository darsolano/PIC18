/*
 * pic18F45K20
  EEP24C32.h - library for 24C32 EEPROM from TinyRTC v1.1 by Weiman
 *Created on June 26, 2014, 12:36 AM
 * Author: Darwin O. Solano
 */

#include "EEP24C32.h"
#include "tinyRTC.h"

PRIVATE Status
eepWrite (UCHAR8* data, int len)
{
  I2C_Tranfer_Cfg_Type eepi2c = {0};

  eepi2c.slave_addr = I2C_24C32;
  eepi2c.txdata = data;
  eepi2c.txdatalen = len;

  if (I2C_MasterDataTransfer (&eepi2c) == I2C_Success) return SUCCESS;
  else return ERROR;
}

PRIVATE Status
eepRead (UINT16 address, UCHAR8* buffer, int len)
{
  UCHAR8 buf[2] = {0};
  I2C_Tranfer_Cfg_Type eepi2c = {0};

  buf[0] = ((address & 0xff00) >> 8);
  buf[1] = address & 0x00ff;

  eepi2c.slave_addr = I2C_24C32;
  eepi2c.rxdata = buffer;
  eepi2c.rxdatalen = len;
  eepi2c.txdata = buf;
  eepi2c.txdatalen = 2;

  if (I2C_MasterDataTransfer (&eepi2c) == I2C_Errors) return ERROR;
  else return SUCCESS;
}

/*
 * Read a single byte from pointed address of EEPROM
 */
UCHAR8
EEPReadByte (UINT16 address)
{
  UCHAR8 data;

  eepRead (address, &data, 1);
  //printf ("read byte is: %02X\n", data);
  return (data);
}

/*
 * Write a single Byte to EEPROM
 */
Status
EEPWriteByte (UINT16 addr, UCHAR8 data)
{
  UCHAR8 buf[3] = {0};
  buf[0] = ((addr & 0xff00) >> 8);
  buf[1] = addr & 0x00ff;
  buf[2] = data;

  eepWrite (buf, 3);
  if (data == eepRead (addr)) return (SUCCESS);
  else return ERROR;
}

/*
 * Write Multiple bytes to EEPROM 1 page + 32 bytes
 */
Status
EEPWriteString (UINT16 addr, UCHAR8* buff, UINT8 size)
{
  int i = 2;
  UCHAR8 txdata[size + 2];
  
  txdata[0] = ((addr & 0xff00) >> 8);
  txdata[1] = addr & 0x00ff;
  while (i++ < size)
    {
      txdata[i] = *buff++;
    }
  eepWrite (txdata , size+2);
}

/*
 * Read multiple bytes from EEPROM
 */
UCHAR8*
EEPReadString (UINT16 addr, UINT8 size, UCHAR8 *ptr)
{
  eepRead (addr, ptr, size);
  return (ptr);
}
