#include <stm32f4xx.h>
#include <stdlib.h>
#include <header.h>
#include "DIALOG.h"
#include "CAN.h"

void assert_failed(uint8_t* file, uint32_t line)
{
	while(1){};
}

//srec_cat input.bin -bin --crc32-l-e -max-addr input.bin -bin -o output.bin -output
/***********************************************************************************************************************/
//$K\ARM\BIN\srec_cat Core4x9I.bin -bin -offset 0x4 		\
																				-crc32-l-e 0x0 -o Core4x9I.bin -bin	сдвинуть +4 байта и crc32 в начало		 
/*************************************************************************************************************************/
//$K\ARM\BIN\srec_cat Core4x9I.bin -bin -crc32-l-e -max-addr 	\
																				Core4x9I.bin -bin -o Core4x9Icrc.bin -bin crc32 записать в конец
/***************************************************************************************************************************/
//$K\ARM\BIN\srec_cat Core4x9I.bin -bin  -exclude 0x1C 0x1F \
																				 -ex-max-l-e 0x1C 3 \
																				 -crc32-l-e -max-addr\
																					Core4x9I.bin -bin   -o Core4x9Icrc.bin -bin размер bin в 0x1C crc32 в конец файла
/*************************************************************************************************/
//$K\ARM\ARMCC\BIN\fromelf.exe --bin -o Core4x9I.bin !L
//$K\ARM\BIN\srec_cat Core4x9I.bin -bin  -exclude 0x1C 0x1F \
																				 -MAXimum_Little_Endian 0x1C 4\
  																			 -crc32-l-e -max-addr \
																				 Core4x9I.bin -bin   -o Core4x9Icrc.bin -bin
/*****************************************************************************************************************************/	
//$K\ARM\ARMCC\BIN\fromelf.exe --bin -o Core4x9I.bin !L
//$K\ARM\BIN\srec_cat  Core4x9I.bin -bin  -exclude 0x1C 0x28 \
																					-length-l-e 0x1C 4  \
																					-generate 0x20 0x28 -repeat-string Core4x9I \
																					-crc32-l-e -max-addr \
																					Core4x9I.bin -bin	-o Core4x9crc.bin -bin





#define ID_BUTTON_YES (GUI_ID_USER + 0x01)
#define ID_BUTTON_NO (GUI_ID_USER + 0x20)
#define ID_BUTTON_CAN_Data (GUI_ID_USER + 0x21)
#define ID_BUTTON_CAN_Remote (GUI_ID_USER + 0x22)

uint8_t yes=1,no=1;
extern GUI_CONST_STORAGE GUI_BITMAP bmphoto;
WM_HWIN hButton_YES,hButton_NO, hButton_CAN_Data,hButton_CAN_Remote;


uint32_t array[]={exitt,photo,screen,paint,next,prev,date,pwm,sd,alarm,AlarmA,AlarmB,Alarm_d,add_folder,del_folder};
uint32_t farray[]={fexit,fphoto,fscreen,fpaint,fnext,fprev,fdate,fpwm,fsd,falarm,fAlarmA,fAlarmB,fAlarm_d,fadd_folder,fdel_folder};
/*GUI_CONST_STORAGE GUI_BITMAP *p[]={&bmexit,&bmphoto,&bmscreen,&bmpaint,&bmnext,&bmprev,&bmdate,&bmpwm,&bmsd,&bmalarm,
																		&bmAlarmA,&bmAlarmB,&bmAlarm_D,&bmadd_folder,&bmdel_folder};*/

const char *CardType[]={"MULTIMEDIA CARD","SECURE DIGITAL CARD","SECURE DIGITAL IO_CARD",
												"HIGH SPEED MULTIMEDIA CARD","SECURE DIGITAL IO COMBO_CARD",
												"HIGH CAPACITY SD_CARD","HIGH CAPACITY MMC_CARD"};

