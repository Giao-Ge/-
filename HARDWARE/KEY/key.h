#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32F103RC 开发板
//按键初始化，按键分别接在IO口上，wake_up(key1):PA0,key2:PC0, key3:CP13，
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************



#define KEY1   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键1(WK_UP) 
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)//读取按键2
#define KEY3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)//读取按键3

 

#define KEY1_PRES  1	//KEY1按下(即WK_UP/KEY_UP)
#define KEY2_PRES  2	//KEY2按下
#define KEY3_PRES  3	//KEY3按下


void KEY_Init(void);//IO初始化
u8 KEY_Scan(void);  	//按键扫描函数					    
#endif
