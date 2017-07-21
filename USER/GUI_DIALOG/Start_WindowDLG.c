/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.22                          *
*        Compiled Jul  4 2013, 15:16:01                              *
*        (c) 2013 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
// USER END
#include <stdio.h>
#include "DIALOG.h"
#include "header.h"
#include <string.h>
#include "CAN.h"
#include "stm324x9i_eval_sdio_sd.h"
extern GUI_PID_STATE State;
extern void PictureView(void);
extern FIL pFile;

extern uint8_t ADCVal_ready;
extern uint16_t Bat_percent;
extern GUI_COLOR bat_color;

uint32_t button_color;

extern volatile uint8_t write_flashflag;
extern volatile uint8_t new_message;

uint16_t move_y=272;
volatile int16_t first_touch;
uint8_t screen_scroll=0;
int16_t linescroll=272;
uint8_t scroll_up=0,scroll_down=0;;

extern volatile uint8_t canerr_clr,canerr_disp,canconnect;
volatile uint8_t time_disp;

extern RTC_TimeTypeDef								RTC_Time;
extern RTC_DateTypeDef								RTC_Date;
extern RTC_AlarmTypeDef								RTC_AlarmA,RTC_AlarmB;
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define Bank1_LCD_Data    ((uint32_t)0x60020000) // display controller Data ADDR

#define ID_WINDOW_0     (GUI_ID_USER + 0x00)
#define ID_FRAMEWIN_3     (GUI_ID_USER + 0x01)
#define ID_ICON_CALIB    (GUI_ID_USER + 0x04)
#define ID_ICON_NEXT     (GUI_ID_USER + 0x05)

#define ID_WINDOW_1     (GUI_ID_USER + 0x1B)
#define ID_BUTTON_BACK     (GUI_ID_USER + 0x1C)
#define ID_ICON_PAINT     (GUI_ID_USER + 0x15)
#define ID_ICON_EXIT 			(GUI_ID_USER + 0x16)
#define ID_ICON_PHOTO     (GUI_ID_USER + 0x17)

#define ID_ICON_ALARM_A     (GUI_ID_USER + 0x18)
#define ID_ICON_ALARM_B     (GUI_ID_USER + 0x19)
#define ID_ICON_BRIGHT			(GUI_ID_USER + 0x1A)
#define ID_BUTTON_PERFORM			(GUI_ID_USER + 0x29)

#define ID_ICON_BLUE     (GUI_ID_USER + 0x1D)
#define ID_ICON_GREEN     (GUI_ID_USER + 0x1E)
#define ID_ICON_RED     (GUI_ID_USER + 0x1F)
#define ID_ICON_CYAN     (GUI_ID_USER + 0x20)
#define ID_ICON_MAGENTA     (GUI_ID_USER + 0x21)
#define ID_ICON_YELLOW     (GUI_ID_USER + 0x22)
#define ID_ICON_BLACK     (GUI_ID_USER + 0x23)
#define ID_ICON_BROWN     (GUI_ID_USER + 0x24)
#define ID_ICON_ORANGE     (GUI_ID_USER + 0x25)
#define ID_ICON_WHITE     (GUI_ID_USER + 0x26)

#define ID_BUTTON_OK			(GUI_ID_USER + 0x27)
#define ID_TEXT_1					(GUI_ID_USER + 0x28)
#define FLAG_STATUS_SECTOR	0x08004000		//sector 1

// USER START (Optionally insert additional defines)
/*****************************************************************
*													fputs
*****************************************************************/
struct __FILE { int handle;};
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
   return ITM_SendChar(ch);
}

uint8_t sd_insert;
uint8_t sd_ins_rem;
WM_HWIN hWin_start;
ICONVIEW_Handle hIcon_CALIB,hIcon_PAINT,hIcon_NEXT,hButton_BACK,hALARMA,hALARMB,hIcon_EXIT,hIcon_PHOTO,
								hIcon_BRIGHT;
								
ICONVIEW_Handle hIcon[9];
int xD[]={ID_ICON_BLUE,ID_ICON_GREEN,ID_ICON_RED,ID_ICON_CYAN,ID_ICON_MAGENTA,ID_ICON_YELLOW,ID_ICON_WHITE,ID_ICON_BLACK,ID_ICON_ORANGE};
int color[]={GUI_BLUE,GUI_GREEN,GUI_RED,GUI_CYAN,GUI_MAGENTA,GUI_YELLOW,GUI_WHITE,GUI_BLACK,GUI_ORANGE};
extern WM_HWIN hWin_menu;

BUTTON_Handle hBUTTON_PERFORM;
WM_HWIN hWin_message;

extern int _cbButtonSkin(const WIDGET_ITEM_DRAW_INFO *pDrawItemInfo);
void ChangePerformance(void);
void Suspend(void);
void SDRAM_PinConfig(void);

// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/********************************************************************
*
*       LcdWriteReg
*
* Function description:
*   Sets display register
*/
static __INLINE void LcdWriteReg(U16 Data) {
  // ... TBD by user
	LCD_REG_ADDRESS=Data;
}

/********************************************************************
*
*       LcdWriteData
*
* Function description:
*   Writes a value to a display register
*/
static __INLINE  void LcdWriteData(U16 Data) {
  // ... TBD by user
	LCD_DATA_ADDRESS=Data;
}

