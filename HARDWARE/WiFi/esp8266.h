#ifndef __ESP8266_H
#define __ESP8266_H
#include "sys.h"
#include "usart.h"
#include "usart2.h"
#include "delay.h"


//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//STM32F103RC ������
//ESP8266��AT�����ʵ�֣������޸�WiFiΪAPģʽ��STAģʽ��������WiFi������TCP/UDPģʽ
//���е���sz-yy.taobao.com
//����:2019/5/1
//�汾��V1.0
//********************************************************************************



#ifndef NULL
#define NULL ((char *)0)
#endif

#define MAX_AT_PARSE_LINE     20    //��������
#define MAX_AT_PARSE_LEN      500  ///���������������
#define MAX_WIFI_RX_LEN     (MAX_AT_PARSE_LEN)    //���ͽ�������ֽ�
#define MAX_RX_DATA_LEN     3    //��������


//1��TCP service  2:TCP client   3:UDP
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

//AT�������
typedef struct _at_parse_cmd_
{
    u8 line[MAX_AT_PARSE_LINE][MAX_AT_PARSE_LEN];//AT����
    __IO u16 count[MAX_AT_PARSE_LINE];//ÿһ������
    __IO u8 insertIdx;//����������
    __IO u8 parseIdx;//����������
    __IO u16 insertCurCount;//�����λ��
    u8 laseCH;//��������һ���ַ�

}AtParseCmd;

//ONENET�������
typedef struct _onenet_parse_cmd_
{
    u8 line[MAX_RX_DATA_LEN][MAX_AT_PARSE_LEN];//AT����
    __IO u16 count[MAX_RX_DATA_LEN];//ÿһ������
    __IO u8 insertIdx;//����������
    __IO u8 parseIdx;//����������
}OnenetParseCmd;


typedef struct _esp8266_msg_
{
  u8 smartconfig; //1:��ʾ���������С�   2:���óɹ�   ��  ����:������
  u8 mode;//ģʽ��AP��STA��AT��STA

  u8 sta_ssid[32];  //sta������
  u8 sta_password[64];//
  u8 sta_ip[20];//ip
  u8 sta_mac[30];//mac
  s8 rssi;   ///staģʽ��,����AP���ź�


  u8 ap_ssid[32];
  u8 ap_password[64];
  u8 ap_chl;//ͨ����
  u8 ap_ecn;//���ܷ�ʽ
  u8 ap_ip[20];
  u8 ap_mac[30];//mac


  u8 connectMode;//����ģʽ����1��TCP service  2:TCP client   3:UDP
  u8 isConnect;//�����Ƿ񴴽�����1���Ѿ�����
  u8 remoteIp[20];//Զ�̵�AP
  u16 remotePort;//Զ�̶˿�
  u16 localPort;//���ض˿ڣ�TCP������ʹ��
  s8 clientId[4];//tcp���ӵ�ʱ�򣬶�Ӧ��ID�����4����

  u8 data[MAX_WIFI_RX_LEN];//���ݴ洢
  u16 data_len;//����
  u16 curLen;//��ǰ����


  AtParseCmd  parseAt;//�յ��������
}ESP8266_DATA;


extern ESP8266_DATA esp8266;
extern u8 gAtParseBuff[MAX_AT_PARSE_LEN];
extern void AddUartChar(unsigned char c);

extern void ESP8266_Printf(const char * pFormat, ...);

extern void writeAtPrintf(const char * pFormat, ...);

//дһ�����ݵ�����3
//<cmd> ���͵�����
void writeAtCmd(u8* cmd);

extern void ESP8266_ms(u16 ms);
extern u16 ESP8266_GetLine(char* buff);

extern u8 ESP8266_parseMsg(u8* cmd, u32 len);

//��ȷִ�з���1����������0
u8 ESP8266_SetWiFiWorkMode(u8 mode);
//���ݷ���
unsigned char ESP8266_SendData(u8* data, u16 len);

//<ssid>���ӵ�wifi����
//<pwd> ����
//[<bssid>]
u8 ESP8266_ConnectAp(u8* ssid, u8* pwd, u8* bssid);



//<ssid> wifi����
//<pwd> wifi���룬8 ~ 64 �ֽ�
//<chl> ͨ����
//< ecn > ���ܷ�ʽ
// 0 OPEN
// 2 WPA_PSK
// 3 WPA2_PSK
// 4 WPA_WPA2_PSK

u8 ESP8266_SoftAp(u8* ssid, u8* pwd, u8 chl, u8 ecn);






//���� ESP8266 softAP �ľ�̬ IP ��ַ
u8 ESP8266_SoftAp_staticIp(u8* ip);


//���� ESP8266 STAģʽ �ľ�̬ IP ��ַ
u8 ESP8266_Station_staticIp(u8* ip);


//����:��ʱ����2��������ȷִ�з���1,�������ִ�з���3������0,
u8 ESP8266_GetIp(void);


/*
<mode>
 0 : ���� ESP8266 softAP
 1 : ���� ESP8266 station
 2 : ���� ESP8266 softAP ? station
<en>
 0 : �ر� DHCP
 1 : �� DHCP
*/
u8 ESP8266_SetDhcp(u8 mode, u8 en);


//����ESP8266����һ������
void ESP8266_StartSmartConfig(void);
void ESP8266_StopSmartConfig(void);

//ESP8266��ʼ��
u8 ESP8266_init(u8 Esp8266Mode, char *ssid,char *password, char *ip);

//����WIFI�����Ĵ���ģʽ��UDP��TCPservice,tcp_client
u8 ESP8266_SetWorkMode(u8 transportProtocol, u16 localPort, char *remoteIp, u16 remotePort);

void ESP8266_GetStaMacString(u8* mac);
void ScanApInfo(void);
u8 ESP8266_reset(void);

unsigned char ESP8266_run(void);
u8 ReadW25qxxConfig(void);
void WriteW25qxxConfig(void);

#endif
