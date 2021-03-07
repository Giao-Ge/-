#ifndef __ESP8266_H
#define __ESP8266_H
#include "sys.h"
#include "usart.h"
#include "usart2.h"
#include "delay.h"


//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32F103RC 开发板
//ESP8266　AT命令交互实现，可以修改WiFi为AP模式，STA模式，可以让WiFi工作于TCP/UDP模式
//亿研电子sz-yy.taobao.com
//日期:2019/5/1
//版本：V1.0
//********************************************************************************



#ifndef NULL
#define NULL ((char *)0)
#endif

#define MAX_AT_PARSE_LINE     20    //最大解析行
#define MAX_AT_PARSE_LEN      500  ///接收命令最大字数
#define MAX_WIFI_RX_LEN     (MAX_AT_PARSE_LEN)    //发送接收最大字节
#define MAX_RX_DATA_LEN     3    //最大解析行


//1：TCP service  2:TCP client   3:UDP
enum
{
  ESP8266_TCP_SERVICE=1,
  ESP8266_TCP_CLIENT,
  ESP8266_UDP
};

enum
{
  ESP8266_MODE_STA=1,//STA
  ESP8266_MODE_AP,   //AP
  ESP8266_MODE_AP_STA,//AP+STA
};

enum
{
  ESP8266_ECN_OPEN=0,
  ESP8266_ECN_WPA_PSK=2,
  ESP8266_ECN_WPA2_PSK=3,
  ESP8266_ECN_WPA_WPA2_PSK=4,
};

//AT命令解析
typedef struct _at_parse_cmd_
{
    u8 line[MAX_AT_PARSE_LINE][MAX_AT_PARSE_LEN];//AT命令
    __IO u16 count[MAX_AT_PARSE_LINE];//每一行字数
    __IO u8 insertIdx;//插入行索引
    __IO u8 parseIdx;//解析行索引
    __IO u16 insertCurCount;//插入的位置
    u8 laseCH;//插入的最后一个字符

}AtParseCmd;

//ONENET命令解析
typedef struct _onenet_parse_cmd_
{
    u8 line[MAX_RX_DATA_LEN][MAX_AT_PARSE_LEN];//AT命令
    __IO u16 count[MAX_RX_DATA_LEN];//每一行字数
    __IO u8 insertIdx;//插入行索引
    __IO u8 parseIdx;//解析行索引
}OnenetParseCmd;


typedef struct _esp8266_msg_
{
  u8 smartconfig; //1:表示正在配网中　   2:配置成功   　  其他:不配置
  u8 mode;//模式　AP，STA，AT＋STA

  u8 sta_ssid[32];  //sta的名字
  u8 sta_password[64];//
  u8 sta_ip[20];//ip
  u8 sta_mac[30];//mac
  s8 rssi;   ///sta模式下,连接AP的信号


  u8 ap_ssid[32];
  u8 ap_password[64];
  u8 ap_chl;//通道号
  u8 ap_ecn;//加密方式
  u8 ap_ip[20];
  u8 ap_mac[30];//mac


  u8 connectMode;//连接模式　　1：TCP service  2:TCP client   3:UDP
  u8 isConnect;//连接是否创建　　1：已经创建
  u8 remoteIp[20];//远程的AP
  u16 remotePort;//远程端口
  u16 localPort;//本地端口，TCP服务器使用
  s8 clientId[4];//tcp连接的时候，对应的ID，最多4个，

  u8 data[MAX_WIFI_RX_LEN];//数据存储
  u16 data_len;//长度
  u16 curLen;//当前长度


  AtParseCmd  parseAt;//收到的命令缓冲
}ESP8266_DATA;


extern ESP8266_DATA esp8266;
extern u8 gAtParseBuff[MAX_AT_PARSE_LEN];
extern void AddUartChar(unsigned char c);

extern void ESP8266_Printf(const char * pFormat, ...);

extern void writeAtPrintf(const char * pFormat, ...);

//写一行数据到串口3
//<cmd> 发送的命令
void writeAtCmd(u8* cmd);

extern void ESP8266_ms(u16 ms);
extern u16 ESP8266_GetLine(char* buff);

extern u8 ESP8266_parseMsg(u8* cmd, u32 len);

//正确执行返回1，其他返回0
u8 ESP8266_SetWiFiWorkMode(u8 mode);
//数据发送
unsigned char ESP8266_SendData(u8* data, u16 len);

//<ssid>连接的wifi名字
//<pwd> 密码
//[<bssid>]
u8 ESP8266_ConnectAp(u8* ssid, u8* pwd, u8* bssid);



//<ssid> wifi名字
//<pwd> wifi密码，8 ~ 64 字节
//<chl> 通道号
//< ecn > 加密方式
// 0 OPEN
// 2 WPA_PSK
// 3 WPA2_PSK
// 4 WPA_WPA2_PSK

u8 ESP8266_SoftAp(u8* ssid, u8* pwd, u8 chl, u8 ecn);






//设置 ESP8266 softAP 的静态 IP 地址
u8 ESP8266_SoftAp_staticIp(u8* ip);


//设置 ESP8266 STA模式 的静态 IP 地址
u8 ESP8266_Station_staticIp(u8* ip);


//返回:超时返回2，命令正确执行返回1,命令错误执行返回3，其它0,
u8 ESP8266_GetIp(void);


/*
<mode>
 0 : 设置 ESP8266 softAP
 1 : 设置 ESP8266 station
 2 : 设置 ESP8266 softAP ? station
<en>
 0 : 关闭 DHCP
 1 : 打开 DHCP
*/
u8 ESP8266_SetDhcp(u8 mode, u8 en);


//设置ESP8266进行一键配网
void ESP8266_StartSmartConfig(void);
void ESP8266_StopSmartConfig(void);

//ESP8266初始化
u8 ESP8266_init(u8 Esp8266Mode, char *ssid,char *password, char *ip);

//设置WIFI工作的传输模式：UDP，TCPservice,tcp_client
u8 ESP8266_SetWorkMode(u8 transportProtocol, u16 localPort, char *remoteIp, u16 remotePort);

void ESP8266_GetStaMacString(u8* mac);
void ScanApInfo(void);
u8 ESP8266_reset(void);

unsigned char ESP8266_run(void);
u8 ReadW25qxxConfig(void);
void WriteW25qxxConfig(void);

#endif