/*********************************************************************
*       			_cbMESSAGE
*********************************************************************/
static void _cbMESSAGE(WM_MESSAGE * pMsg) {
  GUI_RECT pRECT;
	int     NCode,x,y;
  int     Id;
	switch (pMsg->MsgId) {
		case WM_DELETE:
			hWin_message=0;
		break;	
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
				switch(Id) {
					case ID_BUTTON_OK: 
						switch(NCode) {
							case WM_NOTIFICATION_RELEASED:
								WM_GetWindowRectEx(hWin_message, &pRECT);
								x=WM_GetWindowSizeX(hWin_message);
								y=WM_GetWindowSizeY(hWin_message);
								WM_DeleteWindow(hWin_message);
								GUI_ClearRect(pRECT.x0,pRECT.y0,pRECT.x0+x,pRECT.y0+y);
								
							break;	
							}
					break;
					}
		break;
		default:
			WM_DefaultProc(pMsg);
		break;
	}
}
/*********************************************************************
*      				Message
**********************************************************************/
WM_HWIN Message(const char *p,int flag ){
	WM_HWIN hWin;
	BUTTON_Handle hButton;
	int temp;
	if(hWin_message!=0)
			return hWin_message;
	temp=GUI_GetStringDistX(p);
	hWin_message=FRAMEWIN_CreateEx((240-temp/2),100+SCREEN_1,temp+16, 80,0, WM_CF_SHOW,0,ID_FRAMEWIN_3,0,0);
	FRAMEWIN_SetActive(hWin_message, 1);
	if(flag==0)
	{
		FRAMEWIN_SetBarColor(hWin_message,1,GUI_RED);
		FRAMEWIN_SetText(hWin_message, "ERROR");
	}
	else 
		FRAMEWIN_SetText(hWin_message, "Message");
	
	FRAMEWIN_SetTextAlign(hWin_message,GUI_TA_HCENTER);
	
	hWin=WINDOW_CreateEx(3,22,temp+9,55,hWin_message,WM_CF_SHOW,0,ID_WINDOW_1,_cbMESSAGE);
	WINDOW_SetBkColor(hWin, GUI_WHITE);
	
	hButton=BUTTON_CreateEx(((temp+16)/2-15),32,30,20,hWin, WM_CF_SHOW,0,ID_BUTTON_OK);
	BUTTON_SetText(hButton, "OK");
	
	TEXT_CreateEx(5,10,temp,20,hWin,WM_CF_SHOW,TEXT_CF_HCENTER,ID_TEXT_1,p);
	//WM_MakeModal(hWin_message);
	return hWin_message;
}
/*********************************************************************
*      				_cbSTART
**********************************************************************/
static void _cbSTART(WM_MESSAGE* pMsg) {
 	int     NCode;
  int     Id;
	switch(pMsg->MsgId) {
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id){
				case ID_ICON_PHOTO: // Notifications sent by 'Button'
					switch(NCode) {
						case WM_NOTIFICATION_RELEASED:
							if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)==0)
								{
									Path[0]='0';
									Path[1]=':';
									Path[2]='\0';
									strcat(Path,"Photo");
									/* First entry directory Picture	*/
									fresult=f_opendir(&dir, Path);	
									if(fresult==FR_NO_PATH)
									{
										Message("Invalid IMAGE directory",0);
									}
									else
									{			
										WM_HideWindow(hIcon_EXIT);
										WM_HideWindow(hIcon_BRIGHT);	
										WM_HideWindow(hWin_start);
																
										WM_HideWindow(hALARMA);
										WM_HideWindow(hALARMB);
										WM_HideWindow(PROGBAR_MEM);
										#ifdef FLASHCODE
											time_show=0;
											PictureView();
										#endif
									}	
								}
							else
								Message("Insert SD card!", 0);
						break;
					}
					break;
					case ID_ICON_CALIB: // Notifications sent by 'Button'
					switch(NCode) {
						case WM_NOTIFICATION_RELEASED:
							time_show=0;
							WM_HideWindow(hIcon_EXIT);
							WM_HideWindow(hIcon_BRIGHT);
							WM_HideWindow(hALARMA);
							WM_HideWindow(hALARMB);
							WM_HideWindow(PROGBAR_MEM);
							WM_HideWindow(hWin_start);
							#ifdef FLASHCODE
								Touch_calibration();
							#endif
						break;
					}
					break;
				
				case ID_ICON_NEXT: // Notifications sent by 'Button'
					switch(NCode) {
						case WM_NOTIFICATION_RELEASED:
							time_show=1;
							WM_HideWindow(hWin_start);
							if(hWin_menu==0)
								{__disable_irq();
									#ifdef FLASHCODE
										CreateMENU();
									#endif
								 __enable_irq(); }
							else
								{__disable_irq();
								 WM_ShowWindow(hWin_menu);
								 __enable_irq(); }
						break;
            }
					break;
					
					case ID_ICON_PAINT: // Notifications sent by 'Button'
					switch(NCode) {
							case WM_NOTIFICATION_RELEASED:
							NVIC_DisableIRQ(TIM7_IRQn);
							NVIC_DisableIRQ(RTC_WKUP_IRQn);
							time_show=0;
							/*WM_HideWindow(hIcon_EXIT);
							WM_HideWindow(hIcon_BRIGHT);
							WM_HideWindow(hWin_start);
							WM_HideWindow(hALARMA);
							WM_HideWindow(hALARMB);
							WM_HideWindow(PROGBAR_MEM);*/
							move_y=0;
							GUI_SetOrg(0,544);
							screen_scroll=1;
							scroll_up=1;
							
							
							GUI_SetPenSize(7);
							GUI_SetBkColor(GUI_WHITE);
							GUI_SetColor(GUI_WHITE);
							GUI_ClearRect(0,0+SCREEN_2,479,271+SCREEN_2);
							WM_Paint(hButton_BACK);
								
							
							TIM6->ARR = 40;
						  TIM6->EGR = TIM_EGR_UG;	
						 	drawmode=1;	
						break;
						}
					break;		
					
				}	
			break;	
	 default:
    WM_DefaultProc(pMsg);
  }
}
/*********************************************************************
*      				_cbBkWin
**********************************************************************/
void _cbBkWin(WM_MESSAGE* pMsg) {
	int			i;
	int     NCode;
  int     Id;
	switch(pMsg->MsgId) {
		case WM_TOUCH:
		break;	
		case WM_TOUCH_CHILD:
			if(GPIOB->IDR & GPIO_IDR_IDR_11)	break;
			if(move_y==0)			break;
			if(!first_touch)
				first_touch=State.y;
			if((State.y-first_touch)>24)
					{
						/*i=(State.y-first_touch)/4;					// Сначала разделим на 4 затем умножим чтобы linescroll всегда была кратна 4
						linescroll=252+i*4;
						LcdWriteReg(CMD_SET_SCROLL_START);
						LcdWriteData((linescroll)>>8);
						LcdWriteData(linescroll);
						if(linescroll>292)
						{
							NVIC_DisableIRQ(TIM6_DAC_IRQn);
							first_touch=0;
							GUI_SetOrg(0,0);
							move_y=544;
							screen_scroll=1;
							scroll_down=1;
						}*/
					}
					else if((first_touch-State.y)>24)
					{
						i=(first_touch-State.y)/4;					// Сначала разделим на 4 затем умножим чтобы linescroll всегда была кратна 4
						//if(move_y==272)
						//{
							linescroll=292-i*4;
							LcdWriteReg(CMD_SET_SCROLL_START);
							LcdWriteData((linescroll)>>8);
							LcdWriteData(linescroll);
							if(linescroll<252)
							{
								NVIC_DisableIRQ(TIM6_DAC_IRQn);
								first_touch=0;
								GUI_SetOrg(0,SCREEN_2);
								move_y=0;
								screen_scroll=1;
								scroll_up=1;
								GUI_SetPenSize(7);
								GUI_SetBkColor(GUI_WHITE);
								GUI_ClearRect(0,0+SCREEN_2,479,271+SCREEN_2);
												
								GUI_SetColor(GUI_WHITE);
								WM_Paint(hButton_BACK);
											
								TIM6->ARR = 40;
								TIM6->EGR = TIM_EGR_UG;	
								}
						}
						/*else if(move_y==544)
						{
							linescroll=564-i*4;
							LcdWriteReg(CMD_SET_SCROLL_START);
							LcdWriteData((linescroll)>>8);
							LcdWriteData(linescroll);
							if(linescroll<524)
							{
								NVIC_DisableIRQ(TIM6_DAC_IRQn);
								GUI_SetOrg(0,272);
								move_y=272;
								first_touch=0;
								screen_scroll=1;
								scroll_up=1;
								GUI_SetColor(GUI_YELLOW);
								GUI_SetBkColor(GUI_DARKBLUE);		
							}
						}*/
					//}
		break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;	
			switch(Id){
				case ID_BUTTON_PERFORM:
					switch(NCode){
						case WM_NOTIFICATION_RELEASED:
							ChangePerformance();
							if(performance==PERFORMANCE_HIGH)
							{
								button_color=GUI_LIGHTRED;
								
								BUTTON_SetSkin(hBUTTON_PERFORM, _cbButtonSkin);
								BUTTON_SetText(hBUTTON_PERFORM, "High");
							}
							else
							{
								button_color=GUI_GREEN;
								
								BUTTON_SetSkin(hBUTTON_PERFORM, _cbButtonSkin);
								BUTTON_SetText(hBUTTON_PERFORM, "Low");
								
								
							}
						break;
						}
				break;		
				case ID_ICON_BRIGHT:
					switch(NCode) {
						case WM_NOTIFICATION_RELEASED:
							if(brightness==BRIGHTNESS_LOW)
							{
								brightness=BRIGHTNESS_MED;
								RTC->BKP2R=BRIGHTNESS_MED;
								LcdWriteReg(CMD_SET_PWM_CONF); 			//set PWM for Backlight. Manual p.53
								// 6 parameters to be set
								LcdWriteData(0x0004); 							// PWM Freq =100MHz/(256*(PWMF[7:0]+1))/256  PWMF[7:0]=4 PWM Freq=305Hz
								LcdWriteData(brightness); 					// PWM duty cycle(50%)
								LcdWriteData(0x0001); 							// PWM controlled by host, PWM enable
								LcdWriteData(0x00f0); 							// brightness level 0x00 - 0xFF
								LcdWriteData(0x0000); 							// minimum brightness level =  0x00 - 0xFF
								LcdWriteData(0x0000);								// brightness prescalar 0x0 - 0xF
							}
							else if(brightness==BRIGHTNESS_MED)
							{
								brightness=BRIGHTNESS_HIGH;
								RTC->BKP2R=BRIGHTNESS_HIGH;
								LcdWriteReg(CMD_SET_PWM_CONF); 			//set PWM for Backlight. Manual p.53
								// 6 parameters to be set
								LcdWriteData(0x0004); 							// PWM Freq =100MHz/(256*(PWMF[7:0]+1))/256  PWMF[7:0]=4 PWM Freq=305Hz
								LcdWriteData(brightness); 					// PWM duty cycle(50%)
								LcdWriteData(0x0001); 							// PWM controlled by host, PWM enable
								LcdWriteData(0x00f0); 							// brightness level 0x00 - 0xFF
								LcdWriteData(0x0000); 							// minimum brightness level =  0x00 - 0xFF
								LcdWriteData(0x0000);								// brightness prescalar 0x0 - 0xF
							}
							else if(brightness==BRIGHTNESS_HIGH)
							{
								brightness=BRIGHTNESS_LOW;
								RTC->BKP2R=BRIGHTNESS_LOW;
								LcdWriteReg(CMD_SET_PWM_CONF); 			//set PWM for Backlight. Manual p.53
								// 6 parameters to be set
								LcdWriteData(0x0004); 							// PWM Freq =100MHz/(256*(PWMF[7:0]+1))/256  PWMF[7:0]=4 PWM Freq=305Hz
								LcdWriteData(brightness); 					// PWM duty cycle(50%)
								LcdWriteData(0x0001); 							// PWM controlled by host, PWM enable
								LcdWriteData(0x00f0); 							// brightness level 0x00 - 0xFF
								LcdWriteData(0x0000); 							// minimum brightness level =  0x00 - 0xFF
								LcdWriteData(0x0000);								// brightness prescalar 0x0 - 0xF
							}
						break;	
						}
					break;
					case ID_ICON_EXIT:
					switch(NCode) {
						case WM_NOTIFICATION_RELEASED:
								NVIC_SystemReset();
						
						break;
						}
					break;
					case ID_ICON_ALARM_A: 
					switch(NCode) {
						case WM_NOTIFICATION_CLICKED:
							WM_DeleteWindow(hALARMA);
							hALARMA=ICONVIEW_CreateEx(10,15+SCREEN_1,34,34,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_ALARM_A,24,24);
#ifdef FLASHCODE	
						if((RTC->CR&RTC_CR_ALRAE)==RTC_CR_ALRAE)
							{
								RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
								ICONVIEW_AddBitmapItem(hALARMA,(const GUI_BITMAP*)(Alarm_d+1152),"");
							}
							else
							{
								RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
								ICONVIEW_AddBitmapItem(hALARMA,(const GUI_BITMAP*)(AlarmA+1152),"");
							}
#endif							
						break;
							}
					break;
					case ID_ICON_ALARM_B: 
					switch(NCode) {
						case WM_NOTIFICATION_CLICKED:
							WM_DeleteWindow(hALARMB);
							hALARMB=ICONVIEW_CreateEx(10,45+SCREEN_1,34,34,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_ALARM_B,24,24);
#ifdef FLASHCODE							
							if((RTC->CR&RTC_CR_ALRBE)==RTC_CR_ALRBE)
							{
								RTC_AlarmCmd(RTC_Alarm_B, DISABLE);
								ICONVIEW_AddBitmapItem(hALARMB,(const GUI_BITMAP*)(Alarm_d+1152),"");
							}
							else
							{
								RTC_AlarmCmd(RTC_Alarm_B, ENABLE);
								ICONVIEW_AddBitmapItem(hALARMB,(const GUI_BITMAP*)(AlarmB+1152),"");
							}
#endif							
						break;
						}
					break;		
					case ID_BUTTON_BACK:
					switch(NCode){
						case WM_NOTIFICATION_CLICKED:
							drawmode=0;
						break;	
						case WM_NOTIFICATION_RELEASED:
							GUI_SetColor(GUI_YELLOW);
							GUI_SetBkColor(GUI_DARKBLUE);
							GUI_SetOrg(0,SCREEN_1);
							move_y=272;
							screen_scroll=1;
							scroll_down=1;
							TIM6->ARR=100;
							TIM6->EGR = TIM_EGR_UG;
							
						break;	
						}
					break;
					case ID_ICON_BLUE:
					switch(NCode){
						case WM_NOTIFICATION_CLICKED:			
						GUI_SetColor(GUI_BLUE);
						break;
						}
					break;
					case ID_ICON_GREEN:
					switch(NCode){
						case WM_NOTIFICATION_CLICKED:			
						GUI_SetColor(GUI_GREEN);
						break;
						}
					break;
					case ID_ICON_RED:
					switch(NCode){
						case WM_NOTIFICATION_CLICKED:			
						GUI_SetColor(GUI_RED);
						break;
						}
					break;
					case ID_ICON_CYAN:
					switch(NCode){
						case WM_NOTIFICATION_CLICKED:			
						GUI_SetColor(GUI_CYAN);
						break;
						}
					break;			
					case ID_ICON_MAGENTA:
					switch(NCode){
						case WM_NOTIFICATION_CLICKED:			
						GUI_SetColor(GUI_MAGENTA);
						break;
						}
					break;
					case ID_ICON_YELLOW:
					switch(NCode){
						case WM_NOTIFICATION_CLICKED:			
						GUI_SetColor(GUI_YELLOW);
						break;
						}
					break;
					case ID_ICON_WHITE:
					switch(NCode){
						case WM_NOTIFICATION_CLICKED:			
						GUI_SetPenSize(20);
						GUI_SetColor(GUI_WHITE);
						break;
						}
					break;
					case ID_ICON_BLACK:
					switch(NCode){
						case WM_NOTIFICATION_CLICKED:			
						GUI_SetColor(GUI_BLACK);
						break;
						}
					break;
					case ID_ICON_ORANGE:
					switch(NCode){
						case WM_NOTIFICATION_CLICKED:			
						GUI_SetColor(GUI_ORANGE);
						break;
						}
					break;		
				}	
			break;	
			default:
			WM_DefaultProc(pMsg);
  }
}


