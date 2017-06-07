/*
 ******************************************************************************  
 */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SSD1963_H
#define __SSD1963_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stm32f4xx.h>

/* LCD controller definition */
#define LCDC_SSD1963		
	 
#define  HT  		532	 // horisontal total         
#define  HDP	 	479  // horizontal width     480
#define  HFP   	8		// horizontal front porch 2-8
#define  HBP   	42		//horizontal back porch   8-43
#define  HS    	2	// horizontal pulse width   1-

#define  VT    288 		// vertical total
#define  VDP	 271	// vertical width
#define  VFP   2			// vertical front porch
#define  VBP   6			// vertical back porch	
#define  VS    6			// vertical pulse width

	 
#ifdef LCDC_SSD1963

// SSD 1963 Command Table, see manual p.20
#define CMD_NOP 				0x0000
#define CMD_SOFT_RESET			0x0001
#define CMD_GET_PWR_MODE 		0x000A
#define CMD_GET_ADDR_MODE		0x000B
#define CMD_GET_PIXEL_FMT		0x000C
#define CMD_GET_DISPLAY_MODE 	0x000D
#define CMD_GET_SIGNAL_MODE		0x000E
#define CMD_RESERVED_1			0x000F

// Operation modes
#define CMD_ENTER_SLEEP			0x0010
#define CMD_EXIT_SLEEP			0x0011
#define CMD_ENTER_PARTIAL		0x0012
#define CMD_ENTER_NORMAL		0x0013
#define CMD_EXIT_INVERT			0x0020
#define CMD_ENTER_INVERT		0x0021

#define CMD_SET_GAMMA_CURVE		0x0026
#define CMD_SET_DISPLAY_OFF		0x0028
#define CMD_SET_DISPLAY_ON		0x0029

#define CMD_SET_COL_ADDRESS		0x002A
#define CMD_SET_PAGE_ADDRESS	0x002B

#define CMD_WRITE_MEM_START		0x002C
#define CMD_READ_MEM_START		0x002E

#define CMD_SET_PARTIAL_AREA	0x0030
#define CMD_SET_SCROLL_AREA		0x0033

#define CMD_SET_TEAR_OFF		0x0034
#define CMD_SET_TEAR_ON			0x0035

#define CMD_SET_ADDRESS_MODE	0x0036
#define CMD_SET_SCROLL_START	0x0037

#define CMD_EXIT_IDLE_MODE		0x0038
#define CMD_ENTER_IDLE_MODE		0x0039

#define CMD_SET_PIXEL_FMT		0x003A
#define CMD_WRITE_MEM_CONT		0x003C
#define CMD_READ_MEM_CONT		0x003E

#define CMD_SET_TEAR_SCANLINE	0x0044
#define CMD_GET_SCANLINE		0x0045

#define CMD_READ_DDB			0x00A1
#define CMD_RESERVED_2			0x00A8

#define CMD_SET_LCD_MODE		0x00B0
#define CMD_GET_LCD_MODE		0x00B1

#define CMD_SET_HORIZ_PERIOD	0x00B4
#define CMD_GET_HORIZ_PERIOD	0x00B5

#define CMD_SET_VERT_PERIOD		0x00B6
#define CMD_GET_VERT_PERIOD		0x00B7

#define CMD_SET_GPIO_CONF		0x00B8
#define CMD_GET_GPIO_CONF		0x00B9
#define CMD_SET_GPIO_VAL		0x00BA
#define CMD_GET_GPIO_STATUS		0x00BB

#define CMD_SET_POST_PROC		0x00BC
#define CMD_GET_POST_PROC		0x00BD

#define CMD_SET_PWM_CONF		0x00BE
#define CMD_GET_PWM_CONF		0x00BF

#define CMD_SET_LCD_GEN0		0x00C0
#define CMD_GET_LCD_GEN0		0x00C1
#define CMD_SET_LCD_GEN1		0x00C2
#define CMD_GET_LCD_GEN1		0x00C3
#define CMD_SET_LCD_GEN2		0x00C4
#define CMD_GET_LCD_GEN2		0x00C5
#define CMD_SET_LCD_GEN3		0x00C6
#define CMD_GET_LCD_GEN3		0x00C7

#define CMD_SET_GPIO0_ROP		0x00C8
#define CMD_GET_GPIO0_ROP		0x00C9
#define CMD_SET_GPIO1_ROP		0x00CA
#define CMD_GET_GPIO1_ROP		0x00CB
#define CMD_SET_GPIO2_ROP		0x00CC
#define CMD_GET_GPIO2_ROP		0x00CD
#define CMD_SET_GPIO3_ROP		0x00CE
#define CMD_GET_GPIO3_ROP		0x00CF

#define CMD_SET_DBC_CONF		0x00D0
#define CMD_GET_DBC_CONF		0x00D1
#define CMD_SET_DBC_THRESH		0x00D4
#define CMD_GET_DBC_THRESH		0x00D5

#define CMD_SET_PLL				0x00E0
#define CMD_SET_PLL_MN			0x00E2
#define CMD_GET_PLL_MN			0x00E3
#define CMD_GET_PLL_STATUS		0x00E4
#define CMD_SET_DEEP_SLEEP		0x00E5
#define CMD_SET_PIXCLK_FREQ		0x00E6
#define CMD_GET_PIXCLK_FREQ		0x00E7

#define CMD_RESERVED_3			0x00E8
#define CMD_RESERVED_4			0x00E9

#define CMD_SET_PIXDATA_IF_FMT	0x00F0
#define CMD_GET_PIXDATA_IF_FMT	0x00F1

#define CMD_RESERVED_5			0x00FF

#endif // LCDC_SSD1963

