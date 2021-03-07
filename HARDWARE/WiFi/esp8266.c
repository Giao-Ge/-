#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "esp8266.h"
#include "led.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "w25qxx.h"

//////////////////////////////////////////////////////////////////////////////////
//STM32F103RC ������
//ESP8266��AT�����ʵ�֣������޸�WiFiΪAPģʽ��STAģʽ��������WiFi������TCP/UDPģʽ
//����:2021/3/6
//�汾��V1.0
//********************************************************************************




ESP8266_DATA esp8266;
u8 gAtParseBuff[MAX_AT_PARSE_LEN]={0};

void ESP8266_RxData(u8* cmd, u32 len)
{
  ESP8266_Printf("\r\nRX:%s.\r\n", cmd);


}

//ESP8266������Ϣ�����
void ESP8266_Printf(const char * pFormat, ...)
{
#if 1
	int len=0;
	char info[512] = {0};
	va_list args;

	va_start(args, pFormat);
	vsprintf(&info[0], (const char*)pFormat, args);
	va_end(args);

	len=strlen((const char *)info);

  uart1_send_buff((u8*)info, len);
#endif
}

//��ʱ
void ESP8266_ms(u16 ms)
{
  delay_ms(ms);
}

//��һ�У����ڲ����½��յ��ַ���
void changeLine()
{
  u8 index=0;

  esp8266.parseAt.insertCurCount=0;
  esp8266.parseAt.insertIdx++;
  if(esp8266.parseAt.insertIdx>=MAX_AT_PARSE_LINE)
  {
    esp8266.parseAt.insertIdx=0;
  }

  //��մ���λ��
  index=esp8266.parseAt.insertIdx;
  memset(esp8266.parseAt.line[index], 0, MAX_AT_PARSE_LEN);
  esp8266.parseAt.count[index]=0;
}

//����2�յ�һ���ַ������뵽���뵽�����
//�����յ��������Ƿ���\r����\n�л�����һ��
void AddUartChar(unsigned char c)
{
  u8 index=esp8266.parseAt.insertIdx;//�������
  u16 count=esp8266.parseAt.count[index];//�����λ��
  static u8 lastCH=0;
  //if(c=='\r' || c=='\n')
  if(0)
  {
    if(count>0)//���յ����ַ�����0���ܻ�
    {
      changeLine();//������һ�У��ȴ�����
    }
  }
  else
  {
    if(count>=MAX_AT_PARSE_LEN)
    {
      changeLine();//������һ�У��ȴ�����
    }
    else
    {
      esp8266.parseAt.line[index][count]=c;
      esp8266.parseAt.insertCurCount++;
      esp8266.parseAt.count[index]+=1;

      //������ڷ��Ͷ���Ϣ���յ�>�žͻ���
      if(count==0&&c=='>')
      {
        changeLine();//������һ�У��ȴ�����
      }
      else if(lastCH=='\r'&& c=='\n')
      {
        changeLine();//������һ�У��ȴ�����
      }
    }
  }

  lastCH=c;//��¼���һ���ַ�

}

//ȡ��һ�д���3����
//<buff> �������ݵĻ���
//���أ����յ������ݳ��ȣ�0��ʾû�����ݽ���
u16 ESP8266_GetLine(char* buff)
{
  static u8 line=0;
  static u16 insertCount=0;
  static u8 checkCount=0;
  memset(buff, 0, MAX_AT_PARSE_LEN);
  if(esp8266.parseAt.parseIdx==esp8266.parseAt.insertIdx)
  {
    if(line==esp8266.parseAt.parseIdx
      && esp8266.parseAt.count[line]>0
      && insertCount==esp8266.parseAt.count[line])
    {
      checkCount++;
    }
    else
    {
      line=esp8266.parseAt.parseIdx;
      insertCount=esp8266.parseAt.count[line];
      checkCount=0;
    }

    if(checkCount>3)
    {
      changeLine();//��3�У����������������䣬����������
      ESP8266_Printf("*******changeLine*******\r\n");
    }

    return 0;
  }
  else
  {
    u16 len=0;
    u8 index=esp8266.parseAt.parseIdx;//��ȡ����

    //��ȡ�е�����

    len=esp8266.parseAt.count[index];
    memcpy(buff, esp8266.parseAt.line[index], len);


    ESP8266_Printf("[%d],[%d],%s",esp8266.parseAt.parseIdx,len,esp8266.parseAt.line[index]);

    memset(esp8266.parseAt.line[index], 0, MAX_AT_PARSE_LEN);
    esp8266.parseAt.count[index]=0;

    //�����һ����
    esp8266.parseAt.parseIdx++;
    if(esp8266.parseAt.parseIdx>=MAX_AT_PARSE_LINE)
    {
      esp8266.parseAt.parseIdx=0;
    }

    line=0;
    insertCount=0;
    checkCount=0;
    return len;
  }

	return 0;
}

