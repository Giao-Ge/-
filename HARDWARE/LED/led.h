#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32F103RC 开发板
//LED灯头文件，LED1:PA8，LED2：PC7，LED3：PC6
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************


#define LED1 PAout(8)// PA8
#define LED2 PCout(7)// PC7
#define LED3 PCout(6)// PC6

#define LED1_ON (LED1=0)//打开LED1
#define LED1_OFF (LED1=1)//关闭LED1
#define LED2_ON (LED2=0)//打开LED2
#define LED2_OFF (LED2=1)//关闭LED2
#define LED3_ON (LED3=0)//打开LED3
#define LED3_OFF (LED3=1)//关闭LED3


void LED_Init(void);//初始化
void led1On(int on);
void led2On(int on);
void led3On(int on);


		 				    
#endif