RTC_TimeTypeDef								RTC_Time;
RTC_DateTypeDef								RTC_Date;
EXTI_InitTypeDef							EXTI_InitStruct;
RTC_AlarmTypeDef							RTC_AlarmA,RTC_AlarmB;
SPI_InitTypeDef 							SPI_InitStruct;
GPIO_InitTypeDef 							GPIO_InitStruct;
DMA_InitTypeDef 							DMA_InitStruct;
FMC_NORSRAMTimingInitTypeDef  FMC_NORSRAMTiming;
FMC_NORSRAMInitTypeDef        FMC_NORSRAM;
/****************************************************************/
/*										Настройка часов			        							*/
/****************************************************************/
uint8_t RTC_init(void){

	uint32_t Temp=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	ALARMA_ACTION=RTC_ReadBackupRegister(RTC_BKP_DR0);
	ALARMB_ACTION=RTC_ReadBackupRegister(RTC_BKP_DR1);
	if((RTC->ISR&RTC_ISR_INITS)!=RTC_ISR_INITS)
	  {
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
			PWR_BackupAccessCmd(ENABLE);
	
			RCC->BDCR|=RCC_BDCR_BDRST;
			RCC->BDCR&=~RCC_BDCR_BDRST;
			
			RCC->BDCR|=RCC_BDCR_LSEON;
			while(!(RCC->BDCR&RCC_BDCR_LSERDY)){}
			
			RCC->BDCR|=	RCC_BDCR_RTCSEL_0;
			RCC->BDCR|=RCC_BDCR_RTCEN;
			
			RTC->WPR=0xCA;
			RTC->WPR=0x53;
			
			RTC->ISR|=RTC_ISR_INIT;								// включим  Initialization mode 
			while(!(RTC->ISR&RTC_ISR_INITF)){}

		//PREDIV_A =0x7f
		//PREDIV_S=0x00ff	
		// При тактировании от LSE 32768 предделитель асинхронный и синхронный уже настроены для 1Hz		
			
		// Запишем значения в регистры даты и времени
			Temp=1<<20;
			Temp|=5<<16;	//Year
			Temp|=4<<13;	// Day of week
			Temp|=0<<12;
			Temp|=1<<8;		//Month
			Temp|=0<<4;
			Temp|=1;			// Day
			RTC->DR=Temp;
			Temp=0;
			Temp=0<<20;
			Temp|=0<<16;			// Hour
			Temp|=0<<12;
			Temp|=0<<8;			//Minute	
			RTC->TR=Temp;
				
		  RTC->CR|=RTC_CR_WUCKSEL_2;
			RTC->WUTR=0;
			RTC->CR|=RTC_CR_WUTE;	
			RTC->CR|=RTC_CR_WUTIE;
			RTC->CR|=RTC_CR_BYPSHAD;						//Значение календаря читается с RTC_TR RTC_DR а не с теневого
/***********************************************/
/*				  	Настройка будильника	       		 */			
/***********************************************/
			RTC->CR|=RTC_CR_ALRAIE|RTC_CR_ALRBIE;										
							 
/***********************************************/			
			
			RTC->ISR&=~RTC_ISR_INIT;						// выключим  Initialization mode 
			RTC->WPR=0xff;											// Включим защиту записав не правильное число
						
		}
		else
		{
		RTC->WPR=0xCA;
		RTC->WPR=0x53;
		RTC->CR&=~RTC_CR_WUTE;	
		while((RTC->ISR&RTC_ISR_WUTWF)!=RTC_ISR_WUTWF) {}
		RTC->CR|=RTC_CR_WUTE;	
		RTC->WPR=0xff;											// Включим защиту записав не правильное число
		RTC->ISR&=~RTC_ISR_WUTF;
		}
		return 0;	
}

/****************************************************************/
/*		    Настройка конфигурации  периферийных модулей		      */
/****************************************************************/
void Periph_Init(void){
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|
												 RCC_AHB1Periph_GPIOB|
												 RCC_AHB1Periph_GPIOC|
												 RCC_AHB1Periph_GPIOI|
												 RCC_AHB1Periph_CRC	 |
												 RCC_AHB1Periph_DMA2 	 ,ENABLE);
 			
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6|
												 RCC_APB1Periph_TIM7|	
												 RCC_APB1Periph_SPI2|
												 RCC_APB1Periph_TIM2	, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO|
												 RCC_APB2Periph_SPI1|
												 RCC_APB2Periph_SYSCFG, ENABLE);
#if 0	
/********************************************************************/
	/*		Настройка DMA2 для вывода данных на LCD модуль							*/
