/* 
 * File:   RTC.c
 * Author: Student Companion SA: www.studentcompanion.co.za
 * Real Time Clock Using DS1307 on I2C Bus
 * To simulate in Proteus, The I2C debugger must be connected on the I2C Bus.
 */
 
#include "RTC.h"
#include <stdio.h>
#include <stdlib.h>
#include <delays.h>
#define MSB(x) ((x>>4)+ '0')                    //Display Most Significant Bit of BCD number
#define LSB(x) ((x & 0x0F) + '0')               //Display Least Significant Bit of BCD number

void DelayFor18TCY(void);          //Delay for 18 cycles.
void DelayPORXLCD(void);          //Delay for 15 ms.
void DelayXLCD(void);            //Delay for 5 ms.
void XLCD_init(void);            //Initialize the LCD display
unsigned short read_ds1307(unsigned short addressReg);     //Function to read from the DS1307
void write_ds1307(unsigned short addressReg,unsigned short w_dataReg);      //Function to write to the DS1307
int Binary2BCD(int aa);            //Convert Binary to BCD
int BCD2Binary(int aa);             //Convert BCD to Binary


//*** Global variables ****
char time[] = "00:00:00 PM";
char date[] = "00-00-00";

int second;
int minute;
int hour;
int hr;
int day;
int dday;
int month;
int year;
int ap;

unsigned short set_count = 0;
short set;

void main(void)
{
   OSCCON=0x76;                 //Configure internal oscillator
   XLCD_init();                  // Initialize LCD
   OpenI2C(MASTER, SLEW_OFF);// Initialize I2C module
    SSPADD = 45;             //100kHz Baud clock @8MHz = 19
                             //SSPADD = (FOSC/ Bit Rate) /4 - 1
    //check for bus idle condition in multi master communication
    IdleI2C();
    StartI2C();
    while ( SSPCON2bits.SEN );
    
    putrsXLCD("Time:");
    SetDDRamAddr(0x40);
    putrsXLCD("Date:");
    do{
        //Read Time and Date, when simulating with proteus, it will pick up the system Clock
        //from your PC and might display the AM if your PC time is in 24Hrs mode
        hour= read_ds1307(0x02);   //Read Hour
        hr = hour & 0b00011111;
        ap = hour & 0b00100000;
        minute= read_ds1307(0x01);   //Read minutes
        second= read_ds1307(0x00);   //Read seconds
        dday= read_ds1307(0x03);   //Read The day of the week (sunday to saturday)
        day= read_ds1307(0x04);   //Read The day of the week (1st to 31st)
        month= read_ds1307(0x05);   //Read month
        year= read_ds1307(0x06);   //Read year

        time[0]=MSB(hr);
        time[1]=LSB(hr);
        time[3]=MSB(minute);
        time[4]=LSB(minute);
        time[6]=MSB(second);
        time[7]=LSB(second);
        date[0]=MSB(day);
        date[1]=LSB(day);
        date[3]=MSB(month);
        date[4]=LSB(month);
        date[6]=MSB(year);
        date[7]=LSB(year);
        if(ap)
      {
         time[9] = 'P';
         time[10] = 'M';
      }
      else
      {
         time[9] = 'A';
         time[10] = 'M';
      }
        //Display Time and Date in 12Hrs Mode (with PM or AM)
        SetDDRamAddr(0x05);
        putrsXLCD(time);
        SetDDRamAddr(0x45);
        putrsXLCD(date);

        //Write Time and Date:
        //1. To set the time, press the SET pushbutton once to select the Hour.
        //2. Press the UP button to increament the Hour value or DOWN Button to decreament.
        //3. Press the SET Button again to move to the right to modify the minute.
        //4. Note when SET is in second position, pressing UP or Down will only reset the second value to 00
        //5. Continue doing the same to set the date as well.
        //6. Pressing the SET Button for the 7th time will exit the SET up mode
        set = 0;
     if(PORTCbits.RC0 == 0)
     {
         for(int c=0;c<=20;c++)__delay_ms(5); //wait for 100ms
         if(PORTCbits.RC0 == 0)
         {
             set_count++;
             if(set_count >= 7)
             {
                set_count = 0;
             }
         }
     }
        if(set_count)
     {
        if(PORTCbits.RC1 == 0)
        {
          for(int c=0;c<=20;c++)__delay_ms(5); //wait for 100ms
          if(PORTCbits.RC1 == 0)
              set = 1;
        }

        if(PORTCbits.RC2 == 0)
        {
         for(int c=0;c<=20;c++)__delay_ms(5); //wait for 100ms
          if(PORTCbits.RC2 == 0)
              set = -1;
        }
        if(set_count && set)
        {
          switch(set_count)
          {
            case 1:
                    hour = BCD2Binary(hour);
                    hour = hour + set;
                    hour = Binary2BCD(hour);
                    if((hour & 0x1F) >= 0x13)
                    {
                      hour = hour & 0b11100001;
                      hour = hour ^ 0x20;
                    }
                    else if((hour & 0x1F) <= 0x00)
                    {
                      hour = hour | 0b00010010;
                      hour = hour ^ 0x20;
                    }
                    write_ds1307(2, hour); //write hour
                    break;
            case 2:
                     minute = BCD2Binary(minute);
                     minute = minute + set;
                     if(minute >= 60)
                        minute = 0;
                     if(minute < 0)
                        minute = 59;
                     minute = Binary2BCD(minute);
                     write_ds1307(1, minute); //write min
                     break;
            case 3:
                    if(abs(set))
                      write_ds1307(0,0x00); //Reset second to 0 sec. and start Oscillator
                    break;
            case 4:
                     day = BCD2Binary(day);
                     day = day + set;
                     day = Binary2BCD(day);
                     if(day >= 0x32)
                        day = 1;
                     if(day <= 0)
                        day = 0x31;
                     write_ds1307(4, day); // write date 17
                     break;
            case 5:
                    month = BCD2Binary(month);
                    month = month + set;
                    month = Binary2BCD(month);
                    if(month > 0x12)
                      month = 1;
                    if(month <= 0)
                      month = 0x12;
                    write_ds1307(5,month); // write month 6 June
                    break;
            case 6:
                    year = BCD2Binary(year);
                    year = year + set;
                    year = Binary2BCD(year);
                    if(year <= -1)
                       year = 0x99;
                    if(year >= 0x50)
                       year = 0;
                    write_ds1307(6, year); // write year
                    break;
          }
        }
     }


    } while(1);
              
}
unsigned short read_ds1307(unsigned short address)  //call this function to read date and time
                                                    //from the date and time registers.
{
   char r_data;
  StartI2C();                  // Start condition I2C on bus
    IdleI2C();
     WriteI2C( 0xD0 );            // addresses the chip
         IdleI2C();
  WriteI2C( address );            // write register address
         IdleI2C();
    StopI2C();                   // Stop condition I2C on bus

     RestartI2C();                  // Start condition I2C on bus
         IdleI2C();
     WriteI2C( 0xD1 );            // addresses the chip with a read bit
         IdleI2C();
     r_data = ReadI2C();          // read the value from the RTC and store in result
         IdleI2C();
   NotAckI2C();                 // Not Acknowledge condition.
        IdleI2C();
     StopI2C();                   // Stop condition I2C on bus
     return (r_data);
  }
