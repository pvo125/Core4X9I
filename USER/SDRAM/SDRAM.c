#include <stm32f4xx.h>
#include "header.h"

/****************************************************************/
/*												Настройка контроллера FMC SDRAM 			*/
/****************************************************************/
void SDRAM_LowLevel_Init(void){
	uint32_t tmp;
	
/*  GPIO configuration*/
/*
 +-------------------+--------------------+--------------------+--------------------+
 +                       SDRAM pins assignment                                      +
 +-------------------+--------------------+--------------------+--------------------+
 | PD0  <-> FMC_D2   | PE0  <-> FMC_NBL0  | PF0  <-> FMC_A0    | PG0 <-> FMC_A10    |
 | PD1  <-> FMC_D3   | PE1  <-> FMC_NBL1  | PF1  <-> FMC_A1    | PG1 <-> FMC_A11    |
 | PD8  <-> FMC_D13  | PE7  <-> FMC_D4    | PF2  <-> FMC_A2    | PG4 <-> FMC_A14    |
 | PD9  <-> FMC_D14  | PE8  <-> FMC_D5    | PF3  <-> FMC_A3    | PG5 <-> FMC_A15    |
 | PD10 <-> FMC_D15  | PE9  <-> FMC_D6    | PF4  <-> FMC_A4    | PG8 <-> FC_SDCLK   |
 | PD14 <-> FMC_D0   | PE10 <-> FMC_D7    | PF5  <-> FMC_A5    | PG15 <-> FMC_NCAS  |
 | PD15 <-> FMC_D1   | PE11 <-> FMC_D8    | PF11 <-> FC_NRAS   |--------------------+
 +-------------------| PE12 <-> FMC_D9    | PF12 <-> FMC_A6    |
                     | PE13 <-> FMC_D10   | PF13 <-> FMC_A7    |
                     | PE14 <-> FMC_D11   | PF14 <-> FMC_A8    |
                     | PE15 <-> FMC_D12   | PF15 <-> FMC_A9    |
 +-------------------+--------------------+--------------------+
 | PH7 <-> FMC_SDCKE1| 
 | PH6 <-> FMC_SDNE1 | 
 | PH5 <-> FMC_SDNWE |*/
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|
												 RCC_AHB1Periph_GPIOE|
												 RCC_AHB1Periph_GPIOF|
												 RCC_AHB1Periph_GPIOG|
												 RCC_AHB1Periph_GPIOH,ENABLE);

	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC,   ENABLE);			

	//SYSCFG->MEMRMP|=SYSCFG_MEMRMP_SWP_FMC_0;	// swap SDRAM_BANK_2 PCCARD_BANK 0xD0000000->0x90000000
	
	/* Connect PDx pins to FMC Alternate function  AF12 (1100=0xC) */
	GPIOD->AFR[0]|=0x000000CC;
	GPIOD->AFR[1]|=0xCC000CCC;
	/* Configure PDx pins in Alternate function mode */
	GPIOD->MODER|=0xA02A000A;
	/* Configure PDx pins speed to 100 MHz */
	GPIOD->OSPEEDR|=0xF03F000F;
	/* Configure PDx pins Output type to push-pull */
	GPIOD->OTYPER|=0x00000000;
	/* No pull-up, pull-down for PDx pins */ 
	GPIOD->PUPDR|=0x00000000;
	
	/* Connect PEx pins to FMC Alternate function  AF12 (1100=0xC) */
	GPIOE->AFR[0]|=0xC00000CC;
	GPIOE->AFR[1]|=0xCCCCCCCC;	
	/* Configure PEx pins in Alternate function mode */
	GPIOE->MODER|=0xAAAA800A;
	/* Configure PEx pins speed to 100 MHz */
	GPIOE->OSPEEDR|=0xFFFFC00F;
	/* Configure PEx pins Output type to push-pull */
	GPIOE->OTYPER|=0x00000000;
	/* No pull-up, pull-down for PEx pins */ 
	GPIOE->PUPDR|=0x00000000;
	
	/* Connect PFx pins to FMC Alternate function  AF12 (1100=0xC) */
	GPIOF->AFR[0]=0x00CCCCCC;
	GPIOF->AFR[1]=0xCCCCC000;	
	/* Configure PFx pins in Alternate function mode */
	GPIOF->MODER=0xAA800AAA;
	/* Configure PFx pins speed to 100 MHz */
	GPIOF->OSPEEDR=0xFFC00FFF;
	/* Configure PFx pins Output type to push-pull */
	GPIOF->OTYPER=0x00000000;
	/* No pull-up, pull-down for PFx pins */ 
	GPIOF->PUPDR=0x00000000;

	/* Connect PEx pins to FMC Alternate function  AF12 (1100=0xC) */
	GPIOG->AFR[0]=0x00CC00CC;
	GPIOG->AFR[1]=0xC000000C;	
	/* Configure PGx pins in Alternate function mode */
	GPIOG->MODER=0x80020A0A;
	/* Configure PGx pins speed to 100 MHz */
	GPIOG->OSPEEDR=0xC0030F0F;
	/* Configure PGx pins Output type to push-pull */
	GPIOG->OTYPER=0x00000000;
	/* No pull-up, pull-down for PGx pins */ 
	GPIOG->PUPDR=0x00000000;

	/* Connect PHx pins to FMC Alternate function  AF12 (1100=0xC) */
	GPIOH->AFR[0]=0xCCC00000;
	/* Configure PHx pins in Alternate function mode */
	GPIOH->MODER=0x0000A800;
	/* Configure PHx pins speed to 100 MHz */
	GPIOH->OSPEEDR=0x0000FC00;
	/* Configure PHx pins Output type to push-pull */
	GPIOH->OTYPER=0x00000000;
	/* No pull-up, pull-down for PHx pins */ 
	GPIOH->PUPDR=0x00000000;

	

