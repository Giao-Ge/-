#ifndef __FLASH_H
#define __FLASH_H
#include "sys.h"



//以下字库，必须通过SPI教程写入到FLASH里，否则显示不正确


//4K一个扇区
#define SECTOR_SIZE    4096


#define ADDR_DISPLAY_ASCLL_6_8      (0*SECTOR_SIZE)//共96个字符，每个占6字节，共552字节，占1个扇区
#define ADDR_DISPLAY_ASCLL_8_16      (1*SECTOR_SIZE)//共96个字符，每个占16字节，共1536字节，占1个扇区

//共682个符号，共6912个字符，
//高字节编码0xa1~0xa9,低字节编码0xa0~0xff,这一段是符号段
//高字节编码0xaa~0xaf,低字节编码0xa0~0xff,这一段为空，实际中什么也没有写入
//高字节编码0xb0~0xf7,低字节编码0xa0~0xff,这一段是GB2312汉字段
//实际长度为：（0xf7-0xa1+1）*（0xff-0xa0+1）=8352个字，
//每个字占32字节,一共32*8352=267264，占65个扇区
#define ADDR_DISPLAY_GB2312_16_16      (2*SECTOR_SIZE)


//第200个扇区保存WIFI的配置
#define ADDR_ESP8266_CONFIG      (200*SECTOR_SIZE)



//W25X系列/Q系列芯片列表
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16
//W25Q128 ID  0XEF17
#define W25Q80 	0XEF13
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

extern u16 W25QXX_TYPE;					//定义W25QXX芯片型号

#define	W25QXX_CS 		PAout(4)  		//W25QXX的片选信号

////////////////////////////////////////////////////////////////////////////

//指令表
#define W25X_WriteEnable		0x06
#define W25X_WriteDisable		0x04
#define W25X_ReadStatusReg		0x05
#define W25X_WriteStatusReg		0x01
#define W25X_ReadData			0x03
#define W25X_FastReadData		0x0B
#define W25X_FastReadDual		0x3B
#define W25X_PageProgram		0x02
#define W25X_BlockErase			0xD8
#define W25X_SectorErase		0x20
#define W25X_ChipErase			0xC7
#define W25X_PowerDown			0xB9
#define W25X_ReleasePowerDown	0xAB
#define W25X_DeviceID			0xAB
#define W25X_ManufactDeviceID	0x90
#define W25X_JedecDeviceID		0x9F

void W25QXX_Init(void);
u16  W25QXX_ReadID(void);  	    		//读取FLASH ID
u8	 W25QXX_ReadSR(void);        		//读取状态寄存器
void W25QXX_Write_SR(u8 sr);  			//写状态寄存器
void W25QXX_Write_Enable(void);  		//写使能
void W25QXX_Write_Disable(void);		//写保护
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //读取flash
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u32 NumByteToWrite);//写入flash
void W25QXX_Erase_Chip(void);    	  	//整片擦除
void W25QXX_Erase_Sector(u32 Dst_Addr);	//扇区擦除
void W25QXX_Wait_Busy(void);           	//等待空闲
void W25QXX_PowerDown(void);        	//进入掉电模式
void W25QXX_WAKEUP(void);				//唤醒
#endif
















