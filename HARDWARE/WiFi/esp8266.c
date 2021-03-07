#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "esp8266.h"
#include "led.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "w25qxx.h"

//////////////////////////////////////////////////////////////////////////////////
//STM32F103RC 开发板
//ESP8266　AT命令交互实现，可以修改WiFi为AP模式，STA模式，可以让WiFi工作于TCP/UDP模式
//日期:2021/3/6
//版本：V1.0
//********************************************************************************




ESP8266_DATA esp8266;
u8 gAtParseBuff[MAX_AT_PARSE_LEN]={0};

void ESP8266_RxData(u8* cmd, u32 len)
{
  ESP8266_Printf("\r\nRX:%s.\r\n", cmd);


}

//ESP8266调试信息的输出
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

//延时
void ESP8266_ms(u16 ms)
{
  delay_ms(ms);
}

//换一行，用于插入新接收的字符串
void changeLine()
{
  u8 index=0;

  esp8266.parseAt.insertCurCount=0;
  esp8266.parseAt.insertIdx++;
  if(esp8266.parseAt.insertIdx>=MAX_AT_PARSE_LINE)
  {
    esp8266.parseAt.insertIdx=0;
  }

  //清空存在位置
  index=esp8266.parseAt.insertIdx;
  memset(esp8266.parseAt.line[index], 0, MAX_AT_PARSE_LEN);
  esp8266.parseAt.count[index]=0;
}

//串口2收到一个字符，插入到插入到数组里，
//根据收到的数据是否是\r或者\n切换到下一行
void AddUartChar(unsigned char c)
{
  u8 index=esp8266.parseAt.insertIdx;//插入的行
  u16 count=esp8266.parseAt.count[index];//插入的位置
  static u8 lastCH=0;
  //if(c=='\r' || c=='\n')
  if(0)
  {
    if(count>0)//接收到的字符大于0才能换
    {
      changeLine();//进入下一行，等待插入
    }
  }
  else
  {
    if(count>=MAX_AT_PARSE_LEN)
    {
      changeLine();//进入下一行，等待插入
    }
    else
    {
      esp8266.parseAt.line[index][count]=c;
      esp8266.parseAt.insertCurCount++;
      esp8266.parseAt.count[index]+=1;

      //如果正在发送短消息，收到>号就换行
      if(count==0&&c=='>')
      {
        changeLine();//进入下一行，等待插入
      }
      else if(lastCH=='\r'&& c=='\n')
      {
        changeLine();//进入下一行，等待插入
      }
    }
  }

  lastCH=c;//记录最后一个字符

}

//取出一行串口3数据
//<buff> 接收数据的缓冲
//返回：接收到的数据长度，0表示没有数据接收
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
      changeLine();//读3行，行数和字数都不变，就主动换行
      ESP8266_Printf("*******changeLine*******\r\n");
    }

    return 0;
  }
  else
  {
    u16 len=0;
    u8 index=esp8266.parseAt.parseIdx;//读取的行

    //读取行的内容

    len=esp8266.parseAt.count[index];
    memcpy(buff, esp8266.parseAt.line[index], len);


    ESP8266_Printf("[%d],[%d],%s",esp8266.parseAt.parseIdx,len,esp8266.parseAt.line[index]);

    memset(esp8266.parseAt.line[index], 0, MAX_AT_PARSE_LEN);
    esp8266.parseAt.count[index]=0;

    //变成下一索引
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

//写一行数据到串口3
//<cmd> 发送的命令
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


//写一行数据到串口3
//<cmd> 发送的命令
void writeAtCmd(u8* cmd)
{
  u16 len=0;
  if(cmd==NULL) return;

  len=strlen((const char*)cmd);

  USART2_send_buff(cmd,len);

  ESP8266_Printf("AT>>>:%s\r\n", cmd);
}

//cmd:要发送的命令
//wait:1：等待结果　0:不等待，写完串口直接返回
//返回:超时返回2，命令正确执行返回1,命令错误执行返回3，其它0,
u8 ESP8266_SendAT(char* cmd, u8 wait)
{
  u32 len;
  u8 ok_error=0;
	u8 timeout=0;

  if(cmd==NULL) return 0;

  //写串口
	writeAtCmd((u8*)cmd);

  //不等待，写完串口直接返回
  if(0==wait)
  {
    return 0;
  }

  //等待结果
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
        ok_error=1;//正常
      }
      else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
            ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
      {
        ok_error=3;//错误
      }
      else
      {
        //解析数据
        ESP8266_parseMsg(gAtParseBuff,len);
      }
    }

    timeout++;

    if(ok_error>0) break;

    if(timeout>100)
    {
      ok_error=2;//超时
      break;
    }
	}

  ESP8266_Printf((u8*)"END AT:%s",cmd);

  return ok_error;
}