#define WHITE          0xFFFF
#define BLACK          0x0000
#define GREY           0xB5B6
#define BLUE           0x001F
#define BLUE2          0x051F
#define RED            0xF800
#define MAGENTA        0xF81F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define YELLOW         0xFFE0


	

void LCD_BacklightOn(void);
void LCD_BacklightOff(void);
void LCD_Test(void);
void delay(uint32_t uSec);
void LCD_Test_BlackToWhite(void);
void LCD_DrawRectangle(int x1, int y1, int x2, int y2, short red, short green, short blue);	 


void LCD_Reset(void);
void LCD_WR_REG(unsigned int command);
unsigned int LCD_RD_REG(unsigned int command);
void LCD_WR_CMD(unsigned int index, unsigned int val);
unsigned int LCD_RD_Data(void);
void LCD_WR_Data(unsigned int val);	 

void LCD_Clear(unsigned int);

// PiXCLe Primitives Declarations
void LCD_DrawCheckerPattern(short blue, short green, short red);
void LCD_DrawBackground(short color);
void LCD_DrawRectangle(int x1, int y1, int x2, int y2, short blue, short green, short red);


void LCD_DrawBMP(const unsigned short *bitmap);
void LCD_Test_BlackToWhite(void);
void SSD1963_LowLevel_Init(void);
void ssd1963_Init(void);
void LCD_DrawRectFull(uint16_t x,uint16_t y,uint16_t width,uint16_t heigh,uint16_t color);
void LCD_DrawRect(uint16_t x,uint16_t y,uint16_t width,uint16_t heigh,uint16_t color);
void LCD_DrawLineH(uint16_t x,uint16_t y,uint16_t width, uint16_t color);
void LCD_DrawLineV(uint16_t x,uint16_t y,uint16_t heigh, uint16_t color);
void LCD_PutPixel(uint16_t x,uint16_t y,uint16_t color);
void Set_COL_PAGE_Address(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void LCD_DrawEllipse(int x1, int y1, int x2, int y2, short color);
void SetScrollArea(unsigned int top, unsigned int scroll, unsigned int bottom);
void SetScrollStart(unsigned int line);
void SetTearingCfg(unsigned char state, unsigned char mode);
#if 0



#define LCD_LINE_0               LINE(0)
#define LCD_LINE_1               LINE(1)
#define LCD_LINE_2               LINE(2)
#define LCD_LINE_3               LINE(3)
#define LCD_LINE_4               LINE(4)
#define LCD_LINE_5               LINE(5)
#define LCD_LINE_6               LINE(6)
#define LCD_LINE_7               LINE(7)
#define LCD_LINE_8               LINE(8)
#define LCD_LINE_9               LINE(9)
#define LCD_LINE_10              LINE(10)
#define LCD_LINE_11              LINE(11)
#define LCD_LINE_12              LINE(12)
#define LCD_LINE_13              LINE(13)
#define LCD_LINE_14              LINE(14)
#define LCD_LINE_15              LINE(15)
#define LCD_LINE_16              LINE(16)
#define LCD_LINE_17              LINE(17)
#define LCD_LINE_18              LINE(18)
#define LCD_LINE_19              LINE(19)
#define LCD_LINE_20              LINE(20)
#define LCD_LINE_21              LINE(21)
#define LCD_LINE_22              LINE(22)
#define LCD_LINE_23              LINE(23)
#define LCD_LINE_24              LINE(24)
#define LCD_LINE_25              LINE(25)
#define LCD_LINE_26              LINE(26)
#define LCD_LINE_27              LINE(27)
#define LCD_LINE_28              LINE(28)
#define LCD_LINE_29              LINE(29)
#define LCD_DEFAULT_FONT         Font16x24
#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001
#define LCD_PIXEL_WIDTH          0x0140
#define LCD_PIXEL_HEIGHT         0x00F0

#define ASSEMBLE_RGB(R, G, B)    ((((R)& 0xF8) << 8) | (((G) & 0xFC) << 3) | (((B) & 0xF8) >> 3))  


void LCD_DeInit(void);  
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor); 
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor);
void LCD_SetTextColor(__IO uint16_t Color);
void LCD_SetBackColor(__IO uint16_t Color);
void LCD_ClearLine(uint8_t Line);
void LCD_Clear(uint16_t Color);
void LCD_SetCursor(uint8_t Xpos, uint16_t Ypos);
void LCD_DrawChar(uint8_t Xpos, uint16_t Ypos, const uint16_t *c);
void LCD_DisplayChar(uint8_t Line, uint16_t Column, uint8_t Ascii);
extern void LCD_SetFont(sFONT *fonts);
sFONT *LCD_GetFont(void);
void LCD_DisplayStringLine(uint8_t Line, uint8_t *ptr);
void LCD_SetDisplayWindow(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void LCD_WindowModeDisable(void);
void LCD_DrawLine(uint8_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction);
void LCD_DrawRect(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void LCD_DrawCircle(uint8_t Xpos, uint16_t Ypos, uint16_t Radius);
void LCD_DrawMonoPict(const uint32_t *Pict);
void LCD_WriteBMP(uint32_t BmpAddress);
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void LCD_PolyLine(pPoint Points, uint16_t PointCount);
void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount);
void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount);
void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount);
void LCD_FillPolyLine(pPoint Points, uint16_t PointCount);
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
uint16_t LCD_ReadReg(uint8_t LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t RGB_Code);
uint16_t LCD_ReadRAM(void);
void LCD_PowerOn(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_CtrlLinesConfig(void);
void LCD_FSMCConfig(void);



#endif

#ifdef __cplusplus
}
#endif

#endif /* __MINISTM32_LCD_H */
  
/****END OF FILE****/
