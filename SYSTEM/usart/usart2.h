#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "sys.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//STM32F103RC ������
//����2ͷ�ļ�
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************


#define	MAX485_TX 		1//����
#define	MAX485_RX 		0//����
#define	MAX485_RX_TX 		PBout(1)  		//MAX485���ͽ�������

typedef void (*pUart2_TxCallBack)(u8 ch);

	  	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart2_init(u32 bound);
void uart2_485_init(void);
void USART2_send_buff(u8* buf,u32 len);

void USART2_SetCallback(pUart2_TxCallBack cb);



#endif


