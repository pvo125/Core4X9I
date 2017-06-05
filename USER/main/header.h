
#ifndef _HEADER_H_
#define _HEADER_H_

#include <stm32f4xx.h>
#include "GUI.h"
#include "DIALOG.h"
#include "ff.h"
#include "stm324x9i_eval_sdio_sd.h"
#include "ssd1963.h"
#include "mx25l8005.h"
#include "LCDConf.h"
#include "tsc2046.h"
#include "SDRAM.h"

#define ID_PROGBAR_MEM     (GUI_ID_USER + 0x27)

#define MY_MESSAGE     (WM_USER + 0x01)
// #define FLASHCODE
#define SCREEN_1		0
#define SCREEN_2		272

//#define SRAM
#define NAND

#ifdef SRAM 
#define LCD_BASE           ((uint32_t)(0x60000000))
#define LCD_REG_ADDRESS   (*(volatile U16*)LCD_BASE)
#define LCD_DATA_ADDRESS  (*(volatile U16*)(LCD_BASE + 0x20000))
#endif
#ifdef NAND
#define LCD_BASE           ((uint32_t)(0x70000000))
#define LCD_REG_ADDRESS   (*(volatile U16*)LCD_BASE )
#define LCD_DATA_ADDRESS  (*(volatile U16*)(LCD_BASE+0x10000))
#endif

#define ID_PROGBAR_MEM     (GUI_ID_USER + 0x27)

#define NONE (uint32_t)0x00000000
#define START_PWM (uint32_t)0x00000001
#define STOP_PWM	(uint32_t)0x00000002
#define START_ADC (uint32_t)0x00000003
#define STOP_ADC 	(uint32_t)0x00000004
#if 1
#define exitt   			SDRAM_BASE+0xB0000
#define photo					SDRAM_BASE+0xB1214
#define screen 				SDRAM_BASE+0xB2428				
#define paint  				SDRAM_BASE+0xB363C				
#define next   				SDRAM_BASE+0xB4850
#define prev   				SDRAM_BASE+0xB5A64
#define date   				SDRAM_BASE+0xB6C78
#define pwm    				SDRAM_BASE+0xB7E8C
#define sd     				SDRAM_BASE+0xB90A0
#define alarm  				SDRAM_BASE+0xBA2B4

#define AlarmA  			SDRAM_BASE+0xBB4C8
#define AlarmB				SDRAM_BASE+0xBB95C
#define Alarm_d				SDRAM_BASE+0xBBDF0
#define add_folder		SDRAM_BASE+0xBC284
#define del_folder   	SDRAM_BASE+0xBC718
	

#define fexit   0x00
#define fphoto	0x1300
#define fscreen 0x2600				
#define fpaint  0x3900				
#define fnext   0x4C00
#define fprev   0x5F00
#define fdate   0x7200
#define fpwm    0x8500
#define fsd     0x9800
#define falarm  0xAB00

#define fAlarmA  			0xBE00
#define fAlarmB				0xC300
#define fAlarm_d			0xC800
#define fadd_folder		0xCD00
#define fdel_folder   0xD200
#endif

#define BRIGHTNESS_LOW  0x0030
#define BRIGHTNESS_MED  0x0090
#define BRIGHTNESS_HIGH 0x00f0
typedef enum{
	 BACKLIGHT_OFF=0,
	 BACKLIGHT_LOW,
	 BACKLIGHT_ON
}BACKLIGHT_TypeDef;


extern WIDGET_DRAW_ITEM_FUNC *pWIDGET_DRAW_ITEM_FUNC;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontArial18;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontArial16;
extern uint8_t countSD_files_window;
extern SD_Error (*pBoot_menu)(void);
extern void (*pGUI_Init)(void);

extern uint32_t ALARMA_ACTION;
extern uint32_t ALARMB_ACTION;
extern uint8_t time_show;
extern WM_HWIN PROGBAR_MEM;
extern int usedbytes;
extern int freebytes;

extern  uint16_t folders,files;
extern unsigned char level_DIR[];
extern char Path[256];

extern char drawmode;

extern uint8_t sleep_mode;
extern BACKLIGHT_TypeDef  backlight;
extern uint16_t brightness;//brightness;
extern uint8_t backlight_delay;
extern char start;
extern  uint8_t days [2][13];

extern GUI_PID_STATE State;

extern uint8_t count_SD_open;

extern uint8_t SD_buff[];

extern SD_Error sd_error;
extern SD_CardInfo SDCardInfo;
extern uint16_t SD_SECTOR_SIZE;
extern FATFS fs;
extern FRESULT fresult;
extern FIL fil;	
extern DIR dir;
extern FILINFO finfo;

void DateCalc(void);
void AlarmCalc(void);

extern uint8_t DummyByte;

extern float BrezErr;
extern uint8_t Brez_Count;
extern  int BrezPower;
extern float BrezKoeff;
extern  int PhaseBrez;
extern  int PhasePower;
extern  int TimerONOFF;

WM_HWIN CreateMENU(void);
WM_HWIN CreateTIMER(void);
WM_HWIN CreatePAINT(void);
WM_HWIN CreateALARM(void);
WM_HWIN CreateTIME_DATE(void);
TREEVIEW_Handle SD_Files(void);

extern WM_HWIN Message(const char *p,int flag );
 
extern void CreateStart(void);
extern GUI_CONST_STORAGE GUI_BITMAP bmexit;
extern GUI_CONST_STORAGE GUI_BITMAP bmscreen;
extern GUI_CONST_STORAGE GUI_BITMAP bmpaint;
extern GUI_CONST_STORAGE GUI_BITMAP bmnext;
extern GUI_CONST_STORAGE GUI_BITMAP bmprev;
extern GUI_CONST_STORAGE GUI_BITMAP bmdate;
extern GUI_CONST_STORAGE GUI_BITMAP bmpwm;
extern GUI_CONST_STORAGE GUI_BITMAP bmsd;
extern GUI_CONST_STORAGE GUI_BITMAP bmalarm;
extern GUI_CONST_STORAGE GUI_BITMAP bmAlarmB;
extern GUI_CONST_STORAGE GUI_BITMAP bmAlarmA;
extern GUI_CONST_STORAGE GUI_BITMAP bmAlarm_D;
extern GUI_CONST_STORAGE GUI_BITMAP bmadd_folder;
extern GUI_CONST_STORAGE GUI_BITMAP bmdel_folder;
extern GUI_CONST_STORAGE GUI_BITMAP bmbrightness;

extern GUI_CONST_STORAGE unsigned short _acscreen[];
extern GUI_CONST_STORAGE unsigned short _acpaint[];

extern void Set_COL_PAGE_Address(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
extern void Read_MX25L_FSMC(uint32_t address,uint32_t words,uint16_t *destination);
extern uint8_t SPIBuffer[];
extern uint32_t ID;
#endif
