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

#include "DIALOG.h"
#include "header.h"
#include <string.h>
#include "CAN.h"
extern GUI_PID_STATE State;
extern void PictureView(void);
extern FIL pFile;
extern GUI_JPEG_INFO Info;

extern volatile uint8_t new_message;
//extern uint8_t cycle_start_pwm;

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


	
// USER START (Optionally insert additional defines)

uint8_t sd_insert;
uint8_t sd_ins_rem;
WM_HWIN hWin_start;
ICONVIEW_Handle hIcon_CALIB,hIcon_PAINT,hIcon_NEXT,hButton_BACK,hALARMA,hALARMB,hIcon_EXIT,hIcon_PHOTO,
								hIcon_BRIGHT;
ICONVIEW_Handle hIcon[9];
int xD[]={ID_ICON_BLUE,ID_ICON_GREEN,ID_ICON_RED,ID_ICON_CYAN,ID_ICON_MAGENTA,ID_ICON_YELLOW,ID_ICON_WHITE,ID_ICON_BLACK,ID_ICON_ORANGE};
int color[]={GUI_BLUE,GUI_GREEN,GUI_RED,GUI_CYAN,GUI_MAGENTA,GUI_YELLOW,GUI_WHITE,GUI_BLACK,GUI_ORANGE};
extern WM_HWIN hWin_menu;

extern void Draw_JPG_File(const char *FileName);



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