/********************************************************************/		
	DMA_InitStruct.DMA_BufferSize=0;
	DMA_InitStruct.DMA_Channel=DMA_Channel_0;
	DMA_InitStruct.DMA_DIR=DMA_DIR_MemoryToMemory;
	DMA_InitStruct.DMA_FIFOMode=DMA_FIFOMode_Enable;
	DMA_InitStruct.DMA_FIFOThreshold=DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStruct.DMA_Memory0BaseAddr=LCD_BASE+0x10000;
	DMA_InitStruct.DMA_MemoryBurst=DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Disable;
	DMA_InitStruct.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStruct.DMA_PeripheralBaseAddr=0;
	DMA_InitStruct.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Enable;
	DMA_InitStruct.DMA_Priority=DMA_Priority_High;
	DMA_Init(DMA2_Stream7,&DMA_InitStruct);
#endif	
/********************************************************************/
/*																																	*/
/********************************************************************/	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line4;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	SYSCFG->EXTICR[1]=SYSCFG_EXTICR2_EXTI4_PE;
	
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line22;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	
/***********************************************/
/*				  	Настройка будильника	       		 */			
/***********************************************/
	EXTI_InitStruct.EXTI_Line=EXTI_Line17;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	
/********************************************************************/
/*					SDIO card insert  PB1	  																*/
/********************************************************************/	

		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
		GPIO_Init(GPIOB,&GPIO_InitStruct);
		
		SYSCFG->EXTICR[0]=SYSCFG_EXTICR1_EXTI1_PB;
		
		EXTI_InitStruct.EXTI_Line=EXTI_Line1;
		EXTI_InitStruct.EXTI_LineCmd=ENABLE;
		EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
		EXTI_Init(&EXTI_InitStruct);
		
		
		

/*****************************************************************************/
/* 															Инициализация таймера TIM2									 */	
/*****************************************************************************/

/* Настраиваем выводы для таймера TIM2: PA0 как Input pull-up  (TIM2_CH1_ETR)*/
/*																			PA1 как Alternate function push-pull  (TIM2_CH2)*/
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0,GPIO_AF_TIM2);	
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_DOWN;		// Подтянем выход таймера к GND 	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1,GPIO_AF_TIM2);
	
/*Настройка канала захвата*/
	TIM2->SMCR |=TIM_SMCR_TS_0|				//Источник синхронизации внешнего сигнала Filtered Timer Input 1(TI1FP1)
							 TIM_SMCR_TS_2|				// TS[2:0] 101
					  	 TIM_SMCR_SMS_1|
							 TIM_SMCR_SMS_2;			// SMS[2:0] 110 Slave mode selection выбираем Trigger mode
	TIM2->CCMR1 |=TIM_CCMR1_CC1S_0;		// CC1 Channel is configured as input, IC1 is mapped on TI1

// Настроим входной фильтр триггера для захвата входного сигнала по спадающему фронту
	TIM2->CCER|=TIM_CCER_CC1P;
	
/* Настройка канала сравнения*/
// Выбираем режим  PWM mode 2  установкой битов OC1M регистра TIM2->CCMR1 [2:0]  111
	TIM2->CCMR1 |=TIM_CCMR1_OC2M|TIM_CCMR1_OC2PE; 			//Output compare 2 preload enable
//Зададим коэффициент деления для CK_CNT=CK_PSC/(PSC[15:0]+1)  90000000/(899+1) =100KHz
	TIM2->PSC=899;
	//Включим режим одного импульcа и предварительной загрузки регистра ARR
	TIM2->CR1 |=TIM_CR1_OPM|TIM_CR1_ARPE;

/*****************************************************************************/
/* 															Инициализация таймера TIM6									 */	
/*****************************************************************************/
	TIM6->PSC = 45000 - 1; 			// Настраиваем делитель что таймер тикал 2000 раз в секунду
	TIM6->ARR = 100 ; 					// Чтоб прерывание случалось 10 раз в секунду
	TIM6->DIER |= TIM_DIER_UIE; //разрешаем прерывание от таймера обновления структуры GUI_PID_STATE
	TIM6->EGR = TIM_EGR_UG;		//генерируем "update event". ARR и PSC грузятся из предварительного в теневой регистр. 
	TIM6->SR&=~TIM_SR_UIF; 			//Сбрасываем флаг UIF
	NVIC_ClearPendingIRQ(TIM6_DAC_IRQn);