//默认解析返回的数据
//cmd:要解析的命令
//len:命令长度
//返回:命令已处理返回1，未处理返回0,
u8 ESP8266_parseMsg(u8* cmd, u32 len)
{
  u8 re=0;
  char *ptr = NULL;
  if(cmd==NULL) return 0;

  if(esp8266.smartconfig==1)
  {
    //一键配网的数据解析

    //获得ssid:
    ptr = (char*)strstr((const char*)cmd,(const char*)"ssid:");
    if(ptr!=NULL)
    {
      u8 index=0;
      //跳过"ssid:"
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

    //获得password:
    ptr = (char*)strstr((const char*)cmd,(const char*)"password:");
    if(ptr!=NULL)
    {
      u8 index=0;
      //跳过"password:"
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


    //获得WIFI GOT IP，表示配网成功
    ptr = (char*)strstr((const char*)cmd,(const char*)"WIFI GOT IP");
    if(ptr!=NULL)
    {
      esp8266.mode=ESP8266_MODE_STA;
      esp8266.smartconfig=2;
      WriteW25qxxConfig();//保存配置，下一次开机直接使用配置来写WIFI
      return 1;
    }

    return 1;
  }

  //用户断开TCP服务器
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

  //用户断开TCP服务器
  ptr = (char*)strstr((const char*)cmd,(const char*)"CLOSED");
  if(ptr!=NULL)
  {
    esp8266.isConnect=0;
    re=1;
  }


  //有用户连接TCP服务器
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


  //数据连接不存在
  ptr = (char*)strstr((const char*)cmd,(const char*)"link is not valid");
  if(ptr!=NULL)
  {
    esp8266.isConnect=0;
    re=1;
  }

  //收到数据
  ptr = (char*)strstr((const char*)cmd,(const char*)"+IPD,");
  if(ptr!=NULL)
  {
		int i=0;
    u8 id=0;

    //+IPD,
    ptr+=5;
    if(ptr==NULL) return 0;

    //如果是TCP服务器
    //格式会多一个id
    if(esp8266.connectMode==1)
    {
      //TCP服务的处理
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
      esp8266.data[esp8266.curLen]=*ptr;//数据存储
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

    //一次性把数据接收全
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

  //数据没有接收全
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

    //一次性把数据接收全
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

//设置WiFi模式工作模式:AP,STA,AP+STA
//正确执行返回1，其他返回0
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

//配置STA模式参数
//<ssid>连接的wifi名字
//<pwd> 密码
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


//配置AP模式参数
//<ssid> wifi名字
//<pwd> wifi密码，8 ~ 64 字节
//<chl> 通道号
//< ecn > 加密方式
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

//设置 ESP8266 AP模式 的静态 IP 地址
u8 ESP8266_SoftAp_staticIp(u8* ip)
{
    unsigned char cmd[200]={0};

    if(ip==NULL || strlen(ip)==0) return 0;


    sprintf(cmd, "AT+CIPAP_DEF=\"%s\"\r\n",ip);

    ESP8266_SendAT(cmd,1);

    return 0;
}


//设置 ESP8266 STA模式 的静态 IP 地址
u8 ESP8266_Station_staticIp(u8* ip)
{
    unsigned char cmd[200]={0};

    if(ip==NULL || strlen(ip)==0) return 0;

    sprintf(cmd, "AT+CIPSTA_DEF=\"%s\"\r\n",ip);

    ESP8266_SendAT(cmd,1);

    return 0;
}


//设置 ESP8266 进行一键配网
void ESP8266_StartSmartConfig(void)
{
    unsigned char cmd[200]={0};

    ESP8266_reset();//复位
    ESP8266_SendAT("ATE0\r\n",1);//关回显

    //先设置为STA模式
    ESP8266_SetWiFiWorkMode(ESP8266_MODE_STA);

    sprintf(cmd, "AT+CWSTARTSMART=1\r\n");//进入一键配网
    ESP8266_SendAT(cmd,1);
    esp8266.smartconfig=1;
}


//停止 ESP8266 进行一键配网
void ESP8266_StopSmartConfig(void)
{
    unsigned char cmd[200]={0};


    sprintf(cmd, "AT+CWSTOPSMART\r\n");
    ESP8266_SendAT(cmd,1);
    esp8266.smartconfig=0;
}




//获取ESP8266的IP地址
//返回:超时返回2，命令正确执行返回1,命令错误执行返回3，其它0,
u8 ESP8266_GetIp(void)
{
    char *ptr = NULL;
    u16 trytime=100;
    char cmd[45]={0};
    u16 L=0;
    u8 ok_error=0;

    //已经建立GPRS连接，可以直接发数据
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
        //取STA　IP
        ptr = (char*)strstr((const char*)gAtParseBuff,(const char*)"STAIP");
        if(ptr!=NULL)
        {
            u8 index=0;
            //跳过STAIP,"
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

        //取STA　MAC
        ptr = (char*)strstr((const char*)gAtParseBuff,(const char*)"STAMAC");
        if(ptr!=NULL)
        {
            u8 index=0;
            //跳过APMAC,"
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



        //取AP　IP
        ptr = (char*)strstr((const char*)gAtParseBuff,(const char*)"APIP");
        if(ptr!=NULL)
        {
            u8 index=0;
            //跳过APIP,"
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

        //取AP　MAC
        ptr = (char*)strstr((const char*)gAtParseBuff,(const char*)"APMAC");
        if(ptr!=NULL)
        {
            u8 index=0;
            //跳过APMAC,"
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
          ok_error=1;//正常
        }
        else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
              ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
        {
          ok_error=3;//错误
        }
        else
        {
          //解析数据
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
设置ESP8266　DHCP
<mode>
 0 : 设置 ESP8266 softAP
 1 : 设置 ESP8266 station
 2 : 设置 ESP8266 softAP ? station
<en>
 0 : 关闭 DHCP
 1 : 打开 DHCP
*/
u8 ESP8266_SetDhcp(u8 mode, u8 en)
{
  unsigned char cmd[200]={0};

  sprintf(cmd, "AT+CWDHCP_DEF=%d,%d\r\n",mode, en);
  return ESP8266_SendAT(cmd,1);
}


// TCP 服务器创建（必须是多连接）
//<mode>
// 0 关闭 server
// 1 创建 server
//<port> 默认端口是 333
u8 ESP8266_CreateTcpServer(u8 mode, u16 port)
{
  unsigned char cmd[200]={0};

  if(mode==1)
  {
    //先打开多连接
    ESP8266_SendAT("AT+CIPMUX=1\r\n",1);
    sprintf(cmd, "AT+CIPSERVER=1,%d\r\n",port);
    return ESP8266_SendAT(cmd,1);
  }
  else
  {
    sprintf(cmd, "AT+CIPSERVER=0\r\n");//关闭服务器
    ESP8266_SendAT(cmd,1);
  }

  return 0;
}


// TCP 客户端单连接创建
//<remoteIp> 远程服务器的IP
//<remotePort> 远程服务器的端口

u8 ESP8266_CreateTcpClient(u8* remoteIp, u16 remotePort)
{
  unsigned char cmd[200]={0};

  ESP8266_SendAT("AT+CIPMUX=0\r\n",1);

  sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",remoteIp, remotePort);
  return ESP8266_SendAT(cmd,1);

}


// UDP 单连接
//<remoteIp> 远程服务器的IP
//<remotePort> 远程服务器的端口
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


//创建TCP/UDP连接
//0表示成功
u8 CreateConnect(void)
{
  //连接模式　　1：TCP service  2:TCP client   3:UDP
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

//主动读取串口3命令，查看ESP8266是否主动上报命令/数据
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


    //控制IO口,复位模块
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //使能PC时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         //PC2 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);          //根据设定参数初始化GPIOC2

    //先断电再上电
    GPIO_SetBits(GPIOC,GPIO_Pin_2);             //PC2输出高,拉高RESET脚
    ESP8266_ms(500);
    GPIO_ResetBits(GPIOC,GPIO_Pin_2);             //PC2输出低,拉低RESET脚，复位模块
    ESP8266_ms(500);
    GPIO_SetBits(GPIOC,GPIO_Pin_2);             //PC2输出高,拉高RESET脚

    ESP8266_Printf("\r\nesp8266 reset\r\n");
}


//==========================================================
//	函数名称：	ESP8266_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	无
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//				如ESP8266的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
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

        if(esp8266.isConnect==0)//数据已经断开
        {
            return NULL;
        }

        if(L>0)
        {
            ptrIPD = (char*)strstr((const char*)gAtParseBuff,(const char*)"IPD,");				//搜索“IPD”头
            if(ptrIPD != NULL)											//找到"IPD,"
            {
                len=atoi(ptrIPD+4);
                ptrIPD = (char*)strstr((const char*)ptrIPD,(const char*)":");							//找到':'
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

        ESP8266_ms(5);													//延时等待
    } while(timeOut--);

    return NULL;														//超时还未找到，返回空指针
}



//发送数据
//<data> 要发送的数据
//<len> 数据长度
unsigned char ESP8266_SendData(u8* data, u16 len)
{
    u16 trytime=100;
    char cmd[45]={0};
    u16 L=0;
    u8 i=0;

    printf("ESP8266_SendData(%d).\r\n",len);


    //如果数据连接没有创建
    //先创建数据连接
    if(esp8266.isConnect==0)
    {
      printf("esp8266.isConnect=0.\r\n");
      if(CreateConnect()>0) return 0;

      esp8266.isConnect=1;
    }

    //开始发送数据
    if(esp8266.connectMode==1)
    {

      printf("id[0]=%d，id[1]=%d，id[2]=%d，id[3]=%d\r\n", esp8266.clientId[0], esp8266.clientId[1], esp8266.clientId[2], esp8266.clientId[3]);

      //tcp service数据发送
      for(i=0; i<4; i++)
      {

        if(esp8266.clientId[i]==-1 || esp8266.clientId[i]>3) continue;

        sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", esp8266.clientId[i], len);
        ESP8266_SendAT(cmd,0);


        //等待'>'
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
          else if(esp8266.isConnect==0)//数据已经断开
          {
            return 0;
          }
        }

        ESP8266_Printf("trytime=%d.\r\n",trytime);
        if(trytime>0)
        {
          //发送数据
          USART2_send_buff(data,len);

          //等待"OK"
          trytime=1000;
          while(--trytime>0)
          {
            ESP8266_ms(10);
            L=ESP8266_GetLine(gAtParseBuff);


            if(esp8266.isConnect==0)//数据已经断开
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
      //tcp client 和UDP数据发送

      sprintf(cmd, "AT+CIPSEND=%d\r\n", len);
      ESP8266_SendAT(cmd,0);

      //等待'>'
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
        else if(esp8266.isConnect==0)//数据已经断开
        {
          return 0;
        }

      }

      ESP8266_Printf("trytime=%d.\r\n",trytime);
      if(trytime>0)
      {
        ESP8266_Printf("Send:%s.\r\n",data);

        //发送数据
        USART2_send_buff(data,len);

        //等待"OK"
        trytime=1000;
        while(--trytime>0)
        {
          ESP8266_ms(10);
          L=ESP8266_GetLine(gAtParseBuff);

          if(esp8266.isConnect==0)//数据已经断开
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


//关闭单连接
unsigned char ESP8266_CloseConnect(void)
{
  unsigned char cmd[200]={0};

  sprintf(cmd, "AT+CIPCLOSE\r\n");
  return ESP8266_SendAT(cmd,1);
}

//ESP8266初始化
//Esp8266Mode:WIFI模块的工作模式，STA和AP
//ssid:要生成或者接入的SSID名字，如果已经一键配网，要写NULL
//password:对应SSID的密码，如果已经一键配网，要写NULL
//ip:如果要设置静态IP，要写对应的IP，否则写NULL。
u8 ESP8266_init(u8 Esp8266Mode, char *ssid,char *password, char *ip)
{
    u8 i=0;

    memset(esp8266.sta_ip,0,20);
    memset(esp8266.ap_ip,0,20);

    if(Esp8266Mode==ESP8266_MODE_STA)
    {
        esp8266.mode=ESP8266_MODE_STA;

        //STA模式有可能通过了一键配网模式得到了SSID和PSW
        if(ssid!=NULL) sprintf(esp8266.sta_ssid, "%s", ssid);
        if(password!=NULL) sprintf(esp8266.sta_password, "%s", password);

        if(ip!=NULL) sprintf(esp8266.sta_ip, "%s", ip);//设置STA模式静态IP



        ESP8266_Printf("\r\nSTA ssid:%s, psw:%s\r\n", esp8266.sta_ssid, esp8266.sta_password);
    }
    else
    {
        esp8266.ap_chl=5;//通道号
        esp8266.ap_ecn=ESP8266_ECN_WPA_PSK;//加密方式

        esp8266.mode=ESP8266_MODE_AP;
        sprintf(esp8266.ap_ssid, "%s", ssid);
        sprintf(esp8266.ap_password, "%s", password);

        if(ip!=NULL) sprintf(esp8266.ap_ip, "%s", ip);//设置AP模式静态IP


        ESP8266_Printf("\r\nAP ssid:%s, psw:%s\r\n", esp8266.ap_ssid, esp8266.ap_password);
    }


    esp8266.smartconfig=0;
    ESP8266_reset();

    ESP8266_SendAT("ATE0\r\n",1);//关回显

    //AP模式
    if(esp8266.mode==ESP8266_MODE_AP||esp8266.mode==ESP8266_MODE_AP_STA)
    {
      ESP8266_SetWiFiWorkMode(esp8266.mode);
      ESP8266_SoftAp(esp8266.ap_ssid, esp8266.ap_password, esp8266.ap_chl, esp8266.ap_ecn);
      ESP8266_SoftAp_staticIp(esp8266.ap_ip);
    }

    //STA模式
    if(esp8266.mode==ESP8266_MODE_STA||esp8266.mode==ESP8266_MODE_AP_STA)
    {
      ESP8266_SetWiFiWorkMode(esp8266.mode);
      ESP8266_ConnectAp(esp8266.sta_ssid, esp8266.sta_password, NULL);
      ESP8266_Station_staticIp(esp8266.sta_ip);
    }

    //启动DHCP
    ESP8266_SetDhcp(esp8266.mode, 1);
    ESP8266_SendAT("AT+CWLAPOPT=1,2047\r\n",1);

		return 0;
}

//设置WIFI工作的传输模式
//transportProtocol：UDP，TCPservice,tcp_client
//localPort:本地端口号
//remoteIp：远端的IP，如果是TCPservice模式，请写NULL或者""
//remotePort：远端端口号的PORT
u8 ESP8266_SetWorkMode(u8 transportProtocol, u16 localPort, char *remoteIp, u16 remotePort)
{
    u8 i=0;

    esp8266.localPort=localPort;
    esp8266.isConnect=0;//连接是否创建　　1：已经创建

    //ID为无效的ID
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
      esp8266.connectMode=1;//1：TCP service  2:TCP client   3:UDP
      if(remoteIp!=NULL) sprintf(esp8266.remoteIp, "%s", remoteIp);
      esp8266.remotePort=remotePort;
    }
    else if(ESP8266_TCP_CLIENT==transportProtocol)
    {
      esp8266.connectMode=2;//1：TCP service  2:TCP client   3:UDP
      if(remoteIp!=NULL) sprintf(esp8266.remoteIp, "%s", remoteIp);
      esp8266.remotePort=remotePort;
    }
    else if(ESP8266_UDP==transportProtocol)
    {
      esp8266.connectMode=3;//1：TCP service  2:TCP client   3:UDP
      if(remoteIp!=NULL) sprintf(esp8266.remoteIp, "%s", remoteIp);
      esp8266.remotePort=remotePort;
    }

    ESP8266_Printf("\r\nconnectMode:%d, ip:%s, port:%d\r\n", esp8266.connectMode, esp8266.remoteIp,esp8266.remotePort);

		return 0;
}

//扫描当前可用的 AP
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
        //取STA连接的AP的RSSI
        ptr = (char*)strstr((const char*)gAtParseBuff,(const char*)"+CWLAP:");
        if(ptr!=NULL)
        {
            u8 index=0;
            //跳过+CWLAP:"
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
          ok_error=1;//正常
        }
        else if((char*)strstr((const char*)gAtParseBuff,(const char*)"ERROR")!=NULL
              ||(char*)strstr((const char*)gAtParseBuff,(const char*)"FAIL")!=NULL)
        {
          ok_error=3;//错误
        }
        else
        {
          //解析数据
          ESP8266_parseMsg(gAtParseBuff,L);
        }
      }


      if(ok_error>0) break;

    }
  }
}

//读配置
//返回0表示要进入一键配置
//返回1表示保存的数据里有WIFI的名字和密码
u8 ReadW25qxxConfig(void)
{
  u8 isConfig;
  u8 ssid[32]={0};  //
  u8 password[64]={0};//
  u8 buff[1+32+64]={0};

  W25QXX_Read((unsigned char*)buff,ADDR_ESP8266_CONFIG,1+32+64);

  if(buff[0]==1)
  {
    //如果是1，表示后面的SSID和PASSWORD有效
    memcpy(esp8266.sta_ssid, &buff[1],32);
    memcpy(esp8266.sta_password, &buff[1+32],64);
    return 1;
  }

  return 0;
}


//写配置
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