//дһ�����ݵ�����3
//<cmd> ���͵�����
void writeAtPrintf(const char * pFormat, ...)
{
#if 1
  int len=0;
  char info[512] = {0};
  va_list args;

  va_start(args, pFormat);
  vsprintf(&info[0], (const char*)pFormat, args);
  va_end(args);

  len=strlen((const char *)info);
  USART2_send_buff((u8*)info,len);
#endif
}


//дһ�����ݵ�����3
//<cmd> ���͵�����
void writeAtCmd(u8* cmd)
{
  u16 len=0;
  if(cmd==NULL) return;

  len=strlen((const char*)cmd);

  USART2_send_buff(cmd,len);

  ESP8266_Printf("AT>>>:%s\r\n", cmd);
}

//cmd:Ҫ���͵�����
//wait:1���ȴ������0:���ȴ���д�괮��ֱ�ӷ���
//����:��ʱ����2��������ȷִ�з���1,�������ִ�з���3������0,
u8 ESP8266_SendAT(char* cmd, u8 wait)
{
  u32 len;
  u8 ok_error=0;
	u8 timeout=0;

  if(cmd==NULL) return 0;

  //д����
	writeAtCmd((u8*)cmd);

  //���ȴ���д�괮��ֱ�ӷ���
  if(0==wait)
  {
    return 0;
  }

  //�ȴ����
	ok_error=0;
	timeout=0;
	while(1)
	{
		ESP8266_ms(50);
    len=ESP8266_GetLine(gAtParseBuff);
    if(len>0)
    {
      if((char*)strstr((const char*)gAtParseBuff,(const char*)"OK")!=NULL)
      {
        ok_error=1;//����
      }
      else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
            ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
      {
        ok_error=3;//����
      }
      else
      {
        //��������
        ESP8266_parseMsg(gAtParseBuff,len);
      }
    }

    timeout++;

    if(ok_error>0) break;

    if(timeout>100)
    {
      ok_error=2;//��ʱ
      break;
    }
	}

  ESP8266_Printf((u8*)"END AT:%s",cmd);

  return ok_error;
}

