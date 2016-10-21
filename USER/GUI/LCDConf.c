/*********************************************************************
*          Portions COPYRIGHT 2013 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf_FlexColor_Template.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
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

#include <stm32f4xx.h>

#include "ssd1963.h"
#include "GUI.h"
#include "GUIDRV_FlexColor.h"

#include "header.h"
#include "stdbool.h"
#include "stdlib.h"

//uint8_t cycle_start_pwm=0;
extern char drawmode;
extern WM_HWIN hButton;
extern ICONVIEW_Handle hALARMA,hALARMB,hIcon_EXIT,hIcon_BRIGHT;
extern WM_HWIN hWin_start;
/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/


// Physical display size
//
#define XSIZE_PHYS  480 // To be adapted to x-screen size
#define YSIZE_PHYS  272 // To be adapted to y-screen size

static float Xd[]={0.0f,240.0f,20.0f,460.0f,460.0f,20.0f};
static float Yd[]={0.0f,136.0f,20.0f,20.0f,252.0f,252.0f};
static uint16_t Xt[6];
static uint16_t Yt[6];
static float A/*=0.1292882f*/,B/*=0.000041796f*/,C/*=-24.99898f*/,D/*=-0.00060175f*/,E/*=0.08261617f*/,F/*=-35.31908f*/;

static float a,b,c,d,e;
static uint32_t X1,X2,X3,Y1,Y2,Y3;
static float detX1,detX2,detX3,detY1,detY2,detY3;
static float det;
//static void Touch_SetCS(char OnOff);
static void Touch_SendCMD(U8 Data);
static U16 Touch_GetResult(void);

#define TOUCH_PRESSED		1
#define TOUCH_UNPRESSED	0

/*#define GUI_TOUCH_AD_LEFT 	 	50
#define GUI_TOUCH_AD_RIGHT 	 	3970
#define GUI_TOUCH_AD_TOP 	 		50
#define GUI_TOUCH_AD_BOTTOM  	3970*/
/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   GUICC_565
  #error Color conversion not defined!
#endif
#ifndef   GUIDRV_FLEXCOLOR
  #error No display driver defined!
#endif


	
	/*********************************************************************
*
*       Local functions
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

/********************************************************************
*
*       LcdWriteDataMultiple
*
* Function description:
*   Writes multiple values to a display register.
*/
static __INLINE void LcdWriteDataMultiple(U16 * pData, int NumItems) {
  while (NumItems--) {
    // ... TBD by user
		LCD_DATA_ADDRESS = *pData++;
  }
	/*DMA2_Stream7->NDTR=NumItems;
	DMA2_Stream7->PAR=(uint32_t)pData;
	DMA2_Stream7->CR|=DMA_SxCR_EN;
	while((DMA2->HISR&DMA_HISR_TCIF7)!=DMA_HISR_TCIF7) {};
	DMA2->HIFCR=DMA_HIFCR_CTCIF7|DMA_HIFCR_CHTIF7;*/
}

/********************************************************************
*
*       LcdReadDataMultiple
*
* Function description:
*   Reads multiple values from a display register.
*/
static __INLINE void LcdReadDataMultiple(U16 * pData, int NumItems) {
  while (NumItems--) {
    // ... TBD by user
		*pData++ = LCD_DATA_ADDRESS;
  }
}
/***********************************************************************************/




static void Touch_SendCMD(U8 Data)
{
	SPI2->DR=Data;
	while((SPI2->SR&SPI_SR_RXNE)!=SPI_SR_RXNE)		{}	//while((SPI2->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
}

static U16 Touch_GetResult(void)
{
	uint16_t temp;
		temp=SPI2->DR;		/* Clear RXNE SPI2 */
	SPI2->DR=0x0;
	while((SPI2->SR&SPI_SR_RXNE)!=SPI_SR_RXNE)		{}
	temp=SPI2->DR>>3;
			return temp;
}

