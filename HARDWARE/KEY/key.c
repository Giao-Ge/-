#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h" 
#include "buzzer.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//STM32F103RC 开发板
//按键初始化，按键分别接在IO口上，wake_up(key1):PA0,key2:PB0, key3:PC5，
//日期:2021/3/6
//版本：V1.0
//********************************************************************************



//功能：按键初始化函数
//参数：无
//返回：无
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);//使能PORTA,PORTB,PORTC时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//KEY2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB0

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;//KEY3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC5

	//初始化 WK_UP-->GPIOA.0	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0

}


//按键处理函数
//返回按键值
//0，没有任何按键按下
//1，KEY1按下　WK_UP
//2，KEY2按下
//3，KEY3按下 
//注意此函数有响应优先级,KEY1>KEY2>KEY3!!
u8 KEY_Scan(void)
{	 
	if(KEY1==1||KEY2==0 || KEY3==0)
	{
		delay_ms(50);//去抖动 

		if(KEY1==1)
		{
      while(KEY1==1)
      {
        delay_ms(10);
      }
			Buzzer_flash();
      return KEY1_PRES;//按键松开后，返回键值
		}
		else if(KEY2==0)
		{
      while(KEY2==0)
      {
        delay_ms(10);
      }
			Buzzer_flash();
      return KEY2_PRES;//按键松开后，返回键值
		}
		else if(KEY3==0)
		{
      while(KEY3==0)
      {
        delay_ms(10);
      }
			Buzzer_flash();
      return KEY3_PRES;//按键松开后，返回键值
		}
	}
  
 	return 0;// 无按键按下
}
