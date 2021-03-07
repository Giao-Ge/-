#ifndef _BMP180_H_
#define _BMP180_H_

#include "stm32f10x.h"
#include "sys.h" 
#include "BMP180.h"  
#include "delay.h"

//传感器接在PB6和PB7上,如果要修改IO口，修改以下部分
/////////////////////修改开始////////////////////////////////

//IO方向配置
#define SDA_R()  {GPIOB->CRH&=0X0FFFFFFF;GPIOB->CRH|=8<<28;}
#define SDA_W()  {GPIOB->CRH&=0X0FFFFFFF;GPIOB->CRH|=3<<28;}
#define SCL_W()  {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=3<<24;}

//IO操作函数
#define SCL    (PBout(6)) 		//SCL
#define SDA    (PBout(7)) 		//SDA	 


#define SCL_0()         (SCL=0)  
#define SCL_1()         (SCL=1)
#define SDA_0()         (SDA=0) 
#define SDA_1()         (SDA=1) 
#define SDA_IN()           (PBin(7))//读SDA
#define SCL_IN()           (SCL)


/////////////////////修改结束////////////////////////////////


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32 ;

typedef char SB8;
typedef short int SW16 ;
typedef long SL32 ;

#define uint8       unsigned char
#define uint16      unsigned short


//BMP180校正参数(calibration param)
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


//存储传感器数据
typedef struct {
    uint8  ExistFlag ;  //存在标志

    BMP180_cal_param  cal_param;//修正系数
    uint8 Version ;               //版本

    SL32 UnsetTemperature ;     //未校正的温度值  
    SL32 UnsetGasPress    ;     //未校正的气压值  

    long Temperature ;         /*校正后的温度值,0.1度为单位*/  
    long GasPress ;             /*校正后的气压值, pa为单位*/ 

    float Altitude ;                /*海拔，　米为单位*/
}BMP180_info ;

extern BMP180_info Bmp180Info;

void Init_BMP085(void);//初始化传感器
char getBmp180Data(float* T, float* G, float* A);//读取温度，压力，海拔

#endif
