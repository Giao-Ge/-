#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h" 
#include "buzzer.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//STM32F103RC ������
//������ʼ���������ֱ����IO���ϣ�wake_up(key1):PA0,key2:PB0, key3:PC5��
//����:2021/3/6
//�汾��V1.0
//********************************************************************************



//���ܣ�������ʼ������
//��������
//���أ���
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PORTA,PORTB,PORTCʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//KEY2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB0

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;//KEY3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC5

	//��ʼ�� WK_UP-->GPIOA.0	  ��������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0

}


//����������
//���ذ���ֵ
//0��û���κΰ�������
//1��KEY1���¡�WK_UP
//2��KEY2����
//3��KEY3���� 
//ע��˺�������Ӧ���ȼ�,KEY1>KEY2>KEY3!!
u8 KEY_Scan(void)
{	 
	if(KEY1==1||KEY2==0 || KEY3==0)
	{
		delay_ms(50);//ȥ���� 

		if(KEY1==1)
		{
      while(KEY1==1)
      {
        delay_ms(10);
      }
			Buzzer_flash();
      return KEY1_PRES;//�����ɿ��󣬷��ؼ�ֵ
		}
		else if(KEY2==0)
		{
      while(KEY2==0)
      {
        delay_ms(10);
      }
			Buzzer_flash();
      return KEY2_PRES;//�����ɿ��󣬷��ؼ�ֵ
		}
		else if(KEY3==0)
		{
      while(KEY3==0)
      {
        delay_ms(10);
      }
			Buzzer_flash();
      return KEY3_PRES;//�����ɿ��󣬷��ؼ�ֵ
		}
	}
  
 	return 0;// �ް�������
}