//Ĭ�Ͻ������ص�����
//cmd:Ҫ����������
//len:�����
//����:�����Ѵ�����1��δ������0,
u8 ESP8266_parseMsg(u8* cmd, u32 len)
{
  u8 re=0;
  char *ptr = NULL;
  if(cmd==NULL) return 0;

  if(esp8266.smartconfig==1)
  {
    //һ�����������ݽ���

    //���ssid:
    ptr = (char*)strstr((const char*)cmd,(const char*)"ssid:");
    if(ptr!=NULL)
    {
      u8 index=0;
      //����"ssid:"
      ptr+=5;

      memset(esp8266.sta_ssid, 0, 32);
      while(index<32 && ptr!=NULL && *ptr!='\0' && *ptr!='\r' && *ptr!='\n')
      {

        esp8266.sta_ssid[index]=*ptr;
        ptr++;
        index++;
      }

      printf("ssid=\"%s\"\r\n", esp8266.sta_ssid);
      return 1;
    }

    //���password:
    ptr = (char*)strstr((const char*)cmd,(const char*)"password:");
    if(ptr!=NULL)
    {
      u8 index=0;
      //����"password:"
      ptr+=9;

      memset(esp8266.sta_password, 0, 64);
      while(index<64 && ptr!=NULL && *ptr!='\0' && *ptr!='\r' && *ptr!='\n')
      {

        esp8266.sta_password[index]=*ptr;
        ptr++;
        index++;
      }

      printf("sta_password=\"%s\"\r\n", esp8266.sta_password);
      return 1;
    }


    //���WIFI GOT IP����ʾ�����ɹ�
    ptr = (char*)strstr((const char*)cmd,(const char*)"WIFI GOT IP");
    if(ptr!=NULL)
    {
      esp8266.mode=ESP8266_MODE_STA;
      esp8266.smartconfig=2;
      WriteW25qxxConfig();//�������ã���һ�ο���ֱ��ʹ��������дWIFI
      return 1;
    }

    return 1;
  }

  //�û��Ͽ�TCP������
  ptr = (char*)strstr((const char*)cmd,(const char*)",CLOSED");
  if(ptr!=NULL)
  {
    u8 id=0;
    ptr-=1;
    id=atoi(ptr);

    printf("1id=%d,%s.\r\n",id,ptr);

    if(id<4)
    {
      esp8266.clientId[id]=-1;
    }
    else
    {
      printf("error!!error!!error!!error!!error!!\r\n");
    }

    re=1;
  }

  //�û��Ͽ�TCP������
  ptr = (char*)strstr((const char*)cmd,(const char*)"CLOSED");
  if(ptr!=NULL)
  {
    esp8266.isConnect=0;
    re=1;
  }


  //���û�����TCP������
  ptr = (char*)strstr((const char*)cmd,(const char*)",CONNECT");
  if(ptr!=NULL)
  {
    u8 id=0;
    ptr-=1;


    id=atoi(ptr);

    printf("2id=%d,%s.\r\n",id,ptr);


    if(id<4)
    {
      esp8266.clientId[id]=id;
    }
    else
    {
      printf("error!!error!!error!!error!!error!!\r\n");
    }

    re=1;
  }


  //�������Ӳ�����
  ptr = (char*)strstr((const char*)cmd,(const char*)"link is not valid");
  if(ptr!=NULL)
  {
    esp8266.isConnect=0;
    re=1;
  }

  //�յ�����
  ptr = (char*)strstr((const char*)cmd,(const char*)"+IPD,");
  if(ptr!=NULL)
  {
		int i=0;
    u8 id=0;

    //+IPD,
    ptr+=5;
    if(ptr==NULL) return 0;

    //�����TCP������
    //��ʽ���һ��id
    if(esp8266.connectMode==1)
    {
      //TCP����Ĵ���
      id=atoi(ptr);

      while(ptr!=NULL && *ptr!=',') ptr++;
      ptr++;
    }

    esp8266.data_len=atoi(ptr);
    while(ptr!=NULL && *ptr!=':') ptr++;
    ptr++;

    memset(esp8266.data, 0, MAX_WIFI_RX_LEN);
    esp8266.curLen=0;
    while(esp8266.curLen<MAX_WIFI_RX_LEN && esp8266.curLen<esp8266.data_len && ptr!=NULL && ((ptr-cmd)<=len))
    {
      esp8266.data[esp8266.curLen]=*ptr;//���ݴ洢
      ptr++;
      esp8266.curLen++;
    }

#if 0
    printf("\r\n1len:%d,curLen:%d.len:%d\r\n", esp8266.data_len,esp8266.curLen,len);
    printf("\r\n\r\n");
    for(i=0; i<esp8266.curLen; i++)
    {
      printf("%02x,", esp8266.data[i]);
    }

    printf("\r\n\r\n");
#endif

    //һ���԰����ݽ���ȫ
    if(esp8266.curLen>=esp8266.data_len)
    {
//      printf("aaaa.\r\n");
      ESP8266_RxData(esp8266.data, esp8266.curLen);

      esp8266.curLen=0;
      esp8266.data_len=0;
    }

    re=1;
  }

  if(re>0) return 1;

  //����û�н���ȫ
  if(esp8266.curLen<esp8266.data_len)
  {
    if(esp8266.curLen+2+len<=MAX_WIFI_RX_LEN)
    {
      memcpy(&esp8266.data[esp8266.curLen], "\r\n", 2);
      esp8266.curLen+=2;

      memcpy(&esp8266.data[esp8266.curLen], cmd, len);
      esp8266.curLen+=len;
    }

   // printf("22len:%d,curLen:%d.\r\n", esp8266.data_len,esp8266.curLen);

    //һ���԰����ݽ���ȫ
    if(esp8266.curLen>=esp8266.data_len)
    {
      //printf("bbbbb.\r\n");
      ESP8266_RxData(esp8266.data, esp8266.curLen);
      esp8266.curLen=0;
      esp8266.data_len=0;
    }

  }

  return re;
}

//����WiFiģʽ����ģʽ:AP,STA,AP+STA
//��ȷִ�з���1����������0
u8 ESP8266_SetWiFiWorkMode(u8 mode)
{
    unsigned char cmd[45]={0};

    sprintf(cmd, "AT+CWMODE=%d\r\n",mode);
    if(ESP8266_SendAT(cmd,1)==1)
    {
      esp8266.mode=mode;
      return 1;
    }

    return 0;
}

//����STAģʽ����
//<ssid>���ӵ�wifi����
//<pwd> ����
//[<bssid>]
u8 ESP8266_ConnectAp(u8* ssid, u8* pwd, u8* bssid)
{
  unsigned char cmd[200]={0};

  if(esp8266.mode==ESP8266_MODE_STA || esp8266.mode==ESP8266_MODE_AP_STA)
  {
      if(bssid!=NULL)
      {
        sprintf(cmd, "AT+CWJAP_DEF=\"%s\",\"%s\",\"%s\"\r\n",ssid, pwd, bssid);
      }
      else
      {
        sprintf(cmd, "AT+CWJAP_DEF=\"%s\",\"%s\"\r\n",ssid, pwd);
      }

      if(ESP8266_SendAT(cmd,1)==1) return 1;
  }

  return 0;
}