void CreateStart(void)
{
	__disable_irq();
	GUI_SetAlpha(0);
	WM_SetCallback(WM_HBKWIN, _cbBkWin);
	GUI_SetFont(&GUI_Font8x16);
	if(hWin_start==0)
	{
		hWin_start=WINDOW_CreateEx(60,15+SCREEN_1,410, 257,WM_HBKWIN, WM_CF_SHOW,0,ID_WINDOW_0,_cbSTART);	
		WINDOW_SetBkColor(hWin_start, GUI_LIGHTBLUE);	
				
		hIcon_PHOTO=ICONVIEW_CreateEx(240,0,58,65,hWin_start,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_PHOTO,48,65);
#ifdef FLASHCODE	
		ICONVIEW_AddBitmapItem(hIcon_PHOTO,(const GUI_BITMAP*)(photo+4608),"IMAGE");
#endif
		ICONVIEW_SetFont(hIcon_PHOTO,&GUI_Font8x18);
		ICONVIEW_SetIconAlign(hIcon_PHOTO, ICONVIEW_IA_TOP);
		
		
		hIcon_CALIB=ICONVIEW_CreateEx(340,0,58,65,hWin_start,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_CALIB,48,65);
#ifdef FLASHCODE		
		ICONVIEW_AddBitmapItem(hIcon_CALIB,(const GUI_BITMAP*)(screen+4608),"CALIB");
#endif
		ICONVIEW_SetFont(hIcon_CALIB,&GUI_Font8x18);
		ICONVIEW_SetIconAlign(hIcon_CALIB, ICONVIEW_IA_TOP);
		
		hIcon_PAINT=ICONVIEW_CreateEx(340,90,58,65,hWin_start,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_PAINT,48,65);
#ifdef FLASHCODE	
		ICONVIEW_AddBitmapItem(hIcon_PAINT,(const GUI_BITMAP*)(paint+4608),"PAINT");
#endif
		ICONVIEW_SetFont(hIcon_PAINT,&GUI_Font8x18);
		ICONVIEW_SetIconAlign(hIcon_PAINT, ICONVIEW_IA_TOP);
		
		hIcon_NEXT=ICONVIEW_CreateEx(340,200,58,58,hWin_start,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_NEXT,48,48);
#ifdef FLASHCODE		
	ICONVIEW_AddBitmapItem(hIcon_NEXT,(const GUI_BITMAP*)(next+4608),"");
#endif	
	}
	else
	{
		WM_ShowWindow(hWin_start);
		/*WM_ShowWindow(hIcon_PHOTO);
		WM_ShowWindow(hIcon_CALIB);
		WM_ShowWindow(hIcon_PAINT);
		WM_ShowWindow(hIcon_NEXT);
		WM_ShowWindow(hALARMA);
		WM_ShowWindow(hALARMB);
		WM_ShowWindow(PROGBAR_MEM);*/
	}
		RTC_GetDate(RTC_Format_BIN, &RTC_Date);
		GUI_DispDecAt(RTC_Date.RTC_Date,5,0+SCREEN_1,2);
		GUI_DispString(".");
		GUI_DispDec(RTC_Date.RTC_Month,2);
		GUI_DispString(".20");
		GUI_DispDec(RTC_Date.RTC_Year,2);
		
		__enable_irq();
}


