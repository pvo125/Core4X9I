

#include <stm32f4xx.h>
#include "header.h"


extern GPIO_InitTypeDef 										GPIO_InitStruct;
extern DMA_InitTypeDef											DMA_InitStruct;
extern FMC_NORSRAMTimingInitTypeDef  FMC_NORSRAMTiming;
extern FMC_NORSRAMInitTypeDef        FMC_NORSRAM;
FMC_NAND_PCCARDTimingInitTypeDef     FMC_NANDTiming;
FMC_NANDInitTypeDef                  FMC_NAND;
//SPI_InitTypeDef											SPI_InitStruct;



void delay(uint32_t uSec){
	SysTick->LOAD=(uSec*9);
	SysTick->VAL=0;
	while(!(SysTick->CTRL &SysTick_CTRL_COUNTFLAG_Msk)){}	
}

//=====================================================================================

//	

//=====================================================================================

#define  HT  		532	 // horisontal total         
#define  HDP	 	479  // horizontal width     480
#define  HFP   	8		// horizontal front porch 2-8
#define  HBP   	42		//horizontal back porch   8-43
#define  HS    	2	// horizontal pulse width   1-

#define  VT    288 		// vertical total
#define  VDP	 271	// vertical width
#define  VFP   2			// vertical front porch
#define  VBP   6			// vertical back porch	
#define  VS    6			// vertical pulse width


//=====================================================================================

void LCD_WR_REG(unsigned int command)
{
	LCD_REG_ADDRESS =  command;
}


void LCD_WR_Data(unsigned int val)
{   
	LCD_DATA_ADDRESS =  val; 	
}



void LCD_WR_CMD(unsigned int index, unsigned int val)
{	
	LCD_REG_ADDRESS =  index;	
	LCD_DATA_ADDRESS =  val;
}


unsigned int LCD_RD_REG(unsigned int command)
{
	 LCD_REG_ADDRESS =  command;

	return LCD_DATA_ADDRESS;
}


unsigned int LCD_RD_Data(void)
{
	unsigned int a = 0;
	//a = (*(__IO uint16_t *) (Bank1_LCD_Data)); 	//Dummy
	//a =  *(__IO uint16_t *) (Bank1_LCD_Data);  	//H
	//a = a<<8;
	a = LCD_DATA_ADDRESS; //L

	return(a);	
}


void Set_COL_PAGE_Address(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	
  // Set the start column and page addresses. 
	// sequential 8 bit values. 
	LCD_WR_REG(CMD_SET_COL_ADDRESS); // Manual p.29	
	LCD_WR_Data(x1 >> 8);	// start column high byte    
	LCD_WR_Data(x1 & 0x00ff);	// start column low byte
	LCD_WR_Data(x2 >> 8);  	// end column # high byte	    
	LCD_WR_Data(x2 & 0x00ff); // end column # low byte

  LCD_WR_REG(CMD_SET_PAGE_ADDRESS); // Manual p.29	
	LCD_WR_Data(y1 >> 8);	// start row high byte     
	LCD_WR_Data(y1 & 0x00ff);	// start row low byte
	LCD_WR_Data(y2 >> 8);  	// end row high byte	    
	LCD_WR_Data(y2 & 0x00ff);  // end row low byte
	// Set writing to commence
	LCD_WR_REG(CMD_WRITE_MEM_START);

	return;
} 


void LCD_DrawEllipse(int x1, int y1, int x2, int y2, short color)
{  	

	double x0, y0, a,b, dtest;
	int x, y;
 	int lineWidth;
	int height;
	
	lineWidth  =  x2 - x1 + 1;
	height  =  y2 - y1 + 1;

	Set_COL_PAGE_Address(x1,y1,x2,y2);

	// This is drawing in the [x1,y1] to [x2,y2] region
	x0 = (x1+x2) / 2;
	y0 = (y1+y2) / 2;
	a = (x2-x1) / 2;
	b = (y2-y1) / 2;
	a = a*a;
	b = b*b;

	for(y = y1;y < y1 + height; y++)
	{
		for(x = x1; x < x1 + lineWidth; x++)
		{   
			// Test if the point is in the ellipse and draw it
			dtest =	((x-x0)*(x-x0)/a + (y-y0)*(y-y0)/b);
			if (1.0 > dtest)
          		LCD_WR_Data(color);
			else // increment the counter without writing
			{	
				LCD_WR_Data(0xffff); // TODO: just incr the counter
			}
		}
	}

}

