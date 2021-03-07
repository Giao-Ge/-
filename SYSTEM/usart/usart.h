#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32F103RC 开发板
//串口1头文件
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************

	  	
void uart1_init(u32 bound);
void uart1_send_buff(u8* buf,u32 len);
void uart1_send_char(u8 temp);


#endif