void MainTask(void)
{
	uint8_t flag=0xA7;
	uint16_t count;
		
	NVIC_SetPriority(SysTick_IRQn,1);
	WM_SetCallback(WM_HBKWIN, _cbBkWin);
	pWIDGET_DRAW_ITEM_FUNC=BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	
	GUI_SetBkColor(GUI_LIGHTBLUE);
	GUI_ClearRect(1,17+SCREEN_1,58,270+SCREEN_1);
	GUI_SetColor(GUI_YELLOW);
	GUI_DrawRect(0,16+SCREEN_1,59,271+SCREEN_1);
	GUI_SetBkColor(GUI_DARKBLUE);
	GUI_ClearRect(0,0+SCREEN_1,470,15+SCREEN_1);
	 GUI_DrawRoundedRect(410,0+SCREEN_1,459,14+SCREEN_1,2);
	GUI_DrawRect(460,3+SCREEN_1,464,11+SCREEN_1);
	
	hIcon_EXIT=ICONVIEW_CreateEx(0,214+SCREEN_1,58,58,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_EXIT,48,48);
#ifdef FLASHCODE	
	ICONVIEW_AddBitmapItem(hIcon_EXIT,(const GUI_BITMAP*)(exitt+4608),"");
#endif
	hIcon_BRIGHT=ICONVIEW_CreateEx(10,80+SCREEN_1,34,34,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_BRIGHT,24,24);
#ifdef FLASHCODE		
	ICONVIEW_AddBitmapItem(hIcon_BRIGHT,&bmbrightness,"");
#endif	
	
	hBUTTON_PERFORM=BUTTON_CreateEx(12,125+SCREEN_1,30,30,WM_HBKWIN,WM_CF_SHOW,0,ID_BUTTON_PERFORM);
	if(performance==PERFORMANCE_HIGH)
	{
		button_color=GUI_LIGHTRED;
		BUTTON_SetSkin(hBUTTON_PERFORM, _cbButtonSkin);
		BUTTON_SetText(hBUTTON_PERFORM, "High");
	}
	else
	{
		button_color=GUI_GREEN;
		BUTTON_SetSkin(hBUTTON_PERFORM, _cbButtonSkin);
		BUTTON_SetText(hBUTTON_PERFORM, "Low");
	}
	hALARMA=ICONVIEW_CreateEx(10,15+SCREEN_1,34,34,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_ALARM_A,24,24);
#ifdef FLASHCODE	
	if((RTC->CR&RTC_CR_ALRAE)==RTC_CR_ALRAE)
			ICONVIEW_AddBitmapItem(hALARMA,(const GUI_BITMAP*)(AlarmA+1152),"");
	else
			ICONVIEW_AddBitmapItem(hALARMA,(const GUI_BITMAP*)(Alarm_d+1152),"");
#endif
	hALARMB=ICONVIEW_CreateEx(10,45+SCREEN_1,34,34,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_ALARM_B,24,24);
#ifdef FLASHCODE	
	if((RTC->CR&RTC_CR_ALRBE)==RTC_CR_ALRBE)
			ICONVIEW_AddBitmapItem(hALARMB,(const GUI_BITMAP*)(AlarmB+1152),"");
		else
			ICONVIEW_AddBitmapItem(hALARMB,(const GUI_BITMAP*)(Alarm_d+1152),"");
#endif	
	EXTI_ClearITPendingBit(EXTI_Line1);
	NVIC_ClearPendingIRQ(EXTI1_IRQn);
	
	NVIC_EnableIRQ(EXTI1_IRQn);									//Разрешение EXTI3_IRQn прерывания
	WM_SetDesktopColor(GUI_BLACK);
	
	
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);	
	//FRAMEWIN_SetDefaultSkinClassic();
	SCROLLBAR_SetDefaultWidth(20);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	PROGBAR_MEM=PROGBAR_CreateEx(470,0+SCREEN_1,10,272,WM_HBKWIN,WM_CF_SHOW,PROGBAR_CF_VERTICAL,PROGBAR_MEM);
	PROGBAR_SetMinMax(PROGBAR_MEM,0,400*1024);
	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmA);
	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmB);
	RTC_GetTime(RTC_Format_BIN, &RTC_Time);