// PiXCLe command primitive
void LCD_DrawBackground(short color)
{
    unsigned int lineWidth  =  HDP + 1,height  =  VDP + 1, w;
	Set_COL_PAGE_Address(0,0,HDP,VDP);
	
	// The 4.3" LCD is 480x272
	while(lineWidth--)
	{
	    for(w = 0;w < height;w++)
		{   
			// Write a single pixel.
          	LCD_WR_Data(color);
		}
	}
}


void LCD_DrawRectFull(uint16_t x,uint16_t y,uint16_t width,uint16_t heigh,uint16_t color)
{
	uint32_t i,square=width*heigh;
	
	Set_COL_PAGE_Address(x,y,x+width-1,y+heigh-1);
	for(i=0;i<square;i++)
		LCD_WR_Data(color);
}

void LCD_DrawRect(uint16_t x,uint16_t y,uint16_t width,uint16_t heigh,uint16_t color)
{
	LCD_DrawLineH(x,y,width,color);
	LCD_DrawLineH(x,y+heigh-1,width,color);
	LCD_DrawLineV(x,y,heigh,color);
	LCD_DrawLineV(x+width-1,y,heigh,color);
}

void LCD_DrawLineH(uint16_t x,uint16_t y,uint16_t width, uint16_t color)
{	
	uint16_t i;
	Set_COL_PAGE_Address(x,y,x+width-1,y);
	for(i=0;i<width;i++)
		LCD_WR_Data(color);
}


void LCD_DrawLineV(uint16_t x,uint16_t y,uint16_t heigh, uint16_t color)
{	
	uint16_t i;
	Set_COL_PAGE_Address(x,y,x,y+heigh-1);
	for(i=0;i<heigh;i++)
		LCD_WR_Data(color);
}



void LCD_PutPixel(uint16_t x,uint16_t y,uint16_t color)
{
	Set_COL_PAGE_Address(x,y,x,y);
	LCD_DATA_ADDRESS =  color;
}



void LCD_DrawBMP(const unsigned short *bitmap)
{
	uint32_t i;
	Set_COL_PAGE_Address(0,0,HDP,VDP);
	for(i=0;i<130560;i++)
		LCD_WR_Data(bitmap[i]);
	
}

  

void SetScrollArea(unsigned int top, unsigned int scroll, unsigned int bottom)
{


//	WriteCommand(CMD_SET_SCROLL_AREA);
//	CS_LAT_BIT  =  0;
	LCD_WR_REG(0x33);
	LCD_WR_Data(top>>8);
	LCD_WR_Data(top);
	LCD_WR_Data(scroll>>8);
	LCD_WR_Data(scroll);
	LCD_WR_Data(bottom>>8);
	LCD_WR_Data(bottom);
//	CS_LAT_BIT  =  1;	
}




void SetScrollStart(unsigned int line)
{

//	LCD_WR_REG(0x002A);	
//    LCD_WR_Data(0);	  
//	LCD_WR_REG(CMD_SET_SCROLL_START);
	LCD_WR_REG(0x37);
//	CS_LAT_BIT  =  0;
	LCD_WR_Data(line>>8);
	LCD_WR_Data(line);	
//	CS_LAT_BIT  =  1;
}



void SetTearingCfg(unsigned char state, unsigned char mode)
{


	if(state  == 1)
	{
		LCD_WR_REG(0x35);
		//CS_LAT_BIT  =  0;
		LCD_WR_Data(mode&0x01);
		//CS_LAT_BIT  =  1;
	}
	else
	{
		LCD_WR_REG(0x34);
	}

	
}


/****************************************************************/
/*												Настройка контроллера FMC SRAM 			  */
/****************************************************************/
void SSD1963_LowLevel_Init(void){
		
	/*  Настройка портов GPIO для управления TFT	*/	
	
	//Port D
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7|
				GPIO_Pin_8|	GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15,GPIO_AF_FMC);
	//Port E
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|
				GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14,GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15,GPIO_AF_FMC);
	
	// LCD RESET 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_DOWN;//NOPULL
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOE, GPIO_Pin_2);
	/*            Настройка FMC                   */
