#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//STM32F103RC ������
//LED��ͷ�ļ���LED1:PA8��LED2��PC7��LED3��PC6
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************


#define LED1 PAout(8)// PA8
#define LED2 PCout(7)// PC7
#define LED3 PCout(6)// PC6

#define LED1_ON (LED1=0)//��LED1
#define LED1_OFF (LED1=1)//�ر�LED1
#define LED2_ON (LED2=0)//��LED2
#define LED2_OFF (LED2=1)//�ر�LED2
#define LED3_ON (LED3=0)//��LED3
#define LED3_OFF (LED3=1)//�ر�LED3


void LED_Init(void);//��ʼ��
void led1On(int on);
void led2On(int on);
void led3On(int on);


		 				    
#endif
