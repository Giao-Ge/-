
//****************************************
#include  <stdlib.h>
#include  <stdio.h>
#include "bmp180.h"
#include "math.h"
#include "delay.h"



#define	BMP085_SlaveAddress   0xee	  //����������IIC�����еĴӵ�ַ                               

#define OSS 2	// Oversampling Setting (note: code is not set up to use other OSS values)
//id register 
#define BMP180_ID_FIXED_VALUE		0x55 /*id�̶����(0x55)*/							   

   	
BMP180_info Bmp180Info;

uint8 Single_Read(uint8 REG_Address);                                      //������ȡ�ڲ��Ĵ�������
void  Multiple_Read(uint8,uint8);                                          //�����Ķ�ȡ�ڲ��Ĵ�������
//------------------------------------
void BMP085_Start();
void BMP085_Stop();
void BMP085_SendACK(uint8 ack);
uint8  BMP085_RecvACK();
void BMP085_SendByte(uint8 dat);
uint8 BMP085_RecvByte();
//-----------------------------------

//��ms��ʱ  
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
��ʼ�ź�
**************************************/
void BMP085_Start()
{
    SDA_W();
    SCL_W();
    SDA_1();                    //����������
    SCL_1();                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA_0();                    //�����½���
    Delay5us();                 //��ʱ
    SCL_0();                    //����ʱ����
}

/**************************************
ֹͣ�ź�
**************************************/
void BMP085_Stop()
{
    SDA_W();
    SDA_0();                    //����������
    SCL_1();                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA_1();                    //����������
    Delay5us();                 //��ʱ
}

/**************************************
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
**************************************/
void BMP085_SendACK(uint8 ack)
{
    SDA_W();
		if(ack>0) //дӦ���ź�
			SDA_1();
		else
			SDA_0();
    SCL_1();                    //����ʱ����
    Delay5us();                 //��ʱ
    SCL_0();                    //����ʱ����
    Delay5us();                 //��ʱ
}

/**************************************
����Ӧ���ź�
**************************************/
uint8 BMP085_RecvACK()
{
    uint8 error=0;
    SDA_R();
    SCL_1();                    //����ʱ����
    Delay5us();                 //��ʱ
		error = (SDA_IN()>0?1:0);                   //��Ӧ���ź�
    SCL_0();                    //����ʱ����
    Delay5us();                 //��ʱ

    return error;
}

/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
void BMP085_SendByte(uint8 dat)
{
    uint8 i;

    SDA_W();
    for (i=0x80; i>0; i/=2)         //8λ������
    {
        if(dat&i)
          SDA_1();   //�����ݿ�
        else
          SDA_0();   //�����ݿ�
        Delay5us();             //��ʱ
        SCL_1();                //����ʱ����
        Delay5us();             //��ʱ
        SCL_0();                //����ʱ����
        Delay5us();             //��ʱ
    }
    
    SDA_1();
    BMP085_RecvACK();
}

/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
uint8 BMP085_RecvByte()
{
    uint8 i;
    uint8 dat = 0;

    SDA_R();

    SDA_1();                    //ʹ���ڲ�����,׼����ȡ����,
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;
        SCL_1();                //����ʱ����
        Delay5us();             //��ʱ
				dat |= (SDA_IN()>0?1:0);             //������               
        SCL_0();                  //����ʱ����
        Delay5us();             //��ʱ
    }
    return dat;
}

//���ֽڶ�ȡBMP085�ڲ�����********************************
uint8 Single_Read(uint8 REG_Address)
{
    uint8 REG_data;
    BMP085_Start();                          //��ʼ�ź�
    BMP085_SendByte(BMP085_SlaveAddress);           //�����豸��ַ+д�ź�
    BMP085_SendByte(REG_Address);            //���ʹ洢��Ԫ��ַ	
    BMP085_Start();                          //��ʼ�ź�
    BMP085_SendByte(BMP085_SlaveAddress+1);         //�����豸��ַ+���ź�
    REG_data=BMP085_RecvByte();              //�����Ĵ�������
    BMP085_SendACK(1);   
    BMP085_Stop();                           //ֹͣ�ź�
    return REG_data; 
}

