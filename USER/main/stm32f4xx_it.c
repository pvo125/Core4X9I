/**
  ******************************************************************************
  * @file    stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    13-November-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "header.h"
#include "CAN.h"
/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
extern RTC_TimeTypeDef								RTC_Time;
extern RTC_DateTypeDef								RTC_Date;
uint8_t sd_ins_rem;
extern volatile uint8_t new_message;

extern volatile uint8_t time_disp;
extern volatile uint8_t date_disp;

extern GUI_COLOR color_array[];
uint8_t ADCVal_ready,bat_disp=1;
uint16_t VBat,Bat_percent;
GUI_COLOR bat_color;

uint8_t move_y_last;
extern uint8_t move_y;
extern uint8_t screen_scroll;
extern uint16_t screen_x;

volatile uint8_t backlight_flag=0;
uint8_t charge_plug_unplug=0;
extern volatile USBCONN_TypeDef usb_conn;

volatile uint8_t number_array;

volatile uint8_t canconnect,canerr_clr,canerr_disp;
static volatile uint8_t count;
volatile uint8_t count_sd=0;
volatile uint8_t disp_sd;
	
/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
/*void SysTick_Handler(void)
{
	
}*/

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/

/**
  * @brief  This function handles RTC_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void RTC_WKUP_IRQHandler(void)
{
	static uint8_t number=0;
		
	if(sd_ins_rem)
	{
		count_sd++;
		if(count_sd>2)
		{
			sd_ins_rem=0;
			disp_sd=1;
		}
	}		
	if(((RTC->TR&0x003F0000)==0)&&((RTC->TR&0x00007F00)==0)&&((RTC->TR&0x0000007F)==0))
	{
		date_disp=1;
	}
	if(time_show)
	{
		time_disp=1;
		if(canconnect)
		{
			canerr_disp=1;
			count++;
			if(count>3)
			{
				canconnect=0;
				canerr_clr=1;
				canerr_disp=0;
			}
		}
	}
	if(charge_plug_unplug)
	{
		number++;
		if(number==2)
			{
				number=0;
				charge_plug_unplug=0;
				if(USB_DETECT_PORT->IDR & USB_DETECT_IDRx)
				{
					Bat_percent=0;
					usb_conn=USBCONN_PLUG;
					number_array=0;
					ADC_Cmd(ADC1,DISABLE);
				
				}
				else if(usb_conn==USBCONN_PLUG)
				{
					usb_conn=USBCONN_UNPLUG;
					ADC_Cmd(ADC1,ENABLE);
					TIM3->EGR = TIM_EGR_UG;			//генерируем "update event". ARR и PSC грузятся из предварительного в теневой регистр. 
					TIM3->SR&=~TIM_SR_UIF; 			//Сбрасываем флаг UIF	
				}
				backlight_flag=1;
				if(backlight==BACKLIGHT_OFF_SLEEP)
					backlight=BACKLIGHT_SLEEPtoON;
				else
					backlight=BACKLIGHT_ON;
				backlight_delay=0;
				TIM7->CNT=0;
			}
	}
	if((usb_conn==USBCONN_PLUG)&&(!(CHARGE_INDIC_PORT->IDR & CHARGE_INDIC_IDRx)))
	{
		if((ADCVal_ready==0)&&(bat_disp))
		{
			Bat_percent+=20;
			if(Bat_percent>100) 
				Bat_percent=20;
			bat_color=color_array[number_array];
			number_array++;
			if(number_array>4)	
				number_array=0;
			ADCVal_ready=1;
		}
	}
	else if((usb_conn==USBCONN_PLUG)&&(CHARGE_INDIC_PORT->IDR & CHARGE_INDIC_IDRx))
	{
		if((ADCVal_ready==0)&&(bat_disp))
		{
			if(Bat_percent!=100)
			{
				Bat_percent+=20;
				number_array++;
				bat_color=color_array[number_array];
			}
			ADCVal_ready=1;
		}
	}
		
	RTC->ISR&=~RTC_ISR_WUTF;//RTC->ISR&=~(RTC_ISR_WUTF|0x00000080);//RTC_ClearITPendingBit(RTC_IT_WUT);//RTC->ISR&=~RTC_ISR_WUTF;
	EXTI_ClearITPendingBit(EXTI_Line22);
}
/**
  * @brief  This function handles ADC_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */

void ADC_IRQHandler (void)
{
	uint16_t temp;
	temp=(ADCBuff[0]+ADCBuff[1]+ADCBuff[2]+ADCBuff[3] )/4;
	VBat=(6590*temp)/4095;
	Bat_percent=(VBat-3100)/10;
	if(Bat_percent>80)
	{
		bat_color=color_array[4];
		if(Bat_percent>100) Bat_percent=100;
	}
	else if((60<Bat_percent)&&(Bat_percent<=80))	
		bat_color=color_array[3];
		
	else if((40<Bat_percent)&&(Bat_percent<=60))	
		bat_color=color_array[2];
		
	else if((20<Bat_percent)&&(Bat_percent<=40))	
		bat_color=color_array[1];
		
	else if((10<Bat_percent)&&(Bat_percent<=20))	
		bat_color=color_array[0];
		
	if(bat_disp)
		ADCVal_ready=1;	
}
/**
  * @brief  This function handles EXTI0_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler (void)
{
	count_sd=0;
	sd_ins_rem=1;
	EXTI_ClearITPendingBit(EXTI_Line1);
	NVIC_ClearPendingIRQ(EXTI1_IRQn);
	
}
/**
  * @brief  This function handles EXTI0_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler (void)
{
	uint32_t i;
	for(i=0;i<100000;i++);
	EXTI_ClearITPendingBit(EXTI_Line2);
	NVIC_ClearPendingIRQ(EXTI2_IRQn);
	
}

/**
  * @brief  This function handles EXTI9_5_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler (void)
{
	charge_plug_unplug=1;
	EXTI_ClearITPendingBit(EXTI_Line6);
	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
	
	
}
/**
  * @brief  This function handles TIM2_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
	TIM2->SR &=~TIM_SR_CC1IF;  //Сбрасываем флаг вызвавшего прерывание от захвата TIM2_CH1
	BrezErr+=BrezKoeff;
	//Brez_Count++;
	if (BrezErr>.5f)
		{
			TIM2->CCER|=TIM_CCER_CC2E;  //Подключаем выход таймера к каналу сравнения
			BrezErr-=1;
		}
	else 
		TIM2->CCER &=~TIM_CCER_CC2E;	//Отключаем выход таймера от канала сравнения
	//if(Brez_Count==100)
	//	{
	//		Brez_Count=0;
	//		BrezErr=0;
	//	}
}

/**
  * @brief  This function handles RTC_Alarm_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void RTC_Alarm_IRQHandler(void){
	
	if((RTC->ISR&RTC_ISR_ALRAF)==RTC_ISR_ALRAF)	
	{	
		RTC->ISR&=~RTC_ISR_ALRAF;
		switch(ALARMA_ACTION){
			case NONE:
			break;	
			case START_PWM:
			TimerONOFF=1;
			TIM2->CCER |=TIM_CCER_CC2E;												// Enable канал сравнения CC2
			TIM2->CCER|=TIM_CCER_CC1E;												// Enable capture CC1	
			break;
			case STOP_PWM:
			break;
			case START_ADC:
			break;
			case STOP_ADC:
			break;
			default:
			break;		
		}
	}
	else
	{
		RTC->ISR&=~RTC_ISR_ALRBF;
		switch(ALARMB_ACTION){
			case NONE:
			break;
			case START_PWM:
			break;
			case STOP_PWM:
			TIM2->CCER &=~TIM_CCER_CC2E;												// Disable канал сравнения CC2
			TIM2->CCER&=~TIM_CCER_CC1E;												//Disable capture CC1
			Brez_Count=0;
			BrezErr=0;
			TimerONOFF=0;	
			break;
			case START_ADC:
			break;
			case STOP_ADC:
			break;
			default:
			break;	
			}
	}
	EXTI_ClearITPendingBit(EXTI_Line17);
	NVIC_ClearPendingIRQ(RTC_Alarm_IRQn);
}

/**
  * @brief  This function handles TIM6_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void TIM6_DAC_IRQHandler (void){
	
	_CheckUpdateTouch();
		if(drawmode==0)
		{
			if(PROGBAR_MEM!=0)
			{
				usedbytes=GUI_ALLOC_GetNumUsedBytes();
				PROGBAR_SetValue(PROGBAR_MEM,usedbytes);
				//WM_Paint(PROGBAR_MEM);
			}
		}
		TIM6->SR &= ~TIM_SR_UIF; 			//Сбрасываем флаг UIF
		NVIC_ClearPendingIRQ(TIM6_DAC_IRQn);
}
/**
  * @brief  This function handles TIM7_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void TIM7_IRQHandler (void)
{
		backlight_delay++;
		backlight_flag=1;
	
	if(backlight==BACKLIGHT_ON)
		backlight=BACKLIGHT_LOW;
	else if((backlight==BACKLIGHT_LOW)&&(backlight_delay==2))
	{
		backlight=BACKLIGHT_OFF_SLEEP;
		backlight_delay=0;
	}
	else if((backlight==BACKLIGHT_OFF_SLEEP)&&(backlight_delay==1))
	{
		if((canconnect==0)&&(!(USB_DETECT_PORT->IDR & USB_DETECT_IDRx)))
			sleep_mode=1;
		else
			backlight_delay=0;
	}
	TIM7->SR &= ~TIM_SR_UIF; 			//Сбрасываем флаг UIF
	NVIC_ClearPendingIRQ(TIM7_IRQn);
		
}
/**
  * @brief  This function handles SDIO_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
/*void SDIO_IRQHandler(void)

{
	//SD_ProcessIRQSrc();
}*/
void SD_SDIO_DMA_IRQHANDLER(void)
{
  SD_ProcessDMAIRQ();  
}     
/**
  * @brief  This function handles CAN1_RX0_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler (void) 
{
	
	CAN_Receive_IRQHandler(0);
	CAN_RXProcess0();
	

}
/**
  * @brief  This function handles CAN1_RX1_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */

void CAN1_RX1_IRQHandler (void) 
{
	canconnect=1;//canerr_clr=0;
	count=0;
	
	CAN_Receive_IRQHandler(1);
	CAN_RXProcess1();
}
/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
