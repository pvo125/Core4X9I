/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/


#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: USB drive control */
//#include "atadrive.h"	/* Example: ATA drive control */
#include "sdcard.h"		/* Example: MMC/SDC contorl */
#include "header.h"

/* Definitions of physical drive number for each drive */
#define ATA		1	/* Example: Map ATA drive to drive number 0 */
#define MMC		0	/* Example: Map MMC/SD card to drive number 1 */
#define USB		2	/* Example: Map USB drive to drive number 2 */

//extern Time_Type Time;
extern RTC_TimeTypeDef								RTC_Time;
extern RTC_DateTypeDef								RTC_Date;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	if(pdrv==0)
		return 0;
	else
		return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat=STA_NOINIT;
	if(sd_error==SD_OK)
	{
		stat=0;
		return stat;
	}
	else
	{
	sd_error=SD_Init();
	if(sd_error==SD_OK)
		//	Get the SD CID and CSD data.
		{
		sd_error=SD_GetCardInfo(&sd_cardinfo);
			if(sd_error==SD_OK)
				sd_error=SD_SelectDeselect((u32)sd_cardinfo.RCA<<16);
			if(sd_error==SD_OK)		
				sd_error=SD_EnableWideBusOperation(SDIO_BusWide_4b);
			if(sd_error==SD_OK)	
			{
				sd_error=SD_SetDeviceMode(SD_DMA_MODE);
				stat=0;
			}
			
		}
		return stat;
	}
	
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	BYTE count		/* Number of sectors to read */
)
{
	SD_Error sderror;	
	if(count==1)
		sderror=SD_ReadBlock(sector*SD_SECTOR_SIZE, buff, SD_SECTOR_SIZE);//(sd_cardinfo.SD_csd.MaxWrBlockLen)
	else
		sderror=SD_ReadMultiBlocks(sector*SD_SECTOR_SIZE, buff, SD_SECTOR_SIZE, count);
	if(sderror==SD_OK)
		return RES_OK;
	else		
		return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	BYTE count			/* Number of sectors to write */
)
{	
	
	SD_Error sderror;
	if(count==1)
		sderror=SD_WriteBlock(sector*SD_SECTOR_SIZE,  (uint8_t*)buff, SD_SECTOR_SIZE);
	else
		sderror=SD_WriteMultiBlocks(sector*SD_SECTOR_SIZE, (uint8_t*)buff, SD_SECTOR_SIZE, count);
	if(sderror==SD_OK)
		return RES_OK;
	else		
		return RES_ERROR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result=1;

	switch (pdrv) {
	case ATA :
	return res;

	
	
	
	case MMC :
		switch(cmd){
			case GET_SECTOR_COUNT:
			result<<=(sd_cardinfo.SD_csd.DeviceSizeMul+2);
			*(DWORD*) buff=(sd_cardinfo.SD_csd.DeviceSize+1)*result;
			res=RES_OK;
			return res;
			
			case GET_SECTOR_SIZE:
			*(DWORD*) buff=SD_SECTOR_SIZE;
			res=RES_OK;
			return res;	
				
			case GET_BLOCK_SIZE:
			*(DWORD*) buff = 32;
			res = RES_OK;
			return res;
			}	
	
			case USB :
			return res;
	}

	return RES_PARERR;
}

DWORD get_fattime (void)
{
		DWORD t;
		 
	t=(RTC_Date.RTC_Year+20)<<25;
	t|=RTC_Date.RTC_Month<<21;
	t|=RTC_Date.RTC_Date<<16;
	t|=RTC_Time.RTC_Hours<<11;
	t|=RTC_Time.RTC_Minutes<<5;
	t|=RTC_Time.RTC_Seconds/2;
	return t;

}


#endif
