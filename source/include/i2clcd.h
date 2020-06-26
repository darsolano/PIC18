/* 
 * File:   i2clcd.h
 * Author: dsolano
 *
 * Created on June 6, 2014, 6:33 PM
 */

#ifndef I2CLCD_H
#define	I2CLCD_H

#ifdef	__cplusplus
extern "C" {
#endif

/* PIC18 I2C LCD peripheral routines.
 *
 *   Notes:
 *      - These libraries routines are written to support the
 *        Hitachi HD44780 LCD controller.
 *      - The user must define the following items:
 *          - The LCD interface type (4- or 8-bits)
 *          - If 4-bit mode
 *              - whether using the upper or lower nibble
 *          - The I2C Serial Comm
 *              - Setup I2C
 *          - The user must provide three delay routines:
 *              - DelayFor18TCY() provides a 18 Tcy delay
 *              - DelayPORi2cLCD() provides at least 15ms delay
 *              - Delayi2cLCD() provides at least 5ms delay
 */
#include <string.h>
#include <stdio.h>
#include <xc.h>
#include <types.h>
#include <stdint.h>

/* Change this to near if building small memory model versions of the libraries. */
#define MEM_MODEL
//#define DEBUGGING

//****************************************************
// LCD_I2C_SAINSMART_DRIVER FOR MICROCHIP PIC
// Written for PIC18F45K20 By Darwin O. Solano
// reference from SaintSmart Libs and Datasheets
//
// Microchip MPLAB XC8 C Compiler (Free Mode) V1.30
// Copyright (C) 2014 Microchip Technology Inc.
// License type: Node Configuration
// STARTED : 04 May 2014
// REVISION: REVIEW OF I2C CODE, SEEM TO FAULTY ON PILIB XC8
//****************************************************
//***************************************************************************************
// I2C LCD Address (0x27 in this driver) can be changed below (e.g. to 0x27) if required
//
// WRITES to 20x4 LCD marked J204A using Sainsmart I2C board marked LCD2004
// Does not READ
//
// Note: Regarding the Sainsmart I2C adapter, output-P3 (pin-7) from IC
//       PCF8574 is wired such that if LOW, it shuts off
//       the LCD's LED backlight.  P3 doesn't seem to affect any other
//       operations since P3 is not wired to any of the HD44780
//       data inputs.  Therefore P3 must be HIGH on all write operations.
//
//    P7 - D7        P3 - BACKLIGHT LED
//    P6 - D6        P2 - E
//    P5 - D5        P1 - R/W
//    P4 - D4        P0 - RS
//
//***************************************************************************************
#define LCD_I2C 		0x27	// Enter LCD address given by manufacturer 0x27 <<= 0x42
//#define I2C_READ		0x01	// ored with shifted address
//#define I2C_WRITE		0x00	// anded with shifted address
    
// Following line creates appropriate address byte for I2C interface (shift left 1-bit)
#define LCD_I2C_ADDR            (LCD_I2C << 1)

// Line addresses for LCDs which use
// the Hitachi HD44780U controller chip
#define LCD_LINE_1_ADDRESS 0x00
#define LCD_LINE_2_ADDRESS 0x40
#define LCD_LINE_3_ADDRESS 0x14
#define LCD_LINE_4_ADDRESS 0x54

//========List of definitions for command and statement for LCD SaintSmart2004

//# commands
#define LCD_CLEARDISPLAY            0x01
#define LCD_RETURNHOME              0x02
#define LCD_ENTRYMODESET            0x04
#define LCD_DISPLAYCONTROL          0x08
#define LCD_CURSORSHIFT             0x10
#define LCD_FUNCTIONSET             0x20
#define LCD_SETCGRAMADDR            0x40
#define LCD_SETDDRAMADDR            0x80

//# flags for display entry mode
#define LCD_ENTRYRIGHT              0x00
#define LCD_ENTRYLEFT               0x02
#define LCD_ENTRYSHIFTINCREMENT     0x01
#define LCD_ENTRYSHIFTDECREMENT     0x00

//# flags for display on/off control
#define LCD_DISPLAYON               0x04
#define LCD_DISPLAYOFF              0x00
#define LCD_CURSORON                0x02
#define LCD_CURSOROFF               0x00
#define LCD_BLINKON                 0x01
#define LCD_BLINKOFF                0x00

//# flags for display/cursor shift
#define LCD_DISPLAYMOVE             0x08
#define LCD_CURSORMOVE              0x00
#define LCD_MOVERIGHT               0x04
#define LCD_MOVELEFT                0x00

//# flags for function set
#define LCD_8BITMODE                0x10
#define LCD_4BITMODE                0x00
#define LCD_2LINE                   0x08
#define LCD_1LINE                   0x00
#define LCD_5x10DOTS                0x04
#define LCD_5x8DOTS                 0x00

//# flags for backlight control
#define LCD_BACKLIGHT               0x08
#define LCD_NOBACKLIGHT             0x00

#define En                          0b00000100 //# Enable bit
#define Rw                          0b00000010 //# Read/Write bit
#define Rs                          0b00000001 //# Register select bit

#define ON              1
#define OFF             0

        // Character to display using putc
#define CUST_FONT_FARENHEIT			0x00
#define CUST_FONT_CELSIUS			0x01
#define CUST_FONT_AM    			0x02
#define CUST_FONT_PM    			0x03
#define CUST_FONT_DEGREES			0x04
#define CUST_FONT_BAR   			0x05
#define CUST_FONT_ANTENNA			0x06
#define CUST_FONT_BATTERY			0x07

// Variables
unsigned char lcd_buffer;
unsigned char lcd2004_line;


//=======================Funtion Prototypes===================//
/*
 * void LCDI2C_Putc(unsigned char c);
 * Print any character defined in the current cursor position
 * Return : void
 * Note: could be inserted in the putch function to complement printf();
 *       must be  enclosed in single quotes
 */
void LCDI2C_Putc(unsigned char c);      //Print a character from current cursor position

/*
 * void LCDI2C_Prints(char * buffer);
 * Print any string null terminated defined in the current cursor position
 * Return : void
 * Note: could be inserted in the putch function to complement printf();
 */
void LCDI2C_Prints(char * buffer);      //Prints a string from current cursor position in RAM

/***********************************************************************************
Function    : LCDI2C_Printrs

Include     : i2c.h

Description : Macro disables I2C  Interrupt

Arguments   : const char buffer

Remarks     : String must set in rom as constant string
 *              or enclosed in double quotes directly
***********************************************************************************/
void LCDI2C_Printrs(const MEM_MODEL char * buffer); //Print string from ROM

/***********************************************************************************
Function    : LCDI2C_ClearDisplay

Include     : i2c.h

Description : Macro disables I2C  Interrupt

Arguments   : const char buffer

Remarks     : None
***********************************************************************************/
void LCDI2C_ClearDisplay(void);         //Clear entire display

/***********************************************************************************
Function    : void LCDI2C_Init(void)

Include     : i2c.h

Description : Macro disables I2C  Interrupt

Arguments   : const char buffer

Remarks     : None
***********************************************************************************/
void LCDI2C_Init(void);


void LCDI2C_DisplayOnOff(char status );  //Turns on and Off Display
void LCDI2C_CursorOnOff(char status );
void StartLCD_I2C(void);
void StopLCD_I2C(void);
void LCDI2C_Write(unsigned char data, unsigned char RS);
void LCDI2C_SetCursorPos(unsigned char x, unsigned char y);
void LCDI2C_CursorBlink(char status);
void LCDI2C_BackLight( char status );
void LCDI2C_ReturnHome(void);
void LCDI2C_PutDec99(uint8_t decnum);
void LCDI2C_PutDec(uint8_t decnum);
void LCDI2C_PutDec16(uint16_t decnum);
void LCDI2C_PutDec32(uint32_t decnum);
void LCDI2C_PutHex_(uint8_t hexnum);
void LCDI2C_PutHex(uint8_t hexnum);
void LCDI2C_PutHex16(uint16_t hexnum);
void LCDI2C_PutHex32(uint32_t hexnum);

void LCDI2C_InitHD44780CustomChars(void);
void LCDI2C_DefineSpecialChars(const char* spclchar, uint8_t charpos);


#ifdef	__cplusplus
}
#endif

#endif	/* I2CLCD_H */