/* FMC Configuration ---------------------------------------------------------*/
/* FMC SDRAM Bank configuration
*/
// Initialization step 1

#define SDCR_WriteProtection_RESET ((uint32_t)0x00007DFF)
	FMC_Bank5_6->SDCR[0]=FMC_SDCR1_RPIPE_0|							//	RPIPE HCLK clock cycle delay  2
											 FMC_SDCR1_RBURST |							//  Burst read
											 FMC_SDCR1_SDCLK_1;							//  SDCLK period = 2 x HCLK periods
	
	FMC_Bank5_6->SDCR[1]&=SDCR_WriteProtection_RESET;		//  Write accesses allowed
	FMC_Bank5_6->SDCR[1]=FMC_SDCR2_CAS |								//  CAS Latency  3 cycles
											 FMC_SDCR2_NB		 |							//  Number of internal banks 4
											 FMC_SDCR2_MWID_0|							//  Memory data bus width    16b
											 FMC_SDCR2_NR_0;								//  Number of row address bits	12bit	
																											//  Number of column address bits 8bit
	
	
// 90 MHz 11.11 ns per cycle
// tCK Clock Cycle Time        7.5   ns CAS3, 10  ns CAS2
// tAC Access Time From CLK    5.4 ns CAS3, 6 ns CAS2
// tDS Input Data Setup Time   1.5 ns
// tDH Input Data Hold Time    0.8 ns
// tRC Command Period (REF to REF) 63 ns    	(6)
// tRAS Command Period (ACT to PRE) 42 ns     (4)
// tRP Command Period (PRE to ACT) 15 ns      (2)
// tRCD Active Command to Read/Write 15 ns    (2)
// tMRD Mode Register Program Time cycle      (2)
// tXSR Self Refresh Exit Time 70 ns         	(7) 

// Initialization step 2 
	FMC_Bank5_6->SDTR[0]=			(2-1)<<20|									//	tRP=(2)=0010=FMC_SDTR2_TRP_0		
														(6-1)<<12	;	               	//  tRC=(6)=0110=FMC_SDTR1_TRC_1|FMC_SDTR1_TRC_2
	FMC_Bank5_6->SDTR[1]=			(2-1)<<24|									//	tRCD=(2)=0001=FMC_SDTR2_TRCD_0
														(2-1)<<16|									//	TWR ≥ TRAS - TRCD and TWR ≥TRC - TRCD - TRP	
																												//	TWR ≥4-2=2  or 6-2-2=2 0001= FMC_SDTR2_TWR_0
														(4-1)<<8| 									//	tRAS=(4)=0100=FMC_SDTR2_TRAS_2
														(7-1)<<4|										//	tXSR=(7)=0001=FMC_SDTR2_TXSR_0
														(2-1);								     //  tMRD=(2)=0001=FMC_SDTR2_TMRD_0		
// Initialization step 3	
		FMC_Bank5_6->SDCMR=FMC_SDCMR_MODE_0|								// 001 Clock Configuration Enable	
											 FMC_SDCMR_CTB2  |								// Command issued to SDRAM Bank 2		
											 FMC_SDCMR_NRFS_0;								// 2 Auto-refresh cycles
// Initialization step 4
		/* Delay 100 mS*/
		for(tmp = 0; tmp < 1000000; tmp++);