/*****************************************************************************/
/* 															Инициализация таймера TIM7									 */	
/*****************************************************************************/
	TIM7->PSC = 45000 - 1; 			// Настраиваем делитель что таймер тикал 2000 раз в секунду
	TIM7->ARR = 60000 ; 				// Чтоб прерывание случалось  раз в 10 секунд
  TIM7->DIER |= TIM_DIER_UIE; //разрешаем прерывание от таймера подсветки дисплея
	TIM7->EGR = TIM_EGR_UG;			//генерируем "update event". ARR и PSC грузятся из предварительного в теневой регистр. 
	TIM7->SR&=~TIM_SR_UIF; 			//Сбрасываем флаг UIF
	NVIC_ClearPendingIRQ(TIM7_IRQn);
/********************************************************************/
/* 										Инициализация контроллера NVIC							 	*/
/********************************************************************/
	NVIC_SetPriority(TIM2_IRQn,2);
	NVIC_SetPriority(TIM6_DAC_IRQn,1);
	NVIC_SetPriority(TIM7_IRQn,2);
	NVIC_SetPriority(RTC_WKUP_IRQn,2);
	NVIC_SetPriority(SD_SDIO_DMA_IRQn,0);
	NVIC_SetPriority(EXTI1_IRQn,2);
	NVIC_SetPriority(EXTI4_IRQn,0);
	
	NVIC_SetPriority(RTC_Alarm_IRQn,2);
	DBGMCU->CR|=DBGMCU_CR_DBG_SLEEP;
	DBGMCU->APB1FZ|=DBGMCU_APB1_FZ_DBG_TIM2_STOP;
}

/****************************************************************/
/*										Функция	HEX в BIN	      									*/
/****************************************************************/
void Hex_to_Bin(uint8_t *Src)
{
	uint8_t temp8,i,m,j,k;
	uint16_t temp16;
	uint32_t temp32,*Dst;
	Dst=(uint32_t*)SDRAM_BASE;
	for(;;)
	{
		if(*(uint16_t*)(Src+7)==0x3430)  
			{
	  	Src+=9;
			temp16=0;
			for(i=0;i<4;i++)
			{ temp8=*(Src+i);
				if((temp8&0xF0)==0x40)
					temp16|=(temp8-0x37)<<(12-i*4);
				else
				temp16|=(temp8-0x30)<<(12-i*4);
			} 
			//Dst=(uint32_t*)((uint32_t)temp16<<16);
			Src+=8;
			continue;
			}		
		else if(*(uint16_t*)(Src+7)==0x3030)
		{
				if(*(uint16_t*)(Src+1)==0x4330)
				{
					temp16=(*(Src+1)-0x30)<<4;
					temp16|=(*(Src+2)-0x37);
				}
				else		
				{
					temp16=(*(Src+1)-0x30)<<4;
					temp16|=(*(Src+2)-0x30);
				}
			k=temp16/4;
			Src+=9;	
			for(m=0;m<k;m++)
			{		temp32=0;		
					for(i=0;i<4;i++)
						{		
							temp8=0;
							for(j=0;j<2;j++)		
								{ 
									if((*(Src+j)&0xF0)==0x40)
										temp8|=(*(Src+j)-0x37)<<(1-j)*4;
									else
										temp8|=(*(Src+j)-0x30)<<(1-j)*4;
								 }
							temp32|=temp8<<i*8;
							Src+=2;		
						}			
				*Dst++=temp32;
				}
			Src+=4;	
			continue;	
			}
		else if(*(uint16_t*)(Src+7)==0x3530)
		{
			Src+=21;
			continue;
		}
		else if(*(uint16_t*)(Src+7)==0x3130)
			break;
	}
}
	
/****************************************************************/
/*											Настройка FMC       										*/
/****************************************************************/
void FMC_SRAM_Change(void)
{
	FMC_NORSRAMCmd(FMC_Bank1_NORSRAM1, DISABLE);
	
	FMC_NORSRAMTiming.FMC_AccessMode=FMC_AccessMode_A;
	FMC_NORSRAMTiming.FMC_AddressHoldTime=1;//1
	FMC_NORSRAMTiming.FMC_AddressSetupTime=1;//2
	FMC_NORSRAMTiming.FMC_BusTurnAroundDuration=0;//0
	FMC_NORSRAMTiming.FMC_CLKDivision=1;//1
	FMC_NORSRAMTiming.FMC_DataLatency=1; //1
	FMC_NORSRAMTiming.FMC_DataSetupTime=9; //10
		
	FMC_NORSRAMInit(&FMC_NORSRAM);
	FMC_NORSRAMCmd(FMC_Bank1_NORSRAM1, ENABLE);
}	

