/* 
 * File:   board_44pin_PIC18F45k20.h
 * Author: Darwin O. Solano
 *
 * Created on April 24, 2014, 12:23 PM
 */
#ifndef BOARD_44PIN_PIC18F45K20_H
#define	BOARD_44PIN_PIC18F45K20_H

#ifdef	__cplusplus
extern "C" {
#endif

/** C O N F I G U R A T I O N   B I T S ******************************/

#pragma config FOSC = INTIO67, FCMEN = OFF, IESO = OFF                  // CONFIG1H
#pragma config PWRT = OFF, BOREN = SBORDIS, BORV = 30                   // CONFIG2L
#pragma config WDTEN = OFF, WDTPS = 32768                               // CONFIG2H
#pragma config MCLRE = OFF, LPT1OSC = OFF, PBADEN = ON, CCP2MX = PORTC  // CONFIG3H
#pragma config STVREN = ON, LVP = OFF, XINST = OFF			// CONFIG4L
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF               // CONFIG5L
#pragma config CPB = OFF, CPD = OFF                                     // CONFIG5H
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF           // CONFIG6L
#pragma config WRTB = OFF, WRTC = OFF, WRTD = OFF                       // CONFIG6H
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF       // CONFIG7L
#pragma config EBTRB = OFF                                              // CONFIG7H


/** I N C L U D E S **************************************************/
#include <xc.h>
  
#include "types.h"
#include "delay.h"
#define _XTAL_FREQ 64000000

#define ON  0
#define OFF 1
#define Switch_Pin      PORTBbits.RB0
#define DetectsInARow   5



//*********************************************
// BOARD MAIN FUNCTIONALITY, ROUTINES AND MORE...DECLARATION OF...
//*********************************************

//*********************************************
//LEDS FUNCTIONS
//*********************************************
void led_toggle(int pos);
void set_leds_ON_OFF( unsigned pos ,  unsigned state);


//*********************************************
//SWITCH FUNCTIONS
//*********************************************
unsigned char get_switch_state();
unsigned char debounce_sw();                 //ELIMINATE NOISE FRON ANALOG SWITCH FUNCTIONS

//*********************************************
// INITIALIZE MAIN REGISTERS
//*********************************************
void system_init();                 //INITIALIZATION FUNCTION DEFINITION


//*********************************************
// IMPLEMENTATION AREA OF FUNCTIONS ABOVE
//*********************************************

void system_init(){             //INITIALIZATION FUNCTION IMPLEMENTATION

    //OSCCONbits.IDLEN = 0x0;     //LEAVE IT AS DEFAULT                               OSCCON REGISTER
    //OSCCONbits.IRCF2 = 0x1;     //SET MAX FREQ TO 16MHZ IRCF = 0x7  IDLEN | IRCF2 | IRCF1 | IRCF0 | OSTS(1) | IOFS | SCS1 | SCS0
    //OSCCONbits.IRCF1 = 0x1;     //SET MAX FREQ TO 16MHZ IRCF = 0x7   DEF      1       1       1
    //OSCCONbits.IRCF0 = 0x1;     //SET MAX FREQ TO 16MHZ IRCF = 0x7
    //OSCTUNE = 0x1F;
    //OSCTUNEbits.PLLEN = 0x1;
    // SCS FOSC; OSTS intosc; IRCF 16MHz_HFINTOSC/4; IDLEN disabled; 
    OSCCON = 0x70;
    // INTSRC disabled; PLLEN enabled; TUN 0; 
    OSCTUNE = 0x40;
    // Set the secondary oscillator

    WDTCONbits.SWDTEN = 0x1;
    TRISD   = 0x00;               //SET ALL BITS OUT FOR PORT D - 7 LEDS ON BOARD
    LATD    = 0x00;                //SETS ALL BITS TO 0
    ANSELH  = 0x00;              // AN8-12 are digital inputs (AN12 on RB0)
/*********************************************************************************************/
//SET PORTB AS INPUT ON BIT 0
//
    TRISB = 0x01;               // FOR ON BOARD SWITCH PORT A BIT 0  SET AS INPUT
    WPUBbits.WPUB0 = 0x01;      // WPUB enable for PORT B Bit 0 as input fro Switch 1
    INTCON2bits.nRBPU = 0x01;   // Weak Pull Up enable on INTCON" register bit 7 for Switch 1
}

void led_toggle(int pos)
{
    LATD ^= (1<<pos);
}


void set_led_ON_OFF( UCHAR8 pos ,  unsigned state){
    if (state)
        LATD |= (1 << pos);
    else
        LATD &= ~(1 << pos);
}

unsigned char get_switch_state(){
    if (Switch_Pin){
        
        return (0);
    }else{
        while (Switch_Pin != 1);    // wait for switch to be released
        
        return(debounce_sw());
    }
}

unsigned char debounce_sw(){
    int Switch_Count = 5;

        do
        { // monitor switch input for 5 lows in a row to debounce
            if (Switch_Pin == 0)
            { // pressed state detected
                Switch_Count++;
            }
            else
            {
                Switch_Count = 0;
                break;
            }   
            dly_ms(100);
        } while (Switch_Count < DetectsInARow);
        
    return(1);
}


//
#ifdef	__cplusplus
}
#endif

#endif	/* BOARD_44PIN_PIC18F45K20_H */
