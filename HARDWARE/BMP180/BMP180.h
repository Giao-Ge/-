#ifndef _BMP180_H_
#define _BMP180_H_

#include "stm32f10x.h"
#include "sys.h" 
#include "BMP180.h"  
#include "delay.h"

//����������PB6��PB7��,���Ҫ�޸�IO�ڣ��޸����²���
/////////////////////�޸Ŀ�ʼ////////////////////////////////

//IO��������
#define SDA_R()  {GPIOB->CRH&=0X0FFFFFFF;GPIOB->CRH|=8<<28;}
#define SDA_W()  {GPIOB->CRH&=0X0FFFFFFF;GPIOB->CRH|=3<<28;}
#define SCL_W()  {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=3<<24;}

//IO��������
#define SCL    (PBout(6)) 		//SCL
#define SDA    (PBout(7)) 		//SDA	 


#define SCL_0()         (SCL=0)  
#define SCL_1()         (SCL=1)
#define SDA_0()         (SDA=0) 
#define SDA_1()         (SDA=1) 
#define SDA_IN()           (PBin(7))//��SDA
#define SCL_IN()           (SCL)


/////////////////////�޸Ľ���////////////////////////////////


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32 ;

typedef char SB8;
typedef short int SW16 ;
typedef long SL32 ;

#define uint8       unsigned char
#define uint16      unsigned short


//BMP180У������(calibration param)
typedef struct {
    SW16 ac1 ;
    SW16 ac2 ;
    SW16 ac3 ;
    uint16 ac4 ;
    uint16 ac5 ;
    uint16 ac6 ;
    SW16 b1 ;
    SW16 b2 ;
    SW16 mb ;
    SW16 mc ;
    SW16 md ;
}BMP180_cal_param;


//�洢����������
typedef struct {
    uint8  ExistFlag ;  //���ڱ�־

    BMP180_cal_param  cal_param;//����ϵ��
    uint8 Version ;               //�汾

    SL32 UnsetTemperature ;     //δУ�����¶�ֵ  
    SL32 UnsetGasPress    ;     //δУ������ѹֵ  

    long Temperature ;         /*У������¶�ֵ,0.1��Ϊ��λ*/  
    long GasPress ;             /*У�������ѹֵ, paΪ��λ*/ 

    float Altitude ;                /*���Σ�����Ϊ��λ*/
}BMP180_info ;

extern BMP180_info Bmp180Info;

void Init_BMP085(void);//��ʼ��������
char getBmp180Data(float* T, float* G, float* A);//��ȡ�¶ȣ�ѹ��������

#endif
