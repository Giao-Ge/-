#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
#include "w25qxx.h"

//////////////////////////////////////////////////////////////////////////////////	 
//STM32F103RC ������
//LCD��ʾ����ʾ�����ӿڶ���
//LCD�ͺţ�ST7735S
//����:2021/3/6
//�汾��V1.0
//********************************************************************************



void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc) 
{//Bresenham�㷨 
    unsigned short  a,b; 
    int c; 
    a=0; 
    b=R; 
    c=3-2*R; 
    while (a<b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc);     //        7 
        Gui_DrawPoint(X-a,Y+b,fc);     //        6 
        Gui_DrawPoint(X+a,Y-b,fc);     //        2 
        Gui_DrawPoint(X-a,Y-b,fc);     //        3 
        Gui_DrawPoint(X+b,Y+a,fc);     //        8 
        Gui_DrawPoint(X-b,Y+a,fc);     //        5 
        Gui_DrawPoint(X+b,Y-a,fc);     //        1 
        Gui_DrawPoint(X-b,Y-a,fc);     //        4 

        if(c<0) c=c+4*a+6; 
        else 
        { 
            c=c+4*(a-b)+10; 
            b-=1; 
        } 
       a+=1; 
    } 
    if (a==b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y-b,fc); 
        Gui_DrawPoint(X-a,Y-b,fc); 
        Gui_DrawPoint(X+b,Y+a,fc); 
        Gui_DrawPoint(X-b,Y+a,fc); 
        Gui_DrawPoint(X+b,Y-a,fc); 
        Gui_DrawPoint(X-b,Y-a,fc); 
    } 
	
} 
//���ߺ�����ʹ��Bresenham �����㷨
void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color)   
{
int dx,             // difference in x's
    dy,             // difference in y's
    dx2,            // dx,dy * 2
    dy2, 
    x_inc,          // amount in pixel space to move during drawing
    y_inc,          // amount in pixel space to move during drawing
    error,          // the discriminant i.e. error i.e. decision variable
    index;          // used for looping	


	Lcd_SetXY(x0,y0);
	dx = x1-x0;//����x����
	dy = y1-y0;//����y����

	if (dx>=0)
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
		dx    = -dx;  
	} 
	
	if (dy>=0)
	{
		y_inc = 1;
	} 
	else
	{
		y_inc = -1;
		dy    = -dy; 
	} 

	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)//x�������y���룬��ôÿ��x����ֻ��һ���㣬ÿ��y���������ɸ���
	{//���ߵĵ�������x���룬��x���������
		// initialize error term
		error = dy2 - dx; 

		// draw the line
		for (index=0; index <= dx; index++)//Ҫ���ĵ������ᳬ��x����
		{
			//����
			Gui_DrawPoint(x0,y0,Color);
			
			// test if error has overflowed
			if (error >= 0) //�Ƿ���Ҫ����y����ֵ
			{
				error-=dx2;

				// move to next line
				y0+=y_inc;//����y����ֵ
			} // end if error overflowed

			// adjust the error term
			error+=dy2;

			// move to the next pixel
			x0+=x_inc;//x����ֵÿ�λ���󶼵���1
		} // end for
	} // end if |slope| <= 1
	else//y�����x�ᣬ��ÿ��y����ֻ��һ���㣬x�����ɸ���
	{//��y��Ϊ��������
		// initialize error term
		error = dx2 - dy; 

		// draw the line
		for (index=0; index <= dy; index++)
		{
			// set the pixel
			Gui_DrawPoint(x0,y0,Color);

			// test if error overflowed
			if (error >= 0)
			{
				error-=dy2;

				// move to next line
				x0+=x_inc;
			} // end if error overflowed

			// adjust the error term
			error+=dx2;

			// move to the next pixel
			y0+=y_inc;
		} // end for
	} // end else |slope| > 1
}



void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc)
{
	Gui_DrawLine(x,y,x+w,y,0xEF7D);
	Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
	Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
	Gui_DrawLine(x,y,x,y+h,0xEF7D);
    Gui_DrawLine(x+1,y+1,x+1+w-2,y+1+h-2,bc);
}
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode)
{
	if (mode==0)	{
		Gui_DrawLine(x,y,x+w,y,0xEF7D);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
		Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
		Gui_DrawLine(x,y,x,y+h,0xEF7D);
		}
	if (mode==1)	{
		Gui_DrawLine(x,y,x+w,y,0x2965);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xEF7D);
		Gui_DrawLine(x,y+h,x+w,y+h,0xEF7D);
		Gui_DrawLine(x,y,x,y+h,0x2965);
	}
	if (mode==2)	{
		Gui_DrawLine(x,y,x+w,y,0xffff);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xffff);
		Gui_DrawLine(x,y+h,x+w,y+h,0xffff);
		Gui_DrawLine(x,y,x,y+h,0xffff);
	}
}