/*********************************************************************************************************************/	
	hButton_BACK=BUTTON_CreateEx(400,220+SCREEN_2,50,30,WM_HBKWIN,WM_CF_SHOW,0,ID_BUTTON_BACK);
	BUTTON_SetText(hButton_BACK, "BACK");	
	for(int i=0;i<9;i++)
	{
	hIcon[i]=ICONVIEW_CreateEx(0,(i*30+SCREEN_2),30,30,WM_HBKWIN,WM_CF_SHOW,0,xD[i],30,30);
	ICONVIEW_SetBkColor(hIcon[i], ICONVIEW_CI_BK, color[i]);	
	}	
	
/**************************************************************************************************************************/	
	NVIC_EnableIRQ(CAN1_RX0_IRQn);
	NVIC_EnableIRQ(CAN1_RX1_IRQn);	
		
	//GUI_EnableAlpha(1);
	CreateStart();
	
	while(1)
	{
		GUI_Delay(5);	
		if(ADCVal_ready)
		{
			ADCVal_ready=0;
			GUI_SetColor(bat_color);
			GUI_FillRoundedRect(421, 1, 421+Bat_percent/2, 13,2);
			GUI_SetColor(GUI_GRAY);
			GUI_FillRect(421+Bat_percent/2, 1, 457, 13);
		}			
		if(canerr_clr)
		{
			GUI_SetBkColor(GUI_DARKBLUE);
			GUI_ClearRect(120,5+SCREEN_1,290,15+SCREEN_1);
			canerr_clr=0;
		}
		if(canerr_disp)	
		{
				GUI_SetFont(&GUI_Font6x8);
				GUI_DispStringAt("REC ",120,5+SCREEN_1);
				GUI_DispDec((uint8_t)((CAN1->ESR)>>24),3);
				GUI_DispStringAt("TEC ",190,5+SCREEN_1);
				GUI_DispDec((uint8_t)((CAN1->ESR)>>16),3);
				GUI_DispStringAt("ERF ",260,5+SCREEN_1);
				GUI_DispDec((uint8_t)(CAN1->ESR),1);
				canerr_disp=0;
		}
#ifdef DEBUG_MODE
		if(time_disp)
		{
			printf("ITM_TCR %.8X\t	TPI_ACPR %.8X\t	DWT_CTRL %.8X\n",ITM->TCR,TPI->ACPR,DWT->CTRL);
		}
#endif		
		if(time_disp)
		{
			GUI_SetFont(&GUI_Font8x16);
			GUI_DispDecAt(RTC_Time.RTC_Hours,350,0+SCREEN_1,2);
			GUI_DispString(":");
			GUI_DispDec(RTC_Time.RTC_Minutes,2);
			time_disp=0;	
		}
		
		if(sd_ins_rem)
		{	
			sd_ins_rem=0;
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)==0)	
			{
				if(sd_error==SD_OK)
					Message("SD card is OK!",1);
				else
					Message("SD card is error!", 0);
			}			
			else
				Message("SD card is removed!", 1);
		}
		if(sleep_mode)
			Suspend();
			
		if(write_flashflag)
		{
			count=0;
			while(*(uint8_t*)(FLAG_STATUS_SECTOR+count)!=0xFF)		// Перебираем байты пока не дойдем до неписанного поля 0xFF 
			{
				count++;
				if(count>=0x3fff)
				{
					count=0;
					Flash_sect_erase(NAMBER_FLAG_STATUS_SECTOR,1);		// Очистим 		FLAG_STATUS_SECTOR
					break;
				}
			}		
				
			Flash_prog(&flag,(uint8_t*)(FLAG_STATUS_SECTOR+count),1,1);		// В ячейке где 0xFF лежит запишем значения флага для bootloader flag=0xA7
			GUI_Delay(1000);
			NVIC_SystemReset();
		}
		if(screen_scroll)
		{
			if(scroll_up)
			{
				linescroll-=4;
				LcdWriteReg(CMD_SET_SCROLL_START);
				LcdWriteData(linescroll>>8);
				LcdWriteData(linescroll);
				if(linescroll==move_y)
				{
					NVIC_EnableIRQ(TIM6_DAC_IRQn);
					screen_scroll=0;
					scroll_up=0;
					if(move_y==0)	drawmode=1;
				}
			}
			else if(scroll_down)
			{
				linescroll+=4;
				LcdWriteReg(CMD_SET_SCROLL_START);
				LcdWriteData(linescroll>>8);
				LcdWriteData(linescroll);
				if(linescroll==move_y)
				{ 
					NVIC_EnableIRQ(TIM6_DAC_IRQn);
					screen_scroll=0;
					scroll_down=0;
					//if(!hWin_photo)
					//	PictureView();
					
					/*TIM7->CNT=0;
					NVIC_EnableIRQ(TIM7_IRQn);
					NVIC_EnableIRQ(RTC_WKUP_IRQn);
					NVIC_EnableIRQ(TIM6_DAC_IRQn);*/
				}
			}
		}
	__WFI();
	}
}
/*
*/
void ChangePerformance(void){
	
	while(FMC_Bank5_6->SDSR&FMC_SDSR_BUSY);
	FMC_Bank5_6->SDCMR = ((uint32_t)0x00000004)|FMC_SDCMR_MODE_0| 	// 101  Self-Refresh mode
											 FMC_SDCMR_CTB2| 														// Command issued to SDRAM Bank 2
											 FMC_SDCMR_NRFS_0;													// 2 Auto-refresh cycles*/
	// Select HSI as system clock source 
   RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
   RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSE; 
	// Wait till HSE is used as system clock source 
   while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x4){ }
		// Disable PLL 
  RCC->CR &= ~RCC_CR_PLLON;
		 
	if(performance==PERFORMANCE_LOW)
	{	// enable PERFORMANCE_HIGH
#ifdef DEBUG_MODE		
		//CoreDebug->DEMCR = CoreDebug_DEMCR_TRCENA_Msk; /* enable trace in core debug */
		TPI->ACPR=0x00000059;
#endif
		performance=PERFORMANCE_HIGH;
		SystemCoreClock=180000000;
		RCC->PLLCFGR &=~RCC_PLLCFGR_PLLP;				// 00 PLLP=2
		PWR->CR |= PWR_CR_VOS;									// Scale 1 mode(reset) 
	 		
		RCC->CFGR &=~(RCC_CFGR_PPRE1|RCC_CFGR_PPRE2);						
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV2  		        // PCLK2 = HCLK / 2 0x100
							  |RCC_CFGR_PPRE1_DIV4;	      	 		// PCLK1 = HCLK / 4 0x101
		
		
		RCC->CR|= RCC_CR_PLLON;
		while((RCC->CR& RCC_CR_PLLRDY)!=RCC_CR_PLLRDY) {}
		// Enable the Over-drive to extend the clock frequency to 180 Mhz 
		PWR->CR |= PWR_CR_ODEN;
		while((PWR->CSR & PWR_CSR_ODRDY) == 0){}
		PWR->CR |= PWR_CR_ODSWEN;
		while((PWR->CSR & PWR_CSR_ODSWRDY) == 0){}
		// Select the main PLL as system clock source 
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
		RCC->CFGR |= RCC_CFGR_SW_PLL;
		while((RCC->CFGR&RCC_CFGR_SWS_PLL)!=RCC_CFGR_SWS_PLL) {}
			
		// Initialization step 8
		while(FMC_Bank5_6->SDSR&FMC_SDSR_BUSY);
		FMC_Bank5_6->SDRTR=(1386<<1);													// 64mS/4096=15.625uS
																														// 15.625*90MHz-20=1386		
	}
	else	
	{	// enable PERFORMANCE_LOW
#ifdef DEBUG_MODE
		//CoreDebug->DEMCR = CoreDebug_DEMCR_TRCENA_Msk; /* enable trace in core debug */
		TPI->ACPR=0x0000002C;
#endif
		performance=PERFORMANCE_LOW;
		SystemCoreClock=90000000;
		RCC->PLLCFGR|=RCC_PLLCFGR_PLLP_0;				// 01 PLLP=4
		PWR->CR &= ~PWR_CR_VOS;									
		PWR->CR |= PWR_CR_VOS_0;								// Scale 3 mode <120MHz 
		
		//RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;				// PCLK1 = HCLK / 2
		RCC->CFGR &=~(RCC_CFGR_PPRE1|RCC_CFGR_PPRE2);						
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV1  		        // PCLK2 = HCLK /1  0x000
							  |RCC_CFGR_PPRE1_DIV2;	      	 		// PCLK1 = HCLK / 2 0x100
		
		PWR->CR &= ~PWR_CR_ODSWEN;
		while((PWR->CSR & PWR_CSR_ODSWRDY) != 0){}
		PWR->CR &= ~PWR_CR_ODEN;	
		while((PWR->CSR & PWR_CSR_ODRDY) != 0){}
	
		RCC->CR|= RCC_CR_PLLON;
		while((RCC->CR& RCC_CR_PLLRDY)!=RCC_CR_PLLRDY) {}
		// Select the main PLL as system clock source 
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
		RCC->CFGR |= RCC_CFGR_SW_PLL;
		while((RCC->CFGR&RCC_CFGR_SWS_PLL)!=RCC_CFGR_SWS_PLL) {}
			
		// Initialization step 8
		//while(FMC_Bank5_6->SDSR&FMC_SDSR_BUSY);
		FMC_Bank5_6->SDRTR=(1386<<1);													// 64mS/4096=15.625uS
																													// 15.625*45MHz-20=683	
	}
	
	SysTick_Config(SystemCoreClock/1000);  /* SysTick IRQ each 1 ms */
	
	while(FMC_Bank5_6->SDSR&FMC_SDSR_BUSY);
	FMC_Bank5_6->SDCMR =((uint32_t)0x00000000)			// 000: normal mode 
											|FMC_SDCMR_CTB2							// Command issued to SDRAM Bank 2
											|FMC_SDCMR_NRFS_2;					// Number of Auto-refresh 8 cycle (0111)*/
}
/*
*/
void Suspend(void){
	__IO uint32_t StartUpCounter = 0, HSEStatus = 0;
	GPIO_InitTypeDef GPIO_InitStruct;
	uint8_t i;
			
	NVIC_DisableIRQ(TIM6_DAC_IRQn);
	NVIC_DisableIRQ(TIM7_IRQn);
	NVIC_DisableIRQ(RTC_WKUP_IRQn);
	NVIC_DisableIRQ(EXTI1_IRQn);									
	NVIC_DisableIRQ(ADC_IRQn);
	
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	SysTick->CTRL&=~SysTick_CTRL_TICKINT_Msk;
	
	while(FMC_Bank5_6->SDSR&FMC_SDSR_BUSY);
	FMC_Bank5_6->SDCMR = ((uint32_t)0x00000004)|FMC_SDCMR_MODE_0| 	// 101  Self-Refresh mode
											 FMC_SDCMR_CTB2| 														// Command issued to SDRAM Bank 2
											 FMC_SDCMR_NRFS_0;													// 2 Auto-refresh cycles*/
	
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;	
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|
													 GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12/*|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15*/;
	GPIO_Init(GPIOA, &GPIO_InitStruct);	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|/*GPIO_Pin_3|GPIO_Pin_4|*/GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|
													 GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin=/*GPIO_Pin_0|GPIO_Pin_1*/GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|
													 GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5/*|GPIO_Pin_6|GPIO_Pin_7*/|GPIO_Pin_8|
													 GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOH, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_All;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	GPIO_Init(GPIOG, &GPIO_InitStruct);
	GPIO_Init(GPIOI, &GPIO_InitStruct);
	
	TIM7->CNT=0;
	ADC_Cmd(ADC1,DISABLE);
	
/*	
 // Select HSI as system clock source 
   RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
   RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI; 
// Wait till HSI is used as system clock source 
   while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x0){ }
	 
// Disable PLL 
  RCC->CR &= ~RCC_CR_PLLON;
	
	PWR->CR &= ~PWR_CR_ODSWEN;
	while((PWR->CSR & PWR_CSR_ODSWRDY) != 0){}
	PWR->CR &= ~PWR_CR_ODEN;	
	while((PWR->CSR & PWR_CSR_ODRDY) != 0){}
		
	*/
	/*	Low-power voltage regulator ON during Stop mode*/
	PWR->CR|=	PWR_CR_LPDS;	
	//Flash memory in power-down and Low-power regulator in under-drive mode when the device is in Stop mode
	PWR->CR |=PWR_CR_FPDS|PWR_CR_LPUDS;			
	//PWR->CR &=~PWR_CR_VOS_1;		//Scale mode 3
	
	PWR->CR |=PWR_CR_UDEN;
	//while(!(PWR->CSR & PWR_CSR_UDSWRDY)) {}	

	SCB->SCR|=SCB_SCR_SLEEPDEEP_Msk;					// Разрешаем SLEEPDEEP по команде WFI WFE	
	
	
				
	__WFI();
/**********************************************************		  
*						WakeUp from stop mode													*	
**********************************************************/				
	SCB->SCR&=~SCB_SCR_SLEEPDEEP_Msk;					// Запрешаем SLEEPDEEP по команде WFI WFE
 
	/*	Main regulator ON during Stop mode*/
	PWR->CR&=	~PWR_CR_LPDS;	
	PWR->CR &=~(PWR_CR_FPDS|PWR_CR_LPUDS);		
	PWR->CR &=~PWR_CR_UDEN;	
/**********************************************************		  
*						SWPOWER_LCD_PIN																*	
**********************************************************/				

	GPIO_InitStruct.GPIO_Pin=SWPOWER_LCD_PIN;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(SWPOWER_LCD_PORT,&GPIO_InitStruct);
	
	GPIO_SetBits(SWPOWER_LCD_PORT, SWPOWER_LCD_PIN);		// Включаем  LDO для LCD + CAN transsiver	
	
	RCC->CR|= RCC_CR_HSEON;
	while((RCC->CR&RCC_CR_HSERDY)!=RCC_CR_HSERDY) {}
	RCC->CR|= RCC_CR_PLLON;
	while((RCC->CR& RCC_CR_PLLRDY)!=RCC_CR_PLLRDY) {}
	if(performance==PERFORMANCE_HIGH)
	{// Enable the Over-drive to extend the clock frequency to 180 Mhz 
		PWR->CR |= PWR_CR_ODEN;
		while((PWR->CSR & PWR_CSR_ODRDY) == 0){}
		PWR->CR |= PWR_CR_ODSWEN;
		while((PWR->CSR & PWR_CSR_ODSWRDY) == 0){}
	}
	 /* Select the main PLL as system clock source */
   RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
   RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR&RCC_CFGR_SWS_PLL)!=RCC_CFGR_SWS_PLL) {}
