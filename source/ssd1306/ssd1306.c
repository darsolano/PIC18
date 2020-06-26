/*
 * File:   ssd1306.c
 * Must include ssd1306.h
 * Author: dsolano
 * I2C Driven SSD1306 or SH1106 SaintSmart OLED LCD 128x64
 * Created on July 5, 2014, 1:06 AM
 */
#include "ssd1306.h"
#include "font5x7.h"
#include "m_i2c.h"
#include <string.h>
#include <delay.h>


I2C_Results_Type status;
PRIVATE uint8_t buff[12];
PRIVATE uint8_t shadowFB[SHADOW_FB_SIZE];
PRIVATE uint8_t const  font_mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

PRIVATE I2C_Results_Type GLCDI2CWriteData(uint8_t* buffer , uint8_t len , uint8_t addr);

/** Set up everything so that the display is ready to start having text
  * rendered on it. By default, this will not turn on the display; use
  * displayOn() to do that. */

void GLCDssd1306Init(void){
    i2c_init(I2C_SPEED);
    GLCDResetSSD1306();
    dly_us((int)10);
}

/* I2C Write Data*/
PRIVATE I2C_Results_Type GLCDI2CWriteData(uint8_t* buffer , uint8_t len , uint8_t addr){
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


/** Turn display on. This must be called in order to have anything appear
  * on the screen. */
void GLCDisplayOn(void)
{
	GLCDWriteCommand( GLCD_DISP_ON); // display on
}

/** Turn display off. This will cause the SSD1306 controller to enter a
  * low power state. */
void GLCDisplayOff(void)
{
	GLCDWriteCommand( GLCD_DISP_OFF); // display off
}

/******************************************************************************
 *
 * Description:
 *    Write len number of same data to the display
 *
 * Params:
 *   [in] data - data (color) to write to the display
 *   [in] len  - number of bytes to write
 *
 *****************************************************************************/
static void GLCD_WriteDataLen(unsigned char data, unsigned int len)
{
    int i;
    uint8_t buf[140];

    buf[0] = 0x40; // write Co & D/C bits

    for (i = 1; i < len+1; i++) {
        buf[i] = data;
    }
	status = GLCDI2CWriteData(buf,len+1,SSD1306_I2C_ADDR);
}

/** Reset and initialise the SSD1306 display controller. This mostly follows
  * Figure 2 ("Software Initialization Flow Chart") on page 64 of the
  * SSD1306 datasheet.
  * This does not turn the display on (displayOn() does that). This is so
  * that display GDDRAM can be cleared. If the display is turned on
  * immediately, undefined junk will appear on the display.
  */
static void GLCDResetSSD1306(void)
{
    /*
     * Recommended Initial code according to manufacturer
     */

    GLCDWriteCommand(0x02);//set low column address
    GLCDWriteCommand(0x12);//set high column address
    GLCDWriteCommand(0x40);//(display start set)
    GLCDWriteCommand(0x2e);//(stop horzontal scroll)
    GLCDWriteCommand(0x81);//(set contrast control register)
    GLCDWriteCommand(0x7f);
    GLCDWriteCommand(0x82);//(brightness for color banks)
    GLCDWriteCommand(0x80);//(display on)
    GLCDWriteCommand(0xa1);//(set segment re-map)
    GLCDWriteCommand(0xa6);//(set normal/inverse display)
    //GLCDWriteCommand(0xa7);//(set inverse display)
    GLCDWriteCommand(0xa8);//(set multiplex ratio)
    GLCDWriteCommand(0x3F);
    GLCDWriteCommand(0xd3);//(set display offset)
    GLCDWriteCommand(0x40);
    GLCDWriteCommand(0xad);//(set dc-dc on/off)
    GLCDWriteCommand(0x8E);//
    GLCDWriteCommand(0xc8);//(set com output scan direction)
    GLCDWriteCommand(0xd5);//(set display clock divide ratio/oscillator/frequency)
    GLCDWriteCommand(0xf0);//
    GLCDWriteCommand(0xd8);//(set area color mode on/off & low power display mode )
    GLCDWriteCommand(0x05);//
    GLCDWriteCommand(0xd9);//(set pre-charge period)
    GLCDWriteCommand(0xF1);
    GLCDWriteCommand(0xda);//(set com pins hardware configuration)
    GLCDWriteCommand(0x12);
    GLCDWriteCommand(0xdb);//(set vcom deselect level)
    GLCDWriteCommand(0x34);
    GLCDWriteCommand(0x91);//(set look up table for area color)
    GLCDWriteCommand(0x3f);
    GLCDWriteCommand(0x3f);
    GLCDWriteCommand(0x3f);
    GLCDWriteCommand(0x3f);
    GLCDWriteCommand(0xaf);//(display on)
    GLCDWriteCommand(0xa4);//(display on)
    GLCD_WriteDataLen(OLED_COLOR_BLACK,132);
    dly_us(10);
}

/******************************************************************************
 *
 * Description:
 *    Clear the entire screen
 *
 * Params:
 *   [in] color - color to fill the screen with
 *
 *****************************************************************************/
void GLCD_ClearScreen(oled_color_t color)
{
    uint8_t i;
    uint8_t c = 0;

    if (color == OLED_COLOR_WHITE)
        c = 0xff;

    for(i=0xB0;i<0xB8;i++) {            // Go through all 8 pages
        mSetAddress(i,0x00,0x10);
        GLCD_WriteDataLen(c, 132);
    }
    memset(shadowFB, (int)c, SHADOW_FB_SIZE);
}

static void GLCDWriteCommand(uint8_t cmd)
{
	I2C_Tranfer_Cfg_Type glcdcfg;
	buff[0] = 0x00;	// write Co & D/C bits
	buff[1] = cmd;	// Command

	status = GLCDI2CWriteData(buff,2,SSD1306_I2C_ADDR);
	}

static void GLCDWriteData(uint8_t data){
	I2C_Tranfer_Cfg_Type glcdcfg;
	buff[0] = 0x40;	// write Co & D/C bits
	buff[1] = data;	// data

	status = GLCDI2CWriteData(buff,2,SSD1306_I2C_ADDR);
}


/******************************************************************************
 *
 * Description:
 *    Draw a horizontal line
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - y position
 *   [in] x1 - end y position
 *   [in] color - color of the pixel
 *
 *****************************************************************************/
static void GLCD_hLine(uint8_t x0, uint8_t y0, uint8_t x1, oled_color_t color)
{
    // TODO: optimize

    uint8_t bak;

    if (x0 > x1)
    {
        bak = x1;
        x1 = x0;
        x0 = bak;
    }

    while(x1 >= x0)
    {
        GLCD_putPixel(x0, y0, color);
        x0++;
    }
}

/******************************************************************************
 *
 * Description:
 *    Draw a vertical line
 *
 * Params:
 *   [in] x0 - x position
 *   [in] y0 - start y position
 *   [in] y1 - end y position
 *   [in] color - color of the pixel
 *
 *****************************************************************************/
static void GLCD_vLine(uint8_t x0, uint8_t y0, uint8_t y1, oled_color_t color)
{
    uint8_t bak;

    if(y0 > y1)
    {
        bak = y1;
        y1 = y0;
        y0 = bak;
    }

    while(y1 >= y0)
    {
        GLCD_putPixel(x0, y0, color);
        y0++;
    }
    return;
}


/******************************************************************************
 *
 * Description:
 *    Draw a line on the display starting at x0,y0 and ending at x1,y1
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - start y position
 *   [in] x1 - end x position
 *   [in] y1 - end y position
 *   [in] color - color of the line
 *
 *****************************************************************************/
void GLCD_Line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, oled_color_t color)
{
    int16_t   dx = 0, dy = 0;
    int8_t    dx_sym = 0, dy_sym = 0;
    int16_t   dx_x2 = 0, dy_x2 = 0;
    int16_t   di = 0;

    dx = x1-x0;
    dy = y1-y0;

    if(dx == 0)           /* vertical line */
    {
        GLCD_vLine(x0, y0, y1, color);
        return;
    }

    if(dx > 0)
    {
        dx_sym = 1;
    }
    else
    {
        dx_sym = -1;
    }


    if(dy == 0)           /* horizontal line */
    {
        GLCD_hLine(x0, y0, x1, color);
        return;
    }


    if(dy > 0)
    {
        dy_sym = 1;
    }
    else
    {
        dy_sym = -1;
    }

    dx = dx_sym*dx;
    dy = dy_sym*dy;

    dx_x2 = dx*2;
    dy_x2 = dy*2;

    if(dx >= dy)
    {
        di = dy_x2 - dx;
        while(x0 != x1)
        {

            GLCD_putPixel(x0, y0, color);
            x0 += dx_sym;
            if(di<0)
            {
                di += dy_x2;
            }
            else
            {
                di += dy_x2 - dx_x2;
                y0 += dy_sym;
            }
        }
        GLCD_putPixel(x0, y0, color);
    }
    else
    {
        di = dx_x2 - dy;
        while(y0 != y1)
        {
            GLCD_putPixel(x0, y0, color);
            y0 += dy_sym;
            if(di < 0)
            {
                di += dx_x2;
            }
            else
            {
                di += dx_x2 - dy_x2;
                x0 += dx_sym;
            }
        }
        GLCD_putPixel(x0, y0, color);
    }
    return;
}