/*static void Touch_SetCS(char OnOff)
{
	if(OnOff)
		GPIO_SetBits(GPIOB,  GPIO_Pin_12);
	else
		GPIO_ResetBits(GPIOB,  GPIO_Pin_12);
}
*/
uint16_t _ReadZ2(void)
{
	uint16_t  z2;
	SPI2->DR=0xC4;
	while((SPI2->SR&SPI_SR_RXNE)!=SPI_SR_RXNE)	{} //while((SPI2->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
	z2=SPI2->DR;		/* Clear RXNE SPI2 */
	
	SPI2->DR=0x0;
	while(!(SPI2->SR&SPI_SR_RXNE))	{}											
	z2=SPI2->DR>>3;
	return z2;																										
}

uint16_t _ReadZ1(void)
{
	uint16_t  z1;
	SPI2->DR=0xB4;
	while((SPI2->SR&SPI_SR_RXNE)!=SPI_SR_RXNE)	{} //while((SPI2->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}	
	z1=SPI2->DR;		/* Clear RXNE SPI2 */
	
	SPI2->DR=0x0;
	while(!(SPI2->SR&SPI_SR_RXNE))	{}											
	z1=SPI2->DR>>3;
	return z1;																										
}

uint16_t _TouchPositionX(void)
{
	uint16_t  x;
	SPI2->DR=0xD0;
	while((SPI2->SR&SPI_SR_RXNE)!=SPI_SR_RXNE)	{} //while((SPI2->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}	
	x=SPI2->DR;		/* Clear RXNE SPI2 */
	
	SPI2->DR=0x00;
	while(!(SPI2->SR&SPI_SR_RXNE))	{}											
	x=SPI2->DR>>3;
	return x;																										
																															
	
}	

uint16_t _TouchPositionY(void)
{
	uint16_t y;
	SPI2->DR=0x90;
	while((SPI2->SR&SPI_SR_RXNE)!=SPI_SR_RXNE)	{} //while((SPI2->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}	
	y=SPI2->DR;		/* Clear RXNE SPI2 */			  	
	
	SPI2->DR=0x00;
	while(!(SPI2->SR&SPI_SR_RXNE))	{}
	y=SPI2->DR>>3;															
	return y;
}

uint8_t _TouchIsPresset(void)
{
	if(( GPIOB->IDR&GPIO_IDR_IDR_11)==RESET)   //GPIO_ReadInputDataBit ( GPIOB,GPIO_Pin_11 ) == RESET )
		return 1;
	else
		return 0;
}