/***********************************************************/	
	SDRAM_PinConfig();
	
	while(FMC_Bank5_6->SDSR&FMC_SDSR_BUSY);
	FMC_Bank5_6->SDCMR =((uint32_t)0x00000000)			// 000: normal mode 
											|FMC_SDCMR_CTB2							// Command issued to SDRAM Bank 2
											|FMC_SDCMR_NRFS_2;					// Number of Auto-refresh 8 cycle (0111)*/
	
	SSD1963_LowLevel_Init();
	SD_LowLevel_Init();	
	TSC2046_LowLevel_Init();
	MX25_LowLevel_Init();
	bxCAN_LowLevel_Init();	
	
/********************************************************************/
/*								ADC_SWITCH_PIN	  																*/
/********************************************************************/			
		GPIO_InitStruct.GPIO_Pin=ADC_SWITCH_PIN;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType=GPIO_OType_OD;
		GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
		GPIO_Init(ADC_SWITCH_PORT,&GPIO_InitStruct);
		GPIO_ResetBits(ADC_SWITCH_PORT,ADC_SWITCH_PIN);			// Enable ADC_SWITCH Vbat/2 input for ADC1_IN3
		
	
	SysTick->VAL=0;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk|SysTick_CTRL_TICKINT_Msk;
	
	GUI_Delay(500);
	
	ssd1963_Init();
	if(WM_IsVisible(hWin_start))
	{
		WM_HideWindow(hWin_start);
		WM_ShowWindow(hWin_start);
	}
	else if(WM_IsVisible(hWin_menu))
	{
		WM_HideWindow(hWin_menu);
		WM_ShowWindow(hWin_menu);
	}
	WM_Exec();
	
	GUI_SetBkColor(GUI_LIGHTBLUE);
	GUI_ClearRect(1,17+SCREEN_1,58,270+SCREEN_1);
	GUI_SetColor(GUI_YELLOW);
	GUI_DrawRect(0,16+SCREEN_1,59,271+SCREEN_1);
	GUI_SetBkColor(GUI_DARKBLUE);
	GUI_ClearRect(0,0+SCREEN_1,470,15+SCREEN_1);
		
	for(i=0;i<9;i++)
		WM_Paint(hIcon[i]);
	RTC_GetDate(RTC_Format_BIN, &RTC_Date);
	GUI_DispDecAt(RTC_Date.RTC_Date,5,0+SCREEN_1,2);
	GUI_DispString(".");
	GUI_DispDec(RTC_Date.RTC_Month,2);
	GUI_DispString(".20");
	GUI_DispDec(RTC_Date.RTC_Year,2);
		
	WM_Paint(hALARMA);
	WM_Paint(hALARMB);
	WM_Paint(hIcon_EXIT);	
	WM_Paint(hIcon_BRIGHT);
	WM_Paint(PROGBAR_MEM);	
	WM_Paint(hBUTTON_PERFORM);
	
	
	