/******************************************************************************
 *
 * Description:
 *    Draw a circle on the display starting at x0,y0 with radius r
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - start y position
 *   [in] r - radius
 *   [in] color - color of the circle
 *
 *****************************************************************************/
void GLCD_circle(uint8_t x0, uint8_t y0, uint8_t r, oled_color_t color)
{
    int16_t draw_x0, draw_y0;
    int16_t draw_x1, draw_y1;
    int16_t draw_x2, draw_y2;
    int16_t draw_x3, draw_y3;
    int16_t draw_x4, draw_y4;
    int16_t draw_x5, draw_y5;
    int16_t draw_x6, draw_y6;
    int16_t draw_x7, draw_y7;
    int16_t xx, yy;
    int16_t  di;

    if(r == 0)          /* no radius */
    {
        return;
    }

    draw_x0 = draw_x1 = x0;
    draw_y0 = draw_y1 = y0 + r;
    if(draw_y0 < OLED_DISPLAY_HEIGHT)
    {
        GLCD_putPixel(draw_x0, draw_y0, color);     /* 90 degree */
    }

    draw_x2 = draw_x3 = x0;
    draw_y2 = draw_y3 = y0 - r;
    if(draw_y2 >= 0)
    {
        GLCD_putPixel(draw_x2, draw_y2, color);    /* 270 degree */
    }

    draw_x4 = draw_x6 = x0 + r;
    draw_y4 = draw_y6 = y0;
    if(draw_x4 < OLED_DISPLAY_WIDTH)
    {
        GLCD_putPixel(draw_x4, draw_y4, color);     /* 0 degree */
    }

    draw_x5 = draw_x7 = x0 - r;
    draw_y5 = draw_y7 = y0;
    if(draw_x5>=0)
    {
        GLCD_putPixel(draw_x5, draw_y5, color);     /* 180 degree */
    }

    if(r == 1)
    {
        return;
    }

    di = 3 - 2*r;
    xx = 0;
    yy = r;
    while(xx < yy)
    {

        if(di < 0)
        {
            di += 4*xx + 6;
        }
        else
        {
            di += 4*(xx - yy) + 10;
            yy--;
            draw_y0--;
            draw_y1--;
            draw_y2++;
            draw_y3++;
            draw_x4--;
            draw_x5++;
            draw_x6--;
            draw_x7++;
        }
        xx++;
        draw_x0++;
        draw_x1--;
        draw_x2++;
        draw_x3--;
        draw_y4++;
        draw_y5++;
        draw_y6--;
        draw_y7--;

        if( (draw_x0 <= OLED_DISPLAY_WIDTH) && (draw_y0>=0) )
        {
            GLCD_putPixel(draw_x0, draw_y0, color);
        }

        if( (draw_x1 >= 0) && (draw_y1 >= 0) )
        {
            GLCD_putPixel(draw_x1, draw_y1, color);
        }

        if( (draw_x2 <= OLED_DISPLAY_WIDTH) && (draw_y2 <= OLED_DISPLAY_HEIGHT) )
        {
            GLCD_putPixel(draw_x2, draw_y2, color);
        }

        if( (draw_x3 >=0 ) && (draw_y3 <= OLED_DISPLAY_HEIGHT) )
        {
            GLCD_putPixel(draw_x3, draw_y3, color);
        }

        if( (draw_x4 <= /*OLED_DISPLAY_HEIGHT*/OLED_DISPLAY_WIDTH) && (draw_y4 >= 0) )
        {
            GLCD_putPixel(draw_x4, draw_y4, color);
        }

        if( (draw_x5 >= 0) && (draw_y5 >= 0) )
        {
            GLCD_putPixel(draw_x5, draw_y5, color);
        }
        if( (draw_x6 <= OLED_DISPLAY_WIDTH) && (draw_y6 <= OLED_DISPLAY_HEIGHT) )
        {
            GLCD_putPixel(draw_x6, draw_y6, color);
        }
        if( (draw_x7 >= 0) && (draw_y7 <= OLED_DISPLAY_HEIGHT) )
        {
            GLCD_putPixel(draw_x7, draw_y7, color);
        }
    }
    return;
}