static void _cbSTART(WM_MESSAGE* pMsg) {
  char i;
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
										GUI_MessageBox("Invalid IMAGE directory"," Message", GUI_MESSAGEBOX_CF_MODAL);
										//GUI_SetBkColor(GUI_LIGHTBLUE);
										GUI_ClearRect(170,105,305,165);
									}
									else
									{			
									time_show=0;
									//	start=0;
									WM_HideWindow(hIcon_EXIT);
									WM_HideWindow(hIcon_BRIGHT);	
									WM_HideWindow(hWin_start);
															
									WM_HideWindow(hALARMA);
									WM_HideWindow(hALARMB);
									WM_HideWindow(PROGBAR_MEM);
									PictureView();
									}	
									
								}
							else
								{
									GUI_MessageBox("Insert SD card!"," Message", GUI_MESSAGEBOX_CF_MODAL);
									GUI_ClearRect(180,105,296,165);
								}	
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
						
							Touch_calibration();
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
									CreateMENU();
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
							WM_HideWindow(hIcon_EXIT);
							WM_HideWindow(hIcon_BRIGHT);
							WM_HideWindow(hWin_start);
							WM_HideWindow(hALARMA);
							WM_HideWindow(hALARMB);
							WM_HideWindow(PROGBAR_MEM);
							
							GUI_SetPenSize(7);
							GUI_SetBkColor(GUI_WHITE);
							GUI_SetColor(GUI_WHITE);
							GUI_Clear();
							TIM6->ARR = 40;
						  TIM6->EGR = TIM_EGR_UG;	
						  hButton_BACK=BUTTON_CreateEx(400,220,50,30,WM_HBKWIN,WM_CF_SHOW,0,ID_BUTTON_BACK);
							BUTTON_SetText(hButton_BACK, "BACK");	
							
							for(i=0;i<9;i++)
						 {
							hIcon[i]=ICONVIEW_CreateEx(0,i*30,30,30,WM_HBKWIN,WM_CF_SHOW,0,xD[i],30,30);
							ICONVIEW_SetBkColor(hIcon[i], ICONVIEW_CI_BK, color[i]);
						 }
						 
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
void _cbBkWin(WM_MESSAGE* pMsg) {
  
	int			i;
	int     NCode;
  int     Id;
	switch(pMsg->MsgId) {
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id){
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
							hALARMA=ICONVIEW_CreateEx(10,15,34,34,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_ALARM_A,24,24);
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
						break;
							}
					break;
					case ID_ICON_ALARM_B: 
					switch(NCode) {
						case WM_NOTIFICATION_CLICKED:
							WM_DeleteWindow(hALARMB);
							hALARMB=ICONVIEW_CreateEx(10,45,34,34,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_ALARM_B,24,24);
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
						break;
						}
					break;		
					case ID_BUTTON_BACK:
					switch(NCode){
						case WM_NOTIFICATION_RELEASED:
							drawmode=0;
							WM_DeleteWindow(hButton_BACK);
							hButton_BACK=0;
							for(i=0;i<9;i++)
							{WM_DeleteWindow(hIcon[i]);
							hIcon[i]=0;
							}
							TIM6->ARR=250;
							TIM6->EGR = TIM_EGR_UG;
							TIM7->CNT=0;
													
							NVIC_EnableIRQ(TIM7_IRQn);
							NVIC_EnableIRQ(RTC_WKUP_IRQn);
							
							GUI_SetBkColor(GUI_LIGHTBLUE);
							GUI_ClearRect(1,17,58,270);
							GUI_SetColor(GUI_YELLOW);
							GUI_DrawRect(0,16,59,271);
							GUI_SetBkColor(GUI_DARKBLUE);
							GUI_ClearRect(0,0,470,15);
							time_show=1;
							CreateStart();
							WM_ShowWindow(hALARMA);
							WM_ShowWindow(hALARMB);
							WM_ShowWindow(hIcon_EXIT);
							WM_ShowWindow(hIcon_BRIGHT);
							WM_ShowWindow(PROGBAR_MEM);
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
	//GUI_SetBkColor(GUI_LIGHTBLUE);
	//GUI_Clear();
	//GUI_SetColor(GUI_BLACK);
	if(hWin_start==0)
	{
		hWin_start=WINDOW_CreateEx(60,15,410, 260,WM_HBKWIN, WM_CF_SHOW,0,ID_WINDOW_0,_cbSTART);	
		WINDOW_SetBkColor(hWin_start, GUI_LIGHTBLUE);	
		
		
		
		
		hIcon_PHOTO=ICONVIEW_CreateEx(240,0,58,65,hWin_start,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_PHOTO,48,65);
		ICONVIEW_AddBitmapItem(hIcon_PHOTO,(const GUI_BITMAP*)(photo+4608),"IMAGE");
		ICONVIEW_SetFont(hIcon_PHOTO,&GUI_Font8x18);
		ICONVIEW_SetIconAlign(hIcon_PHOTO, ICONVIEW_IA_TOP);
		
		
		hIcon_CALIB=ICONVIEW_CreateEx(340,0,58,65,hWin_start,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_CALIB,48,65);
		ICONVIEW_AddBitmapItem(hIcon_CALIB,(const GUI_BITMAP*)(screen+4608),"CALIB");
		ICONVIEW_SetFont(hIcon_CALIB,&GUI_Font8x18);
		ICONVIEW_SetIconAlign(hIcon_CALIB, ICONVIEW_IA_TOP);
		
		hIcon_PAINT=ICONVIEW_CreateEx(340,90,58,65,hWin_start,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_PAINT,48,65);
		ICONVIEW_AddBitmapItem(hIcon_PAINT,(const GUI_BITMAP*)(paint+4608),"PAINT");
		ICONVIEW_SetFont(hIcon_PAINT,&GUI_Font8x18);
		ICONVIEW_SetIconAlign(hIcon_PAINT, ICONVIEW_IA_TOP);
		
		hIcon_NEXT=ICONVIEW_CreateEx(340,200,58,58,hWin_start,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_NEXT,48,48);
		ICONVIEW_AddBitmapItem(hIcon_NEXT,(const GUI_BITMAP*)(next+4608),"");
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
		GUI_DispDecAt(RTC_Date.RTC_Date,5,0,2);
		GUI_DispString(".");
		GUI_DispDec(RTC_Date.RTC_Month,2);
		GUI_DispString(".20");
		GUI_DispDec(RTC_Date.RTC_Year,2);
		
		__enable_irq();
}


void MainTask(void)
{
	NVIC_SetPriority(SysTick_IRQn,1);
	WM_SetCallback(WM_HBKWIN, _cbBkWin);
	
	GUI_SetBkColor(GUI_LIGHTBLUE);
	GUI_ClearRect(1,17,58,270);
	GUI_SetColor(GUI_YELLOW);
	GUI_DrawRect(0,16,59,271);
	GUI_SetBkColor(GUI_DARKBLUE);
	GUI_ClearRect(0,0,470,15);
	
	hIcon_EXIT=ICONVIEW_CreateEx(0,214,58,58,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_EXIT,48,48);
	ICONVIEW_AddBitmapItem(hIcon_EXIT,(const GUI_BITMAP*)(exitt+4608),"");
	hIcon_BRIGHT=ICONVIEW_CreateEx(10,80,34,34,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_BRIGHT,24,24);
	ICONVIEW_AddBitmapItem(hIcon_BRIGHT,&bmbrightness,"");
	hALARMA=ICONVIEW_CreateEx(10,15,34,34,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_ALARM_A,24,24);
	if((RTC->CR&RTC_CR_ALRAE)==RTC_CR_ALRAE)
			ICONVIEW_AddBitmapItem(hALARMA,(const GUI_BITMAP*)(AlarmA+1152),"");
	else
			ICONVIEW_AddBitmapItem(hALARMA,(const GUI_BITMAP*)(Alarm_d+1152),"");
	hALARMB=ICONVIEW_CreateEx(10,45,34,34,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_ALARM_B,24,24);
		if((RTC->CR&RTC_CR_ALRBE)==RTC_CR_ALRBE)
			ICONVIEW_AddBitmapItem(hALARMB,(const GUI_BITMAP*)(AlarmB+1152),"");
		else
			ICONVIEW_AddBitmapItem(hALARMB,(const GUI_BITMAP*)(Alarm_d+1152),"");
	
	EXTI_ClearITPendingBit(EXTI_Line1);
	NVIC_ClearPendingIRQ(EXTI1_IRQn);
	
	NVIC_EnableIRQ(EXTI1_IRQn);									//Разрешение EXTI3_IRQn прерывания
	WM_SetDesktopColor(GUI_BLACK);
	pWIDGET_DRAW_ITEM_FUNC=BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);	
	SCROLLBAR_SetDefaultWidth(20);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	PROGBAR_MEM=PROGBAR_CreateEx(470,0,10,272,WM_HBKWIN,WM_CF_SHOW,PROGBAR_CF_VERTICAL,PROGBAR_MEM);
	PROGBAR_SetMinMax(PROGBAR_MEM,0,400*1024);
	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmA);
	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmB);
	RTC_GetTime(RTC_Format_BIN, &RTC_Time);
	
	GUI_EnableAlpha(1);
	CreateStart();
	
	while(1)
	{
		GUI_Delay(5);	
		
		if(sd_ins_rem)
		{	
			sd_ins_rem=0;
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)==0)	
			{
				if(sd_error==SD_OK)
					{
					GUI_MessageBox("SD card is OK!", "Message", GUI_MESSAGEBOX_CF_MODAL);
					if(start)
						GUI_ClearRect(180,105,296,165);
					}	
				else
					{
					GUI_MessageBox("SD card is error!", "Message", GUI_MESSAGEBOX_CF_MODAL);
					if(start)
						GUI_ClearRect(180,105,296,165);
					}
			}			
			else
			{
			GUI_MessageBox("SD card is removed!", "Message", GUI_MESSAGEBOX_CF_MODAL);
			if(start)
				GUI_ClearRect(180,105,296,165);
			}
		}
		if(sleep_mode)
		{
			
			NVIC_DisableIRQ(TIM6_DAC_IRQn);
			NVIC_DisableIRQ(TIM7_IRQn);
			NVIC_DisableIRQ(RTC_WKUP_IRQn);
			SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
			SysTick->CTRL&=~SysTick_CTRL_TICKINT_Msk;
			__WFI();
		  
			SysTick->VAL=0;
			SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
			SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;
			NVIC_EnableIRQ(TIM6_DAC_IRQn);
			TIM7->CNT=0;
			TIM7->SR &= ~TIM_SR_UIF; 			//Сбрасываем флаг UIF
			NVIC_ClearPendingIRQ(TIM7_IRQn);
			NVIC_EnableIRQ(TIM7_IRQn);
			NVIC_EnableIRQ(RTC_WKUP_IRQn);
			sleep_mode=0;
		}
	}
}

// USER END

/*************************** End of file ****************************/