#ifdef NAND
	FMC_NANDTiming.FMC_HiZSetupTime=4;//3;//2;//1;
	FMC_NANDTiming.FMC_HoldSetupTime=4;//3;//2;//2;
	FMC_NANDTiming.FMC_SetupTime=3;//2;//1;//1;
	FMC_NANDTiming.FMC_WaitSetupTime=7;//5;//4;//3;
	
	FMC_NAND.FMC_AttributeSpaceTimingStruct=&FMC_NANDTiming;
	FMC_NAND.FMC_Bank=FMC_Bank2_NAND;
	FMC_NAND.FMC_CommonSpaceTimingStruct=&FMC_NANDTiming;
	FMC_NAND.FMC_ECC=FMC_ECC_Disable;
	FMC_NAND.FMC_ECCPageSize= FMC_ECCPageSize_512Bytes;  
	FMC_NAND.FMC_MemoryDataWidth=FMC_NAND_MemoryDataWidth_16b;
	FMC_NAND.FMC_TARSetupTime=0;
	FMC_NAND.FMC_TCLRSetupTime=0;
	FMC_NAND.FMC_Waitfeature= FMC_Waitfeature_Disable;
	
	FMC_NANDInit(&FMC_NAND);
	FMC_NANDCmd(FMC_Bank2_NAND, ENABLE);
#endif
#ifdef SRAM
			FMC_NORSRAMTiming.FMC_AccessMode=FMC_AccessMode_A;
			FMC_NORSRAMTiming.FMC_AddressHoldTime=5;//1
			FMC_NORSRAMTiming.FMC_AddressSetupTime=5;//2
			FMC_NORSRAMTiming.FMC_BusTurnAroundDuration=5;//0
			FMC_NORSRAMTiming.FMC_CLKDivision=2;//1
			FMC_NORSRAMTiming.FMC_DataLatency=5; //1
			FMC_NORSRAMTiming.FMC_DataSetupTime=15; //10
				
			FMC_NORSRAM.FMC_AsynchronousWait=FMC_AsynchronousWait_Disable;
			FMC_NORSRAM.FMC_Bank=FMC_Bank1_NORSRAM1;
			FMC_NORSRAM.FMC_BurstAccessMode=FMC_BurstAccessMode_Disable;
			FMC_NORSRAM.FMC_ContinousClock=FMC_CClock_SyncOnly;
			FMC_NORSRAM.FMC_DataAddressMux=FMC_DataAddressMux_Disable;
			FMC_NORSRAM.FMC_ExtendedMode=FMC_ExtendedMode_Disable;
			FMC_NORSRAM.FMC_MemoryDataWidth=FMC_NORSRAM_MemoryDataWidth_16b;
			FMC_NORSRAM.FMC_MemoryType=FMC_MemoryType_SRAM;
			FMC_NORSRAM.FMC_WaitSignal=FMC_WaitSignal_Disable;
			FMC_NORSRAM.FMC_WaitSignalActive=FMC_WaitSignalActive_BeforeWaitState;
			FMC_NORSRAM.FMC_WaitSignalPolarity=FMC_WaitSignalPolarity_Low;
			FMC_NORSRAM.FMC_WrapMode=FMC_WrapMode_Disable;
			FMC_NORSRAM.FMC_WriteBurst=FMC_WriteBurst_Disable;
			FMC_NORSRAM.FMC_WriteOperation=FMC_WriteOperation_Enable;
			FMC_NORSRAM.FMC_WriteTimingStruct=     &FMC_NORSRAMTiming;
			FMC_NORSRAM.FMC_ReadWriteTimingStruct= &FMC_NORSRAMTiming;
			FMC_NORSRAMInit(&FMC_NORSRAM);
			FMC_NORSRAMCmd(FMC_Bank1_NORSRAM1, ENABLE);
#endif



}

/*void LCD_BacklightOn(void)
{
	GPIO_SetBits(GPIOD, GPIO_Pin_13);
}



void LCD_BacklightOff(void)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_13);
}
*/


/*****END OF FILE****/
