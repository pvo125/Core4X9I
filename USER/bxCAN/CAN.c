#include "stm32f4xx.h"
//#include <stm32f429xx.h>
#include "CAN.h"
#include "header.h"
CANTX_TypeDef CAN_Data_TX;
CANRX_TypeDef CAN_Data_RX[2];

extern RTC_TimeTypeDef								RTC_Time;
extern RTC_DateTypeDef								RTC_Date;
extern RTC_AlarmTypeDef							RTC_AlarmA,RTC_AlarmB;
extern int maskA,maskB;

volatile uint32_t count;
volatile int size_firmware;
volatile uint8_t write_flashflag=0;

extern volatile uint8_t backlight_flag;

extern ICONVIEW_Handle hALARMA, hALARMB;

extern int _cbButtonSkin(const WIDGET_ITEM_DRAW_INFO *pDrawItemInfo);
extern WM_HWIN hWin_timer;
#define ID_ICON_ALARM_A     (GUI_ID_USER + 0x18)
#define ID_ICON_ALARM_B     (GUI_ID_USER + 0x19)
#define ID_BUTTON_0     (GUI_ID_USER + 0x10)
#define ID_BUTTON_1     (GUI_ID_USER + 0x11)
#define ID_PROGBAR_0     (GUI_ID_USER + 0x13)
#define ID_SPINBOX_0     (GUI_ID_USER + 0x14)
#define ID_SPINBOX_1     (GUI_ID_USER + 0x16)
#define ID_PROGBAR_1     (GUI_ID_USER + 0x17)
#define ID_DROPDOWN_0    (GUI_ID_USER + 0x18)

#define FLAG_STATUS_SECTOR	0x08004000		//sector 1
#define FIRM_UPD_SECTOR 		0x08080000			//sector12		firmware update base

#define NAMBER_WORK_SECTOR			2						//	первый work сектор 				2
																						//  последний work сектор   	7
#define NAMBER_UPD_SECTOR				8						//	первый update	 сектор 		8
#define NAMBER_SECT_U_END 			12					//  последний update сектор		11

#define NETNAME_INDEX  01   //Core4X9I 

extern const uint32_t crc32_table[];
uint32_t crc32_check(const uint8_t *buff,uint32_t count);
void Flash_unlock(void);
void Flash_lock(void);
void Flash_sect_erase(uint8_t numsect,uint8_t count);
void Flash_prog(uint8_t *src,uint8_t *dst,uint32_t nbyte,uint8_t psize);


