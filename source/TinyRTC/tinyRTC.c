/*
 * tinyrtc.c
 *
 *  Created on: Aug 4, 2014
 *      Author: dsolano
 *	TinyRTCC v.1.1 by weiman
 */

#include <tinyrtc.h>
#include "ssd1306.h"


// private functions
PRIVATE void    StartRTC(void);
PRIVATE void    StopRTC(void);
// end of private functions

PRIVATE persistent UCHAR8				txbuff[12];
PRIVATE persistent UCHAR8				rxbuff[12];
PRIVATE persistent Status_Type  		status;

const UCHAR8 *weekdays[] = {"","Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
const UCHAR8 *hour_ampm[] = {"AM","PM"};
//*********************************************************
// Local Functions
//*********************************************************

PRIVATE void StartRTC(){
	/* Sets data to be send to RTC to init*/
	SECONDS_type secs;

	secs.Seconds_Units = 0;
	secs.Seconds_Tenth = 0;
	secs.Clock_Halt = Start_RTC;

	txbuff[0] = Mins_Reg;	// Address for minutes register
	txbuff[1] = secs.Seconds_Reg;

	// Send data to I2C
	I2CWriteData(txbuff , 2 , DS1307_I2C_ADDR);
}

PRIVATE void StopRTC(){
	/* Sets data to be send to RTC to init*/
	SECONDS_type secs;

	secs.Seconds_Units = 0;
	secs.Seconds_Tenth = 0;
	secs.Clock_Halt = Stop_RTC;

	txbuff[0] = Mins_Reg;	// Address for minutes register
	txbuff[1] = secs.Seconds_Reg;

	// Send data to I2C
	I2CWriteData(txbuff , 2 , DS1307_I2C_ADDR);
}
//*********************************************************
// End Local Functions
//*********************************************************




//*********************************************************
// Public Functions
//*********************************************************
//Properly init DS1307 I2C RTC, Packed BCD
void DS1307_Init(void){

	/* Sets data to be send to RTC to init*/
	CONTROL_reg_Type ctrl;

	ctrl.OUT = 0;
	ctrl.SQWE = SQWE_Enb;
	ctrl.Rate_Select = CLOCK_OUT_1Hz;

	// Sets buffer to send
	txbuff[0] = Mins_Reg;	// Address for minutes register
	txbuff[1] = 0;	//Mins
	txbuff[2] = 0;	//hours
	txbuff[3] = Sunday;
	txbuff[4] = 1;	// Date
	txbuff[5] = 1;	// Month
	txbuff[6] = 0;	// year
	txbuff[7] = ctrl.Control_Register;

	I2CWriteData(txbuff,8,DS1307_I2C_ADDR);

	StartRTC();
}

void	DS1307_DeInit(void){
	StopRTC();
}
//Save a Byte of data to DS1307 pointed by the address to be written
void DS1307SaveRAMByte(UCHAR8 address, UCHAR8 data){

}


UCHAR8 DS1307ReadRAMByte(UCHAR8 address){
	UCHAR8 byte = 0;

	return byte;
}

Status_Type DS1307SetTime(UCHAR8 sec, UCHAR8 min, HOURS_Type* hourcfg){
	UCHAR8 *ptime;
	//first get time in order to not chage the config content of each register
	ptime = DS1307GetTime();
	
	/*if (hourcfg->HOURS_12_Type->Time_Mode == Hours_12H){
		if ( hourcfg->HOURS_12_Type->Hours_Tenth > 1 ||
			 hourcfg->HOURS_12_Type->Hours_Mode != AM ||
			 hourcfg->HOURS_12_Type->Hours_Mode != PM )
				return errors;
	}*/
	ptime[0] &= 0x80;
	txbuff[0] = Secs_Reg;
	txbuff[1] = sec | ptime[0];
	txbuff[2] = min;
	txbuff[3] = hourcfg->Hours_Register;	// Watch for AM/PM and 12/24 hours format mode
	status = I2CWriteData(txbuff,4,DS1307_I2C_ADDR);
	return status;
}

Status_Type DS1307SetDate(DS1307_REGS_Type* rtccfg){
	//first get date in order to not chage the config content of each register

	txbuff[0] = Day_Reg;
	txbuff[1] = (rtccfg->day_r);
	txbuff[2] = (rtccfg->date_r );
	txbuff[3] = (rtccfg->month_r);
	txbuff[4] = (rtccfg->year_r );
	status = I2CWriteData(txbuff,5,DS1307_I2C_ADDR);
	return status;
}


UCHAR8* DS1307GetTime(void){
	//Start I2C sequence to get bytes from address 0x00 from DS1307
	txbuff[0] = Secs_Reg;
	status = I2CWriteData(txbuff,1,DS1307_I2C_ADDR);
	// Set to get data from DS1307
	rxbuff[0] = 0;
	//Set sequence to get time data
	status = I2CReadData(rxbuff,3,DS1307_I2C_ADDR);
	// Set time structures to hold raw time in BCD
	return (rxbuff);
}

UCHAR8* DS1307GetDate(void){		//Byte convert
	//Start I2C sequence to get bytes from address 0x00
	txbuff[0] = Day_Reg;
	rxbuff[0] = 0;
	//Set sequence to get time data
	status = I2CWriteData(txbuff,1,DS1307_I2C_ADDR);
	//read data from RTC for Date
	status = I2CReadData(rxbuff,4,DS1307_I2C_ADDR);
	return (rxbuff);
}

/* I2C Write Data*/
Status_Type I2CWriteData(UCHAR8* buffer , UCHAR8 len , UCHAR8 addr){
	/* Sets data to be send to RTC to init*/
	I2C_Tranfer_Cfg_Type i2ctx;			//Data structure to be used to send byte thru I2C Master Data Transfer

	// Fill Data Structure with proper data
	i2ctx.rxdata = 0;
	i2ctx.rxdatalen = 0;
	i2ctx.slave_addr = addr;
	i2ctx.txdata = buffer;
	i2ctx.txdatalen = len;
	// Send data to I2C
	status = I2C_MasterDataTransfer( &i2ctx );
	return status;
}

Status_Type I2CReadData(UCHAR8* buffer , UCHAR8 len , UCHAR8 addr){
	/* Sets data to be send to RTC to init*/
	I2C_Tranfer_Cfg_Type i2crx;			//Data structure to be used to send byte thru I2C Master Data Transfer

	// Fill Data Structure with proper data
	i2crx.rxdata = buffer;
	i2crx.rxdatalen = len;
	i2crx.slave_addr = addr;
	i2crx.txdata = 0;
	i2crx.txdatalen = 0;
	// Send data to I2C
	status = I2C_MasterDataTransfer( &i2crx );
	return status;
}

void DS1307_PrintTime(void){
	UCHAR8 *pbuff;
	UCHAR8 printbuff[30];
	UCHAR8 varampm;

	pbuff = DS1307GetTime();

	/*if (*(pbuff+2) && TIME_12){

		if ( *(pbuff+2) && HOUR_PM) varampm = PM;
			else varampm = AM;
	}
	else{
		sprintf(printbuff,"Time: %02X:%02X:%02X", *(pbuff+2),*(pbuff+1),*(pbuff));
		GLCD_putString(0,10,printbuff,OLED_COLOR_WHITE,OLED_COLOR_BLACK);
	}*/

}

void    DS1307_PrintDate(void){
	UCHAR8 *pbuff;
	UCHAR8 printbuff[30];
	pbuff = DS1307GetDate();
	sprintf(printbuff,"Date: %02X/%02X/20%02X %s\n", *(pbuff+1),*(pbuff+2),*(pbuff+3),weekdays[*(pbuff)]);
	GLCD_putString(0,20,printbuff,OLED_COLOR_WHITE,OLED_COLOR_BLACK);
}

//-------------------------------------------------------------
// This function converts an 8 bit binary value
// to an 8 bit BCD value.
// The input range must be from 0 to 99.

UCHAR8 bin2bcd(UCHAR8 binary_value){
	UCHAR8 temp;
	UCHAR8 retval;

	temp = binary_value;
	retval = 0;

	while(1){
	// Get the tens digit by doing multiple subtraction
	// of 10 from the binary value.
	if(temp >= 10){
		temp -= 10;
		retval += 0x10;
	}
	else // Get the ones digit by adding the remainder.
	{
		retval += temp;
		break;
		}
	}
	return(retval);
}

//--------------------------------------------------------------
// This function converts an 8 bit BCD value to
// an 8 bit binary value.
// The input range must be from 00 to 99.

UCHAR8 bcd2bin(UCHAR8 bcd_value){
	UCHAR8 temp;
	temp = bcd_value;
	
	// Shifting upper digit right by 1 is same as multiplying by 8.
	temp >>= 1;
	// Isolate the bits for the upper digit.
	temp &= 0x78;
	// Now return: (Tens * 8) + (Tens * 2) + Ones
	return(temp + (temp >> 2) + (bcd_value & 0x0f));
}

//-------------------- Formats date and time
void Transform_Time(SECONDS_type *secs, MINUTES_Type *mins, HOURS_Type *hour, WEEKDAY_Type *day, \
					DATE_Type *date, MONTH_Type *month , YEAR_Type *year) {

  secs->Seconds_Reg  	=  	((secs->Seconds_Reg & 0x70) >> 4)*10 + (secs->Seconds_Reg & 0x0F);
  mins->Minutes_reg  	=  	((mins->Minutes_reg & 0xF0) >> 4)*10 + (mins->Minutes_reg & 0x0F);
  hour->Hours_Register	=  	((hour->Hours_Register & 0x30) >> 4)*10 + (hour->Hours_Register & 0x0F);
  day->Weekday_Register =	(day->Weekday_Register & 0x07);
  date->Date_Register 	=  	((date->Date_Register & 0xF0) >> 4)*10 + (date->Date_Register & 0x0F);
  month->Month_Register	=  	((month->Month_Register & 0x10) >> 4)*10 + (month->Month_Register & 0x0F);
  year->Year_Register	=	((year->Year_Register& 0xF0)>>4)*10+(year->Year_Register & 0x0F);

}

//*********************************************************
// End Public Functions
//*********************************************************
