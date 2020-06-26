/*********************************************************************
 *
 *                 1-Wire Communication Protocol
 *
 *********************************************************************
 * FileName:        1wire.c
 * Dependencies:
 * Processor:       PIC18
 * Complier:        MCC18 v3.13
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright � 2004-2007 Microchip Technology Inc.  All rights reserved.
 *
 * Microchip licenses to you the right to use, copy and distribute Software 
 * only when embedded on a Microchip microcontroller or digital signal 
 * controller and used with a Microchip radio frequency transceiver, which 
 * are integrated into your product or third party product (pursuant to the 
 * sublicense terms in the accompanying license agreement).  You may NOT 
 * modify or create derivative works of the Software.  
 *
 *
 * You should refer to the license agreement accompanying this Software for 
 * additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED �AS IS� WITHOUT WARRANTY OF ANY 
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY 
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR 
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED 
 * UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF 
 * WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR 
 * EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, 
 * PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF 
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY 
 * THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER 
 * SIMILAR COSTS.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Sasha     			12/20/07    Original
 ********************************************************************/

/****** I N C L U D E S **********************************************************/
#include <_1wire.h>
#include <delay.h>
#include <define_pins.h>

//****** V A R I A B L E S ********************************************************/
unsigned char macro_delay;
DEFINE_PIN(OW_PIN,C,0)
/**********************************************************************
 * Function:        void drive_OW_low (void)
 * PreCondition:    None
 * Input:		   None	
 * Output:		   None	
 * Overview:		   Configure the OW_PIN as Output and drive the OW_PIN LOW.	
 ***********************************************************************/
void drive_OW_low(void)
{
    OW_PIN_OUTPUT();
    OW_PIN_LOW();
}

/**********************************************************************
 * Function:        void drive_OW_high (void)
 * PreCondition:    None
 * Input:		   None	
 * Output:		   None	
 * Overview:		   Configure the OW_PIN as Output and drive the OW_PIN HIGH.	
 ***********************************************************************/
void drive_OW_high(void)
{
    OW_PIN_OUTPUT();
    OW_PIN_HIGH();
}

/**********************************************************************
 * Function:        unsigned char read_OW (void)
 * PreCondition:    None
 * Input:		   None	
 * Output:		   Return the status of OW pin.	
 * Overview:		   Configure as Input pin and Read the status of OW_PIN 	
 ***********************************************************************/
uint8_t read_OW(void)
{

    uint8_t read_data = 0;
    OW_PIN_INPUT();
    if (OW_PIN_READ())
        read_data = SET;
    else
        read_data = CLEAR;
    return read_data;
}

/**********************************************************************
 * Function:        unsigned char OW_reset_pulse(void)
 * PreCondition:    None
 * Input:		   None	
 * Output:		   Return the Presense Pulse from the slave.	
 * Overview:		   Initialization sequence start with reset pulse.
 *				   This code generates reset sequence as per the protocol
 ***********************************************************************/
PRESENCE_t OW_reset_pulse(void)
{

    PRESENCE_t presence;
    drive_OW_low(); // Drive the bus low
    __delay_us(DELAY_480Us); // delay 480 microsecond (us)
    drive_OW_high(); // Release the bus
    __delay_us(DELAY_70Us); // delay 70 microsecond (us)
    presence = read_OW(); //Sample for presence pulse from slave
    __delay_us(DELAY_410Us); // delay 410 microsecond (us)
    drive_OW_high(); // Release the bus
    if (presence) return (OW_DEV_NOT_PRESENT);
    else return (OW_DEV_PRESENT);
}

/**********************************************************************
 * Function:        void OW_write_bit (unsigned char write_data)
 * PreCondition:    None
 * Input:		   Write a bit to 1-wire slave device.
 * Output:		   None
 * Overview:		   This function used to transmit a single bit to slave device.
 *				   
 ***********************************************************************/

void OW_write_bit(uint8_t write_bit)
{
    if (write_bit)
    {
        //writing a bit '1'
        drive_OW_low(); // Drive the bus low
        __delay_us(DELAY_6Us); // delay 6 microsecond (us)
        drive_OW_high(); // Release the bus
        __delay_us(DELAY_64Us); // delay 64 microsecond (us)
    }
    else
    {
        //writing a bit '0'
        drive_OW_low(); // Drive the bus low
        __delay_us(DELAY_60Us); // delay 60 microsecond (us)
        drive_OW_high(); // Release the bus
        __delay_us(DELAY_10Us); // delay 10 microsecond for recovery (us)
    }
}

/**********************************************************************
 * Function:        unsigned char OW_read_bit (void)
 * PreCondition:    None
 * Input:		   None
 * Output:		   Return the status of the OW PIN
 * Overview:		   This function used to read a single bit from the slave device.
 *				   
 ***********************************************************************/

uint8_t OW_read_bit(void)
{
    uint8_t read_data;
    //reading a bit 
    drive_OW_low(); // Drive the bus low
    __delay_us(DELAY_6Us); // delay 6 microsecond (us)
    drive_OW_high(); // Release the bus
    __delay_us(DELAY_9Us); // delay 9 microsecond (us)
    read_data = read_OW(); //Read the status of OW_PIN
    __delay_us(DELAY_55Us); // delay 55 microsecond (us)
    return read_data;
}

/**********************************************************************
 * Function:        void OW_write_byte (unsigned char write_data)
 * PreCondition:    None
 * Input:		   Send byte to 1-wire slave device
 * Output:		   None
 * Overview:		   This function used to transmit a complete byte to slave device.
 *				   
 ***********************************************************************/
void OW_write_byte(uint8_t write_data)
{
    uint8_t loop;
    for (loop = 0; loop < 8; loop++)
    {
        OW_write_bit(write_data & 0x01); //Sending LS-bit first
        write_data >>= 1; // shift the data byte for the next bit to send
    }
}

/**********************************************************************
 * Function:        unsigned char OW_read_byte (void)
 * PreCondition:    None
 * Input:		   None
 * Output:		   Return the read byte from slave device
 * Overview:		   This function used to read a complete byte from the slave device.
 *				   
 ***********************************************************************/

uint8_t OW_read_byte(void)
{
    uint8_t loop, result = 0;
    for (loop = 0; loop < 8; loop++)
    {
        result >>= 1; // shift the result to get it ready for the next bit to receive
        if (OW_read_bit())
            result |= 0x80; // if result is one, then set MS-bit
    }
    return result;
}

/********************************************************************************************
                  E N D     O F     1 W I R E . C  
 *********************************************************************************************/