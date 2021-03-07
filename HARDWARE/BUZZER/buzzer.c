#include "buzzer.h"
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////	 
//STM32F103RC ������
//���������ơ�ʹ��PB1���ƣ��͵�ƽ����
//����:2021/3/6
//�汾��V1.0
//********************************************************************************


//���ܣ���������ʼ��
//������on��1���졡0����
//���أ���
void Buzzer_Init(void)
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;
  		

  //buzzer
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB1�˿�ʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //PC1 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOC1
  GPIO_SetBits(GPIOB,GPIO_Pin_1);						 //PC1�����
}

//���ܣ����Ʒ�����
//������on��1���졡0����
//���أ���
void Buzzer_on(int on){
  if(on>0) BUZZER=0;
  else BUZZER=1;
}

//���ܣ����Ʒ�������һ��
//��������
//���أ���
void Buzzer_flash(void){

  Buzzer_on(1);
  delay_ms(100);
  Buzzer_on(0);
}


 
