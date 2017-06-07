#ifndef 	_MX25L8005_H_
#define 	_MX25L8005_H_

#include <stm32f4xx.h>
#define STMicro 			0x0000 //130560*2=261120 байт с адреса 0х00000
#define ARMCortex		 	0x40000// 50592*2=101184 байт с адреса 0х40000
#define stm				   	0x60000// 43400*2=86800 байт с адреса 0х60000
#define Next					0x80000
#define Cancel				0x81400




#define WREN 0x06 
#define WRDI 0x04
#define RDID 0x9F
#define RDSR 0x05	
#define WRSR 0x01
#define READ 0x03
#define FastRead 0x0B
#define SE 		0x20
#define BE		0xD8
#define CE		0xC7
#define PP		0x02
#define CS_LOW()	GPIO_ResetBits(GPIOA, GPIO_Pin_4)  
#define CS_HIGH()	GPIO_SetBits(GPIOA, GPIO_Pin_4)
 void MX25_LowLevel_Init(void);
 void MX25_Init(void);
 void WriteDisable(void);
 void WriteEnable(void);
 void WriteStatusReg_MX25L(uint8_t status);
 uint8_t ReadStatusReg_MX25L(void);
 uint32_t ReadID_MX25L (void);
 void Read_MX25L_FSMC(uint32_t address,uint32_t words,uint16_t *destination);
 void Read_MX25L(uint32_t address,uint32_t bytes,uint8_t *destination);
 void FastRead_MX25L(uint32_t address,uint16_t bytes,uint8_t *destination);
 void SectorErase_MX25L(uint32_t address);
 void BlockErase_MX25L(uint32_t address);
 void ChipErase_MX25L(void);
 void PagePrg_MX25L(uint32_t address, uint8_t *SourceBuffer);
 void Prg_MX25L_16(uint32_t address,uint32_t bytes,const unsigned short *SourceBuffer);
 void Prg_MX25L_8(uint32_t address,uint32_t bytes,const uint8_t *SourceBuffer);
 void Read_MX25L_DMA(uint32_t address,uint16_t bytes);	
#endif
