/* 
 * File:   T_18B20.h
 * Author: dsolano
 *
 * Created on June 27, 2014, 6:42 PM
 */

#ifndef T_18B20_H
#define	T_18B20_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include <types.h>

// Commands for DS18B20 Dallas Semiconductor 18B20
    // ROM Commands
#define SEARCH_ROM          0xF0    //Verify existance od Devices on multislave network
#define READ_ROM            0x33    //Get ROM Code out of Device
#define MATCH_ROM           0x55    //ID device with the address collected
#define SKIP_ROM            0xCC
#define ALARM_SEARCH        0xEC

    //Function Set Commands
#define CONVERT_T           0x44    // Start Temperature convertions
#define WRITE_SCRATCHPAD    0x4E    // Writes USer Byte and Config Register
#define READ_SCRATCHPAD     0xBE    // Read all 9 Bytes from memory on DS18B20
#define COPY_SCRATCHPAD     0x48    // Copy Memory to EEPROM on DS18B20
#define RECALL_E2           0xB8    // Restore EEPROM values to memory
#define READ_POWER_SUPPLY   0xB4    // Get the Power Supply status on DS18B20

    /*Temperature type currently stored in struct data*/
    typedef enum {
        CELSIUS = 0,
        FARENHEIT = 1
    } TEMP_TYPE_e;

    typedef struct DEVICE_DS18B20_s {

        struct ROMADDR_s {
            uint8_t ROMAddressID[8];
        } ROMADDR_s;

        struct CFGBITS_s {
            unsigned reserved : 5; //reads 1 always
            unsigned R0 : 1;
            unsigned R1 : 1;
            unsigned : 1; // reads 0 always
        } CONFIGBits_s;

        struct SPAD_s {
            UCHAR8 TempLow;
            UCHAR8 TempHi;
            UCHAR8 UserByte1;
            UCHAR8 UserByte2;
            UCHAR8 Config; //config register
            UCHAR8 reserved0; // unknown
            UCHAR8 reserved1; // reads 0x10
            UCHAR8 reserved2;
            UCHAR8 CRC; // Verify CRC calculations
        } ScratchPad_s;

        struct TEMP_s {
            UCHAR8 Temp_LowByte;
            UCHAR8 Temp_HighByte;
            UCHAR8 Temp_Whole;
            UCHAR8 Temp_Decimal;
            TEMP_TYPE_e Temp_Type;
            UCHAR8 Highest_Temp;
            UCHAR8 Lowest_Temp;
        } Temperature_s;
    } DS18b20_t;

    //DS18b20_t ds18b20; /* Create an instance object for DS18B20 */

//Define the type of Power Supply that a device has in a particular time.
typedef enum{
    PARASITE,
    EXTERNAL
}POWERTYPE_t;

// Funtions Prototypes
Bool DS18B20Init(DS18b20_t *tp);
Bool DS18B20CmdGetROMIDCode(DS18b20_t *tp);
Bool DS18B20ConfirmIDofDevice(DS18b20_t *tp);
Bool DS18B20WriteAlarm_CfgReg(UINT16 Alarm, UCHAR8 Config);
Bool  DS18B20GetTemperature(DS18b20_t *tp);
void DS18B20RestoreEEPROMValues(void);
void DS18B20SaveSRAMValues(void);
POWERTYPE_t DS18B20GetPWRSupplyStatus(void);
UINT8 DS18B20CalcAndCompareCRC(UCHAR8 _crc);
Bool DS18B20GetScrachtPadReg(DS18b20_t *tp);



#ifdef	__cplusplus
}
#endif

#endif	/* T_18B20_H */