void bxCAN_LowLevel_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct;
/*Настройка выводов CAN  CAN1_TX=PB9   CAN1_RX=PB8  */
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=CAN1_TX|CAN1_RX;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Fast_Speed;
	GPIO_Init(CAN1_PORT,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(CAN1_PORT,CAN1_TX_Source,GPIO_AF_CAN1);
	GPIO_PinAFConfig(CAN1_PORT,CAN1_RX_Source,GPIO_AF_CAN1);
}
/****************************************************************************************************************
*														bxCAN_Init
****************************************************************************************************************/
void bxCAN_Init(void){
	 
	bxCAN_LowLevel_Init();
	
	/*Включаем тактирование CAN в модуле RCC*/	
	RCC->APB1ENR|=RCC_APB1ENR_CAN1EN;
	
	CAN1->RF1R|=CAN_RF0R_RFOM0;
	CAN1->RF1R|=CAN_RF1R_RFOM1;
	
	/*Настройка NVIC для bxCAN interrupt*/
	NVIC_SetPriority( CAN1_RX0_IRQn, 1);
	NVIC_SetPriority(CAN1_RX1_IRQn,  1);
	
	/*Exit SLEEP mode*/
	CAN1->MCR&=~CAN_MCR_SLEEP;
	/*Enter Init mode bxCAN*/
	CAN1->MCR|=CAN_MCR_INRQ;  /*Initialization Request */
	while((CAN1->MSR&CAN_MSR_INAK)!=CAN_MSR_INAK)		{}   /*while Initialization Acknowledge*/

	CAN1->MCR|=CAN_MCR_DBF;			// CAN работает в режиме отладки//CAN останавливается в режиме отладки
	CAN1->MCR|=CAN_MCR_ABOM;		// Контроллер выходит из состояния «Bus-Off» автоматически 
	CAN1->MCR&=~CAN_MCR_TTCM;
	CAN1->MCR&=~CAN_MCR_AWUM;
	CAN1->MCR&=~CAN_MCR_NART;		// запрет автоматической повторной передачи снят(повтор передач).
	CAN1->MCR&=~CAN_MCR_RFLM;
	CAN1->MCR|=CAN_MCR_TXFP;		//	сообщения отправляются в хроног. порядке.
	/*Тестовый режиим работы выключен CAN  SILM=0  LBKM=0 */
	
	CAN1->BTR&=~CAN_BTR_LBKM;	
	CAN1->BTR&=~CAN_BTR_SILM;	

	CAN1->BTR|=CAN_BTR_BRP&29;														/* tq=(29+1)*tPCLK1=2/3 uS   */
	CAN1->BTR|=CAN_BTR_SJW_0;															/*SJW[1:0]=1  (SJW[1:0]+1)*tCAN=tRJW PROP_SEG =+- 2* tq	*/		
	
	//CAN1->BTR&=~CAN_BTR_TS1_0;
	CAN1->BTR|=CAN_BTR_TS1_2;															/* TS1[3:0]=0X07 */ //tBS1=tq*(7+1)=8*tq
	//CAN1->BTR&=~CAN_BTR_TS2_1;
	//CAN1->BTR|=CAN_BTR_TS2_0;														/* TS2[2:0]=0X02 */ //tBS2=tq*(2+1)=3*tq
	
																												// | 1tq | 		8tq 				 |  3tq		| 		T=12*tq=12*2/3=8uS f=125kHz
																												// |-----------------------|---------|		
																												// 								Sample point = 75%		
		/*Init filters*/
	
	CAN1->FMR|=	CAN_FMR_FINIT;																		// Filter Init Mode
	CAN1->FM1R|=CAN_FM1R_FBM0|CAN_FM1R_FBM1|CAN_FM1R_FBM2;  				// Filters bank 0 1 2  mode ID List
	CAN1->FS1R&=~(CAN_FS1R_FSC0|CAN_FS1R_FSC1|CAN_FS1R_FSC2);				// Filters bank 0 1 2  scale 16 bits
	CAN1->FFA1R&=~(CAN_FFA1R_FFA0|CAN_FFA1R_FFA1|CAN_FFA1R_FFA2);		// Filters bank 0 1 2  FIFO0		
		
	CAN1->FM1R|=CAN_FM1R_FBM3|CAN_FM1R_FBM4|CAN_FM1R_FBM5;					// Filters bank 3 4 5  mode ID List		
	CAN1->FS1R&=~(CAN_FS1R_FSC3|CAN_FS1R_FSC4|CAN_FS1R_FSC5);				// Filters bank 3 4 5  scale 16 bits	
	CAN1->FFA1R|=CAN_FFA1R_FFA3|CAN_FFA1R_FFA4|CAN_FFA1R_FFA5;			// Filters bank 3 4 5 FIFO1		

	/*ID filters */
  //FOFO0
	CAN1->sFilterRegister[0].FR1=0x10105000;	//Filters bank 0 fmi 00 ID=0x280 IDE=0 RTR=0	// 
																						//							 fmi 01 ID=0x080 IDE=0 RTR=1	// GET_RTC(remote) 
	CAN1->sFilterRegister[0].FR2=0x10505020;	//Filters bank 0 fmi 02 ID=0x281 IDE=0 RTR=0	//
																						//							 fmi 03 ID=0x082 IDE=0 RTR=1	// GET_TIMER_DATA(remote)
	CAN1->sFilterRegister[1].FR1=0x50505040;	//Filters bank 1 fmi 04 ID=0x282 IDE=0 RTR=0	// 
																						//							 fmi 05 ID=0x282 IDE=0 RTR=1	// ENABLE_TIMER
	CAN1->sFilterRegister[1].FR2=0x50705060;	//Filters bank 1 fmi 06 ID=0x283 IDE=0 RTR=0	// SET_TIMER_DATA
																						//							 fmi 07 ID=0x283 IDE=0 RTR=1	// DISABLE_TIMER
	CAN1->sFilterRegister[2].FR1=0x50905080;	//Filters bank 2 fmi 08 ID=0x284 IDE=0 RTR=0	// GET_ALARM_A
																						//							 fmi 09 ID=0x284 IDE=0 RTR=1	// ENABLE_ALARM_A	
	CAN1->sFilterRegister[2].FR2=0x50B050A0;	//Filters bank 2 fmi 10 ID=0x285 IDE=0 RTR=0	// SET_ALARM_A
																						//							 fmi 11 ID=0x285 IDE=0 RTR=1	// DISABLE_ALARM_A				
	//FIFO1  
	CAN1->sFilterRegister[3].FR1=0x50D050C0;	//Filters bank 3 fmi 00 ID=0x286 IDE=0 RTR=0	// GET_ALARM_B
																						//							 fmi 01 ID=0x286 IDE=0 RTR=1	// ENABLE ALARM_B
	CAN1->sFilterRegister[3].FR2=0x50F050E0;	//Filters bank 3 fmi 02 ID=0x287 IDE=0 RTR=0	// SET_ALARM_B
																						//							 fmi 03 ID=0x287 IDE=0 RTR=1	// DISABLE ALARM_B
																						
	CAN1->sFilterRegister[4].FR1=0x4E304E20;	//Filters bank 4 fmi 04 ID=0x271 IDE=0 RTR=0	//  UPDATE_FIRMWARE_REQ 
																						//							 fmi 05 ID=0x271 IDE=0 RTR=1
	CAN1->sFilterRegister[4].FR2=0x4E704E60;	//Filters bank 4 fmi 06 ID=0x273 IDE=0 RTR=0	//	DOWNLOAD_FIRMWARW
																						//							 fmi 07 ID=0x273 IDE=0 RTR=1	
	
	CAN1->sFilterRegister[5].FR1=0x10F010E0;	//Filters bank 5 fmi 08 ID=0x087 IDE=0 RTR=0	 
																						//							 fmi 09 ID=0x087 IDE=0 RTR=1	// 
	CAN1->sFilterRegister[5].FR2=0x11101100;	//Filters bank 5 fmi 10 ID=0x088 IDE=0 RTR=0	//  
																						//							 fmi 11 ID=0x088 IDE=0 RTR=1	// 	GET_NETNAME																			
	
	/* Filters activation  */	
	CAN1->FA1R|=CAN_FFA1R_FFA0|CAN_FFA1R_FFA1|CAN_FFA1R_FFA2|
							CAN_FFA1R_FFA3|CAN_FFA1R_FFA4|CAN_FFA1R_FFA5;		//
							
	/*Exit filters init mode*/
	CAN1->FMR&=	~CAN_FMR_FINIT;
	
	/*Разрешение прерываний FIFO0 FIFO1*/
	CAN1->IER|=CAN_IER_FMPIE0|CAN_IER_FMPIE1;

//	 Exit Init mode bxCAN	

	CAN1->MCR&=~CAN_MCR_INRQ;  														/*Initialization Request */	
	while((CAN1->MSR&CAN_MSR_INAK)==CAN_MSR_INAK)		{}   /*while Initialization Acknowledge*/		

			
}
/*****************************************************************************************************************
*													CAN_Transmit_DataFrame
******************************************************************************************************************/
CAN_TXSTATUS CAN_Transmit_DataFrame(CANTX_TypeDef *Data){
		uint32_t temp=0;
		uint8_t mailbox_index;
	
	if((CAN1->TSR&CAN_TSR_TME0)==CAN_TSR_TME0)
		mailbox_index=0;
	else if((CAN1->TSR&CAN_TSR_TME1)==CAN_TSR_TME1)
		mailbox_index=1;
	else if((CAN1->TSR&CAN_TSR_TME2)==CAN_TSR_TME2)
		mailbox_index=2;
	else
		return CAN_TXBUSY;
	

	CAN1->sTxMailBox[mailbox_index].TIR=(uint32_t)(Data->ID<<21);//&0xffe00000);
	
	CAN1->sTxMailBox[mailbox_index].TDTR&=(uint32_t)0xfffffff0;
	CAN1->sTxMailBox[mailbox_index].TDTR|=Data->DLC;
	
	temp=(Data->Data[3]<<24)|(Data->Data[2]<<16)|(Data->Data[1]<<8)|(Data->Data[0]);
	CAN1->sTxMailBox[mailbox_index].TDLR=temp;
	temp=(Data->Data[7]<<24)|(Data->Data[6]<<16)|(Data->Data[5]<<8)|(Data->Data[4]);
	CAN1->sTxMailBox[mailbox_index].TDHR=temp;
	
	/*Send message*/
	CAN1->sTxMailBox[mailbox_index].TIR|=CAN_TI0R_TXRQ;
	return CAN_TXOK;
		
}	
/*****************************************************************************************************************
*													CAN_Transmit_RemoteFrame
******************************************************************************************************************/

