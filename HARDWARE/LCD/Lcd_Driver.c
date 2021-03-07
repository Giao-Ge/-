#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////
//STM32F103RC 开发板
//LCD显示屏初始化，SDI(D0)：PC13，SCL(D1)：PC0，RESET：PC1，DC：PB8，BLK：PC8
//LCD型号：ST7735S
//日期:2021/3/6
//版本：V1.0
//********************************************************************************



//本测试程序使用的是模拟SPI接口驱动
//可自由更改接口IO配置，使用任意最少4 IO即可完成本款液晶驱动显示
/******************************************************************************
接口定义在Lcd_Driver.h内定义，请根据接线修改并修改相应IO初始化LCD_GPIO_Init()
*******************************************************************************/

//是否启动LCD缓冲的宏定义，如果启动，会占用32K的内容，我们STM32一共是48K内存
//#define LCD_BUFFER


#ifdef LCD_BUFFER
unsigned short g_LcdBuff[Y_MAX_PIXEL][X_MAX_PIXEL]={0};
#endif

//16*16一行数据的缓冲
unsigned short g_LcdBuffLine16[16][X_MAX_PIXEL]={0};


//液晶IO初始化配置
void LCD_GPIO_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB ,ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}
//向SPI总线传输一个8位数据
void  SPI_WriteData(u8 Data)
{
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
		if(Data&0x80)
	  LCD_SDA_SET; //输出数据
      else LCD_SDA_CLR;

      LCD_SCL_CLR;
      LCD_SCL_SET;
      Data<<=1;
	}
}

//向液晶屏写一个8位指令
void Lcd_WriteIndex(u8 Index)
{
   //SPI 写命令时序开始
   LCD_RS_CLR;
	 SPI_WriteData(Index);
}

//向液晶屏写一个8位数据
void Lcd_WriteData(u8 Data)
{
   LCD_RS_SET;
   SPI_WriteData(Data);
}
//向液晶屏写一个16位数据
void LCD_WriteData_16Bit(u16 Data)
{
   LCD_RS_SET;
	 SPI_WriteData(Data>>8); 	//写入高8位数据
	 SPI_WriteData(Data); 			//写入低8位数据
}

void Lcd_WriteReg(u8 Index,u8 Data)
{
	Lcd_WriteIndex(Index);
  Lcd_WriteData(Data);
}

void Lcd_Reset(void)
{
	LCD_RST_CLR;
	delay_ms(100);
	LCD_RST_SET;
	delay_ms(50);
}

//LCD ST7735R初始化
void Lcd_Init(void)
{
	LCD_GPIO_Init();//引脚初始化
	Lcd_Reset(); //复位 LCD
	Lcd_Blk(1);//打开背光
	//LCD Init For 1.44Inch LCD Panel with ST7735R.
	Lcd_WriteIndex(0x11);//Sleep exit
	delay_ms (120);

	//ST7735R Frame Rate
	Lcd_WriteIndex(0xB1);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x2D);

	Lcd_WriteIndex(0xB2);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x2D);

	Lcd_WriteIndex(0xB3);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x2D);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x2D);

	Lcd_WriteIndex(0xB4); //Column inversion
	Lcd_WriteData(0x07);

	//ST7735R Power Sequence
	Lcd_WriteIndex(0xC0);
	Lcd_WriteData(0xA2);
	Lcd_WriteData(0x02);
	Lcd_WriteData(0x84);
	Lcd_WriteIndex(0xC1);
	Lcd_WriteData(0xC5);

	Lcd_WriteIndex(0xC2);
	Lcd_WriteData(0x0A);
	Lcd_WriteData(0x00);

	Lcd_WriteIndex(0xC3);
	Lcd_WriteData(0x8A);
	Lcd_WriteData(0x2A);
	Lcd_WriteIndex(0xC4);
	Lcd_WriteData(0x8A);
	Lcd_WriteData(0xEE);

	Lcd_WriteIndex(0xC5); //VCOM
	Lcd_WriteData(0x0E);

	Lcd_WriteIndex(0x36); //MX, MY, RGB mode
	Lcd_WriteData(0xC8);

	//ST7735R Gamma Sequence
	Lcd_WriteIndex(0xe0);
	Lcd_WriteData(0x0f);
	Lcd_WriteData(0x1a);
	Lcd_WriteData(0x0f);
	Lcd_WriteData(0x18);
	Lcd_WriteData(0x2f);
	Lcd_WriteData(0x28);
	Lcd_WriteData(0x20);
	Lcd_WriteData(0x22);
	Lcd_WriteData(0x1f);
	Lcd_WriteData(0x1b);
	Lcd_WriteData(0x23);
	Lcd_WriteData(0x37);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x07);
	Lcd_WriteData(0x02);
	Lcd_WriteData(0x10);

	Lcd_WriteIndex(0xe1);
	Lcd_WriteData(0x0f);
	Lcd_WriteData(0x1b);
	Lcd_WriteData(0x0f);
	Lcd_WriteData(0x17);
	Lcd_WriteData(0x33);
	Lcd_WriteData(0x2c);
	Lcd_WriteData(0x29);
	Lcd_WriteData(0x2e);
	Lcd_WriteData(0x30);
	Lcd_WriteData(0x30);
	Lcd_WriteData(0x39);
	Lcd_WriteData(0x3f);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x07);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0x10);

	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x7f);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x9f);

	Lcd_WriteIndex(0xF0); //Enable test command
	Lcd_WriteData(0x01);
	Lcd_WriteIndex(0xF6); //Disable ram power save mode
	Lcd_WriteData(0x00);

	Lcd_WriteIndex(0x3A); //65k mode
	Lcd_WriteData(0x05);


	Lcd_WriteIndex(0x29);//Display on
	Lcd_Clear(WHITE);
  Lcd_Updata();
}