/******************************************************************************
 *
 * Description:
 *    Draw a rectangle on the display starting at x0,y0 and ending at x1,y1
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - start y position
 *   [in] x1 - end x position
 *   [in] y1 - end y position
 *   [in] color - color of the rectangle
 *
 *****************************************************************************/
void GLCD_rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, oled_color_t color)
{
    GLCD_hLine(x0, y0, x1, color);
    GLCD_hLine(x0, y1, x1, color);
    GLCD_vLine(x0, y0, y1, color);
    GLCD_vLine(x1, y0, y1, color);
}

/******************************************************************************
 *
 * Description:
 *    Fill a rectangle on the display starting at x0,y0 and ending at x1,y1
 *
 * Params:
 *   [in] x0 - start x position
 *   [in] y0 - start y position
 *   [in] x1 - end x position
 *   [in] y1 - end y position
 *   [in] color - color of the rectangle
 *
 *****************************************************************************/
void GLCD_fillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, oled_color_t color)
{
    uint8_t i = 0;

    if(x0 > x1)
    {
        i  = x0;
        x0 = x1;
        x1 = i;
    }

    if(y0 > y1)
    {
        i  = y0;
        y0 = y1;
        y1 = i;
    }

    if(y0 == y1)
    {
        GLCD_hLine(x0, y0, x1, color);
        return;
    }

    if(x0 == x1)
    {
        GLCD_vLine(x0, y0, y1, color);
        return;
    }

    while(y0 <= y1)
    {
        GLCD_hLine(x0, y0, x1, color);
        y0++;
    }
    return;
}