CAN_TXSTATUS CAN_Transmit_RemoteFrame(uint16_t ID){
	
	uint8_t mailbox_index;
	
	if((CAN1->TSR&CAN_TSR_TME0)==CAN_TSR_TME0)
		mailbox_index=0;
	else if((CAN1->TSR&CAN_TSR_TME1)==CAN_TSR_TME1)
		mailbox_index=1;
	else if((CAN1->TSR&CAN_TSR_TME2)==CAN_TSR_TME2)
		mailbox_index=2;
	else
		return CAN_TXBUSY;
	
	CAN1->sTxMailBox[mailbox_index].TIR=(uint32_t)((ID<<21)|0x2);
	CAN1->sTxMailBox[mailbox_index].TDTR&=(uint32_t)0xfffffff0;
	
	/*Send message*/
	CAN1->sTxMailBox[mailbox_index].TIR|=CAN_TI0R_TXRQ;
	return CAN_TXOK;
}
/*****************************************************************************************************************
*													CAN_Receive
******************************************************************************************************************/
void CAN_Receive_IRQHandler(uint8_t FIFONumber){
	
	
	if((CAN1->sFIFOMailBox[FIFONumber].RIR&CAN_RI0R_RTR)!=CAN_RI0R_RTR)
	{
		
		CAN_Data_RX[FIFONumber].Data[0]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDLR);
		CAN_Data_RX[FIFONumber].Data[1]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDLR>>8);
		CAN_Data_RX[FIFONumber].Data[2]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDLR>>16);
		CAN_Data_RX[FIFONumber].Data[3]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDLR>>24);
		
		CAN_Data_RX[FIFONumber].Data[4]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDHR);
		CAN_Data_RX[FIFONumber].Data[5]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDHR>>8);
		CAN_Data_RX[FIFONumber].Data[6]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDHR>>16);
		CAN_Data_RX[FIFONumber].Data[7]=(uint8_t)0xFF&(CAN1->sFIFOMailBox[FIFONumber].RDHR>>24);
		
		CAN_Data_RX[FIFONumber].DLC=(uint8_t)0x0F & CAN1->sFIFOMailBox[FIFONumber].RDTR;
		
	}
		CAN_Data_RX[FIFONumber].ID= (uint16_t)0x7FF & (CAN1->sFIFOMailBox[FIFONumber].RIR>>21);
		CAN_Data_RX[FIFONumber].FMI=(uint8_t)0xFF & (CAN1->sFIFOMailBox[FIFONumber].RDTR>>8);
	
	/*Запрет прерываний FIFO0 FIFO1 на время обработки сообщения*/
		if(FIFONumber)
			CAN1->IER&=~CAN_IER_FMPIE1;	
		else
			CAN1->IER&=~CAN_IER_FMPIE0;
	/*Release FIFO*/
	
	if(FIFONumber)
		CAN1->RF1R|=CAN_RF1R_RFOM1;
	else	
		CAN1->RF0R|=CAN_RF0R_RFOM0;

}

