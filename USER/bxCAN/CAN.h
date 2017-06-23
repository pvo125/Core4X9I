#ifndef _CAN_H_
#define _CAN_H_

#include <stm32f4xx.h>


#define CAN1_TX GPIO_Pin_9
#define CAN1_RX GPIO_Pin_8

#define CAN1_TX_Source GPIO_PinSource9
#define CAN1_RX_Source GPIO_PinSource8

#define CAN1_PORT GPIOB

#define OPEN429 	 0x1
#define STM32f429  0x2
#define F103_KIT	 0x3

#define FLAG_STATUS_SECTOR	0x08004000		//sector 1
#define NAMBER_FLAG_STATUS_SECTOR  1

typedef enum
{
	CAN_TXOK=0,
	CAN_TXERROR,
	CAN_TXBUSY
} CAN_TXSTATUS;

typedef enum
{
	CAN_RXOK=0,
	CAN_RXERROR,
} CAN_RXSTATUS;

typedef struct
{
	uint16_t ID;
	uint8_t DLC;
	uint8_t Data[8];
} CANTX_TypeDef; 

typedef struct
{
	uint16_t ID;
	uint8_t FMI;
	uint8_t DLC;
	uint8_t Data[8];
} CANRX_TypeDef; 

extern CANTX_TypeDef CAN_Data_TX;
//extern CANRX_TypeDef CAN_Data_RX;
//extern CANRX_TypeDef *pCAN_Data_RX[];

void bxCAN_LowLevel_Init(void);
void bxCAN_Init(void);
CAN_TXSTATUS CAN_Transmit_DataFrame(CANTX_TypeDef *Data);
CAN_TXSTATUS CAN_Transmit_RemoteFrame(uint16_t ID);
void CAN_Receive_IRQHandler(uint8_t FIFONumber);
void CAN_RXProcess0(void);
void CAN_RXProcess1(void);

extern void Flash_prog(uint8_t * src,uint8_t * dst,uint32_t nbyte,uint8_t psize);
extern void Flash_sect_erase(uint8_t numsect,uint8_t count);







#endif
