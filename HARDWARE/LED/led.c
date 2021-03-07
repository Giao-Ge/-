#include "led.h"

//////////////////////////////////////////////////////////////////////////////////	 
//STM32F103RC ������
//LED�Ƴ�ʼ����LED1:PA8��LED2��PC7��LED3��PC6	   
//����:2021/3/6
//�汾��V1.0
//********************************************************************************


//��ʼ��PA8��PC6��PC7Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��GPIOA,GPIOC�˿�ʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED1-->PA8 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA8
  GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PA8�����

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 //LED2-->PC7 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC7
  GPIO_SetBits(GPIOC,GPIO_Pin_7);						 //PD2�����


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;        //LED3-->PC6 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);           //�����趨������ʼ��GPIOC6
  GPIO_SetBits(GPIOC,GPIO_Pin_6);            //PB12�����

}

void led1On(int on)
{
  if(on>0)
  {
    LED1=0;
  }
  else
  {
    LED1=1;
  }
}

void led2On(int on)
{
  if(on>0)
  {
    LED2=0;
  }
  else
  {
    LED2=1;
  }
}

void led3On(int on)
{
  if(on>0)
  {
    LED3=0;
  }
  else
  {
    LED3=1;
  }
}

 