/*****************************************************************************************************************
*													CAN_RXProcess0
******************************************************************************************************************/
void CAN_RXProcess0(void){
	WM_HWIN hItem;
	int temp;
	switch(CAN_Data_RX[0].FMI) {
		case 0://(id=280 )
		//
		break;
		case 1://(id=080 GET_RTC remote )
		CAN_Data_TX.ID=(NETNAME_INDEX<<8)|0x80;
		CAN_Data_TX.DLC=6;
		CAN_Data_TX.Data[0]=NETNAME_INDEX;  // netname_index для Core4X9I
		CAN_Data_TX.Data[1]=RTC_Time.RTC_Hours;
		CAN_Data_TX.Data[2]=RTC_Time.RTC_Minutes;
		CAN_Data_TX.Data[3]=RTC_Date.RTC_Date;
		CAN_Data_TX.Data[4]=RTC_Date.RTC_Month;
		CAN_Data_TX.Data[5]=RTC_Date.RTC_Year;
		CAN_Transmit_DataFrame(&CAN_Data_TX);
		break;	
		case 2://(id=281 )
		//
		RTC_Time.RTC_Hours=CAN_Data_RX[0].Data[0];
		RTC_Time.RTC_Minutes=CAN_Data_RX[0].Data[1];
		RTC_SetTime(RTC_Format_BIN, &RTC_Time);
		RTC_Date.RTC_Date=CAN_Data_RX[0].Data[2];
		RTC_Date.RTC_Month=CAN_Data_RX[0].Data[3];
		RTC_Date.RTC_Year=CAN_Data_RX[0].Data[4];
		RTC_SetDate(RTC_Format_BIN, &RTC_Date);
		break;
		case 3://(id=082 GET_TIMER_DATA remote )
		//
		CAN_Data_TX.ID=(NETNAME_INDEX<<8)|0x82;
		CAN_Data_TX.Data[0]=NETNAME_INDEX; //netname_index для Core4X9I
		CAN_Data_TX.Data[1]=(uint8_t)PhaseBrez;
		CAN_Data_TX.Data[2]=(uint8_t)PhasePower;
		CAN_Data_TX.Data[3]=(uint8_t)BrezPower;
		CAN_Data_TX.Data[4]=(uint8_t)TimerONOFF;
		CAN_Data_TX.Data[7]=(uint8_t)(CAN1->ESR>>24);		//REC
		CAN_Data_TX.Data[6]=(uint8_t)(CAN1->ESR>>16);		//TEC
		CAN_Data_TX.Data[5]=(uint8_t)(CAN1->ESR);				//ERF
		CAN_Data_TX.DLC=8;
		CAN_Transmit_DataFrame(&CAN_Data_TX);
		break;
		case 5://(id=282 ENABLE_TIMER remote )
		//
		TIM2->CCER |=TIM_CCER_CC2E;												// Enable канал сравнения CC2
		TIM2->CCER|=TIM_CCER_CC1E;												// Enable capture CC1
		TimerONOFF=1;
		if(backlight==BACKLIGHT_OFF_SLEEP)
				backlight=BACKLIGHT_SLEEPtoON;
			else if(backlight==BACKLIGHT_LOW)
				backlight=BACKLIGHT_ON;
			backlight_flag=1;
			backlight_delay=0;
			TIM7->CNT=0;					// Каждый раз при получении сообщения CAN обнуляем таймер подсветки
		if(hWin_timer)
		{
			
			hItem = WM_GetDialogItem(hWin_timer, ID_BUTTON_0);		
			BUTTON_SetSkin(hItem, _cbButtonSkin);
			WM_SetFocus(hItem);	
			hItem = WM_GetDialogItem(hWin_timer, ID_BUTTON_1);	
			BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
			
		}
		break;
		case 6://(id=283 SET_TIMER_DATA data )
		//
		if(backlight==BACKLIGHT_OFF_SLEEP)
			backlight=BACKLIGHT_SLEEPtoON;
		else if(backlight==BACKLIGHT_LOW)
			backlight=BACKLIGHT_ON;
		backlight_flag=1;
		backlight_delay=0;
		TIM7->CNT=0;					// Каждый раз при получении сообщения CAN обнуляем таймер подсветки
		PhaseBrez=CAN_Data_RX[0].Data[1];
		if(PhaseBrez)
		{
			TIM2->CCMR1 |=TIM_CCMR1_IC1PSC_0; // Канал захвата настроим - прерывание на каждый второй спад импульса
			TIM2->DIER |=TIM_DIER_CC1IE;		  //Если меняем режим на Брезенхема то для TIM2 по Capture/Compare 1 interrupt enable
			TIM2->ARR=35;
			TIM2->CCR2=5;
			BrezPower=CAN_Data_RX[0].Data[3];
			temp=BrezPower/2;
			BrezKoeff=temp/50.0f;
			if(hWin_timer)
			{
				hItem = WM_GetDialogItem(hWin_timer, ID_PROGBAR_0);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(hWin_timer, ID_SPINBOX_0);
				WM_DisableWindow(hItem);
				hItem = WM_GetDialogItem(hWin_timer, ID_PROGBAR_1);
				PROGBAR_SetValue(hItem, BrezPower);
				WM_ShowWindow(hItem);
				hItem = WM_GetDialogItem(hWin_timer, ID_SPINBOX_1);
				SPINBOX_SetValue(hItem,BrezPower);
				WM_EnableWindow(hItem);
			}
		}
		else
		{
			PhasePower=CAN_Data_RX[0].Data[2];
			TIM2->CCMR1 &=~TIM_CCMR1_IC1PSC_0;	// Канал захвата настроим на каждый спад импульса
			TIM2->DIER &=~TIM_DIER_CC1IE;				//Если меняем режим на Фазовый то для TIM2 по Capture/Compare 1 interrupt disable
			TIM2->ARR=1000-PhasePower*10;
			TIM2->CCR2=970-PhasePower*10;
			if(hWin_timer)
			{
				hItem = WM_GetDialogItem(hWin_timer, ID_PROGBAR_1);
				WM_HideWindow(hItem);
				hItem = WM_GetDialogItem(hWin_timer, ID_SPINBOX_1);
				WM_DisableWindow(hItem);
				hItem = WM_GetDialogItem(hWin_timer, ID_PROGBAR_0);
				PROGBAR_SetValue(hItem, PhasePower);
				WM_ShowWindow(hItem);
				hItem = WM_GetDialogItem(hWin_timer, ID_SPINBOX_0);
				SPINBOX_SetValue(hItem,PhasePower);
				WM_EnableWindow(hItem);
			}
		}
		if(hWin_timer)
		{
			hItem = WM_GetDialogItem(hWin_timer, ID_DROPDOWN_0);
			DROPDOWN_SetSel(hItem,PhaseBrez);
		}
		break;
		case 7://(id=283 DISABLE_TIMER remote )
		//
		TIM2->CCER &=~TIM_CCER_CC2E;											// Disable канал сравнения CC2
		TIM2->CCER&=~TIM_CCER_CC1E;												//Disable capture CC1
		Brez_Count=0;
		BrezErr=0;
		TimerONOFF=0;
		if(backlight==BACKLIGHT_OFF_SLEEP)
			backlight=BACKLIGHT_SLEEPtoON;
		else if(backlight==BACKLIGHT_LOW)
			backlight=BACKLIGHT_ON;
		backlight_flag=1;
		backlight_delay=0;
		TIM7->CNT=0;					// Каждый раз при получении сообщения CAN обнуляем таймер подсветки
		if(hWin_timer)
		{
			hItem = WM_GetDialogItem(hWin_timer, ID_BUTTON_1);		
			BUTTON_SetSkin(hItem, _cbButtonSkin);
			WM_SetFocus(hItem);	
			hItem = WM_GetDialogItem(hWin_timer, ID_BUTTON_0);	
			BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
			
		}
		break;
		case 8://(id=284 GET_ALARM_A data )
		//
		CAN_Data_TX.ID=(CAN_Data_RX[0].Data[0]<<8|CAN_Data_RX[0].Data[1]); // Пакуем в ID адресс контроллера сделавшего запрос
		CAN_Data_TX.Data[0]=RTC_AlarmA.RTC_AlarmTime.RTC_Hours;
		CAN_Data_TX.Data[1]=RTC_AlarmA.RTC_AlarmTime.RTC_Minutes;
		CAN_Data_TX.Data[2]=(uint8_t)ALARMA_ACTION;
		if(maskA==0)
		{
			CAN_Data_TX.Data[3]=(uint8_t)maskA;
			CAN_Data_TX.Data[4]=RTC_AlarmA.RTC_AlarmDateWeekDay;
			CAN_Data_TX.DLC=5;
		}
		else
		{
			CAN_Data_TX.Data[3]=(uint8_t)maskA;
			CAN_Data_TX.DLC=4;
		}
		CAN_Transmit_DataFrame(&CAN_Data_TX);
		break;
		case 9://(id=284 ENABLE_ALARM_A	 remote )
		//
		WM_DeleteWindow(hALARMA);
		hALARMA=ICONVIEW_CreateEx(10,15,34,34,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_ALARM_A,24,24);
		RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
		ICONVIEW_AddBitmapItem(hALARMA,(const GUI_BITMAP*)(AlarmA+1152),"");
		break;
		case 10://(id=285 SET_ALARM_A data )
		//
		RTC_AlarmA.RTC_AlarmTime.RTC_Hours=CAN_Data_RX[0].Data[0];
		RTC_AlarmA.RTC_AlarmTime.RTC_Minutes=CAN_Data_RX[0].Data[1];
		RTC_AlarmA.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_Date;
		RTC_AlarmA.RTC_AlarmTime.RTC_Seconds=0;
		ALARMA_ACTION=(uint32_t)CAN_Data_RX[0].Data[2];
		if(CAN_Data_RX[0].Data[3]==0)
		{
			RTC_AlarmA.RTC_AlarmMask=RTC_AlarmMask_None;
			RTC_AlarmA.RTC_AlarmDateWeekDay=CAN_Data_RX[0].Data[4]; //Установим день будильника
		}
		else
			RTC_AlarmA.RTC_AlarmMask=RTC_AlarmMask_DateWeekDay;		//День будильника не учитывается т е каждый день "звенит"
			
		
		RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A, &RTC_AlarmA);
		
		break;
		case 11://(id=285 DISABLE_ALARM_A remote )
		//
		WM_DeleteWindow(hALARMA);
		hALARMA=ICONVIEW_CreateEx(10,15,34,34,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_ALARM_A,24,24);
		RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
		ICONVIEW_AddBitmapItem(hALARMA,(const GUI_BITMAP*)(Alarm_d+1152),"");
		
		break;
		default:
		break;	
	
	}
	/*Разрешение прерываний FIFO0 */
	CAN1->IER|=CAN_IER_FMPIE0;
}	

