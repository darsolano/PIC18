/*
 * pic32mx360f512l
   T_18B20.h - library for 18B20 Thermometer
 * Created on June 26, 2014, 12:36 AM
*/

#include "T_18B20.h"
#include "_1wire.h"
#include <delay.h>

Bool DS18B20Init(DS18b20_t *tp){
    if (!OW_reset_pulse()) return FALSE;
    tp->Temperature_s.Temp_Type = CELSIUS;
    if (DS18B20CmdGetROMIDCode(tp) && DS18B20GetScrachtPadReg(tp) && DS18B20GetTemperature(tp))
    {
        tp->Temperature_s.Lowest_Temp = tp->Temperature_s.Temp_Whole;
        return TRUE;
    }
    return FALSE;
}

Bool DS18B20CmdGetROMIDCode(DS18b20_t *tp){
    UINT8 loop;
    if (!OW_reset_pulse()) return FALSE;
    OW_write_byte(READ_ROM);
    for (loop = 0; loop < 8; loop++)
    {
        tp->ROMADDR_s.ROMAddressID[loop] = OW_read_byte();
    }
    return TRUE;
}

Bool DS18B20ConfirmIDofDevice(DS18b20_t *tp){
    UCHAR8 i;
    if(OW_reset_pulse()) return FALSE;
    OW_write_byte(MATCH_ROM); // match ROM
    for(i=0;i<8;i++){
        OW_write_byte(tp->ROMADDR_s.ROMAddressID[i]); //send ROM code
        dly_us(10);
    }
    return TRUE;
}

Bool DS18B20WriteAlarm_CfgReg(UINT16 Alarm, UCHAR8 Config){
    return TRUE;
}

Bool  DS18B20GetTemperature(DS18b20_t *tp){
    int whole=0, decimal=0;
    int c16,c2,f10;

    if (!OW_reset_pulse()) return FALSE;
    OW_write_byte(SKIP_ROM); //Skip ROM
    OW_write_byte(CONVERT_T); // Start Conversion
    dly_us(5);
    OW_reset_pulse();
    OW_write_byte(SKIP_ROM); // Skip ROM
    OW_write_byte(READ_SCRATCHPAD); // Read Scratch Pad
    //from scratchpad read, the first 2 byte are from temp register, others are dropped
    tp->ScratchPad_s.TempLow = OW_read_byte();
    tp->ScratchPad_s.TempHi = OW_read_byte();

    if (!OW_reset_pulse()) return FALSE;

    if (tp->Temperature_s.Temp_Type == CELSIUS)
    {
        // Celsius calculation
        tp->Temperature_s.Temp_Whole = (tp->ScratchPad_s.TempHi & 0x07) << 4; // grab lower 3 bits of t1
        tp->Temperature_s.Temp_Whole |= tp->ScratchPad_s.TempLow >> 4; // and upper 4 bits of t0
        tp->Temperature_s.Temp_Decimal = tp->ScratchPad_s.TempLow & 0x0F; // decimals in lower 4 bits of t0
        tp->Temperature_s.Temp_Decimal *= 625; // conversion factor for 12-bit resolution
        
        /* Set the high and low temperature reading */
        if (tp->Temperature_s.Temp_Whole >= tp->Temperature_s.Highest_Temp)
            tp->Temperature_s.Highest_Temp = tp->Temperature_s.Temp_Whole;

        if (tp->Temperature_s.Temp_Whole <= tp->Temperature_s.Lowest_Temp)
            tp->Temperature_s.Lowest_Temp = tp->Temperature_s.Temp_Whole;
    }
    /* Start Farenheit convertion */
    if (tp->Temperature_s.Temp_Type == FARENHEIT)
    {
        // Farenheit convertion
        c16 = (tp->ScratchPad_s.TempHi << 8) + tp->ScratchPad_s.TempLow; // result is temp*16, in celcius
        c2 = c16 / 8; // get t*2, with fractional part lost
        f10 = c16 + c2 + 320; // F=1.8C+32, so 10F = 18C+320 = 16C + 2C + 320
        tp->Temperature_s.Temp_Whole = f10 / 10; // get whole part
        tp->Temperature_s.Temp_Decimal = f10 % 10; // get fractional part
        /* Set the high and low temperature reading */
        if (tp->Temperature_s.Temp_Whole >= tp->Temperature_s.Highest_Temp)
            tp->Temperature_s.Highest_Temp = tp->Temperature_s.Temp_Whole;

        if (tp->Temperature_s.Temp_Whole <= tp->Temperature_s.Lowest_Temp)
            tp->Temperature_s.Lowest_Temp = tp->Temperature_s.Temp_Whole;
    }
    return TRUE;
}       

void DS18B20RestoreEEPROMValues(void){

}

void DS18B20SaveSRAMValues(void){

}

POWERTYPE_t DS18B20GetPWRSupplyStatus(void){
    return PARASITE;
}

UINT8 DS18B20CalcAndCompareCRC(UCHAR8 _crc){
    return 0;
}

Bool DS18B20GetScrachtPadReg(DS18b20_t *tp){
    if (!OW_reset_pulse()) return FALSE;
    OW_write_byte(SKIP_ROM); // Skip ROM
    OW_write_byte(READ_SCRATCHPAD); // Read Scratch Pad
    tp->ScratchPad_s.TempLow = OW_read_byte();
    tp->ScratchPad_s.TempHi = OW_read_byte();
    tp->ScratchPad_s.UserByte1 = OW_read_byte();
    tp->ScratchPad_s.UserByte2 = OW_read_byte();
    tp->ScratchPad_s.Config = OW_read_byte();
    tp->ScratchPad_s.reserved0 = OW_read_byte();
    tp->ScratchPad_s.reserved1 = OW_read_byte();
    tp->ScratchPad_s.reserved2 = OW_read_byte();
    tp->ScratchPad_s.CRC = OW_read_byte();
    return TRUE;
}