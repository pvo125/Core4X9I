#include <stm32f4xx.h>
#include "header.h"
#include "GUI.h"

uint32_t ALARMA_ACTION;
uint32_t ALARMB_ACTION;
uint8_t time_show=1;

WIDGET_DRAW_ITEM_FUNC *pWIDGET_DRAW_ITEM_FUNC;

uint8_t countSD_files_window;
WM_HWIN PROGBAR_MEM;
int usedbytes;
int freebytes;
 uint16_t folders,files;
uint8_t count_SD_open;

void (*pGUI_Init)(void);
SD_Error (*pBoot_menu)(void);
unsigned char level_DIR[1];
char Path[256]="0:";
char start;
uint16_t SD_SECTOR_SIZE=512;
SD_Error sd_error=SD_ERROR;
SD_CardInfo sd_cardinfo;

uint8_t sleep_mode=0;

BACKLIGHT_TypeDef backlight=BACKLIGHT_ON;
uint16_t brightness;
uint8_t backlight_delay=0;

FATFS fs;
FRESULT fresult=FR_OK;
FIL fil;
DIR	dir;
FILINFO finfo;

GUI_PID_STATE State;

char drawmode;

uint8_t DummyByte=0;
uint32_t ID;
int PhaseBrez=0;
int PhasePower=0;
int BrezPower=0;
float BrezKoeff=0;
int TimerONOFF=0;
uint8_t Brez_Count=0;
float BrezErr=0;

/*--------------------------------------- Массив дней для високосного и не високосного года----------------------------------*/

uint8_t days [2][13]={
	{0,31,28,31,30,31,30,31,31,30,31,30,31},
	{0,31,29,31,30,31,30,31,31,30,31,30,31}
	};		