//*********************************************************
//����BMP085�ڲ�����,��������
//*********************************************************
uint16 Multiple_read(uint8 ST_Address)
{   
    uint8 msb, lsb;
    uint16 _data;
    BMP085_Start();                          //��ʼ�ź�
    BMP085_SendByte(BMP085_SlaveAddress);    //�����豸��ַ+д�ź�
    BMP085_SendByte(ST_Address);             //���ʹ洢��Ԫ��ַ
    BMP085_Start();                          //��ʼ�ź�
    BMP085_SendByte(BMP085_SlaveAddress+1);         //�����豸��ַ+���ź�
    
    msb = BMP085_RecvByte();                 //BUF[0]�洢
    BMP085_SendACK(0);                       //��ӦACK
    lsb = BMP085_RecvByte();     
    BMP085_SendACK(1);                       //���һ��������Ҫ��NOACK
    
    BMP085_Stop();                           //ֹͣ�ź�
    delay_nms(5);
    _data = msb << 8;
    _data |= lsb;	
    return _data;
}
//********************************************************************
long bmp085ReadTemp(void)
{

    BMP085_Start();                  //��ʼ�ź�
    BMP085_SendByte(BMP085_SlaveAddress);   //�����豸��ַ+д�ź�
    BMP085_SendByte(0xF4);	          // write register address
    BMP085_SendByte(0x2E);       	// write register data for temp
    BMP085_Stop();                   //����ֹͣ�ź�
    delay_nms(5);     // max time is 4.5ms
    
    return (long) Multiple_read(0xF6);
}
//*************************************************************
long bmp085ReadPressure(void)
{
    long pressure = 0;
    
    BMP085_Start();                   //��ʼ�ź�
    BMP085_SendByte(BMP085_SlaveAddress);   //�����豸��ַ+д�ź�
    BMP085_SendByte(0xF4);	          // write register address
    BMP085_SendByte(0x34+(OSS<<6));       	  // write register data for pressure
    BMP085_Stop();                    //����ֹͣ�ź�
    delay_nms(10);                   // max time is 4.5ms
    
    pressure = Multiple_read(0xF6);
    pressure = (((SL32)pressure <<8) + Single_Read(0xf8)) >>(8-OSS) ;
   
    return pressure;
}

//**************************************************************

//��ʼ��BMP085��������Ҫ��ο�pdf�����޸�**************
void IO_INIT(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	                                                                                        
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);    /*��ʹ������IO PORTBʱ��    */
                                                                                           
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	    /* �˿����� */                 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	    /*�������*/                       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		    /*IO���ٶ�Ϊ50MHz*/              
  GPIO_Init(GPIOB, &GPIO_InitStructure);			    /*�����趨������ʼ��GPIO */              
	                                                                                         
  GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);			/*PB6,PB7 �����*/
}


void Init_BMP085(void)
{
  IO_INIT();
  if(Single_Read(0xd0)==BMP180_ID_FIXED_VALUE)
  {
    Bmp180Info.ExistFlag=1;//����������
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
    Bmp180Info.ExistFlag=0;//����������    
  }
}

void bmp085Convert(void)
{     
    SL32 x1, x2, B5, B6, x3, B3, p;  
    unsigned long b4, b7;  
  
    //δУ�����¶�ֵ  
    Bmp180Info.UnsetTemperature = bmp085ReadTemp();  
    //δУ������ѹֵ  
    Bmp180Info.UnsetGasPress = bmp085ReadPressure();  // ��ȡѹǿ();  

    //������꣬�¶�Ϊ15����ѹΪ69.965,
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
    //δУ������ѹֵ  
    Bmp180Info.UnsetGasPress = 23843;  // ��ȡѹǿ();  

#endif    
    
    
    //�¶�У��  
    x1 = (((Bmp180Info.UnsetTemperature) - Bmp180Info.cal_param.ac6) * (Bmp180Info.cal_param.ac5)) >> 15;  
    x2 = ((SL32)(Bmp180Info.cal_param.mc) << 11) / (x1 + Bmp180Info.cal_param.md);  
    B5 = x1 + x2;  
    Bmp180Info.Temperature= ((B5 + 8) >> 4);  
  
    
    //��ѹУ��  
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
    
    
    //���μ���  
    Bmp180Info.Altitude =(44330.0 * (1.0-pow((float)(Bmp180Info.GasPress) / 101325.0, 1.0/5.255)) );

}

char getBmp180Data(float* T, float* G, float* A)
{
    if(Bmp180Info.ExistFlag==1)
    {
        bmp085Convert();//ȡ����������
        
        Bmp180Info.Temperature=Bmp180Info.Temperature%1000;
        Bmp180Info.GasPress=Bmp180Info.GasPress%1000000;
    
        *T=((float)Bmp180Info.Temperature/10.0);//ת�ɶ�
        *G=((float)Bmp180Info.GasPress/1000.0);//ת��KPa
        *A=Bmp180Info.Altitude;
    }
    
    return Bmp180Info.ExistFlag;
}
