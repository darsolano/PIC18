//**********************************************************************
//
//							 Software License Agreement
//
// The software supplied herewith by Microchip Technology Incorporated 
// (the "Company") for its PICmicro® Microcontroller is intended and 
// supplied to you, the Company’s customer, for use solely and 
// exclusively on Microchip PICmicro Microcontroller products. The 
// software is owned by the Company and/or its supplier, and is 
// protected under applicable copyright laws. All rights are reserved. 
//  Any use in violation of the foregoing restrictions may subject the 
// user to criminal sanctions under applicable laws, as well as to 
// civil liability for the breach of the terms and conditions of this 
// license.
//
// THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES, 
// WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
// TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT, 
// IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR 
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
//
//
//**********************************************************************
//  Filename:   delay.c
//*********************************************************************
//  Company:    Microchip Technology
//********************************************************************
/*
 *	Delay functions
 *	See delay.h for details
 *
 *	Make sure this code is compiled with full optimization!!!
 */

#include "delay.h"
//#define _XTAL_FREQ 8000000


//void DelayMs(unsigned char cnt)
//{
//#if	XTAL_FREQ <= 2MHZ
//	do {
//		DelayUs(996);
//	} while(--cnt);
//#endif
//
//#if    XTAL_FREQ > 2MHZ	
//	unsigned char	i;
//	do {
//		i = 4;
//		do {
//			DelayUs(250);
//		} while(--i);
//	} while(--cnt);
//#endif
//}

/*
 * mili is the amount of miliseconds to delay
 */
void dly_ms(int mili)
{
    int i;
    for ( i=0;i<mili;i++)
        __delay_ms(1);
}

void dly_us(int micro)
{
    int i;
    for ( i=0;i<micro;i++)
        __delay_us(1);
}

void start_timer_0(int base)
{
    T0CONbits.T0CS = 0;     // timer mode
    T0CONbits.T08BIT = 0;   // 16 bit counter
    T0CONbits.TMR0ON = 1;   // Enable timer 0
    INTCONbits.TMR0IE = 1;  // Enable interrupt
    INTCONbits.TMR0IF = 0;  // Clear Interrupt flag
}

