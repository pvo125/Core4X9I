



#include "DIALOG.h"
#include "header.h"


const char *_alarm_action[]={"","START PWM","STOP PWM","START ADC","STOP ADC"};

const char *_hoursA[]={"00","01","02","03","04","05","06","07","08","09",
												"10","11","12","13","14","15","16","17",
												"18","19","20","21","22","23",NULL};
const char *_minuteA[]={"00","01","02","03","04","05","06","07","08","09",
												"10","11","12","13","14","15","16","17",
												"18","19","20","21","22","23","24","25",
												"26","27","28","29","30","31","32","33",
												"34","35","36","37","38","39","40","41",
												"42","43","44","45","46","47","48","49",
												"50","51","52","53","54","55","56","57","58","59",NULL};
													
const char *_daysA[]={"01","02","03","04","05","06","07","08","09","10",
											"11","12","13","14","15","16","17","18","19","20",
											"21","22","23","24","25","26","27","28","29","30","31",NULL};
										
WM_HWIN hWin_alarm;
extern WM_HWIN hWin_menu;
extern RTC_AlarmTypeDef				   	  	RTC_AlarmA,RTC_AlarmB;								

char Alarm_A_B;									
int maskA,maskB;											
uint8_t hourA,minuteA;
uint16_t dayA;										
/*********************************************************************
*
*       Defines
*
**********************************************************************/
#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x0F)
#define ID_DROPDOWN_0     (GUI_ID_USER + 0x10)

#define ID_LISTWHEEL_0		(GUI_ID_USER + 0x11)
#define ID_LISTWHEEL_1		(GUI_ID_USER + 0x12)
#define ID_LISTWHEEL_2		(GUI_ID_USER + 0x13)

#define ID_BUTTON_SAVE	   (GUI_ID_USER + 0x16)
#define ID_RADIO_0     (GUI_ID_USER + 0x17)
#define ID_DROPDOWN_1     (GUI_ID_USER + 0x18)
/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "ALARM", ID_FRAMEWIN_0 , 120, 5, 280, 210, 0, 0x0, 0 },
  { DROPDOWN_CreateIndirect, NULL, ID_DROPDOWN_0, 10, 120, 70, 35, 0, 0x0, 0 },
	{ DROPDOWN_CreateIndirect, NULL, ID_DROPDOWN_1, 180, 30, 90, 70, 0, 0x0, 0 },
	{ RADIO_CreateIndirect, NULL, ID_RADIO_0, 100, 120, 85, 40, 0, 0x0, 0 },
	{ LISTWHEEL_CreateIndirect, NULL, ID_LISTWHEEL_0, 10, 20, 30, 83, 0, 0x0, 0 },
	{ LISTWHEEL_CreateIndirect, NULL, ID_LISTWHEEL_1, 45, 20, 30, 83, 0, 0x0, 0 },
	{ LISTWHEEL_CreateIndirect, NULL, ID_LISTWHEEL_2, 120, 20, 30, 83, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "SAVE", ID_BUTTON_SAVE, 200, 120, 50, 30, 0, 0x0, 0 },
};

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*    
*/