/****************************************************************/
/*											Функция  сallback Boot_menu							*/
/****************************************************************/
static void _cbBoot_menu(WM_MESSAGE* pMsg) {
  int     NCode;
  int     Id;
	
	switch(pMsg->MsgId) {
  case WM_NOTIFY_PARENT:
				Id    = WM_GetId(pMsg->hWinSrc);
				NCode = pMsg->Data.v;
				switch(Id){
					case ID_BUTTON_YES:
					switch(NCode){
							case WM_NOTIFICATION_CLICKED:
								yes=0;
								
							break;
						}
					break;
					case ID_BUTTON_NO:
					switch(NCode){
							case WM_NOTIFICATION_CLICKED:
								no=0;
								
							break;
						}
						
					}	
	break;
	default:
    WM_DefaultProc(pMsg);
  }
}					
							
/****************************************************************/
/*					       Функция     Boot_menu                 	      */
/****************************************************************/
SD_Error Boot_menu (void){
	void (*pMainTask)(void);
	
	uint32_t temp,temp1;
	uint32_t br;
	float sd_size;
	
	yes=1; no=1;
	
	GUI_SetFont(GUI_FONT_16_1); 
	GUI_SetColor(GUI_YELLOW);
	if(sd_error!=SD_OK)
	{
		sd_error=SD_Init();
		if(sd_error==SD_OK)
			f_mount (0,&fs);
		else
			{
			Message("SD card is error",0);	
			return sd_error;
			}
		}		
	 GUI_DispStringAt(" BOOT FROM SD CARD ? ",170,50+SCREEN_1);
	 BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
   hButton_YES=BUTTON_CreateEx(150,100+SCREEN_1,40,30,WM_HBKWIN,WM_CF_SHOW,0,ID_BUTTON_YES);
	 BUTTON_SetText(hButton_YES, "YES");
	 hButton_NO=BUTTON_CreateEx(300,100+SCREEN_1,40,30,WM_HBKWIN,WM_CF_SHOW,0,ID_BUTTON_NO);
	 BUTTON_SetText(hButton_NO, "NO");
	 WM_SetCallback(WM_HBKWIN, _cbBoot_menu);
		while(yes&&no)
		{
			GUI_Exec();
		}
		WM_DeleteWindow(hButton_YES);
		WM_DeleteWindow(hButton_NO);
		hButton_YES=0;
		hButton_NO=0;
		GUI_Clear();
		if(!yes)
		{
		f_mount (0,&fs);
		fresult=f_opendir(&dir, "0:");
		if(fresult==FR_OK)
			{					
			GUI_DispStringAt(" CARD TYPE : ",5,10+SCREEN_1);
			GUI_DispString(CardType[SDCardInfo.CardType]);
										
			GUI_DispStringAt(" SD_CARD SIZE :  ",5,30+SCREEN_1);
			temp=1<<(SDCardInfo.SD_csd.DeviceSizeMul+2);
			temp1=(SDCardInfo.SD_csd.DeviceSize+1)*temp*(1<<SDCardInfo.SD_csd.RdBlockLen);
			sd_size=temp1/1048576.0f;
			GUI_DispFloat(sd_size,6);
			GUI_DispString(" MBYTE");
					
			GUI_DispStringAt(" SECTOR SIZE : ",5,50+SCREEN_1);
			GUI_DispDec(SD_SECTOR_SIZE,3);
			GUI_DispString("  BYTE");
				
			GUI_DispStringAt(" RRODUCT SN : ",5,70+SCREEN_1);
			GUI_DispDec(SDCardInfo.SD_cid.ProdSN,10);
			
			GUI_DispStringAt(" RCA : ",5,90+SCREEN_1);
			GUI_DispDec(SDCardInfo.RCA,5);
					
			GUI_DispStringAt(" MANUFACTURER ID : ",5,110+SCREEN_1);
			GUI_DispDec(SDCardInfo.SD_cid.ManufacturerID,2);
		  Message("SD card is OK!",1);
			fresult=f_open (&fil, "0:stm32.hex", FA_READ);	// open file
			if(fresult==FR_OK)	
				{	
					for(;;)
						{
							fresult=f_read(&fil,(uint32_t*)(SDRAM_BASE+0x30000),fil.fsize,&br);
							if (fresult || br == 0) 
									break;    // error or eof 
						}		
						f_close(&fil);
						Hex_to_Bin((uint8_t*)(SDRAM_BASE+0x30000));			
						pMainTask=(void(*)(void))*(__IO uint32_t*)(SDRAM_BASE+4);	
						SCB->VTOR=SDRAM_BASE;
						__set_MSP(*(__IO uint32_t*)SDRAM_BASE);
						pMainTask();
				}
			}				
			else
			{	
			Message("SD card is not initialize",0);
			fresult=f_mount (0,NULL);
			}	
			
		}
	else
		__nop();
		//MainTask();
	


	return sd_error;
}	