//����APģʽ����
//<ssid> wifi����
//<pwd> wifi���룬8 ~ 64 �ֽ�
//<chl> ͨ����
//< ecn > ���ܷ�ʽ
// 0 OPEN
// 2 WPA_PSK
// 3 WPA2_PSK
// 4 WPA_WPA2_PSK

u8 ESP8266_SoftAp(u8* ssid, u8* pwd, u8 chl, u8 ecn)
{
    unsigned char cmd[200]={0};

    if(esp8266.mode==ESP8266_MODE_AP || esp8266.mode==ESP8266_MODE_AP_STA)
    {
      sprintf(cmd, "AT+CWSAP_DEF=\"%s\",\"%s\",%d,%d,4\r\n",ssid, pwd, chl, ecn);

      if(ESP8266_SendAT(cmd,1)==1) return 1;
    }

    return 0;
}

//���� ESP8266 APģʽ �ľ�̬ IP ��ַ
u8 ESP8266_SoftAp_staticIp(u8* ip)
{
    unsigned char cmd[200]={0};

    if(ip==NULL || strlen(ip)==0) return 0;


    sprintf(cmd, "AT+CIPAP_DEF=\"%s\"\r\n",ip);

    ESP8266_SendAT(cmd,1);

    return 0;
}


//���� ESP8266 STAģʽ �ľ�̬ IP ��ַ
u8 ESP8266_Station_staticIp(u8* ip)
{
    unsigned char cmd[200]={0};

    if(ip==NULL || strlen(ip)==0) return 0;

    sprintf(cmd, "AT+CIPSTA_DEF=\"%s\"\r\n",ip);

    ESP8266_SendAT(cmd,1);

    return 0;
}


//���� ESP8266 ����һ������
void ESP8266_StartSmartConfig(void)
{
    unsigned char cmd[200]={0};

    ESP8266_reset();//��λ
    ESP8266_SendAT("ATE0\r\n",1);//�ػ���

    //������ΪSTAģʽ
    ESP8266_SetWiFiWorkMode(ESP8266_MODE_STA);

    sprintf(cmd, "AT+CWSTARTSMART=1\r\n");//����һ������
    ESP8266_SendAT(cmd,1);
    esp8266.smartconfig=1;
}


//ֹͣ ESP8266 ����һ������
void ESP8266_StopSmartConfig(void)
{
    unsigned char cmd[200]={0};


    sprintf(cmd, "AT+CWSTOPSMART\r\n");
    ESP8266_SendAT(cmd,1);
    esp8266.smartconfig=0;
}




//��ȡESP8266��IP��ַ
//����:��ʱ����2��������ȷִ�з���1,�������ִ�з���3������0,
u8 ESP8266_GetIp(void)
{
    char *ptr = NULL;
    u16 trytime=100;
    char cmd[45]={0};
    u16 L=0;
    u8 ok_error=0;

    //�Ѿ�����GPRS���ӣ�����ֱ�ӷ�����
    sprintf(cmd, "AT+CIFSR\r\n");
    ESP8266_SendAT(cmd,0);

    //
    trytime=1000;
    while(--trytime>0)
    {
      ESP8266_ms(10);
      L=ESP8266_GetLine(gAtParseBuff);
      if(L>0)
      {
        //ȡSTA��IP
        ptr = (char*)strstr((const char*)gAtParseBuff,(const char*)"STAIP");
        if(ptr!=NULL)
        {
            u8 index=0;
            //����STAIP,"
            ptr+=7;

            memset(esp8266.sta_ip, 0, 20);
            while(index<20 && ptr!=NULL && *ptr!='\"')
            {

              esp8266.sta_ip[index]=*ptr;
              ptr++;
              index++;
            }
            continue;
        }

        //ȡSTA��MAC
        ptr = (char*)strstr((const char*)gAtParseBuff,(const char*)"STAMAC");
        if(ptr!=NULL)
        {
            u8 index=0;
            //����APMAC,"
            ptr+=8;

            memset(esp8266.sta_mac, 0, 30);
            while(index<30 && ptr!=NULL && *ptr!='\"')
            {

              esp8266.sta_mac[index]=*ptr;
              ptr++;
              index++;
            }

            continue;
        }



        //ȡAP��IP
        ptr = (char*)strstr((const char*)gAtParseBuff,(const char*)"APIP");
        if(ptr!=NULL)
        {
            u8 index=0;
            //����APIP,"
            ptr+=6;

            memset(esp8266.ap_ip, 0, 20);
            while(index<20 && ptr!=NULL && *ptr!='\"')
            {

              esp8266.ap_ip[index]=*ptr;
              ptr++;
              index++;
            }

            continue;
        }

        //ȡAP��MAC
        ptr = (char*)strstr((const char*)gAtParseBuff,(const char*)"APMAC");
        if(ptr!=NULL)
        {
            u8 index=0;
            //����APMAC,"
            ptr+=7;

            memset(esp8266.ap_mac, 0, 30);
            while(index<30 && ptr!=NULL && *ptr!='\"')
            {

              esp8266.ap_mac[index]=*ptr;
              ptr++;
              index++;
            }

            continue;
        }


        if((char*)strstr((const char*)gAtParseBuff,(const char*)"OK")!=NULL)
        {
          ok_error=1;//����
        }
        else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
              ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
        {
          ok_error=3;//����
        }
        else
        {
          //��������
          ESP8266_parseMsg(gAtParseBuff,L);
        }
      }


      if(ok_error>0) break;

    }


    ESP8266_Printf((u8*)"\r\ntrytime=%d.\r\n",trytime);


    if(trytime==0) return 2;

		return ok_error;
}





