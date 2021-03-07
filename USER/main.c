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

#define WIFI_SSID    "sz-yy.taobao.com"    //����AP������
#define WIFI_PASSWORD "12345678"           //����AP������
#define WIFI_IP      "192.168.1.1"       //����AP��IP
#define WIFI_PORT     10000       			//����AP�Ķ˿ں�

u8 buff[100]={0};
void BMP180(void);   //��Ӵ�����������ֲ

int main(void)
{
    u8 key;
    u16 count=0;
    u8 firstDisplay=1;
    u8 t,h;
		
		u8 p;//��ѹ

    delay_init();	    	 //��ʱ������ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�

    uart1_init(115200);	 	//���ڳ�ʼ��Ϊ115200
    uart2_init(115200);	 	//���ڳ�ʼ��Ϊ115200

    W25QXX_Init();			//W25QXX��ʼ��
    LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
    Lcd_Init();			   	//��ʼ��LCD
    KEY_Init();				//������ʼ��
    Buzzer_Init();
    //DHT11_Init();
		Init_BMP085();	//��ʼ��BMP180

		//LCD������ʾ
    Gui_DrawFont_GBK16_line(HAL_LCD_LINE_1,VIOLET,WHITE,"WIFI+BMP180 TEST");
    Gui_DrawFont_GBK16_line(HAL_LCD_LINE_2,RED,WHITE,"WIFIģʽ:APģʽ");
    sprintf(buff, "SSID:%s", WIFI_SSID);
    LCD_P6x8Str(0,40,BLUE,WHITE,buff);
    sprintf(buff, "PSW:%s", WIFI_PASSWORD);
    LCD_P6x8Str(0,50,BLUE,WHITE,buff);
    sprintf(buff, "IP:%s", WIFI_IP);
    LCD_P6x8Str(0,60,BLUE,WHITE,buff);
    sprintf(buff, "PORT:%d", WIFI_PORT);
    LCD_P6x8Str(0,70,BLUE,WHITE,buff);


    //����������APģʽ
    ESP8266_init(ESP8266_MODE_AP, WIFI_SSID, WIFI_PASSWORD, WIFI_IP);

    //����Э��ģʽ��TCP��������TCP�ͻ��˻���UDP
    //����Ҫ���ӵ�IP�Ͷ˿ں�

    //���¶������ò���ֻ�ܴ�����һ����ѡ��TCP����UDPͨ��

    //����TCP����������
    ESP8266_SetWorkMode(ESP8266_TCP_SERVICE, WIFI_PORT, NULL, 10000);

    //����UDP�Ĳ���
    //�����IP"192.168.1.90"ΪUDP���������ڵĵ���IP������ʵ������޸�
    //ESP8266_SetWorkMode(ESP8266_UDP, WIFI_PORT, "192.168.1.2", 10000);

    while(1)
    {
        if((count%20)==0)
        {
  					ESP8266_run();
        }

        //3��ɼ�һ������
        if((count%300)==0)
        {
            //DHT11(&t, &h);//�ɼ���ʪ��
						BMP180();//�ɼ�����LCD���ϴ�ӡ����
						
					ESP8266_SendData(buff, strlen(buff));  //wifiģ�鷢������
        }

        delay_ms(10);

        //���Ʊ�ʾ�������
        if((count%20)==0) LED1=!LED1;

        count++;
    }
}

void BMP180()
{
		float Temperature;
    float GasPress;
    float Altitude;
		u8 str_print_lcd[100] = {0};//��ʾ����ӡ
		if(getBmp180Data(&Temperature, &GasPress, &Altitude)>0)
		{
			sprintf(str_print_lcd, "�¶�:%.1f ��", Temperature);
			Gui_DrawFont_GBK16_line(HAL_LCD_LINE_7,BLUE,WHITE,str_print_lcd);
			sprintf(str_print_lcd, "��ѹ:%.03f Kpa", GasPress);
			Gui_DrawFont_GBK16_line(HAL_LCD_LINE_8,BLUE,WHITE,str_print_lcd);
			
			sprintf(buff, "�¶�:%.1f ��,��ѹ:%.03f Kpa\r\n", Temperature,GasPress);
			//Gui_DrawFont_GBK16_line(HAL_LCD_LINE_7,BLUE,WHITE,buff);
			uart1_send_buff(buff, strlen(buff));
			
			//���ڵ��Դ���  ��ע��  ��Ҫ���ڵ��Խ��ע��
			
//			sprintf(buff, "��ѹ:%.03f Kpa", GasPress);
//			Gui_DrawFont_GBK16_line(HAL_LCD_LINE_7,BLUE,WHITE,buff);
//			uart1_send_buff(buff, strlen(buff));
//			uart1_send_buff("\r\n", 2);

//			sprintf(buff, "����:%.03f ��", Altitude);
//			Gui_DrawFont_GBK16_line(HAL_LCD_LINE_8,BLUE,WHITE,buff);
//			uart1_send_buff(buff, strlen(buff));
//			uart1_send_buff("\r\n", 2);
		}
		else
		{
			Gui_DrawFont_GBK16_line(HAL_LCD_LINE_8,RED,WHITE,"     error!!");   
		}
}





