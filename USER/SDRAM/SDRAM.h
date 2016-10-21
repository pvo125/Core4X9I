#ifndef 	_SDRAM_H_
#define 	_SDRAM_H_

#include <stm32f4xx.h>

#define SDRAM_BASE (uint32_t)0x90000000  // swap SDRAM_Bank2 PCCARD_Bank  0xD0000000->0x90000000  
#define SDRAM_SIZE (uint32_t)0x800000
#define SDRAM_END  (uint32_t)0x907fffff
void SDRAM_LowLevel_Init(void);

void SDRAM_WriteBuffer(uint32_t* pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize);
void SDRAM_ReadBuffer(uint32_t* pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize);
#endif