/*
����ESP8266��DHCP
<mode>
 0 : ���� ESP8266 softAP
 1 : ���� ESP8266 station
 2 : ���� ESP8266 softAP ? station
<en>
 0 : �ر� DHCP
 1 : �� DHCP
*/
u8 ESP8266_SetDhcp(u8 mode, u8 en)
{
  unsigned char cmd[200]={0};

  sprintf(cmd, "AT+CWDHCP_DEF=%d,%d\r\n",mode, en);
  return ESP8266_SendAT(cmd,1);
}


// TCP �����������������Ƕ����ӣ�
//<mode>
// 0 �ر� server
// 1 ���� server
//<port> Ĭ�϶˿��� 333
u8 ESP8266_CreateTcpServer(u8 mode, u16 port)
{
  unsigned char cmd[200]={0};

  if(mode==1)
  {
    //�ȴ򿪶�����
    ESP8266_SendAT("AT+CIPMUX=1\r\n",1);
    sprintf(cmd, "AT+CIPSERVER=1,%d\r\n",port);
    return ESP8266_SendAT(cmd,1);
  }
  else
  {
    sprintf(cmd, "AT+CIPSERVER=0\r\n");//�رշ�����
    ESP8266_SendAT(cmd,1);
  }

  return 0;
}


// TCP �ͻ��˵����Ӵ���
//<remoteIp> Զ�̷�������IP
//<remotePort> Զ�̷������Ķ˿�

u8 ESP8266_CreateTcpClient(u8* remoteIp, u16 remotePort)
{
  unsigned char cmd[200]={0};

  ESP8266_SendAT("AT+CIPMUX=0\r\n",1);

  sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",remoteIp, remotePort);
  return ESP8266_SendAT(cmd,1);

}


// UDP ������
//<remoteIp> Զ�̷�������IP
//<remotePort> Զ�̷������Ķ˿�
u8 ESP8266_CreateUdp(u8* remoteIp, u16 remotePort)
{
  unsigned char cmd[200]={0};

  ESP8266_SendAT("AT+CIPMUX=0\r\n",1);

  sprintf(cmd, "AT+CIPSTART=\"UDP\",\"%s\",%d\r\n",remoteIp, remotePort,20);
  return ESP8266_SendAT(cmd,1);

}

void ESP8266_GetStaMacString(u8* mac)
{
  int i=0;
  int index=0;
  int len=strlen(esp8266.sta_mac);

  for(i=0; i<len; i++)
  {
    if(esp8266.sta_mac[i]!=':')
    {
      mac[index]=esp8266.sta_mac[i];
      index++;
    }
  }
}


//����TCP/UDP����
//0��ʾ�ɹ�
u8 CreateConnect(void)
{
  //����ģʽ����1��TCP service  2:TCP client   3:UDP
  if(esp8266.connectMode==1)
  {
    if(1==ESP8266_CreateTcpServer(1, esp8266.localPort)) return 0;
  }
  else if(esp8266.connectMode==2)
  {
    if(1==ESP8266_CreateTcpClient(esp8266.remoteIp, esp8266.remotePort)) return 0;
  }
  else if(esp8266.connectMode==3)
  {
    if(1==ESP8266_CreateUdp(esp8266.remoteIp, esp8266.remotePort)) return 0;
  }
  else
  {

  }

  return 1;
}

//������ȡ����3����鿴ESP8266�Ƿ������ϱ�����/����
unsigned char ESP8266_run(void)
{
  u16 L=ESP8266_GetLine(gAtParseBuff);

  if(L>0)
  {
    ESP8266_parseMsg(gAtParseBuff, L);
  }

	return 0;
}