/********************************************************************/
/*					SDIO card insert  PB1	  																*/
/********************************************************************/	
		GPIO_InitStruct.GPIO_Pin=SDCARD_INSERT_PIN;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
		GPIO_Init(SDCARD_INSERT_PORT,&GPIO_InitStruct);	
		EXTI_ClearITPendingBit(EXTI_Line1);
	
	ADC_Cmd(ADC1,ENABLE);
	NVIC_EnableIRQ(ADC_IRQn);
	
	NVIC_ClearPendingIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);									//Разрешение EXTI1_IRQn прерывания
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	TIM7->CNT=0;
	TIM7->SR &= ~TIM_SR_UIF; 			//Сбрасываем флаг UIF
	NVIC_ClearPendingIRQ(TIM7_IRQn);
	NVIC_EnableIRQ(TIM7_IRQn);
	NVIC_EnableIRQ(RTC_WKUP_IRQn);
	sleep_mode=0;
	backlight=BACKLIGHT_ON;
	backlight_delay=0;


}
void SDRAM_PinConfig(void){
	/* Connect PDx pins to FMC Alternate function  AF12 (1100=0xC) */
	GPIOD->AFR[0]|=0x000000CC;
	GPIOD->AFR[1]|=0xCC000CCC;
	/* Configure PDx pins in Alternate function mode */
	GPIOD->MODER|=0xA02A000A;
	/* Configure PDx pins speed to 100 MHz */
	GPIOD->OSPEEDR|=0xF03F000F;
	/* Configure PDx pins Output type to push-pull */
	GPIOD->OTYPER|=0x00000000;
	/* No pull-up, pull-down for PDx pins */ 
	GPIOD->PUPDR|=0x00000000;
	
	/* Connect PEx pins to FMC Alternate function  AF12 (1100=0xC) */
	GPIOE->AFR[0]|=0xC00000CC;
	GPIOE->AFR[1]|=0xCCCCCCCC;	
	/* Configure PEx pins in Alternate function mode */
	GPIOE->MODER|=0xAAAA800A;
	/* Configure PEx pins speed to 100 MHz */
	GPIOE->OSPEEDR|=0xFFFFC00F;
	/* Configure PEx pins Output type to push-pull */
	GPIOE->OTYPER|=0x00000000;
	/* No pull-up, pull-down for PEx pins */ 
	GPIOE->PUPDR|=0x00000000;
	
	/* Connect PFx pins to FMC Alternate function  AF12 (1100=0xC) */
	GPIOF->AFR[0]=0x00CCCCCC;
	GPIOF->AFR[1]=0xCCCCC000;	
	/* Configure PFx pins in Alternate function mode */
	GPIOF->MODER=0xAA800AAA;
	/* Configure PFx pins speed to 100 MHz */
	GPIOF->OSPEEDR=0xFFC00FFF;
	/* Configure PFx pins Output type to push-pull */
	GPIOF->OTYPER=0x00000000;
	/* No pull-up, pull-down for PFx pins */ 
	GPIOF->PUPDR=0x00000000;

	/* Connect PEx pins to FMC Alternate function  AF12 (1100=0xC) */
	GPIOG->AFR[0]=0x00CC00CC;
	GPIOG->AFR[1]=0xC000000C;	
	/* Configure PGx pins in Alternate function mode */
	GPIOG->MODER=0x80020A0A;
	/* Configure PGx pins speed to 100 MHz */
	GPIOG->OSPEEDR=0xC0030F0F;
	/* Configure PGx pins Output type to push-pull */
	GPIOG->OTYPER=0x00000000;
	/* No pull-up, pull-down for PGx pins */ 
	GPIOG->PUPDR=0x00000000;

	/* Connect PHx pins to FMC Alternate function  AF12 (1100=0xC) */
	GPIOH->AFR[0]=0xCCC00000;
	/* Configure PHx pins in Alternate function mode */
	GPIOH->MODER=0x0000A800;
	/* Configure PHx pins speed to 100 MHz */
	GPIOH->OSPEEDR=0x0000FC00;
	/* Configure PHx pins Output type to push-pull */
	GPIOH->OTYPER=0x00000000;
	/* No pull-up, pull-down for PHx pins */ 
	GPIOH->PUPDR=0x00000000;
}

/*
*/
	
// USER END

/*************************** End of file ****************************/
