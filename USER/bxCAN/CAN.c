#include "stm32f4xx.h"
//#include <stm32f429xx.h>
#include "CAN.h"
#include "header.h"
CANTX_TypeDef CAN_Data_TX;
CANRX_TypeDef CAN_Data_RX[2];

volatile int size_firmware;
extern RTC_TimeTypeDef								RTC_Time;
extern RTC_DateTypeDef								RTC_Date;
extern RTC_AlarmTypeDef							RTC_AlarmA,RTC_AlarmB;
extern int maskA,maskB;
//volatile uint8_t count_message;
//volatile uint8_t new_message;
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

#define NAMBER_WORK_SECTOR			2						//	первый work сектор 				2
																						//  последний work сектор   	7
#define NAMBER_UPD_SECTOR				8						//	первый update	 сектор 		8
#define NAMBER_SECT_U_END 			12					//  последний update сектор		11

#define NETNAME_INDEX  01   //Core4X9I 

void Flash_unlock(void);
void Flash_lock(void);
void Flash_sect_erase(uint8_t numsect,uint8_t count);
void Flash_prog(uint8_t *src,uint8_t *dst,uint32_t nbyte);

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
/****************************************************************************************************************
*														bxCAN_Init
****************************************************************************************************************/
void bxCAN_Init(void){

	GPIO_InitTypeDef GPIO_InitStruct;
		
	
	/*Включаем тактирование CAN в модуле RCC*/	
	RCC->APB1ENR|=RCC_APB1ENR_CAN1EN;
	/*Настройка выводов CAN  CAN1_TX=PB9   CAN1_RX=PB8  */
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;//GPIO_InitStruct. Alternate=GPIO_AF9_CAN1;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;//GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
	GPIO_InitStruct.GPIO_Pin=CAN1_TX|CAN1_RX;//GPIO_InitStruct.Pin=CAN1_TX|CAN1_RX;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;//GPIO_InitStruct.Pull=GPIO_PULLUP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Fast_Speed;//GPIO_InitStruct.Speed=GPIO_SPEED_FAST;
	GPIO_Init(CAN1_PORT,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(CAN1_PORT,CAN1_TX_Source,GPIO_AF_CAN1);
	GPIO_PinAFConfig(CAN1_PORT,CAN1_RX_Source,GPIO_AF_CAN1);
	
	CAN1->RF1R|=CAN_RF0R_RFOM0;
	CAN1->RF1R|=CAN_RF1R_RFOM1;
	
	/*Настройка NVIC для bxCAN interrupt*/
	NVIC_SetPriority( CAN1_RX0_IRQn, 1);
	NVIC_SetPriority(CAN1_RX1_IRQn,  1);
	

//			Init mode				//

	//CAN1->MCR|=CAN_MCR_RESET;
	
	/*Exit SLEEP mode*/
	CAN1->MCR&=~CAN_MCR_SLEEP;
	/*Enter Init mode bxCAN*/
	CAN1->MCR|=CAN_MCR_INRQ;  /*Initialization Request */
	while((CAN1->MSR&CAN_MSR_INAK)!=CAN_MSR_INAK)		{}   /*while Initialization Acknowledge*/

	CAN1->MCR|=CAN_MCR_DBF;			// CAN работает в режиме отладки//CAN останавливается в режиме отладки
	CAN1->MCR|=CAN_MCR_ABOM;		// Контроллер выходит из состояния «Bus-Off» автоматически 
	CAN1->MCR&=~CAN_MCR_TTCM;
	CAN1->MCR&=~CAN_MCR_AWUM;
	CAN1->MCR&=~CAN_MCR_NART;	
	CAN1->MCR&=~CAN_MCR_RFLM;
	CAN1->MCR&=~CAN_MCR_TXFP;	
	/*Тестовый режиим работы выключен CAN  SILM=0  LBKM=0 */
	
	CAN1->BTR&=~CAN_BTR_LBKM;	
	CAN1->BTR&=~CAN_BTR_SILM;	

	CAN1->BTR|=CAN_BTR_BRP&29;														/* tq=(29+1)*tPCLK1=2/3 uS   */
	CAN1->BTR|=CAN_BTR_SJW_0;															/*SJW[1:0]=1  (SJW[1:0]+1)*tCAN=tRJW PROP_SEG =+- 2* tq	*/		
	
	//CAN1->BTR&=~CAN_BTR_TS1_0;
	CAN1->BTR|=CAN_BTR_TS1_2;													/* TS1[3:0]=0X07 */ //tBS1=tq*(7+1)=8*tq
	
	//CAN1->BTR&=~CAN_BTR_TS2_1;
	//CAN1->BTR|=CAN_BTR_TS2_0;													/* TS2[2:0]=0X02 */ //tBS2=tq*(2+1)=3*tq
	
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
																						//							 fmi 09 ID=0x284 IDE=0 RTR=1	// ENABLE ALARM_A	
	CAN1->sFilterRegister[2].FR2=0x50B050A0;	//Filters bank 2 fmi 10 ID=0x285 IDE=0 RTR=0	// SET_ALARM_A
																						//							 fmi 11 ID=0x285 IDE=0 RTR=1	// DISABLE ALARM_A				
	//FIFO1  
	CAN1->sFilterRegister[3].FR1=0x50D050C0;	//Filters bank 3 fmi 00 ID=0x286 IDE=0 RTR=0	// GET_ALARM_B
																						//							 fmi 01 ID=0x286 IDE=0 RTR=1	// ENABLE ALARM_B
	CAN1->sFilterRegister[3].FR2=0x50F050E0;	//Filters bank 3 fmi 02 ID=0x287 IDE=0 RTR=0	// SET_ALARM_B
																						//							 fmi 03 ID=0x287 IDE=0 RTR=1	// DISABLE ALARM_B
	CAN1->sFilterRegister[4].FR1=0x51105100;	//Filters bank 4 fmi 04 ID=0x288 IDE=0 RTR=0	//  UPDATE_FIRMWARE_REQ 
																						//							 fmi 05 ID=0x288 IDE=0 RTR=1
	CAN1->sFilterRegister[4].FR2=0x51305120;	//Filters bank 4 fmi 06 ID=0x289 IDE=0 RTR=0	//	DOWNLOAD_FIRMWARW
																						//							 fmi 07 ID=0x289 IDE=0 RTR=1	
	
	CAN1->sFilterRegister[5].FR1=0x10F010E0;	//Filters bank 5 fmi 08 ID=0x087 IDE=0 RTR=0	 
																						//							 fmi 09 ID=0x087 IDE=0 RTR=1	// 
	CAN1->sFilterRegister[5].FR2=0x11101100;	//Filters bank 5 fmi 10 ID=0x088 IDE=0 RTR=0	//  
																						//							 fmi 11 ID=0x088 IDE=0 RTR=1	// 	GET_NET_NAME																			
	
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

	NVIC_EnableIRQ(CAN1_RX0_IRQn);
	NVIC_EnableIRQ(CAN1_RX1_IRQn);		
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
		case 1://(id=080 remote GET_RTC)
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
		case 4://(id=282 data )
		//
		break;
		case 5://(id=282 remote enable timer )
		//
		TIM2->CCER |=TIM_CCER_CC2E;												// Enable канал сравнения CC2
		TIM2->CCER|=TIM_CCER_CC1E;												// Enable capture CC1
		TimerONOFF=1;
		if(hWin_timer)
		{
			if((backlight==BACKLIGHT_OFF)||(backlight==BACKLIGHT_LOW))
			{/* Включаем PWM на подсветке */
					LcdWriteReg(CMD_EXIT_SLEEP);
					for(temp=0;temp<180000;temp++);//GUI_Delay(5);
					LcdWriteReg(CMD_SET_PWM_CONF); 			//set PWM for Backlight. Manual p.53
					// 6 parameters to be set
					LcdWriteData(0x0004); 							// PWM Freq =100MHz/(256*(PWMF[7:0]+1))/256  PWMF[7:0]=4 PWM Freq=305Hz
					LcdWriteData(brightness); 					// PWM duty cycle(50%)
					LcdWriteData(0x0001); 							// PWM controlled by host, PWM enable
					LcdWriteData(0x00f0); 							// brightness level 0x00 - 0xFF
					LcdWriteData(0x0000); 							// minimum brightness level =  0x00 - 0xFF
					LcdWriteData(0x0000);								// brightness prescalar 0x0 - 0xF
					backlight=BACKLIGHT_ON;
					backlight_delay=0;
				}
			TIM7->CNT=0;					// Каждый раз при получении сообщения CAN обнуляем таймер подсветки
			hItem = WM_GetDialogItem(hWin_timer, ID_BUTTON_0);		
			BUTTON_SetSkin(hItem, _cbButtonSkin);
			WM_SetFocus(hItem);	
			hItem = WM_GetDialogItem(hWin_timer, ID_BUTTON_1);	
			BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
			
		}
		break;
		case 6://(id=283 data set_timer)
		//
		if((backlight==BACKLIGHT_OFF)||(backlight==BACKLIGHT_LOW))
			{/* Включаем PWM на подсветке */
					LcdWriteReg(CMD_EXIT_SLEEP);
					for(temp=0;temp<180000;temp++);//GUI_Delay(5);
					LcdWriteReg(CMD_SET_PWM_CONF); 			//set PWM for Backlight. Manual p.53
					// 6 parameters to be set
					LcdWriteData(0x0004); 							// PWM Freq =100MHz/(256*(PWMF[7:0]+1))/256  PWMF[7:0]=4 PWM Freq=305Hz
					LcdWriteData(brightness); 					// PWM duty cycle(50%)
					LcdWriteData(0x0001); 							// PWM controlled by host, PWM enable
					LcdWriteData(0x00f0); 							// brightness level 0x00 - 0xFF
					LcdWriteData(0x0000); 							// minimum brightness level =  0x00 - 0xFF
					LcdWriteData(0x0000);								// brightness prescalar 0x0 - 0xF
					backlight=BACKLIGHT_ON;
					backlight_delay=0;
				}
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
		case 7://(id=283 remote disable_timer)
		//
		TIM2->CCER &=~TIM_CCER_CC2E;											// Disable канал сравнения CC2
		TIM2->CCER&=~TIM_CCER_CC1E;												//Disable capture CC1
		Brez_Count=0;
		BrezErr=0;
		TimerONOFF=0;
		if(hWin_timer)
		{
			if((backlight==BACKLIGHT_OFF)||(backlight==BACKLIGHT_LOW))
			{/* Включаем PWM на подсветке */
					LcdWriteReg(CMD_EXIT_SLEEP);
					for(temp=0;temp<180000;temp++);//GUI_Delay(5);
					LcdWriteReg(CMD_SET_PWM_CONF); 			//set PWM for Backlight. Manual p.53
					// 6 parameters to be set
					LcdWriteData(0x0004); 							// PWM Freq =100MHz/(256*(PWMF[7:0]+1))/256  PWMF[7:0]=4 PWM Freq=305Hz
					LcdWriteData(brightness); 					// PWM duty cycle(50%)
					LcdWriteData(0x0001); 							// PWM controlled by host, PWM enable
					LcdWriteData(0x00f0); 							// brightness level 0x00 - 0xFF
					LcdWriteData(0x0000); 							// minimum brightness level =  0x00 - 0xFF
					LcdWriteData(0x0000);								// brightness prescalar 0x0 - 0xF
					backlight=BACKLIGHT_ON;
					backlight_delay=0;
				}
			TIM7->CNT=0;					// Каждый раз при получении сообщения CAN обнуляем таймер подсветки
			hItem = WM_GetDialogItem(hWin_timer, ID_BUTTON_1);		
			BUTTON_SetSkin(hItem, _cbButtonSkin);
			WM_SetFocus(hItem);	
			hItem = WM_GetDialogItem(hWin_timer, ID_BUTTON_0);	
			BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
			
		}
		break;
		case 8://(id=284 data get alarm_a)
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
		case 9://(id=284 remote enable alarm_a)
		//
		WM_DeleteWindow(hALARMA);
		hALARMA=ICONVIEW_CreateEx(10,15,34,34,WM_HBKWIN,WM_CF_SHOW|WM_CF_HASTRANS,0,ID_ICON_ALARM_A,24,24);
		RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
		ICONVIEW_AddBitmapItem(hALARMA,(const GUI_BITMAP*)(AlarmA+1152),"");
		break;
		case 10://(id=285 data set alarm_a)
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
		case 11://(id=285 remote disable alarm_a)
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
	//new_message=0;

}	

/*****************************************************************************************************************
*													CAN_RXProcess1
******************************************************************************************************************/

void CAN_RXProcess1(void){
	uint16_t count;
	uint8_t flag=0xA7;
	switch(CAN_Data_RX[1].FMI) {
		case 0://(id=286 data get alarm_b)
		//
		break;
		case 1://(id=286 remote enable alarm_b)
		//
		break;
		case 2://(id=287 data set alarm_b)
		//
		break;
		case 3://(id=287 remote disable alarm_b)
		//
		break;
		
		case 4:	//(id=289 remote UPDATE_FIRMWARE_REQ)
			// если получили запрос на обновление 
		// * вытащить из CAN_Data_RX[1].Data[0]-CAN_Data_RX[1].Data[3] размер прошивки и записать в size_firmware;
		// * разблокировать flash 
		// * стереть сектора второй половины flash 
		// * отправить подтверждение по CAN для запроса UPDATE_FIRMWARE_REQ
		size_firmware=0;
		size_firmware=CAN_Data_RX[1].Data[0];
		size_firmware|=CAN_Data_RX[1].Data[1]<<8;
		size_firmware|=CAN_Data_RX[1].Data[2]<<16;
		size_firmware|=CAN_Data_RX[1].Data[3]<<24;
		Flash_unlock();
		Flash_sect_erase(NAMBER_UPD_SECTOR,4);		// Очистим 8,9,10,11 сектора всего 4 сектора
		CAN_Data_TX.ID=(NETNAME_INDEX<<8)|0x89;
		CAN_Data_TX.DLC=3;
		CAN_Data_TX.Data[0]=NETNAME_INDEX;
		CAN_Data_TX.Data[1]='o';
		CAN_Data_TX.Data[2]='k';
		CAN_Transmit_DataFrame(&CAN_Data_TX);
		break;		
		case 6:	//(id=289 DOWNLOAD_FIRMWARE)
			
		break;
		case 9: //(id=087 remote update firmware)
			/* Если приняли данное сообщение выставляем во flash флаг обновления через CAN  и  перезагрузка для принятия прошивки */
			/* sector 0 0x0800 0000 - 0x0800 3FFF 
				 sector 1 0x0800 4000 - 0x0800 7FFF	*/
			while(*(uint8_t*)(0x08004000+count)!=0xff)
				{
				count++;
				if(count>=0x3FFF)
					/* делаем стирание sectora 1  и обнуление count */
					{
						count=0;
						/* Подготовим flash для стирания и программирования */
						Flash_unlock();		// Для доступа к FLASH->CR
						Flash_sect_erase(1,1);	
					}
				}	
			
			Flash_prog(&flag,(uint8_t*)(0x08004000+count),1);
			Flash_lock();
			NVIC_SystemReset();		
		break;	
		case 11://(id=088 remote get net name)
		//
		CAN_Data_TX.ID=(NETNAME_INDEX<<8)|0x88;
		CAN_Data_TX.DLC=1;
		CAN_Data_TX.Data[0]=NETNAME_INDEX;  // // netname_index для Core4X9I
		/*CAN_Data_TX.Data[1]='o';
		CAN_Data_TX.Data[2]='r';
		CAN_Data_TX.Data[3]='e';
		CAN_Data_TX.Data[4]='4';
		CAN_Data_TX.Data[5]='X';
		CAN_Data_TX.Data[6]='9';
		CAN_Data_TX.Data[7]='I';*/
		CAN_Transmit_DataFrame(&CAN_Data_TX);
		
		GUI_SetFont(&GUI_Font6x8);
		GUI_DispStringAt("REC ",120,5);
		GUI_DispDec((uint8_t)((CAN1->ESR)>>24),3);
		GUI_DispStringAt("TEC ",190,5);
		GUI_DispDec((uint8_t)((CAN1->ESR)>>16),3);
		GUI_DispStringAt("ERF ",260,5);
		GUI_DispDec((uint8_t)(CAN1->ESR),1);
		
		
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
	
	FLASH->CR |= FLASH_CR_PSIZE_1;			// 10 program/erase x32
	FLASH->CR |=FLASH_CR_SER;																	// флаг  очистки сектора
	for(i=numsect;i<numsect+count;i++)
		{
			while((FLASH->SR & FLASH_SR_BSY)==FLASH_SR_BSY)	{}
			FLASH->CR &= ~(FLASH_CR_SNB<<3);											// очистим биты SNB[3:7] 
			FLASH->CR|=(uint32_t)(i<<3);													// запишем номер сектора для erase
			FLASH->CR |=FLASH_CR_STRT;														// запуск очистки заданного сектора
			while((FLASH->SR & FLASH_SR_BSY)==FLASH_SR_BSY)	{}		// ожидание готовности
		}
	FLASH->CR &= ~FLASH_CR_SER;																	//сбросим  флаг  очистки сектора
}

void Flash_prog(uint8_t *src,uint8_t *dst,uint32_t nbyte){
	uint32_t i;
	
	FLASH->CR |=FLASH_CR_PG;
	FLASH->CR &= ~FLASH_CR_PSIZE_1;			// 00 program x8
	for(i=0;i<(nbyte/4+1);i++)
	{
		while((FLASH->SR & FLASH_SR_BSY)==FLASH_SR_BSY) {}
		*(dst+i)=*(src+i);
	}
	FLASH->CR &= ~FLASH_CR_PG;

}
void Flash_lock(void){

	FLASH->KEYR=0xFFFFFFFF;
	FLASH->KEYR=0xFFFFFFFF;
}