// Initialization step 5
	while(FMC_Bank5_6->SDSR&FMC_SDSR_BUSY);
	FMC_Bank5_6->SDCMR=FMC_SDCMR_MODE_1|									// 010 PALL (“All Bank Precharge”) command
										 FMC_SDCMR_CTB2	 |									// Command issued to SDRAM Bank 2
										 FMC_SDCMR_NRFS_0;									// 2 Auto-refresh cycles
// Initialization step 6
	while(FMC_Bank5_6->SDSR&FMC_SDSR_BUSY);
	FMC_Bank5_6->SDCMR=  FMC_SDCMR_MODE_2|								// 011: Auto-refresh command
										   FMC_SDCMR_CTB2|									// Command issued to SDRAM Bank 2
	FMC_SDCMR_NRFS_0|FMC_SDCMR_NRFS_1|FMC_SDCMR_NRFS_2;		// Number of Auto-refresh 8 cycle (0111)
// Initialization step 7
 #define SDRAM_BURST_LENGTH_1                ((uint16_t)0x0000)
 #define SDRAM_BURST_LENGTH_2                ((uint16_t)0x0001)
 #define SDRAM_BURST_LENGTH_4                ((uint16_t)0x0002)
 #define SDRAM_BURST_LENGTH_8                ((uint16_t)0x0004)
 #define SDRAM_BURST_TYPE_SEQUENTIAL         ((uint16_t)0x0000)
 #define SDRAM_BURST_TYPE_INTERLEAVED        ((uint16_t)0x0008)
 #define SDRAM_CAS_LATENCY_2                 ((uint16_t)0x0020)
 #define SDRAM_CAS_LATENCY_3                 ((uint16_t)0x0030)
 #define SDRAM_OPERATING_MODE_STANDARD       ((uint16_t)0x0000)
 #define SDRAM_WRITEBURST_MODE_PROGRAMMED    ((uint16_t)0x0000)
 #define SDRAM_WRITEBURST_MODE_SINGLE        ((uint16_t)0x0200)
 #define FMC_SDCMR_MODE_3										 ((uint32_t)0x00000004)		
	while(FMC_Bank5_6->SDSR&FMC_SDSR_BUSY);
	FMC_Bank5_6->SDCMR=FMC_SDCMR_MODE_3|									// Command mode 100: Load Mode Register 
											FMC_SDCMR_CTB2 |									//   Command issued to SDRAM Bank 2
											FMC_SDCMR_NRFS_0|							  	// 2 Auto-refresh cycles
											SDRAM_BURST_LENGTH_2<<9|
											SDRAM_BURST_TYPE_SEQUENTIAL<<9|
											SDRAM_CAS_LATENCY_3<<9|
											SDRAM_OPERATING_MODE_STANDARD<<9|
											SDRAM_WRITEBURST_MODE_SINGLE<<9;										
// Initialization step 8
	while(FMC_Bank5_6->SDSR&FMC_SDSR_BUSY);
	FMC_Bank5_6->SDRTR|=(1386<<1);													// 64mS/4096=15.625uS
																													// 15.625*90MHz-20=1386
	// Clear SDRAM 
	while(FMC_Bank5_6->SDSR&FMC_SDSR_BUSY);
	for(tmp=SDRAM_BASE;tmp<(SDRAM_BASE+SDRAM_SIZE);tmp+=4)
	*(uint32_t*)tmp=0x0;
}

/**
  * @brief  Writes a Entire-word buffer to the SDRAM memory. 
  * @param  pBuffer: pointer to buffer. 
  * @param  uwWriteAddress: SDRAM memory internal address from which the data will be 
  *         written.
  * @param  uwBufferSize: number of words to write. 
  * @retval None.
  */
void SDRAM_WriteBuffer(uint32_t* pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwWriteAddress;

  /* Disable write protection */
  FMC_SDRAMWriteProtectionConfig(FMC_Bank2_SDRAM, DISABLE);
  
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }

  /* While there is data to write */
  for (; uwBufferSize != 0; uwBufferSize--) 
  {
    /* Transfer data to the memory */
    *(uint32_t *) (SDRAM_BASE + write_pointer) = *pBuffer++;

    /* Increment the address*/
    write_pointer += 4;
  }
    
}

/**
  * @brief  Reads data buffer from the SDRAM memory. 
  * @param  pBuffer: pointer to buffer. 
  * @param  ReadAddress: SDRAM memory internal address from which the data will be 
  *         read.
  * @param  uwBufferSize: number of words to write. 
  * @retval None.
  */
void SDRAM_ReadBuffer(uint32_t* pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwReadAddress;
  
   
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  
  /* Read data */
  for(; uwBufferSize != 0x00; uwBufferSize--)
  {
   *pBuffer++ = *(__IO uint32_t *)(SDRAM_BASE + write_pointer );
    
   /* Increment the address*/
    write_pointer += 4;
  } 
}