UINT8 GLCD_putChar(UINT8 x, UINT8 y, UINT8 ch, oled_color_t fb, oled_color_t bg)
{
    UCHAR8 data = 0;
    UCHAR8 i = 0, j = 0;

    oled_color_t color = OLED_COLOR_BLACK;

    if((x >= (OLED_DISPLAY_WIDTH - 8)) || (y >= (OLED_DISPLAY_HEIGHT - 8)) )
    {
        return 0;
    }

    if( (ch < 0x20) || (ch > 0x7f) )
    {
        ch = 0x20;      /* unknown character will be set to blank */
    }

    ch -= 0x20;
    for(i=0; i<8; i++)
    {
        data = font5x7[ch][i];
        for(j=0; j<6; j++)
        {
            if( (data&font_mask[j])==0 )
            {
                color = bg;
            }
            else
            {
                color = fb;
            }
            GLCD_putPixel(x, y, color);
            x++;
        }
        y++;
        x -= 6;
    }
    return( 1 );
}

void GLCD_putString(UINT8 x, UINT8 y, UINT8 *pStr, oled_color_t fb, oled_color_t bg)
{
  while(1)
  {
      if( (*pStr)=='\0' )
      {
          break;
      }
      if( GLCD_putChar(x, y, *pStr++, fb, bg) == 0 )
      {
        break;
    }
    x += 6;
  }
  return;
}

/******************************************************************************
 *
 * Description:
 *    Draw one pixel on the display
 *
 * Params:
 *   [in] x - x position
 *   [in] y - y position
 *   [in] color - color of the pixel
 *
 *****************************************************************************/
void GLCD_putPixel(UINT8 x, UINT8 y, oled_color_t color) {
    UINT8 page;
    UINT16 add;
    UINT8 lAddr;
    UINT8 hAddr;
    UINT8 mask;
    UINT32 shadowPos = 0;

    if (x > OLED_DISPLAY_WIDTH) {
        return;
    }
    if (y > OLED_DISPLAY_HEIGHT) {
        return;
    }

    /* page address */
         if(y < 8)  page = 0xB0;
    else if(y < 16) page = 0xB1;
    else if(y < 24) page = 0xB2;
    else if(y < 32) page = 0xB3;
    else if(y < 40) page = 0xB4;
    else if(y < 48) page = 0xB5;
    else if(y < 56) page = 0xB6;
    else            page = 0xB7;

    add = x + X_OFFSET;
    lAddr = 0x0F & add;             // Low address
    hAddr = 0x10 | (add >> 4);      // High address

    // Calculate mask from rows basically do a y%8 and remainder is bit position
    add = y>>3;                     // Divide by 8
    add <<= 3;                      // Multiply by 8
    add = y - add;                  // Calculate bit position
    mask = 1 << add;                // Left shift 1 by bit position

    mSetAddress(page, lAddr, hAddr) // Set the address (sets the page,
                                    // lower and higher column address pointers)

    shadowPos = (page-0xB0)*OLED_DISPLAY_WIDTH+x;

    if(color > 0)
        shadowFB[shadowPos] |= mask;
    else
        shadowFB[shadowPos] &= ~mask;

    GLCDWriteData(shadowFB[shadowPos]);
}