static void _cbDialog(WM_MESSAGE * pMsg) {
  uint8_t i;
	WM_HWIN hItem;
  int     NCode;
  int     Id;
	switch (pMsg->MsgId) {
			case WM_DELETE:
				RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmA);
				RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_B, &RTC_AlarmB);
				Alarm_A_B=0;
				hWin_alarm=0;
			break;	
			case WM_INIT_DIALOG:
				if(RTC_AlarmA.RTC_AlarmMask==RTC_AlarmMask_None)
					maskA=0;
				else if(RTC_AlarmA.RTC_AlarmMask==RTC_AlarmMask_DateWeekDay)
					maskA=1;
				hItem = pMsg->hWin;
				FRAMEWIN_SetTitleHeight(hItem, 25);
				FRAMEWIN_SetFont(hItem,GUI_FONT_16_1);
				FRAMEWIN_AddCloseButton(hItem, FRAMEWIN_BUTTON_RIGHT,0);
				FRAMEWIN_SetMoveable(hItem,1);	
				
				hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_0);
				RADIO_SetText(hItem,"ONCE",0);
				RADIO_SetText(hItem,"EVERY DAY",1);
				RADIO_SetValue(hItem,maskA);
								
				ALARMA_ACTION=RTC_ReadBackupRegister(RTC_BKP_DR0);
				ALARMB_ACTION=RTC_ReadBackupRegister(RTC_BKP_DR1);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_1);
				for(i=0;i<5;i++)
					DROPDOWN_AddString(hItem, _alarm_action[i]);
				DROPDOWN_SetSel(hItem,ALARMA_ACTION);
				
				
				
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
				DROPDOWN_AddString(hItem, "ALARM_A");
				DROPDOWN_AddString(hItem, "ALARM_B");
				
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
				LISTWHEEL_SetText(hItem,_hoursA);
				LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
				LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
			  LISTWHEEL_SetSnapPosition(hItem,32);
				LISTWHEEL_SetPos(hItem,RTC_AlarmA.RTC_AlarmTime.RTC_Hours+1);
				LISTWHEEL_SetVelocity(hItem,3);
							
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
				LISTWHEEL_SetText(hItem,_minuteA);
				LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
				LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
				LISTWHEEL_SetSnapPosition(hItem,32);
				LISTWHEEL_SetPos(hItem,RTC_AlarmA.RTC_AlarmTime.RTC_Minutes+1);
				LISTWHEEL_SetVelocity(hItem,3);
				
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
				LISTWHEEL_SetText(hItem,_daysA);
				LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
				LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
				LISTWHEEL_SetSnapPosition(hItem,32);
				LISTWHEEL_SetPos(hItem,RTC_AlarmA.RTC_AlarmDateWeekDay);
				LISTWHEEL_SetVelocity(hItem,3);
				if(maskA==1)
					{
						WM_DisableWindow(hItem);
						LISTWHEEL_SetBkColor(hItem,LISTWHEEL_CI_UNSEL,GUI_LIGHTRED);
					}
			break;
			case WM_NOTIFY_PARENT:
				Id    = WM_GetId(pMsg->hWinSrc);
				NCode = pMsg->Data.v;
				switch(Id) {
					case ID_DROPDOWN_0: // Notifications sent by 'Dropdown'
						switch(NCode) {
							case WM_NOTIFICATION_SEL_CHANGED:
							hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
							Alarm_A_B=DROPDOWN_GetSel(hItem);
							if(Alarm_A_B==0)
								{
									if(RTC_AlarmA.RTC_AlarmMask==RTC_AlarmMask_None)
										maskA=0;
									else if(RTC_AlarmA.RTC_AlarmMask==RTC_AlarmMask_DateWeekDay)
										maskA=1;
									hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_0);
									RADIO_SetValue(hItem,maskA);
									
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
									LISTWHEEL_SetText(hItem,_hoursA);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,RTC_AlarmA.RTC_AlarmTime.RTC_Hours+1);
									LISTWHEEL_SetVelocity(hItem,3);
										
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
									LISTWHEEL_SetText(hItem,_minuteA);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,RTC_AlarmA.RTC_AlarmTime.RTC_Minutes+1);
									LISTWHEEL_SetVelocity(hItem,3);
									
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
									if(maskA==1)
									{	
										WM_DisableWindow(hItem);
										LISTWHEEL_SetBkColor(hItem,LISTWHEEL_CI_UNSEL,GUI_LIGHTRED);
									}
									else
									{		
										LISTWHEEL_SetText(hItem,_daysA);
										LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
										LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
										LISTWHEEL_SetSnapPosition(hItem,32);
										LISTWHEEL_SetPos(hItem,RTC_AlarmA.RTC_AlarmDateWeekDay);
										LISTWHEEL_SetVelocity(hItem,3);
										LISTWHEEL_SetBkColor(hItem,LISTWHEEL_CI_UNSEL,GUI_WHITE);
										WM_EnableWindow(hItem);
									}
									hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_1);
									DROPDOWN_SetSel(hItem,ALARMA_ACTION);		
								}
							else
								{
									if(RTC_AlarmB.RTC_AlarmMask==RTC_AlarmMask_None)
										maskB=0;
									else if(RTC_AlarmB.RTC_AlarmMask==RTC_AlarmMask_DateWeekDay)
										maskB=1;
									hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_0);
									RADIO_SetValue(hItem,maskB);
									
									
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
									LISTWHEEL_SetText(hItem,_hoursA);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,RTC_AlarmB.RTC_AlarmTime.RTC_Hours+1);
									LISTWHEEL_SetVelocity(hItem,3);
									
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
									LISTWHEEL_SetText(hItem,_minuteA);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,RTC_AlarmB.RTC_AlarmTime.RTC_Minutes+1);
									LISTWHEEL_SetVelocity(hItem,3);
								
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
									if(maskB==1)
									{	
										WM_DisableWindow(hItem);
										LISTWHEEL_SetBkColor(hItem,LISTWHEEL_CI_UNSEL,GUI_LIGHTGREEN);
									}
									else
									{	
										LISTWHEEL_SetText(hItem,_daysA);
										LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
										LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
										LISTWHEEL_SetSnapPosition(hItem,32);
										LISTWHEEL_SetPos(hItem,RTC_AlarmB.RTC_AlarmDateWeekDay);
										LISTWHEEL_SetVelocity(hItem,3);
										LISTWHEEL_SetBkColor(hItem,LISTWHEEL_CI_UNSEL,GUI_WHITE);
										WM_EnableWindow(hItem);
									}
									hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_1);
									DROPDOWN_SetSel(hItem,ALARMB_ACTION);
								}		
							break;
							}
					break;
					case ID_LISTWHEEL_0: 
						switch(NCode) {
							case WM_NOTIFICATION_SEL_CHANGED:
								hItem=WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
								hourA=LISTWHEEL_GetPos(hItem);
								LISTWHEEL_SetSel(hItem,hourA);
								WM_Paint(hItem);
							break;
							}
					break;
					case ID_LISTWHEEL_1: 
						switch(NCode) {
							case WM_NOTIFICATION_SEL_CHANGED:
								hItem=WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
								minuteA=LISTWHEEL_GetPos(hItem);
								LISTWHEEL_SetSel(hItem,minuteA);
								WM_Paint(hItem);
							break;
							}
					break;
					case ID_LISTWHEEL_2: 
						switch(NCode) {
							case WM_NOTIFICATION_SEL_CHANGED:
								hItem=WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
								dayA=LISTWHEEL_GetPos(hItem);
								LISTWHEEL_SetSel(hItem,dayA);
								WM_Paint(hItem);
							break;
							}
					break;
					case ID_RADIO_0: 
						switch(NCode) {
							case WM_NOTIFICATION_CLICKED:
							hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
							Alarm_A_B=DROPDOWN_GetSel(hItem);
							hItem=WM_GetDialogItem(pMsg->hWin, ID_RADIO_0);	
							if(Alarm_A_B==0)
								maskA=RADIO_GetValue(hItem);
							else
								maskB=RADIO_GetValue(hItem);	
							hItem=WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
							if(Alarm_A_B==0)
							{		if(maskA==0)
									{
									RTC_AlarmA.RTC_AlarmMask=RTC_AlarmMask_None ;
									LISTWHEEL_SetBkColor(hItem,LISTWHEEL_CI_UNSEL,GUI_WHITE);
									WM_EnableWindow(hItem);
									}
									else if(maskA==1)
									{
									RTC_AlarmA.RTC_AlarmMask=RTC_AlarmMask_DateWeekDay;
									LISTWHEEL_SetBkColor(hItem,LISTWHEEL_CI_UNSEL,GUI_LIGHTRED);
									WM_DisableWindow(hItem);
									}
							}
							else 
							{
								if(maskB==0)
									{
									RTC_AlarmB.RTC_AlarmMask=RTC_AlarmMask_None ;
									LISTWHEEL_SetBkColor(hItem,LISTWHEEL_CI_UNSEL,GUI_WHITE);
									WM_EnableWindow(hItem);
									}
								else if(maskB==1)
									{
									RTC_AlarmB.RTC_AlarmMask=RTC_AlarmMask_DateWeekDay;
									LISTWHEEL_SetBkColor(hItem,LISTWHEEL_CI_UNSEL,GUI_LIGHTGREEN);
									WM_DisableWindow(hItem);
									}
							}
							break;
							}
					break;				
					case ID_DROPDOWN_1: 
						switch(NCode) {
							case WM_NOTIFICATION_SEL_CHANGED:
								hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_1);
								if(Alarm_A_B==0)
									ALARMA_ACTION=DROPDOWN_GetSel(hItem);
								else
									ALARMB_ACTION=DROPDOWN_GetSel(hItem);
							break;
							}	
					break;			
					case ID_BUTTON_SAVE: 
						switch(NCode) {
							case WM_NOTIFICATION_CLICKED:
								  if(Alarm_A_B==0)
									{	
										if((RTC->CR&RTC_CR_ALRAE)==RTC_CR_ALRAE)
										{
											GUI_MessageBox("DISABLE ALARM_A", "Message", 0);
											break;
										}	
										RTC_WriteBackupRegister(RTC_BKP_DR0,ALARMA_ACTION);
										RTC_AlarmA.RTC_AlarmTime.RTC_Hours=hourA;
										RTC_AlarmA.RTC_AlarmTime.RTC_Minutes=minuteA;
										RTC_AlarmA.RTC_AlarmTime.RTC_Seconds=0;
										RTC_AlarmA.RTC_AlarmDateWeekDay=dayA+1;
										RTC_AlarmA.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_Date;
										RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A, &RTC_AlarmA);
									}	
								 else
									{
										if((RTC->CR&RTC_CR_ALRBE)==RTC_CR_ALRBE)
										{
											GUI_MessageBox("DISABLE ALARM_B", "Message", 0);
											break;
										}	
										RTC_WriteBackupRegister(RTC_BKP_DR1,ALARMB_ACTION);
										RTC_AlarmB.RTC_AlarmTime.RTC_Hours=hourA;
										RTC_AlarmB.RTC_AlarmTime.RTC_Minutes=minuteA;
										RTC_AlarmB.RTC_AlarmTime.RTC_Seconds=0;
										RTC_AlarmB.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_Date;
										RTC_AlarmB.RTC_AlarmDateWeekDay=(dayA+1);
										RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_B, &RTC_AlarmB);
									}
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
			
WM_HWIN CreateALARM(void) {
  hWin_alarm = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, hWin_menu, 0, 0);
  return hWin_alarm;
}

/*************************** End of file ****************************/