/*****************************************************************************************************************
*													CAN_RXProcess1
******************************************************************************************************************/

void CAN_RXProcess1(void){
	uint32_t crc;
	uint8_t temp;
	switch(CAN_Data_RX[1].FMI) {
		case 4:	//(id=271 UPDATE_FIRMWARE_REQ)
			// если получили запрос на обновление 
		// * вытащить из CAN_Data_RX[1].Data[0]...CAN_Data_RX[1].Data[3] размер прошивки и записать в size_firmware;
		// * разблокировать flash 
		// * стереть сектора второй половины flash 
		// * отправить подтверждение по CAN для запроса UPDATE_FIRMWARE_REQ
		count=0;
		size_firmware=0;
		size_firmware=CAN_Data_RX[1].Data[0];
		size_firmware|=CAN_Data_RX[1].Data[1]<<8;
		size_firmware|=CAN_Data_RX[1].Data[2]<<16;
		size_firmware|=CAN_Data_RX[1].Data[3]<<24;
		
		Flash_unlock();
		if(size_firmware<=131072)
			temp=1;
		else if(size_firmware<=262144)
			temp=2;
		else if(size_firmware<=393216)
			temp=3;
		else
			temp=4;
		
		Flash_sect_erase(NAMBER_UPD_SECTOR,temp);		// Очистим 8,9... сектора ((размер bin)/128K)+1 max 4 сектора
		CAN_Data_TX.ID=(NETNAME_INDEX<<8)|0x72;
		CAN_Data_TX.DLC=2;
		CAN_Data_TX.Data[0]=NETNAME_INDEX;
		CAN_Data_TX.Data[1]='g';								// GET_DATA!
		CAN_Transmit_DataFrame(&CAN_Data_TX);
		break;		
		case 6:	//(id=273 DOWNLOAD_FIRMWARE)
			if((size_firmware-count)>=8)
			{
				Flash_prog(&CAN_Data_RX[1].Data[0],(uint8_t*)(FIRM_UPD_SECTOR+count),8,4);		
				count+=8;
				CAN_Data_TX.ID=(NETNAME_INDEX<<8)|0x72;
				CAN_Data_TX.DLC=2;
				CAN_Data_TX.Data[0]=NETNAME_INDEX;
				CAN_Data_TX.Data[1]='g';								// GET_DATA!
				CAN_Transmit_DataFrame(&CAN_Data_TX);
				
				if((count%240)==0)
				{	
					if(LEDPORT->IDR & LEDPIN_IDR)
						LEDPORT->BSRRH=LEDPIN_BSSR;
					else
						LEDPORT->BSRRL=LEDPIN_BSSR;	
				}
			}
			else 
			{
				Flash_prog(&CAN_Data_RX[1].Data[0],(uint8_t*)(FIRM_UPD_SECTOR+count),(size_firmware-count),4);
				count+=(size_firmware-count);
			}
			
			if(size_firmware==count)	
			{
				
				crc=crc32_check((const uint8_t*)FIRM_UPD_SECTOR,(size_firmware-4));
				if(crc==*(uint32_t*)(FIRM_UPD_SECTOR+size_firmware-4))
				{
					CAN_Data_TX.ID=(NETNAME_INDEX<<8)|0x72;
					CAN_Data_TX.DLC=2;
					CAN_Data_TX.Data[0]=NETNAME_INDEX;
					CAN_Data_TX.Data[1]='c';								// CRC OK!	
					CAN_Transmit_DataFrame(&CAN_Data_TX);
					
					write_flashflag=1;
					
					
				}
				else
				{
					FLASH->CR |=FLASH_CR_LOCK;
					
					CAN_Data_TX.ID=(NETNAME_INDEX<<8)|0x72;
					CAN_Data_TX.DLC=2;
					CAN_Data_TX.Data[0]=NETNAME_INDEX;
					CAN_Data_TX.Data[1]='e';								// CRC ERROR!		
					CAN_Transmit_DataFrame(&CAN_Data_TX);
				}
			}
		break;
		case 11://(id=088 GET_NETNAME remote)
		//
			CAN_Data_TX.ID=(NETNAME_INDEX<<8)|0x88;
			CAN_Data_TX.DLC=1;
			CAN_Data_TX.Data[0]=NETNAME_INDEX;  // // netname_index для Core4X9I
			CAN_Transmit_DataFrame(&CAN_Data_TX);
		break;
		
		default:
		break;
	}
	/*Разрешение прерываний FIFO1*/
	CAN1->IER|=CAN_IER_FMPIE1;
}