u8 ESP8266_reset(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;


    //����IO��,��λģ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //ʹ��PCʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         //PC2 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);          //�����趨������ʼ��GPIOC2

    //�ȶϵ����ϵ�
    GPIO_SetBits(GPIOC,GPIO_Pin_2);             //PC2�����,����RESET��
    ESP8266_ms(500);
    GPIO_ResetBits(GPIOC,GPIO_Pin_2);             //PC2�����,����RESET�ţ���λģ��
    ESP8266_ms(500);
    GPIO_SetBits(GPIOC,GPIO_Pin_2);             //PC2�����,����RESET��

    ESP8266_Printf("\r\nesp8266 reset\r\n");
}


//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	��
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{
    int len=0;
    char *ptr = NULL;
    char *ptrIPD = NULL;
    int L=0,i=0;

    do
    {
        L=ESP8266_GetLine(gAtParseBuff);

        if(esp8266.isConnect==0)//�����Ѿ��Ͽ�
        {
            return NULL;
        }

        if(L>0)
        {
            ptrIPD = (char*)strstr((const char*)gAtParseBuff,(const char*)"IPD,");				//������IPD��ͷ
            if(ptrIPD != NULL)											//�ҵ�"IPD,"
            {
                len=atoi(ptrIPD+4);
                ptrIPD = (char*)strstr((const char*)ptrIPD,(const char*)":");							//�ҵ�':'
                if(ptrIPD != NULL)
                {
                    ptrIPD++;
                    ptr=ptrIPD;
                    printf("\r\n");
                    for(i=0; i<len; i++)
                    {
                        printf("%02x,", ptr[i]);
                    }
                    printf("\r\n");

                    return (unsigned char *)(ptrIPD);
                }
            }
        }

        ESP8266_ms(5);													//��ʱ�ȴ�
    } while(timeOut--);

    return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��
}



//��������
//<data> Ҫ���͵�����
//<len> ���ݳ���
unsigned char ESP8266_SendData(u8* data, u16 len)
{
    u16 trytime=100;
    char cmd[45]={0};
    u16 L=0;
    u8 i=0;

    printf("ESP8266_SendData(%d).\r\n",len);


    //�����������û�д���
    //�ȴ�����������
    if(esp8266.isConnect==0)
    {
      printf("esp8266.isConnect=0.\r\n");
      if(CreateConnect()>0) return 0;

      esp8266.isConnect=1;
    }

    //��ʼ��������
    if(esp8266.connectMode==1)
    {

      printf("id[0]=%d��id[1]=%d��id[2]=%d��id[3]=%d\r\n", esp8266.clientId[0], esp8266.clientId[1], esp8266.clientId[2], esp8266.clientId[3]);

      //tcp service���ݷ���
      for(i=0; i<4; i++)
      {

        if(esp8266.clientId[i]==-1 || esp8266.clientId[i]>3) continue;

        sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", esp8266.clientId[i], len);
        ESP8266_SendAT(cmd,0);


        //�ȴ�'>'
        trytime=1000;
        while(--trytime>0)
        {
          ESP8266_ms(10);
          L=ESP8266_GetLine(gAtParseBuff);
          if(L==1 && gAtParseBuff[0]=='>')
          {
            break;
          }
          else if(L>0)
          {
            ESP8266_parseMsg(gAtParseBuff, L);
          }
          else if(esp8266.isConnect==0)//�����Ѿ��Ͽ�
          {
            return 0;
          }
        }

        ESP8266_Printf("trytime=%d.\r\n",trytime);
        if(trytime>0)
        {
          //��������
          USART2_send_buff(data,len);

          //�ȴ�"OK"
          trytime=1000;
          while(--trytime>0)
          {
            ESP8266_ms(10);
            L=ESP8266_GetLine(gAtParseBuff);


            if(esp8266.isConnect==0)//�����Ѿ��Ͽ�
            {
              return 0;
            }
            else if(L>0)
            {
              if((char*)strstr((const char*)gAtParseBuff,(const char*)"SEND OK")!=NULL)
              {
                ESP8266_Printf("SendData OK!!.\r\n");
                //return 1;
                break;
              }
              else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL)
              {
                ESP8266_Printf("SendData end2.\r\n");
                break;
                //return 0;
              }
              else
              {
                ESP8266_parseMsg(gAtParseBuff, L);
              }
            }
          }
        }
      }

      printf("end tcp send.\r\n");
    }
    else
    {
      //tcp client ��UDP���ݷ���

      sprintf(cmd, "AT+CIPSEND=%d\r\n", len);
      ESP8266_SendAT(cmd,0);

      //�ȴ�'>'
      trytime=1000;
      while(--trytime>0)
      {
        ESP8266_ms(10);
        L=ESP8266_GetLine(gAtParseBuff);
        if(L==1 && gAtParseBuff[0]=='>')
        {
          break;
        }
        else if(L>0)
        {
          ESP8266_parseMsg(gAtParseBuff, L);
        }
        else if(esp8266.isConnect==0)//�����Ѿ��Ͽ�
        {
          return 0;
        }

      }

      ESP8266_Printf("trytime=%d.\r\n",trytime);
      if(trytime>0)
      {
        ESP8266_Printf("Send:%s.\r\n",data);

        //��������
        USART2_send_buff(data,len);

        //�ȴ�"OK"
        trytime=1000;
        while(--trytime>0)
        {
          ESP8266_ms(10);
          L=ESP8266_GetLine(gAtParseBuff);

          if(esp8266.isConnect==0)//�����Ѿ��Ͽ�
          {
            return 0;
          }
          else if(L>0)
          {
            if((char*)strstr((const char*)gAtParseBuff,(const char*)"SEND OK")!=NULL)
            {
              ESP8266_Printf("SendData OK!!.\r\n");
              return 1;
            }
            else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL)
            {
              ESP8266_Printf("SendData end2.\r\n");
              return 0;
            }
            else if(L==1 && gAtParseBuff[0]==' ')
            {
              continue;
            }
            else
            {
              ESP8266_parseMsg(gAtParseBuff, L);
            }
          }
        }
      }
    }
		return 0;
}