/**************************************************************************************
��������: ����Ļ��ʾһ͹��İ�ť��
��    ��: u16 x1,y1,x2,y2 ��ť�����ϽǺ����½�����
��    ��: ��
**************************************************************************************/
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2)
{
	Gui_DrawLine(x1,  y1,  x2,y1, GRAY2);  //H
	Gui_DrawLine(x1+1,y1+1,x2,y1+1, GRAY1);  //H
	Gui_DrawLine(x1,  y1,  x1,y2, GRAY2);  //V
	Gui_DrawLine(x1+1,y1+1,x1+1,y2, GRAY1);  //V
	Gui_DrawLine(x1,  y2,  x2,y2, WHITE);  //H
	Gui_DrawLine(x2,  y1,  x2,y2, WHITE);  //V
}

/**************************************************************************************
��������: ����Ļ��ʾһ���µİ�ť��
��    ��: u16 x1,y1,x2,y2 ��ť�����ϽǺ����½�����
��    ��: ��
**************************************************************************************/
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2)
{
  Gui_DrawLine(x1,  y1,  x2,y1, WHITE); //H
  Gui_DrawLine(x1,  y1,  x1,y2, WHITE); //V

  Gui_DrawLine(x1+1,y2-1,x2,y2-1, GRAY1);  //H
  Gui_DrawLine(x1,  y2,  x2,y2, GRAY2);  //H
  Gui_DrawLine(x2-1,y1+1,x2-1,y2, GRAY1);  //V
  Gui_DrawLine(x2  ,y1  ,x2,y2, GRAY2); //V
}


//���һ��16*16����
//x:0~128
//y:0~128
//fc:������ɫ
//bc:������ɫ
void DrawFont_GB2312_16(u8 x, u8 y, u8 *lpChinese, u16 fc, u16 bc)
{
	u8 iColDetail = 0;
	u8 iMask = 0;
	u32 iOffset = 0;
  u8 fontInfo[32]={0};

  //�ֵĴ�С
	u8 iWidth = 16;
	u8 iHight = 16;

  int i,j,k;

	if( x >= 128 || y >= 128 ) return;


	if(((u8)lpChinese[0])<0xa1 || ((u8)lpChinese[1])<0xa0)
	{
    return;
	}
	else
	{
		iOffset = (((u8)lpChinese[0] - 0xa1) * 96 + (u8)lpChinese[1] - 0xa0 ) * 32;//һ������ռ32���ֽ�,96=0xff-0xa0+1
	}

  //ȡ��������Ϣ
  W25QXX_Read((unsigned char*)fontInfo,ADDR_DISPLAY_GB2312_16_16+iOffset,32);
  
	for (i = 0; i < iHight ; i++)
	{
	  //2���ֽڣ�������
    for(j=0; j<2; j++)
    {
      iColDetail = fontInfo[i*2+j];
      iMask = 0x80;
      for(k=0; k<8; k++)
      {
        if((iColDetail & iMask)>0)
        {
          Gui_DrawPoint(x+j*8+k,y+i,fc);
        }
        else
        {
          Gui_DrawPoint(x+j*8+k,y+i,bc);
        }
        iMask = iMask/2;
      }
    }
   }
}

//���һ��8*16��ASCLL
//x:0~128
//y:0~128
//fc:������ɫ
//bc:������ɫ
void DrawFont_ASCLL_16(u8 x, u8 y, u8 *lpAsc, u16 fc, u16 bc)
{

	u32 iColDetail = 0;
	u32 iMask = 0;
	u32 iOffset = ((*lpAsc)-32) * 16;//ÿ��ascռ16�ֽ�
  u8 fontInfo[16]={0};

  //�ֵĴ�С
	u8 iWidth = 8;
	u8 iHight = 16;

  int i,j,k;
  
	if( x >= 128 || y >= 128 ) return;


  //ȡ��������Ϣ
  W25QXX_Read((unsigned char*)fontInfo,ADDR_DISPLAY_ASCLL_8_16+iOffset,16);
  
	for (i = 0; i < iHight ; i++)
	{
		iColDetail = (u8)fontInfo[i];
		iMask = 1<<7;

		for(j = 0; j < iWidth; j++)
		{
      if((iColDetail & iMask)>0)
      {
        Gui_DrawPoint(x+j, y+i,fc);
      }
      else
      {
        Gui_DrawPoint(x+j, y+i,bc);
      }
      iMask = iMask/2;
		}
	}
}

//��ʾһ�к��ֺ�Ӣ�Ļ�ϵ���
//x:0~128
//y:0~128
//fc:������ɫ
//bc:������ɫ
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
	unsigned char i,j;
	unsigned short k,x0;
	x0=x;

	while(*s) 
	{	
		if((*s) < 128) 
		{
		
		  DrawFont_ASCLL_16(x, y, s, fc, bc);
      x+=8;
			s++;
		}
			
		else 
		{
      DrawFont_GB2312_16(x, y, s, fc, bc);
			s+=2;x+=16;
		} 
		
	}
}