void Flash_unlock(void){

	FLASH->KEYR=0x45670123;
	FLASH->KEYR=0xCDEF89AB;
}

void Flash_sect_erase(uint8_t numsect,uint8_t count){
	uint8_t i;
	
	while((FLASH->SR & FLASH_SR_BSY)==FLASH_SR_BSY) {}
	if(FLASH->SR & FLASH_SR_EOP)	// если EOP установлен в 1 значит erase/program complete
		FLASH->SR=FLASH_SR_EOP;		// сбросим его для следующей индикации записи
	
	FLASH->CR |=FLASH_CR_EOPIE;					// включим прерывание для индикации флага EOP 
	FLASH->CR |= FLASH_CR_PSIZE_1;			// 10 program/erase x32
	FLASH->CR |=FLASH_CR_SER;																	// флаг  очистки сектора
	for(i=numsect;i<numsect+count;i++)
		{
			FLASH->CR &= ~FLASH_CR_SNB;														// очистим биты SNB[3:7] 
			FLASH->CR|=(uint32_t)(i<<3);													// запишем номер сектора для erase
			FLASH->CR |=FLASH_CR_STRT;														// запуск очистки заданного сектора
			while((FLASH->SR & FLASH_SR_EOP)!=FLASH_SR_EOP) {}		// ожидание готовности
			FLASH->SR=FLASH_SR_EOP;	
		}
	FLASH->CR &= ~FLASH_CR_SER;																	//сбросим  флаг  очистки сектора
}