void _CheckUpdateTouch(void)
{
			uint16_t Xd,Yd,Xt,Xt1,Xt2,Yt,Yt1,Yt2;
			uint16_t z1,z2;
			uint32_t i;
			float Rt;
			if(_TouchIsPresset())
			{
				if((backlight==BACKLIGHT_OFF)||(backlight==BACKLIGHT_LOW))
				{/* Включаем PWM на подсветке */
					LcdWriteReg(CMD_EXIT_SLEEP);
					for(i=0;i<180000;i++);//GUI_Delay(5);
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
				TIM7->CNT=0;					// Каждый раз при нажатии на экран обнуляем таймер подсветки	
				GPIO_ResetBits(GPIOB, GPIO_Pin_12);    //GPIOB->BSRR=GPIO_BSRR_BR_12;
				z1=_ReadZ1();
				z2=_ReadZ2();
				Touch_SendCMD(0xD4);
				Xt=Touch_GetResult();								//z2=_ReadZ2();
				Xt1=_TouchPositionX();
				Xt2=_TouchPositionX();		            //Rt=5.0f*Xt*(z2/z1-1.0f)/32760;	
				Touch_SendCMD(0x94);
				Yt=Touch_GetResult();	
				Yt1=_TouchPositionY();
				Yt2=_TouchPositionY();	
				GPIO_SetBits(GPIOB, GPIO_Pin_12);    //GPIOB->BSRR=GPIO_BSRR_BS_12;	
				if((Xt<=Xt1)&&(Xt<=Xt2)){
						Xt=( Xt1<=Xt2) ? Xt1 : Xt2;
					}
					else 
					{
						if((Xt1<=Xt)&&(Xt1<=Xt2)){
							Xt=(Xt<=Xt2) ? Xt : Xt2;
							}
						else{
							Xt=(Xt<=Xt1) ? Xt : Xt1;
						}
					}
					if((Yt<=Yt1)&&(Yt<=Yt2)){
						Yt=( Yt1<=Yt2) ? Yt1 : Yt2;
					}
					else 
					{
						if((Yt1<=Yt)&&(Yt1<=Yt2)){
							Yt=(Yt<=Yt2) ? Yt : Yt2;
							}
						else{
							Yt=(Yt<=Yt1) ? Yt : Yt1;
						}
					}	
			Rt=5.0f*Xt/4096*(z2/z1-1.0f);
				if (Rt<=4.0f)
				{
					Xd=A*Xt+B*Yt+C;
					Yd=D*Xt+E*Yt+F;
					State.y=Yd;
					State.x=Xd;
					State.Layer=0;
					State.Pressed=TOUCH_PRESSED;
					GUI_TOUCH_StoreStateEx(&State);
					if(drawmode)
						GUI_DrawPoint(Xd,Yd);
				}
			}
			else
			{	
			 GUI_TOUCH_GetState(&State);
				if(State.Pressed==TOUCH_PRESSED)
					{
				   State.Pressed=TOUCH_UNPRESSED;
					 GUI_TOUCH_StoreStateEx(&State);	
					}
			}
}


/*********************************************************************
*
*       Public functions
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Function description:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void) {
 // U32  TouchOrientation;
	GUI_DEVICE * pDevice;
  CONFIG_FLEXCOLOR Config = {0};
  GUI_PORT_API PortAPI = {0};
  //
  // Set display driver and color conversion
  //
  pDevice = GUI_DEVICE_CreateAndLink(GUIDRV_FLEXCOLOR, GUICC_M565, 0, 0);
  //
  // Display driver configuration, required for Lin-driver
  //
  LCD_SetSizeEx (0, XSIZE_PHYS , YSIZE_PHYS);
  LCD_SetVSizeEx(0, VXSIZE_PHYS, VYSIZE_PHYS);
	
  //
  // Display driver configuration, required for Lin-driver
  //
   Config.Orientation =GUI_MIRROR_Y|GUI_MIRROR_X;
	//Config.FirstCOM=0;
  //Config.FirstSEG=0;
	GUIDRV_FlexColor_Config(pDevice, &Config);
  //
  // Set controller and operation mode
  //
	//
  GUIDRV_FlexColor_SetReadFunc66720_B16(pDevice, GUIDRV_FLEXCOLOR_READ_FUNC_II); //GUIDRV_FLEXCOLOR_READ_FUNC_II
  PortAPI.pfWrite16_A0  = LcdWriteReg;
  PortAPI.pfWrite16_A1  = LcdWriteData;
  PortAPI.pfWriteM16_A1 = LcdWriteDataMultiple;
  PortAPI.pfReadM16_A1  = LcdReadDataMultiple;
  GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66720, GUIDRV_FLEXCOLOR_M16C0B16);
}

void GUI_TOUCH_X_ActivateX(void) {
}
void GUI_TOUCH_X_ActivateY(void) {
}
int  GUI_TOUCH_X_MeasureX(void) {
  return _TouchPositionX();
}
int  GUI_TOUCH_X_MeasureY(void) {
  return _TouchPositionY();
}

void DrawTarget(uint16_t x,uint16_t y){
		
	GUI_DrawVLine(x,y-5,y+5);
	GUI_DrawHLine(y,x-5,x+5);
	GUI_DrawCircle(x,y,4);	
}

/*************************************************************************************************************************/
void Touch_calibration(void){
	uint8_t i;
	uint32_t* addr;
	NVIC_DisableIRQ(TIM6_DAC_IRQn);
	NVIC_DisableIRQ(TIM7_IRQn);
	NVIC_DisableIRQ(RTC_WKUP_IRQn);
	
	 	
	GUI_SetFont(GUI_FONT_16_1);
	GUI_SetBkColor(GUI_DARKBLUE);
	GUI_Clear();
	GUI_Delay(1000);
	GUI_SetColor(GUI_WHITE);
	for(i=1;i<6;i++)
	{
		GUI_DispStringHCenterAt("PRESS",Xd[i],Yd[i]+10);
		DrawTarget(Xd[i],Yd[i]);
		while(_TouchIsPresset()==0) {}
		GUI_Delay(1500);
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);//GPIOB->BSRR=GPIO_BSRR_BR_12;
		Xt[i]=_TouchPositionX();
		Yt[i]=_TouchPositionY();
		GPIO_SetBits(GPIOB, GPIO_Pin_12);//GPIOB->BSRR=GPIO_BSRR_BS_12;
		GUI_Clear();
		GUI_Delay(2000);
		
	}
	a=Xt[1]*Xt[1]+Xt[2]*Xt[2]+Xt[3]*Xt[3]+Xt[4]*Xt[4]+Xt[5]*Xt[5];
	b=Yt[1]*Yt[1]+Yt[2]*Yt[2]+Yt[3]*Yt[3]+Yt[4]*Yt[4]+Yt[5]*Yt[5];
	c=Xt[1]*Yt[1]+Xt[2]*Yt[2]+Xt[3]*Yt[3]+Xt[4]*Yt[4]+Xt[5]*Yt[5];
	d=Xt[1]+Xt[2]+Xt[3]+Xt[4]+Xt[5];
	e=Yt[1]+Yt[2]+Yt[3]+Yt[4]+Yt[5];
	X1=Xt[1]*Xd[1]+Xt[2]*Xd[2]+Xt[3]*Xd[3]+Xt[4]*Xd[4]+Xt[5]*Xd[5];
	X2=Yt[1]*Xd[1]+Yt[2]*Xd[2]+Yt[3]*Xd[3]+Yt[4]*Xd[4]+Yt[5]*Xd[5];
	X3=Xd[1]+Xd[2]+Xd[3]+Xd[4]+Xd[5];
	
	Y1=Xt[1]*Yd[1]+Xt[2]*Yd[2]+Xt[3]*Yd[3]+Xt[4]*Yd[4]+Xt[5]*Yd[5];
	Y2=Yt[1]*Yd[1]+Yt[2]*Yd[2]+Yt[3]*Yd[3]+Yt[4]*Yd[4]+Yt[5]*Yd[5];
	Y3=Yd[1]+Yd[2]+Yd[3]+Yd[4]+Yd[5];
	
	det=5*(a*b-c*c)+2*c*d*e-a*e*e-b*d*d;
	detX1=5*(X1*b-X2*c)+e*(X2*d-X1*e)+X3*(c*e-b*d);
	detX2=5*(X2*a-X1*c)+d*(X1*e-X2*d)+X3*(c*d-a*e);
	detX3=X3*(a*b-c*c)+X1*(c*e-b*d)+X2*(c*d-a*e);
	detY1=5*(Y1*b-Y2*c)+e*(Y2*d-Y1*e)+Y3*(c*e-b*d);
	detY2=5*(Y2*a-Y1*c)+d*(Y1*e-Y2*d)+Y3*(c*d-a*e);
	detY3=Y3*(a*b-c*c)+Y1*(c*e-b*d)+Y2*(c*d-a*e);
	A=detX1/det;
	B=detX2/det;
	C=detX3/det;
	D=detY1/det;
	E=detY2/det;
	F=detY3/det;
	addr=(uint32_t*)&A;
	RTC->BKP3R=*addr;
	addr=(uint32_t*)&B;
	RTC->BKP4R=*addr;
	addr=(uint32_t*)&C;
	RTC->BKP5R=*addr;
	addr=(uint32_t*)&D;
	RTC->BKP6R=*addr;
	addr=(uint32_t*)&E;
	RTC->BKP7R=*addr;
	addr=(uint32_t*)&F;
	RTC->BKP8R=*addr;
	
	/*A=(Xd[1]*(Yt[2]-Yt[3])+Xd[2]*(Yt[3]-Yt[1])+Xd[3]*(Yt[1]-Yt[2]))/(Xt[1]*(Yt[2]-Yt[3])+Xt[2]*(Yt[3]-Yt[1])+Xt[3]*(Yt[1]-Yt[2]));
	B=(A*(Xt[3]-Xt[2])+Xd[2]-Xd[3])/(Yt[2]-Yt[3]);
	C=Xd[3]-A*Xt[3]-B*Yt[3];
	D=(Yd[1]*(Yt[2]-Yt[3])+Yd[2]*(Yt[3]-Yt[1])+Yd[3]*(Yt[1]-Yt[2]))/(Xt[1]*(Yt[2]-Yt[3])+Xt[2]*(Yt[3]-Yt[1])+Xt[3]*(Yt[1]-Yt[2]));
	E=(D*(Xt[3]-Xt[2])+Yd[2]-Yd[3])/(Yt[2]-Yt[3]);
	F=Yd[3]-D*Xt[3]-E*Yt[3];*/
	
	
	
	GUI_DispStringHCenterAt("CALIBRATION COMLETE",240,136);
	GUI_Delay(1000);	
	GUI_SetBkColor(GUI_LIGHTBLUE);
	GUI_ClearRect(1,17,58,270);
	GUI_SetColor(GUI_YELLOW);
	GUI_DrawRect(0,16,59,271);
	GUI_SetBkColor(GUI_DARKBLUE);
	GUI_ClearRect(0,0,470,15);
	
	CreateStart();
	
	time_show=1;
	WM_ShowWindow(hIcon_EXIT);
	WM_ShowWindow(hIcon_BRIGHT);
	WM_ShowWindow(hALARMA);
	WM_ShowWindow(hALARMB);
	WM_ShowWindow(PROGBAR_MEM);
	
	TIM7->CNT=0;
	TIM7->SR&=~TIM_SR_UIF;
	NVIC_ClearPendingIRQ(TIM7_IRQn);
	NVIC_ClearPendingIRQ(RTC_WKUP_IRQn);
	NVIC_ClearPendingIRQ(TIM6_DAC_IRQn);
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	NVIC_EnableIRQ(TIM7_IRQn);
	NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Function description:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  uint16_t temp=0;
	int r;
	uint32_t * addr;
  (void) LayerIndex;
  (void) pData;
  
  switch (Cmd) {
		case LCD_X_INITCONTROLLER: {
    //
    addr=(uint32_t*)&A;
		*addr=RTC->BKP3R;
		addr=(uint32_t*)&B;
		*addr=RTC->BKP4R;
		addr=(uint32_t*)&C;
		*addr=RTC->BKP5R;
		addr=(uint32_t*)&D;
		*addr=RTC->BKP6R;
		addr=(uint32_t*)&E;
		*addr=RTC->BKP7R;
		addr=(uint32_t*)&F;
		*addr=RTC->BKP8R;
			// Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //Touch_SetCS(0);
		//Touch_SendCMD(0xD0);
		//Touch_SetCS(1);	
    // ...
	/* initial reset */
	LCD_Reset();
	// Set up the Phase Lock Loop circuits. This only has to be done once.
	LcdWriteReg(CMD_SET_PLL_MN); 				// PLL multiplier, set PLL clock to 100MHz
	LcdWriteData(0x001D); 							//  M = 29 
	LcdWriteData(0x0002); 							//  N = 2
	LcdWriteData(0x0004); 							// dummy value, can be anything
	
	LcdWriteReg(CMD_SET_PLL);  					// PLL enable
	LcdWriteData(0x0001);  							// Use PLL output as system clock
	// Wait 200us, allows the PLL to stabilize */
	GUI_Delay(1);
	LcdWriteReg(CMD_GET_PLL_STATUS);
	while(!temp)
	{
		temp=LCD_DATA_ADDRESS;
	}

	LcdWriteReg(CMD_SET_PLL);
	LcdWriteData(0x0003); 							// SSD1963 is switched to PLL output after PLL has stabilized.
	
	/*this is 500 us, allows the PLL to stabilize */
	GUI_Delay(1);
	LcdWriteReg(CMD_SOFT_RESET); 				// software reset, see SSD1963 manual p.20 Command Table.
	GUI_Delay(10);												//The host must wait 5ms before sending any new commands. 						

	LcdWriteReg(CMD_SET_PIXCLK_FREQ);		//PLL setting for PCLK, depends on LCD resolution
	// For the 4.3" LCD				532*288*60=9192960 Hz  LCDC_FPR=99929
	LcdWriteData(0x0001);
	LcdWriteData(0x0086);
	LcdWriteData(0x0059);

	LcdWriteReg(CMD_SET_LCD_MODE);			// LCD SPECIFICATION
	// We have to write 7 parameter values. Various bit values are
	// set. See manual p.43.
	LcdWriteData(0x0020);  							// 24 bit, FRC dithering off, TFT dithering off, other stuff
	LcdWriteData(0x0000);  							// LFRAME signal polarities
  LcdWriteData(0x0001);  							// 2 entries for Set Horizontal width
	LcdWriteData(0x00DF);
  LcdWriteData(0x0001);  							// 2 entries for Set vertical width
	LcdWriteData(0x000F);
  LcdWriteData(0x0000); 							// Even line RGB sequence 000 == RGB

	LcdWriteReg(CMD_SET_HORIZ_PERIOD);	//HSYNC
	LcdWriteData(((HT-1)>>8)&0x00ff);  	//Set HT= HT(532) -1=531 0x0213
	LcdWriteData((HT-1)&0x00ff);
	LcdWriteData(((HS+HBP)>>8)&0x00ff);  //Set HTS= HS+HBP=2+42=42  0x2D
	LcdWriteData((HS+HBP)&0x00ff);
	LcdWriteData((HS-1)&0x00ff);		 		//Set HTW = HPW-1=1
	LcdWriteData(0x0000);  							//Set LPS=0000
	LcdWriteData(0x0000);
	LcdWriteData(0x0000);								// LPSPP. Dummy byte for TFT interface

	LcdWriteReg(CMD_SET_VERT_PERIOD); 	//VSYNC
	LcdWriteData(((VT-1)>>8)&0x00ff);   //Set VT=VP-1=287 
	LcdWriteData((VT-1)&0x00ff);
	LcdWriteData(((VS+VBP)>>8)&0x00ff); //Set VPS= VPW+VBP=10+12=22 
	LcdWriteData((VS+VBP)&0x00ff);
	LcdWriteData((VS-1)&0x00ff);  			//Set VPW=VPW-1
	LcdWriteData(0X0000);  							//Set FPS
	LcdWriteData(0X0000);
		
	LcdWriteReg(CMD_SET_COL_ADDRESS);
	LcdWriteData(0x0000);
	LcdWriteData(0x0000);
	LcdWriteData(0x0001);
	LcdWriteData(0x00DF);
	
	LcdWriteReg(CMD_SET_PAGE_ADDRESS);
	LcdWriteData(0x0000);
	LcdWriteData(0x0000);
	LcdWriteData(0x0001);
	LcdWriteData(0x000F);
	
	LcdWriteReg(CMD_SET_ADDRESS_MODE); //rotation, see p.18
	LcdWriteData(0x0000);
/*----------------------------------------------------------------------------------*/	
	LcdWriteReg(CMD_SET_PIXDATA_IF_FMT); //pixel data interface 16bit (565 format)
	LcdWriteData(0x0003);
	
	LcdWriteReg(CMD_SET_TEAR_OFF); 		
	
	LcdWriteReg(CMD_SET_DISPLAY_ON); 		//display on
								
								//PWM
	//temp=0;
	//while(!temp)
	//{
		//cycle_start_pwm++;
		GUI_Delay(1);
		LcdWriteReg(CMD_SET_PWM_CONF); 			//set PWM for Backlight. Manual p.53
		// 6 parameters to be set
		LcdWriteData(0x0004); 							// PWM Freq =100MHz/(256*(PWMF[7:0]+1))/256  PWMF[7:0]=4 PWM Freq=305Hz
		LcdWriteData(brightness); 					// PWM duty cycle(50%)
		LcdWriteData(0x0001); 							// PWM controlled by host, PWM enabled
		LcdWriteData(0x00f0); 							// brightness level 0x00 - 0xFF
		LcdWriteData(0x0000); 							// minimum brightness level =  0x00 - 0xFF
		LcdWriteData(0x0000);								// brightness prescalar 0x0 - 0xF
		
		/*LcdWriteReg(CMD_GET_PWM_CONF);
		for(i=0;i<3;i++)
			temp=LCD_DATA_ADDRESS;
		for(i=3;i<7;i++)
			temp1=LCD_DATA_ADDRESS;*/
	//}	
	return 0;
  }
  default:
    r = -1;
  }
  return r;
}

		void LCD_Reset(void)
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_2);
	GUI_Delay(10);
	GPIO_SetBits(GPIOE, GPIO_Pin_2);
	GUI_Delay(10);
}
/*************************** End of file ****************************/