//���һ��6*8��ASCLL
//x:0~128
//y:0~128
//fc:������ɫ
//bc:������ɫ
//ch:Ҫ��ʾ����
void LCD_P6x8Str(unsigned char x, unsigned char y, u16 fc, u16 bc,unsigned char ch[])
{
    u32 iOffset = 0;
    u8 fontInfo[6]={0};
    unsigned char c=0,idx=0,i=0,j=0,k=0;      
    while (ch[j]!='\0')
    {    
        idx =ch[j]-32;
        iOffset = idx * 6;//ÿ��ascռ6�ֽ�
        W25QXX_Read((unsigned char*)fontInfo,ADDR_DISPLAY_ASCLL_6_8+iOffset,6);
        
        for(i=0;i<6;i++)     
        {
            c=fontInfo[i];
            for(k=0; k<8; k++)
            {
              if((c&0x01)==1)
              {
                Gui_DrawPoint(x+i,y+k,fc);
              }
              else
              {
                Gui_DrawPoint(x+i,y+k,bc);
              }
              c>>=1;
            }
        }
        x+=6;
        j++;
    }
}


//���һ��16*16���ֵ��л�����
//x:0~128
//y:0~128
//fc:������ɫ
//bc:������ɫ
void DrawFont_GB2312_16_line(u8 x, u8 *lpChinese, u16 fc, u16 bc)
{
	u8 iColDetail = 0;
	u8 iMask = 0;
	u32 iOffset = 0;
  u8 fontInfo[32]={0};

  //�ֵĴ�С
	u8 iWidth = 16;
	u8 iHight = 16;

  int i,j,k;

	if( x >= 128 ) return;

	if(((u8)lpChinese[0])<0xa1 || ((u8)lpChinese[1])<0xa0)
	{
		iOffset = ((0xd7 - 0xa1) * 96 + 0xfb - 0xa0 ) * 32;
	}
	else
	{
		iOffset = (((u8)lpChinese[0] - 0xa1) * 96 + (u8)lpChinese[1] - 0xa0 ) * 32;//һ������ռ32���ֽ�,96=0xff-0xa0+1
	}

  //ȡ��������Ϣ
  W25QXX_Read((unsigned char*)fontInfo,ADDR_DISPLAY_GB2312_16_16+iOffset,32);

	for (i = 0; i < iHight ; i++)
	{
	  //2���ֽڣ�������
    for(j=0; j<2; j++)
    {
      iColDetail = fontInfo[i*2+j];
      iMask = 0x80;
      for(k=0; k<8; k++)
      {
        if((iColDetail & iMask)>0)
        {
          DrawPointToBuff(x+j*8+k, i, fc);
        }
        else
        {
          DrawPointToBuff(x+j*8+k, i, bc);
        }
        iMask = iMask/2;
      }
    }
   }
}

//���һ��8*16��ASCLL���л�����
//x:0~128
//y:0~128
//fc:������ɫ
//bc:������ɫ
void DrawFont_ASCLL_16_line(u8 x, u8 *lpAsc, u16 fc, u16 bc)
{

	u32 iColDetail = 0;
	u32 iMask = 0;
	u32 iOffset = ((*lpAsc)-32) * 16;//ÿ��ascռ16�ֽ�
  u8 fontInfo[16]={0};

  //�ֵĴ�С
	u8 iWidth = 8;
	u8 iHight = 16;

  int i,j,k;

	if( x >= 128) return;


  //ȡ��������Ϣ
  W25QXX_Read((unsigned char*)fontInfo,ADDR_DISPLAY_ASCLL_8_16+iOffset,16);

	for (i = 0; i < iHight ; i++)
	{
		iColDetail = (u8)fontInfo[i];
		iMask = 1<<7;

		for(j = 0; j < iWidth; j++)
		{
      if((iColDetail & iMask)>0)
      {
        DrawPointToBuff(x+j, i, fc);
      }
      else
      {
        DrawPointToBuff(x+j, i, bc);
      }
      iMask = iMask/2;
		}
	}
}


//��ʾһ��16��16�ĺ��֣�����8*16��Ӣ��
//line��ʾ��ʾ���У�һ��8��
//fc��������Ҫ��ʾ����ɫ
//bc������ɫ
//sҪ��ʾ���ַ���
void Gui_DrawFont_GBK16_line(u8 line, u16 fc, u16 bc, u8 *s)
{
	unsigned char i,j;
	unsigned short k,x0;
  unsigned short x=0,y=0;

  //һ��8������
  if(line>8) return;


	while(*s)
	{
		if((*s) < 128)
		{

		  DrawFont_ASCLL_16_line(x, s, fc, bc);
      x+=8;
			s++;
		}

		else
		{
      DrawFont_GB2312_16_line(x, s, fc, bc);
			s+=2;x+=16;
		}

	}

  //û���ַ��Ĳ�����屳��ɫ
  for(i=0;i<16;i++)
  for(j=x;j<X_MAX_PIXEL;j++)
  {
    DrawPointToBuff(j, i, bc);
  }

  //������ʵ��Y����
  y=(line-1)*16;//ÿһ�иߵ���16

  //����ˢ���ݵ���ʾ��
  Lcd_Updata_line(y,y+16);
}


