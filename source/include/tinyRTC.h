/* 
 * File:   tinyRTC.h
 * Author: dsolano
 *
 * Created on June 16, 2014, 1:05 PM
 *  TinyRTCC v1.1 by weiman
 */

#ifndef TINYRTC_H
#define	TINYRTC_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "m_i2c.h"

//********************************************************
//Definition for DS1307Z 64 x 8, Serial, I2C Real-Time Clock + 24C32WP + 18B09 1 wire
//********************************************************

// I2C addres for DS1307
#define DS1307_I2C_ADDR             0x68
#define DS1307_I2C_CLOCK_FREQ       400000      // 400KHz

//Control & Commands for RTC - to be ored or andded with content to be sent
#define ROM_START               	0x08
#define ROM_END                 	0x3f

//SETUP FOR RS BIT IN CONTROL REGISTER ADDRESS 0x07
#define OUT_DISABLE             	0

#define EEP_RTC_START           	0x08
#define EEP_RTC_END             	0x3f

#define TIME_12                        0b01000000;
#define HOUR_PM                        0b00100000;

    typedef enum{
        success,
        errors
    }Status_Type;

// Time mode for Time in Hours register
typedef enum Time_Mode{
	Hours_24H,
	Hours_12H
}e_TIME_MODE_Type;

// Hour Mode in Hours register
typedef enum Hour_Mode{
	AM,
	PM
}e_HOURS_MODE_Type;

// Days Of the Week
typedef enum Days{
    Sunday      =   1,
    Monday      =   2,
    Tuesday     =   3,
    Wednesday   =   4,
    Thursday    =   5,
    Friday      =   6,
    Saturday    =   7
}e_DaysOfWeek_Type;

// Control Register of DS1307
typedef enum Register_Addr{
	Secs_Reg,
	Mins_Reg,
	Hour_Reg,
	Day_Reg,
	Date_Reg,
	Month_Reg,
	Year_Reg,
	Control_Reg
}e_DS1307_REG_Type;

// RS0 and RS1 values for Clock Output on Control Register
typedef enum SQWE{
	CLOCK_OUT_1Hz,
	CLOCK_OUT_4_096kHz,
	CLOCK_OUT_8_192kHz,
	CLOCK_OUT_32_768kHz
}e_CLOCK_OUT_Type;

// Clock Halt bit
typedef enum Clock_Halt{
	Start_RTC,
	Stop_RTC
}e_RTC_Type;

// Seconds Register
typedef union {
    struct{
	unsigned Seconds_Units	:	4;
	unsigned Seconds_Tenth	:	3;
	unsigned Clock_Halt	:	1;
	};
    struct{
	UCHAR8 Seconds_Reg;
    };
}SECONDS_type;

// Minutes Register
typedef union {
    struct{
        unsigned Mins_Units	:	4;
	unsigned Mins_Tenth	:	3;
	unsigned               :        1;
	};
    struct{
	UCHAR8 Minutes_reg;
    };
}MINUTES_Type;

// Hours Register
typedef union {
    struct{
        unsigned Hours_Units_24		:	4;
	unsigned Hours_Tenth_24		:	2;
	unsigned Time_Mode_24		:	1;
	unsigned notused                :	1;
}HOURS_24_Type;
    struct{
	unsigned Hours_Units		:	4;
	unsigned Hours_Tenth		:	1;
	unsigned Hours_Mode		:	1;
	unsigned Time_Mode		:	1;
	unsigned			:	1;
    }HOURS_12_Type;
    struct{
	UCHAR8 Hours_Register;
    };
}HOURS_Type;

// Day Of The Week Register
typedef union {
    struct{
	unsigned Weekday		:	3;
	unsigned notused		:	5;
    };
    struct{
	UCHAR8 Weekday_Register;
    };
}WEEKDAY_Type;

// Date Register
typedef union {
    struct{
	unsigned Date_Units	:	4;
	unsigned Date_Tenth	:	2;
	unsigned notused	:	2;
    };
    struct{
	UCHAR8 Date_Register;
    };
}DATE_Type;

// Month Register
typedef union {
    struct{
	unsigned Month_Units			:	4;
	unsigned Month_Tenth			:	1;
	unsigned notused			:	3;
	};
    struct{
	UCHAR8 Month_Register;
	};
}MONTH_Type;

// Year Register
typedef union {
    struct{
	unsigned Year_Units			:	4;
	unsigned Year_Tenth			:	4;
    };
    struct{
	UCHAR8 Year_Register;
    };
}YEAR_Type;


// SQWE Bit in Control register
typedef enum SQWE_Mode{
	SQWE_Dis,
	SQWE_Enb
}e_SQWE_Type;

// Control Register
typedef union {
    struct{
	unsigned Rate_Select	:	2;
	unsigned		:	2;
	unsigned SQWE           :	1;
	unsigned		:	2;
	unsigned OUT		:	1;
	};
    struct{
    	UCHAR8 Control_Register;
    };
}CONTROL_reg_Type;

// Structure to hold binary values for Time and Date registers
typedef struct{
    UCHAR8 seconds_r;
    UCHAR8 minutes_r;
    UCHAR8 hours_r;
    UCHAR8 day_r;
    UCHAR8 date_r;
    UCHAR8 month_r;
    UCHAR8 year_r;
}DS1307_REGS_Type;

/* Function Prototypes*/
//********DS1307 RTC I2C Address 0x68
Status_Type 	I2CWriteData(UCHAR8* buffer , UCHAR8 len , UCHAR8 addr);
Status_Type     I2CReadData(UCHAR8* buffer , UCHAR8 len , UCHAR8 addr);
Status_Type     DS1307SetTime(UCHAR8 sec, UCHAR8 min, HOURS_Type* hourcfg);
Status_Type     DS1307SetDate(DS1307_REGS_Type*);
void    DS1307_PrintTime(void);
void    DS1307_PrintDate(void);
void    DS1307_Init(void);
void	DS1307_DeInit(void);
void    SetTimeFMT(UCHAR8 format);
void    DS1307SaveRAMByte(UCHAR8 address, UCHAR8 data);
UCHAR8  DS1307ReadRAMByte(UCHAR8 address);
UCHAR8* DS1307GetTime(void);
UCHAR8* DS1307GetDate(void);
UCHAR8  bcd2bin(UCHAR8 bcd_value);
UCHAR8  bin2bcd(UCHAR8 binary_value);
void Transform_Time(SECONDS_type *secs, MINUTES_Type *mins, HOURS_Type *hour, WEEKDAY_Type *day, \
					DATE_Type *date, MONTH_Type *month , YEAR_Type *year);


#ifdef	__cplusplus
}
#endif

#endif	/* TINYRTC_H */