//�رյ�����
unsigned char ESP8266_CloseConnect(void)
{
  unsigned char cmd[200]={0};

  sprintf(cmd, "AT+CIPCLOSE\r\n");
  return ESP8266_SendAT(cmd,1);
}

//ESP8266��ʼ��
//Esp8266Mode:WIFIģ��Ĺ���ģʽ��STA��AP
//ssid:Ҫ���ɻ��߽����SSID���֣�����Ѿ�һ��������ҪдNULL
//password:��ӦSSID�����룬����Ѿ�һ��������ҪдNULL
//ip:���Ҫ���þ�̬IP��Ҫд��Ӧ��IP������дNULL��
u8 ESP8266_init(u8 Esp8266Mode, char *ssid,char *password, char *ip)
{
    u8 i=0;

    memset(esp8266.sta_ip,0,20);
    memset(esp8266.ap_ip,0,20);

    if(Esp8266Mode==ESP8266_MODE_STA)
    {
        esp8266.mode=ESP8266_MODE_STA;

        //STAģʽ�п���ͨ����һ������ģʽ�õ���SSID��PSW
        if(ssid!=NULL) sprintf(esp8266.sta_ssid, "%s", ssid);
        if(password!=NULL) sprintf(esp8266.sta_password, "%s", password);

        if(ip!=NULL) sprintf(esp8266.sta_ip, "%s", ip);//����STAģʽ��̬IP



        ESP8266_Printf("\r\nSTA ssid:%s, psw:%s\r\n", esp8266.sta_ssid, esp8266.sta_password);
    }
    else
    {
        esp8266.ap_chl=5;//ͨ����
        esp8266.ap_ecn=ESP8266_ECN_WPA_PSK;//���ܷ�ʽ

        esp8266.mode=ESP8266_MODE_AP;
        sprintf(esp8266.ap_ssid, "%s", ssid);
        sprintf(esp8266.ap_password, "%s", password);

        if(ip!=NULL) sprintf(esp8266.ap_ip, "%s", ip);//����APģʽ��̬IP


        ESP8266_Printf("\r\nAP ssid:%s, psw:%s\r\n", esp8266.ap_ssid, esp8266.ap_password);
    }


    esp8266.smartconfig=0;
    ESP8266_reset();

    ESP8266_SendAT("ATE0\r\n",1);//�ػ���

    //APģʽ
    if(esp8266.mode==ESP8266_MODE_AP||esp8266.mode==ESP8266_MODE_AP_STA)
    {
      ESP8266_SetWiFiWorkMode(esp8266.mode);
      ESP8266_SoftAp(esp8266.ap_ssid, esp8266.ap_password, esp8266.ap_chl, esp8266.ap_ecn);
      ESP8266_SoftAp_staticIp(esp8266.ap_ip);
    }

    //STAģʽ
    if(esp8266.mode==ESP8266_MODE_STA||esp8266.mode==ESP8266_MODE_AP_STA)
    {
      ESP8266_SetWiFiWorkMode(esp8266.mode);
      ESP8266_ConnectAp(esp8266.sta_ssid, esp8266.sta_password, NULL);
      ESP8266_Station_staticIp(esp8266.sta_ip);
    }

    //����DHCP
    ESP8266_SetDhcp(esp8266.mode, 1);
    ESP8266_SendAT("AT+CWLAPOPT=1,2047\r\n",1);

		return 0;
}

