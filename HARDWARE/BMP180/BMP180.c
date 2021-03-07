
//****************************************
#include  <stdlib.h>
#include  <stdio.h>
#include "bmp180.h"
#include "math.h"
#include "delay.h"



#define	BMP085_SlaveAddress   0xee	  //定义器件在IIC总线中的从地址                               

#define OSS 2	// Oversampling Setting (note: code is not set up to use other OSS values)
//id register 
#define BMP180_ID_FIXED_VALUE		0x55 /*id固定编号(0x55)*/							   

   	
BMP180_info Bmp180Info;

uint8 Single_Read(uint8 REG_Address);                                      //单个读取内部寄存器数据
void  Multiple_Read(uint8,uint8);                                          //连续的读取内部寄存器数据
//------------------------------------
void BMP085_Start();
void BMP085_Stop();
void BMP085_SendACK(uint8 ack);
uint8  BMP085_RecvACK();
void BMP085_SendByte(uint8 dat);
uint8 BMP085_RecvByte();
//-----------------------------------

//以ms延时  
void halMcuWaitMs(uint16 msec)  
{  
    while(msec--) delay_us(1000);  
}  

void Delay5us()  
{  
  delay_us(5);  
	delay_us(5);
	delay_us(5);
}  
void delay_10us()  
{  
  delay_us(10);  
}  
void delay_nms(int n)  
{  
  halMcuWaitMs(n);  
}

/**************************************
起始信号
**************************************/
void BMP085_Start()
{
    SDA_W();
    SCL_W();
    SDA_1();                    //拉高数据线
    SCL_1();                    //拉高时钟线
    Delay5us();                 //延时
    SDA_0();                    //产生下降沿
    Delay5us();                 //延时
    SCL_0();                    //拉低时钟线
}

