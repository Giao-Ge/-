#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "sys.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32F103RC 开发板
//串口2头文件
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************


#define	MAX485_TX 		1//发送
#define	MAX485_RX 		0//接收
#define	MAX485_RX_TX 		PBout(1)  		//MAX485发送接收引脚

typedef void (*pUart2_TxCallBack)(u8 ch);

	  	
//如果想串口中断接收，请不要注释以下宏定义
void uart2_init(u32 bound);
void uart2_485_init(void);
void USART2_send_buff(u8* buf,u32 len);

void USART2_SetCallback(pUart2_TxCallBack cb);



#endif