//����WIFI�����Ĵ���ģʽ
//transportProtocol��UDP��TCPservice,tcp_client
//localPort:���ض˿ں�
//remoteIp��Զ�˵�IP�������TCPserviceģʽ����дNULL����""
//remotePort��Զ�˶˿ںŵ�PORT
u8 ESP8266_SetWorkMode(u8 transportProtocol, u16 localPort, char *remoteIp, u16 remotePort)
{
    u8 i=0;

    esp8266.localPort=localPort;
    esp8266.isConnect=0;//�����Ƿ񴴽�����1���Ѿ�����

    //IDΪ��Ч��ID
    esp8266.clientId[0]=-1;
    esp8266.clientId[1]=-1;
    esp8266.clientId[2]=-1;
    esp8266.clientId[3]=-1;

    memset(esp8266.sta_mac,0,30);
    esp8266.curLen=0;
    esp8266.data_len=0;

    memset(esp8266.remoteIp,0,20);


    if(ESP8266_TCP_SERVICE==transportProtocol)
    {
      esp8266.connectMode=1;//1��TCP service  2:TCP client   3:UDP
      if(remoteIp!=NULL) sprintf(esp8266.remoteIp, "%s", remoteIp);
      esp8266.remotePort=remotePort;
    }
    else if(ESP8266_TCP_CLIENT==transportProtocol)
    {
      esp8266.connectMode=2;//1��TCP service  2:TCP client   3:UDP
      if(remoteIp!=NULL) sprintf(esp8266.remoteIp, "%s", remoteIp);
      esp8266.remotePort=remotePort;
    }
    else if(ESP8266_UDP==transportProtocol)
    {
      esp8266.connectMode=3;//1��TCP service  2:TCP client   3:UDP
      if(remoteIp!=NULL) sprintf(esp8266.remoteIp, "%s", remoteIp);
      esp8266.remotePort=remotePort;
    }

    ESP8266_Printf("\r\nconnectMode:%d, ip:%s, port:%d\r\n", esp8266.connectMode, esp8266.remoteIp,esp8266.remotePort);

		return 0;
}

//ɨ�赱ǰ���õ� AP
void ScanApInfo(void)
{
  char *ptr = NULL;
  u16 trytime=100;
  char cmd[200]={0};
  u16 L=0;
  u8 ok_error=0;

  if(esp8266.mode==ESP8266_MODE_STA)
  {
    sprintf(cmd, "AT+CWLAP=\"%s\"\r\n",esp8266.sta_ssid);
    ESP8266_SendAT(cmd,0);

    //
    trytime=1000;
    while(--trytime>0)
    {
      ESP8266_ms(10);
      L=ESP8266_GetLine(gAtParseBuff);
      if(L>0)
      {
        //ȡSTA���ӵ�AP��RSSI
        ptr = (char*)strstr((const char*)gAtParseBuff,(const char*)"+CWLAP:");
        if(ptr!=NULL)
        {
            u8 index=0;
            //����+CWLAP:"
            ptr+=7;

            esp8266.rssi=0;

            ptr = (char*)strstr((const char*)ptr,(const char*)"\",");

            if(ptr!=NULL)
            {
              ptr+=2;
              if(ptr!=NULL) esp8266.rssi=atoi(ptr);

            }

            ESP8266_Printf((u8*)"rssi:%d.\r\n",esp8266.rssi);
            continue;
        }

        if((char*)strstr((const char*)gAtParseBuff,(const char*)"OK")!=NULL)
        {
          ok_error=1;//����
        }
        else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
              ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
        {
          ok_error=3;//����
        }
        else
        {
          //��������
          ESP8266_parseMsg(gAtParseBuff,L);
        }
      }


      if(ok_error>0) break;

    }
  }
}

//������
//����0��ʾҪ����һ������
//����1��ʾ�������������WIFI�����ֺ�����
u8 ReadW25qxxConfig(void)
{
  u8 isConfig;
  u8 ssid[32]={0};  //
  u8 password[64]={0};//
  u8 buff[1+32+64]={0};

  W25QXX_Read((unsigned char*)buff,ADDR_ESP8266_CONFIG,1+32+64);

  if(buff[0]==1)
  {
    //�����1����ʾ�����SSID��PASSWORD��Ч
    memcpy(esp8266.sta_ssid, &buff[1],32);
    memcpy(esp8266.sta_password, &buff[1+32],64);
    return 1;
  }

  return 0;
}


//д����
void WriteW25qxxConfig(void)
{
  u8 isConfig;
  u8 ssid[32]={0};  //
  u8 password[64]={0};//
  u8 buff[1+32+64]={0};

  buff[0]=1;
  memcpy(&buff[1], esp8266.sta_ssid, 32);
  memcpy(&buff[1+32], esp8266.sta_password, 64);
  W25QXX_Write(buff, ADDR_ESP8266_CONFIG, 1+32+64);
}


