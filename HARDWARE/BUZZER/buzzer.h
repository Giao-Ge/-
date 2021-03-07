#ifndef __BUZZER_H
#define __BUZZER_H	 
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32F103RC 开发板
//蜂鸣器控制　使用PB1控制，低电平触发
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************


#define BUZZER PBout(1)// PB1	

#define BUZZER_ON (BUZZER=0)//打开
#define BUZZER_OFF (BUZZER=1)//关闭


void Buzzer_Init(void);//初始化
void Buzzer_on(int on);
void Buzzer_flash(void);

		 				    
#endif