/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end)
{
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start+2);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end+2);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start+2);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end+2);

	Lcd_WriteIndex(0x2c);

}

/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(u16 x,u16 y)
{
  	Lcd_SetRegion(x,y,x,y);
}


/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：无
返回值：无
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
  if(x>=X_MAX_PIXEL) return;
  if(y>=Y_MAX_PIXEL) return;

#ifdef LCD_BUFFER

  g_LcdBuff[y][x]=Data;
#else
  Lcd_SetRegion(x,y,x+1,y+1);
  LCD_WriteData_16Bit(Data);
#endif
}

/*************************************************
函数名：LCD_DrawPoint
功能：写一行数据的一个点
入口参数：无
返回值：无
*************************************************/
void DrawPointToBuff(u16 x,u16 y,u16 Data)
{
    if(x>=X_MAX_PIXEL) return;
    if(y>=16) return;

    g_LcdBuffLine16[y][x]=Data;
}

/*****************************************
 函数功能：读TFT某一点的颜色
 出口参数：color  点颜色值
******************************************/
unsigned int Lcd_ReadPoint(u16 x,u16 y)
{
  unsigned int Data;
  Lcd_SetXY(x,y);

  //Lcd_ReadData();//丢掉无用字节
  //Data=Lcd_ReadData();
  Lcd_WriteData(Data);
  return Data;
}
/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Clear(u16 Color)
{
#ifdef LCD_BUFFER
    unsigned int x,y;
    for(y=0;y<Y_MAX_PIXEL;y++)
    for(x=0;x<X_MAX_PIXEL;x++)
    {
        g_LcdBuff[y][x]=Color;
    }
#else
    unsigned int i,m;
    Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
    Lcd_WriteIndex(0x2C);
    for(i=0;i<X_MAX_PIXEL;i++)
    for(m=0;m<Y_MAX_PIXEL;m++)
    {
      LCD_WriteData_16Bit(Color);
    }
#endif
}


//把缓冲中的数据刷新到显示屏
void Lcd_Updata(void)
{
#ifdef LCD_BUFFER

    unsigned int x,y;
    Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
    Lcd_WriteIndex(0x2C);
    for(y=0;y<Y_MAX_PIXEL;y++)
    for(x=0;x<X_MAX_PIXEL;x++)
    {
        LCD_WriteData_16Bit(g_LcdBuff[y][x]);
    }
#endif
}

//把缓冲中的一行数据刷新到显示屏
//这一行数据是16*16的，所以X坐标是从0~宽度，即128
void Lcd_Updata_line(u16 y_start,u16 y_end)
{
    unsigned int x,y;

    if(y_start>Y_MAX_PIXEL || y_end>Y_MAX_PIXEL) return;

    Lcd_SetRegion(0,y_start,X_MAX_PIXEL-1,y_end);
    Lcd_WriteIndex(0x2C);

    //真正刷数据到显示屏
    for(y=0;y<16;y++)
    {
      for(x=0;x<X_MAX_PIXEL;x++)
      {
        LCD_WriteData_16Bit(g_LcdBuffLine16[y][x]);
      }
    }
}

/*************************************************
函数名：Lcd_Clear
功能：部分清除
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_ClearRect(u16 x1,u16 y1, u16 x2,u16 y2, u16 Color)
{
   unsigned int i,m;
   Lcd_SetRegion(x1,y1, x2,y2);
   Lcd_WriteIndex(0x2C);
   for(i=x1;i<x2;i++)
   {
    for(m=y1;m<y2;m++)
    {
	  	LCD_WriteData_16Bit(Color);
    }
   }
}

//背光控制，on等于1表示打开背光
void Lcd_Blk(u8 on)
{
	if(on>0)
	{
		LCD_LED_SET;
	}
	else
	{
		LCD_LED_CLR;
	}
}