/**************************************
停止信号
**************************************/
void BMP085_Stop()
{
    SDA_W();
    SDA_0();                    //拉低数据线
    SCL_1();                    //拉高时钟线
    Delay5us();                 //延时
    SDA_1();                    //产生上升沿
    Delay5us();                 //延时
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void BMP085_SendACK(uint8 ack)
{
    SDA_W();
		if(ack>0) //写应答信号
			SDA_1();
		else
			SDA_0();
    SCL_1();                    //拉高时钟线
    Delay5us();                 //延时
    SCL_0();                    //拉低时钟线
    Delay5us();                 //延时
}

/**************************************
接收应答信号
**************************************/
uint8 BMP085_RecvACK()
{
    uint8 error=0;
    SDA_R();
    SCL_1();                    //拉高时钟线
    Delay5us();                 //延时
		error = (SDA_IN()>0?1:0);                   //读应答信号
    SCL_0();                    //拉低时钟线
    Delay5us();                 //延时

    return error;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void BMP085_SendByte(uint8 dat)
{
    uint8 i;

    SDA_W();
    for (i=0x80; i>0; i/=2)         //8位计数器
    {
        if(dat&i)
          SDA_1();   //送数据口
        else
          SDA_0();   //送数据口
        Delay5us();             //延时
        SCL_1();                //拉高时钟线
        Delay5us();             //延时
        SCL_0();                //拉低时钟线
        Delay5us();             //延时
    }
    
    SDA_1();
    BMP085_RecvACK();
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
uint8 BMP085_RecvByte()
{
    uint8 i;
    uint8 dat = 0;

    SDA_R();

    SDA_1();                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL_1();                //拉高时钟线
        Delay5us();             //延时
				dat |= (SDA_IN()>0?1:0);             //读数据               
        SCL_0();                  //拉低时钟线
        Delay5us();             //延时
    }
    return dat;
}

//单字节读取BMP085内部数据********************************
uint8 Single_Read(uint8 REG_Address)
{
    uint8 REG_data;
    BMP085_Start();                          //起始信号
    BMP085_SendByte(BMP085_SlaveAddress);           //发送设备地址+写信号
    BMP085_SendByte(REG_Address);            //发送存储单元地址	
    BMP085_Start();                          //起始信号
    BMP085_SendByte(BMP085_SlaveAddress+1);         //发送设备地址+读信号
    REG_data=BMP085_RecvByte();              //读出寄存器数据
    BMP085_SendACK(1);   
    BMP085_Stop();                           //停止信号
    return REG_data; 
}

//*********************************************************
//读出BMP085内部数据,连续两个
//*********************************************************
uint16 Multiple_read(uint8 ST_Address)
{   
    uint8 msb, lsb;
    uint16 _data;
    BMP085_Start();                          //起始信号
    BMP085_SendByte(BMP085_SlaveAddress);    //发送设备地址+写信号
    BMP085_SendByte(ST_Address);             //发送存储单元地址
    BMP085_Start();                          //起始信号
    BMP085_SendByte(BMP085_SlaveAddress+1);         //发送设备地址+读信号
    
    msb = BMP085_RecvByte();                 //BUF[0]存储
    BMP085_SendACK(0);                       //回应ACK
    lsb = BMP085_RecvByte();     
    BMP085_SendACK(1);                       //最后一个数据需要回NOACK
    
    BMP085_Stop();                           //停止信号
    delay_nms(5);
    _data = msb << 8;
    _data |= lsb;	
    return _data;
}
//********************************************************************
long bmp085ReadTemp(void)
{

    BMP085_Start();                  //起始信号
    BMP085_SendByte(BMP085_SlaveAddress);   //发送设备地址+写信号
    BMP085_SendByte(0xF4);	          // write register address
    BMP085_SendByte(0x2E);       	// write register data for temp
    BMP085_Stop();                   //发送停止信号
    delay_nms(5);     // max time is 4.5ms
    
    return (long) Multiple_read(0xF6);
}
//*************************************************************
long bmp085ReadPressure(void)
{
    long pressure = 0;
    
    BMP085_Start();                   //起始信号
    BMP085_SendByte(BMP085_SlaveAddress);   //发送设备地址+写信号
    BMP085_SendByte(0xF4);	          // write register address
    BMP085_SendByte(0x34+(OSS<<6));       	  // write register data for pressure
    BMP085_Stop();                    //发送停止信号
    delay_nms(10);                   // max time is 4.5ms
    
    pressure = Multiple_read(0xF6);
    pressure = (((SL32)pressure <<8) + Single_Read(0xf8)) >>(8-OSS) ;
   
    return pressure;
}

//**************************************************************

//初始化BMP085，根据需要请参考pdf进行修改**************
void IO_INIT(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	                                                                                        
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);    /*先使能外设IO PORTB时钟    */
                                                                                           
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	    /* 端口配置 */                 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	    /*推挽输出*/                       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		    /*IO口速度为50MHz*/              
  GPIO_Init(GPIOB, &GPIO_InitStructure);			    /*根据设定参数初始化GPIO */              
	                                                                                         
  GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);			/*PB6,PB7 输出高*/
}


void Init_BMP085(void)
{
  IO_INIT();
  if(Single_Read(0xd0)==BMP180_ID_FIXED_VALUE)
  {
    Bmp180Info.ExistFlag=1;//传感器存在
    Bmp180Info.cal_param.ac1 = Multiple_read(0xAA);
    Bmp180Info.cal_param.ac2 = Multiple_read(0xAC);
    Bmp180Info.cal_param.ac3 = Multiple_read(0xAE);
    Bmp180Info.cal_param.ac4 = Multiple_read(0xB0);
    Bmp180Info.cal_param.ac5 = Multiple_read(0xB2);
    Bmp180Info.cal_param.ac6 = Multiple_read(0xB4);
    Bmp180Info.cal_param.b1 =  Multiple_read(0xB6);
    Bmp180Info.cal_param.b2 =  Multiple_read(0xB8);
    Bmp180Info.cal_param.mb =  Multiple_read(0xBA);
    Bmp180Info.cal_param.mc =  Multiple_read(0xBC);
    Bmp180Info.cal_param.md =  Multiple_read(0xBE);
    
    Bmp180Info.Version = Single_Read(0xd1); 
  }
  else
  {
    Bmp180Info.ExistFlag=0;//传感器存在    
  }
}

