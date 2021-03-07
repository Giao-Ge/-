#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "usart.h"
#include "usart2.h"
#include "w25qxx.h"
#include "esp8266.h"
#include "buzzer.h"
#include "ds18b20.h"
#include "dht11.h"
#include "BMP180.h"

#define WIFI_SSID    "sz-yy.taobao.com"    //创建AP的名字
#define WIFI_PASSWORD "12345678"           //创建AP的密码
#define WIFI_IP      "192.168.1.1"       //创建AP的IP
#define WIFI_PORT     10000       			//创建AP的端口号

u8 buff[100]={0};
void BMP180(void);   //外接传感器代码移植

int main(void)
{
    u8 key;
    u16 count=0;
    u8 firstDisplay=1;
    u8 t,h;
		
		u8 p;//气压

    delay_init();	    	 //延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级

    uart1_init(115200);	 	//串口初始化为115200
    uart2_init(115200);	 	//串口初始化为115200

    W25QXX_Init();			//W25QXX初始化
    LED_Init();		  		//初始化与LED连接的硬件接口
    Lcd_Init();			   	//初始化LCD
    KEY_Init();				//按键初始化
    Buzzer_Init();
    //DHT11_Init();
		Init_BMP085();	//初始化BMP180

		//LCD界面提示
    Gui_DrawFont_GBK16_line(HAL_LCD_LINE_1,VIOLET,WHITE,"WIFI+BMP180 TEST");
    Gui_DrawFont_GBK16_line(HAL_LCD_LINE_2,RED,WHITE,"WIFI模式:AP模式");
    sprintf(buff, "SSID:%s", WIFI_SSID);
    LCD_P6x8Str(0,40,BLUE,WHITE,buff);
    sprintf(buff, "PSW:%s", WIFI_PASSWORD);
    LCD_P6x8Str(0,50,BLUE,WHITE,buff);
    sprintf(buff, "IP:%s", WIFI_IP);
    LCD_P6x8Str(0,60,BLUE,WHITE,buff);
    sprintf(buff, "PORT:%d", WIFI_PORT);
    LCD_P6x8Str(0,70,BLUE,WHITE,buff);


    //以下是启动AP模式
    ESP8266_init(ESP8266_MODE_AP, WIFI_SSID, WIFI_PASSWORD, WIFI_IP);

    //配置协议模式：TCP服务器、TCP客户端或者UDP
    //配置要连接的IP和端口号

    //以下二个配置参数只能打开其中一个。选择TCP或者UDP通信

    //配置TCP服务器参数
    ESP8266_SetWorkMode(ESP8266_TCP_SERVICE, WIFI_PORT, NULL, 10000);

    //配置UDP的参数
    //这里的IP"192.168.1.90"为UDP服务器所在的电脑IP，根据实际情况修改
    //ESP8266_SetWorkMode(ESP8266_UDP, WIFI_PORT, "192.168.1.2", 10000);

    while(1)
    {
        if((count%20)==0)
        {
  					ESP8266_run();
        }

        //3秒采集一次数据
        if((count%300)==0)
        {
            //DHT11(&t, &h);//采集温湿度
						BMP180();//采集并在LCD屏上打印数据
						
					ESP8266_SendData(buff, strlen(buff));  //wifi模块发送数据
        }

        delay_ms(10);

        //闪灯表示程序活着
        if((count%20)==0) LED1=!LED1;

        count++;
    }
}

void BMP180()
{
		float Temperature;
    float GasPress;
    float Altitude;
		u8 str_print_lcd[100] = {0};//显示屏打印
		if(getBmp180Data(&Temperature, &GasPress, &Altitude)>0)
		{
			sprintf(str_print_lcd, "温度:%.1f 度", Temperature);
			Gui_DrawFont_GBK16_line(HAL_LCD_LINE_7,BLUE,WHITE,str_print_lcd);
			sprintf(str_print_lcd, "气压:%.03f Kpa", GasPress);
			Gui_DrawFont_GBK16_line(HAL_LCD_LINE_8,BLUE,WHITE,str_print_lcd);
			
			sprintf(buff, "温度:%.1f 度,气压:%.03f Kpa\r\n", Temperature,GasPress);
			//Gui_DrawFont_GBK16_line(HAL_LCD_LINE_7,BLUE,WHITE,buff);
			uart1_send_buff(buff, strlen(buff));
			
			//串口调试代码  已注释  需要串口调试解除注释
			
//			sprintf(buff, "气压:%.03f Kpa", GasPress);
//			Gui_DrawFont_GBK16_line(HAL_LCD_LINE_7,BLUE,WHITE,buff);
//			uart1_send_buff(buff, strlen(buff));
//			uart1_send_buff("\r\n", 2);

//			sprintf(buff, "海拔:%.03f 米", Altitude);
//			Gui_DrawFont_GBK16_line(HAL_LCD_LINE_8,BLUE,WHITE,buff);
//			uart1_send_buff(buff, strlen(buff));
//			uart1_send_buff("\r\n", 2);
		}
		else
		{
			Gui_DrawFont_GBK16_line(HAL_LCD_LINE_8,RED,WHITE,"     error!!");   
		}
}





