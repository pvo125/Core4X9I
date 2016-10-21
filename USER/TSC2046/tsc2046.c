#include <stm32f4xx.h>
#include "header.h"

extern GPIO_InitTypeDef GPIO_InitStruct;
extern SPI_InitTypeDef SPI_InitStruct;

void TSC2046_LowLevel_Init(void)
{
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15,GPIO_AF_SPI2);
	
	/* TSC2046 interrupt(GPIO input) - port B */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;	/* PB11  - TCH INTR  */
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* TSC2046 CS(GPIO output) - port B */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;	/* PB12  - TCH CS    */
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	

	//GPIOB->BSRRL=GPIO_BSRR_BS_12;	

	/* TSC2046 SCK,MOSI,MISO - port B */

	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	/*	PB13 - SPI2_SCK */
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;								/*  PB14 - SPI2_MISO*/
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;											/*  PB15 - SPI2_MOSI*/	
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
		
	SPI_InitStruct.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_32;  //APB2 = 45MHz  SCK=45/32=1406250Hz 
	SPI_InitStruct.SPI_CPHA=SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CPOL=SPI_CPOL_Low;
	SPI_InitStruct.SPI_CRCPolynomial=7;
	SPI_InitStruct.SPI_DataSize=SPI_DataSize_16b;
	SPI_InitStruct.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode=SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS=SPI_NSS_Soft;
	SPI_Init(SPI2, &SPI_InitStruct);
	SPI_Cmd(SPI2, ENABLE);
	
}