void bmp085Convert(void)
{     
    SL32 x1, x2, B5, B6, x3, B3, p;  
    unsigned long b4, b7;  
  
    //未校正的温度值  
    Bmp180Info.UnsetTemperature = bmp085ReadTemp();  
    //未校正的气压值  
    Bmp180Info.UnsetGasPress = bmp085ReadPressure();  // 读取压强();  

    //打开这个宏，温度为15，气压为69.965,
#if 0

    Bmp180Info.cal_param.ac1 = 408;
    Bmp180Info.cal_param.ac2 = -72;
    Bmp180Info.cal_param.ac3 = -14383;
    Bmp180Info.cal_param.ac4 = 32741;
    Bmp180Info.cal_param.ac5 = 32757;
    Bmp180Info.cal_param.ac6 = 23153;
    Bmp180Info.cal_param.b1 =  6190;
    Bmp180Info.cal_param.b2 =  4;
    Bmp180Info.cal_param.mb =  -32767;
    Bmp180Info.cal_param.mc =  -8711;
    Bmp180Info.cal_param.md =  2868;

    Bmp180Info.UnsetTemperature = 27898;  
    //未校正的气压值  
    Bmp180Info.UnsetGasPress = 23843;  // 读取压强();  

#endif    
    
    
    //温度校正  
    x1 = (((Bmp180Info.UnsetTemperature) - Bmp180Info.cal_param.ac6) * (Bmp180Info.cal_param.ac5)) >> 15;  
    x2 = ((SL32)(Bmp180Info.cal_param.mc) << 11) / (x1 + Bmp180Info.cal_param.md);  
    B5 = x1 + x2;  
    Bmp180Info.Temperature= ((B5 + 8) >> 4);  
  
    
    //气压校正  
    B6 = B5- 4000;  
    x1 = ((SL32)(Bmp180Info.cal_param.b2) * (B6 * B6 >> 12)) >> 11;  
    x2 = ((SL32)Bmp180Info.cal_param.ac2) * B6 >> 11;  
    x3 = x1 + x2;  
    B3 = ((((SL32)(Bmp180Info.cal_param.ac1) * 4 + x3)<<OSS) + 2)/4;  
    x1 = ((SL32)Bmp180Info.cal_param.ac3) * B6 >> 13;  
    x2 = ((SL32)(Bmp180Info.cal_param.b1) * (B6 * B6 >> 12)) >> 16;  
    x3 = ((x1 + x2) + 2) >> 2;  
    b4 = ((SL32)(Bmp180Info.cal_param.ac4) * (unsigned long) (x3 + 32768)) >> 15;  
    b7 = ((unsigned long)(Bmp180Info.UnsetGasPress) - B3) * (50000 >> OSS);  
    if( b7 < 0x80000000)  
    {  
         p = (b7 * 2) / b4 ;  
    }  
    else  
    {  
         p = (b7 / b4) * 2;  
    }  
    x1 = (p >> 8) * (p >> 8);  
    x1 = ((SL32)x1 * 3038) >> 16;  
    x2 = (-7357 * p) >> 16;  
    Bmp180Info.GasPress= p + ((x1 + x2 + 3791) >> 4);
    
    
    //海拔计算  
    Bmp180Info.Altitude =(44330.0 * (1.0-pow((float)(Bmp180Info.GasPress) / 101325.0, 1.0/5.255)) );

}

char getBmp180Data(float* T, float* G, float* A)
{
    if(Bmp180Info.ExistFlag==1)
    {
        bmp085Convert();//取传感器数据
        
        Bmp180Info.Temperature=Bmp180Info.Temperature%1000;
        Bmp180Info.GasPress=Bmp180Info.GasPress%1000000;
    
        *T=((float)Bmp180Info.Temperature/10.0);//转成度
        *G=((float)Bmp180Info.GasPress/1000.0);//转成KPa
        *A=Bmp180Info.Altitude;
    }
    
    return Bmp180Info.ExistFlag;
}