void Flash_prog(uint8_t * src,uint8_t * dst,uint32_t nbyte,uint8_t psize){
	uint32_t i;
	
	
	while((FLASH->SR & FLASH_SR_BSY)==FLASH_SR_BSY) {}
	if(FLASH->SR & FLASH_SR_EOP)	// если EOP установлен в 1 значит erase/program complete
		FLASH->SR=FLASH_SR_EOP;		// сбросим его для следующей индикации записи
	
	FLASH->CR |=FLASH_CR_EOPIE;					// включим прерывание для индикации флага EOP 
	switch(psize){
		case 1:
			FLASH->CR &= ~FLASH_CR_PSIZE;			// 00 program x8
			FLASH->CR |=FLASH_CR_PG;
			for(i=0;i<nbyte;i++)
			{
				*(uint8_t*)(dst+i)=*(uint8_t*)(src+i);
				while((FLASH->SR & FLASH_SR_EOP)!=FLASH_SR_EOP) {}
				FLASH->SR=FLASH_SR_EOP;	
			}
		break;
		case 2:
			FLASH->CR |= FLASH_CR_PSIZE_0;			// 01 program x16
			FLASH->CR |=FLASH_CR_PG;	
			for(i=0;i<nbyte;i+=2)
			{
				*(uint16_t*)(dst+i)=*(uint16_t*)(src+i);
				while((FLASH->SR & FLASH_SR_EOP)!=FLASH_SR_EOP) {}
				FLASH->SR=FLASH_SR_EOP;	
			}
		break;
		case 4:
			FLASH->CR |= FLASH_CR_PSIZE_1;			// 10 program x32
			FLASH->CR |=FLASH_CR_PG;	
			for(i=0;i<nbyte;i+=4)
			{
				*(uint32_t*)(dst+i)=*(uint32_t*)(src+i);
				while((FLASH->SR & FLASH_SR_EOP)!=FLASH_SR_EOP) {}
				FLASH->SR=FLASH_SR_EOP;
		  }
		break;	
	}
		
	FLASH->CR &= ~FLASH_CR_PG;

}
void Flash_lock(void){

	FLASH->KEYR=0xFFFFFFFF;
	FLASH->KEYR=0xFFFFFFFF;
}

uint32_t crc32_check(const uint8_t *buff,uint32_t count){

	uint32_t crc=0xffffffff;
	while(count--)
		crc=(crc>>8)^crc32_table[(crc^*buff++) & 0xFF];

	return crc^0xffffffff;

}
const uint32_t crc32_table[256]=
{	
		0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
    0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};
