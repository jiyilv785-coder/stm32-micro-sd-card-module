/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sd_card.h"    /* 包含SD卡驱动 */

/* 驱动器号定义 */
#define DEV_MMC		0	/* SD卡映射到驱动器0 */


/*-----------------------------------------------------------------------*/
/* 获取驱动器状态                                                        */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(BYTE pdrv)
{
    if(pdrv == DEV_MMC)
    {
        return 0;  // 返回0表示正常
    }
    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* 初始化驱动器                                                          */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize(BYTE pdrv)
{
    if(pdrv == DEV_MMC) {
        if(SD_Init() == SD_OK) return 0;
        return STA_NOINIT;
    }
    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* 读扇区                                                                */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
    if(pdrv != DEV_MMC) return RES_PARERR;
    
    LBA_t physical_sector = sector + 32; // 根据分区偏移量调整
    
    for(UINT i=0; i<count; i++) {
        if(SD_ReadSector(physical_sector + i, buff + i*512, 1) != SD_OK)
            return RES_ERROR;
    }
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* 写扇区                                                                */
/*-----------------------------------------------------------------------*/
#if FF_FS_READONLY == 0
DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
    if(pdrv != DEV_MMC) return RES_PARERR;
    
    LBA_t physical_sector = sector + 32;
    
    for(UINT i=0; i<count; i++) {
        if(SD_WriteSector(physical_sector + i, buff + i*512, 1) != SD_OK)
            return RES_ERROR;
    }
    return RES_OK;
}
#endif

/*-----------------------------------------------------------------------*/
/* 设备控制函数                                                          */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
    if(pdrv != DEV_MMC) return RES_PARERR;
    
    switch(cmd) {
        case GET_SECTOR_COUNT:
            *(LBA_t*)buff = SD_GetSectorCount() - 32;
            return RES_OK;
        case GET_SECTOR_SIZE:
            *(WORD*)buff = 512;
            return RES_OK;
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;
            return RES_OK;
        case CTRL_SYNC:
            return RES_OK;
        default:
            return RES_PARERR;
    }
}

/*-----------------------------------------------------------------------*/
/* 获取当前时间（返回固定值）                                            */
/*-----------------------------------------------------------------------*/
DWORD get_fattime(void)
{
    return 0;
}