/****************************************************************/
/*					                MAIN	                      	      */
/****************************************************************/
int main(void){
	uint8_t i;
	//pBoot_menu=(SD_Error(*)(void))Boot_menu;
	//pGUI_Init=(void(*)(void))GUI_Init;
		
	SSD1963_LowLevel_Init();
	RTC_init();	
	Periph_Init();
	MX25_LowLevel_Init();
	TSC2046_LowLevel_Init();
	bxCAN_Init();
	
	// PF7 выход push-pull без подтяжки для моргания светодиодом
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
		GPIO_InitStruct.GPIO_Speed=GPIO_Low_Speed;
		GPIO_Init(GPIOF,&GPIO_InitStruct);	
	
	brightness=(uint16_t)RTC->BKP2R;
	if(!brightness)
		brightness=BRIGHTNESS_MED;
	
	NVIC_EnableIRQ(TIM2_IRQn); 									//Разрешение TIM2_IRQn прерывания
	NVIC_EnableIRQ(TIM6_DAC_IRQn); 							//Разрешение TIM6_DAC_IRQn прерывания
	NVIC_EnableIRQ(TIM7_IRQn); 									//Разрешение TIM7_IRQn прерывания
	NVIC_EnableIRQ(RTC_WKUP_IRQn);							//Разрешение RTC_IRQn прерывания
	//NVIC_EnableIRQ(SDIO_IRQn);									//Разрешение SDIO_IRQn прерывания
	NVIC_EnableIRQ(SD_SDIO_DMA_IRQn);						//Разрешение DMA2_Stream3_IRQn прерывания
	
	NVIC_EnableIRQ(EXTI4_IRQn);		
	NVIC_EnableIRQ(RTC_Alarm_IRQn);
	DBGMCU->APB1FZ|=DBGMCU_APB1_FZ_DBG_CAN1_STOP;
	/*ChipErase_MX25L();
	for(i=0;i<10;i++)
		{
			Prg_MX25L_16(farray[i],2304,(const unsigned short*)p[i]->pData);
			Prg_MX25L_8(farray[i]+4608,20,(const uint8_t*)p[i]);
		}
	//BlockErase_MX25L(0xB000);
		for(i=10;i<15;i++)
		{
			Prg_MX25L_16(farray[i],576,(const unsigned short*)p[i]->pData);
			Prg_MX25L_8(farray[i]+1152,20,(const uint8_t*)p[i]);
		}*/
		
		for(i=0;i<10;i++)
		{
			Read_MX25L(farray[i],4628,(uint8_t* )array[i]);			//0x0-0x1214  exit
		*(__IO uint32_t*)(array[i]+4616)=array[i];
		*(__IO uint32_t*)(array[i]+4624)=(uint32_t)&GUI_BitmapMethodsM565;
		}
		
		for(i=10;i<15;i++)
		{
			Read_MX25L(farray[i],1172,(uint8_t* )array[i]);			//0x0-0x1214  exit
		*(__IO uint32_t*)(array[i]+1160)=array[i];
		*(__IO uint32_t*)(array[i]+1168)=(uint32_t)&GUI_BitmapMethodsM565;
		}
	
	
	GUI_Init();
	GUI_SetOrg(0,0);
		
	TIM6->CR1 |= TIM_CR1_CEN; 	// Начать отсчёт!	
	TIM7->CR1 |= TIM_CR1_CEN; 	// Начать отсчёт!
	
	if(RTC->BKP3R==0)
		Touch_calibration();
	
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)==0)
		Boot_menu();
			
	MainTask();
	while(1) {}
}