void write_ds1307(unsigned short address,unsigned short w_data) //call this function to write date and time
                                                    //to the date and time registers.
{
    StartI2C();                  // Start condition I2C on bus
    IdleI2C();
     WriteI2C( 0xD0 );            // addresses the chip
         IdleI2C();
  WriteI2C( address );            // write register address
  IdleI2C();
  WriteI2C( w_data );            // write register address
  IdleI2C();
   StopI2C();                   // Stop condition I2C on bus

  }
 int Binary2BCD(int a) //Convert Binary to BCD so we can write to the DS1307 Register in BCD
{
   int t1, t2;
   t1 = a%10;
   t1 = t1 & 0x0F;
   a = a/10;
   t2 = a%10;
   t2 = 0x0F & t2;
   t2 = t2 << 4;
   t2 = 0xF0 & t2;
   t1 = t1 | t2;
   return t1;
}


int BCD2Binary(int a) //Convert BCD to Binary so we can do some basic calculations
{
   int r,t;
   t = a & 0x0F;
   r = t;
   a = 0xF0 & a;
   t = a >> 4;
   t = 0x0F & t;
   r = t*10 + r;
   return r;
}

void XLCD_init(void){                //Initialize LCD display
    OpenXLCD(FOUR_BIT&LINES_5X7);
    while(BusyXLCD());
    WriteCmdXLCD(0x06); // move cursor right, don?t shift display
    WriteCmdXLCD(0x0C); //turn display on without cursor
    putrsXLCD("Real Time Clock");
    SetDDRamAddr(0x44);
    putrsXLCD("Starting");
     for(int c=0;c<=20;c++)__delay_ms(60);
    SetDDRamAddr(0x44);
    putrsXLCD("Starting.");
     for(int c=0;c<=20;c++)__delay_ms(50);
    SetDDRamAddr(0x44);
    putrsXLCD("Starting..");
     for(int c=0;c<=20;c++)__delay_ms(50);
    SetDDRamAddr(0x44);
    putrsXLCD("Starting...");
     for(int c=0;c<=20;c++)__delay_ms(50);
    WriteCmdXLCD(0x01); //Clear Screen
    WriteCmdXLCD(0x02); //Go Home
}
void DelayFor18TCY( void ){
Delay10TCYx(20);  //18 cycles delay
}
//*****************************************
void DelayPORXLCD (void){
Delay1KTCYx(15); // Delay of 15ms
return;
}//End DelayPORXLCD
//*****************************************
void DelayXLCD (void){
Delay1KTCYx(5); // Delay of 5ms
return;
}