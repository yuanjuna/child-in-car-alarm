//头文件
#include "./HAL/Air724/Air724.h"
#include "usart.h"

unsigned char Air724_buf[512];
unsigned short Air724_cnt,Air724_cntPre;
unsigned char USART4G_TX_BUF[200];
unsigned char uart4G_value;				//串口2接收缓存变量
#define u4g_printf(...)  HAL_UART_Transmit(&Huart_4G,USART4G_TX_BUF,sprintf((char *)USART4G_TX_BUF,__VA_ARGS__),0xffff)
/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Usart_SendString(unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	for(; count < len; count++)
	{
		HAL_UART_Transmit(&Huart_4G, str++, 1, 999);
	}
}
//==========================================================
//	函数名称：	Air724_Clear
//	函数功能：	清空缓存
//	入口参数：	无
//	返回参数：	无
//==========================================================
void Air724_Clear(void)
{
	memset(Air724_buf, 0, sizeof(Air724_buf));
	Air724_cnt = 0;
}

//==========================================================
//	函数名称：	Air724_WaitRecive
//	函数功能：	等待接收完成
//	入口参数：	无
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//==========================================================
_Bool Air724_WaitRecive(void)
{

	if(Air724_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;

	if(Air724_cnt == Air724_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		Air724_cnt = 0;							//清0接收计数
		return REV_OK;								//返回接收完成标志
	}
	Air724_cntPre = Air724_cnt;					//置为相同
	return REV_WAIT;								//返回接收未完成标志

}

//==========================================================
//	函数名称：	Air724_SendCmd
//	函数功能：	发送命令
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//	返回参数：	0-成功	1-失败
//==========================================================
_Bool Air724_SendCmd(char *cmd, char *res)
{
	unsigned short timeOut = 200;
	Usart_SendString((unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(Air724_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)Air724_buf, res) != NULL)		//如果检索到关键词
			{
				Air724_Clear();									//清空缓存
				return 0;
			}
		}
		DelayXms(10);
	}
	return 1;
}


//==========================================================
//	函数名称：	Air724LinkAp
//	函数功能：	连接网络和设置格式
//	入口参数：	无
//	返回参数：	无
//==========================================================
void Air724LinkAp(void)
{
  while(Air724_SendCmd("ATE0\r\n", "OK"))
    DelayXms(1000);
	while(Air724_SendCmd("AT+CPIN?\r\n", "READY"))
    DelayXms(1000);
  while(Air724_SendCmd("AT+CGATT?\r\n", "+CGATT: 1"))
    DelayXms(1000);
  while(Air724_SendCmd("AT+CSTT\r\n", "OK"))
    DelayXms(1000);
  while(Air724_SendCmd("AT+SAPBR=1,1\r\n", "OK"))
    DelayXms(1000);
  while(Air724_SendCmd("AT+CIICR\r\n", "OK"))
    DelayXms(1000);
  while(Air724_SendCmd("AT+CIFSR\r\n", ""))
    DelayXms(1000);
   while(Air724_SendCmd("AT+CIPSTATUS\r\n", "IP STATUS"))
    DelayXms(1000);
}

//==========================================================
//	函数名称：	Air724LinkloTExplorer
//	函数功能：	连接阿里云，并且订阅主题
//	入口参数：	无
//	返回参数：	无
//==========================================================
void Air724LinkloT(void)
{
  char send_buf[512];
  
  sprintf(send_buf,"AT+MCONFIG=\"%s\",\"%s\",\"%s\"\r\n",clientId,username,passwd);										//发送客户端ID、用户名、密码
	while(Air724_SendCmd(send_buf, "OK"))
		DelayXms(500);

	sprintf(send_buf,"AT+MIPSTART=\"%s\",%s\r\n",mqttHostUrl,port);																			//连接MQTT阿里云服务器 
	while(Air724_SendCmd(send_buf, "CONNECT OK"))
		DelayXms(500);
	
	sprintf(send_buf,"AT+MCONNECT=1,60\r\n");																														//确认连接
	while(Air724_SendCmd(send_buf, "CONNACK OK"))
		DelayXms(500);
	
	sprintf(send_buf,"AT+MSUB=\"/sys/%s/%s/thing/service/property/set\",0\r\n",ProductKey,DeviceName);	//订阅主题
	while(Air724_SendCmd(send_buf, "SUBACK"))
		DelayXms(500);
}

//==========================================================
//	函数名称：	Ali_MQTT_Publish
//	函数功能：	向阿里云发布消息
//	入口参数：	无
//	返回参数：	无
//==========================================================
uint8_t test;
extern uint8_t door_flag;																				                //温度						                                     
extern uint8_t window_flag;                                                        
extern uint16_t temp;																						                
extern uint8_t people_flag;
extern uint16_t light_value;                          
extern uint8_t alarm;
void Ali_MQTT_Publish(void)
{
	char buf[200];
	char txt[500];
  test++;
	memset(txt, 0, sizeof txt);
	strcat(txt, "");
	/* 数据点整合 */
	sprintf(buf, "AT+MPUB=/sys/%s/%s/thing/event/property/post,1,0,", ProductKey,DeviceName);
	strcat(txt, buf);
	strcat(txt, "\"{params:{");
  
    sprintf(buf, "door_flag:%d,", door_flag);
    strcat(txt, buf);

    sprintf(buf, "window_flag:%d,", window_flag);
    strcat(txt, buf);
  
    sprintf(buf, "temp:%.1f,", temp/10.0);
    strcat(txt, buf);

    sprintf(buf, "people_flag:%d,", people_flag);
    strcat(txt, buf);
    
    sprintf(buf, "alarm:%d,", alarm);
    strcat(txt, buf);

    sprintf(buf, "light_value:%d", light_value);
    strcat(txt, buf);
    


    strcat(txt, "}}\"\r\n");
	//发送数据
	Air724_SendCmd(txt, "PUBACK");
}

//==========================================================
//	函数名称：	Ali_MQTT_Recevie
//	函数功能：	接收阿里云下发的消息
//	入口参数：	无
//	返回参数：	无
//==========================================================

void Ali_MQTT_Recevie(void)
{
//	char *Start;
//  if(Air724_WaitRecive() == REV_OK)														//接收数据完成
//	{
//    Start = strstr((char *)Air724_buf, "RELAY_JS");
//    if(Start != NULL)
//    {
//      Start = strstr(Start, ":") + 1;
//      relay_o = atoi((const char *)Start);
//      Air724_Clear();
//      Ali_MQTT_Publish();
//    }
//    
//    Start = strstr((char *)Air724_buf, "HUMI_MAX");
//    if(Start != NULL)
//    {
//      Start = strstr(Start, ":") + 1;
//      test = atoi((const char *)Start);
//      Air724_Clear();
//      Ali_MQTT_Publish();
//    }
//  }
  
}

//==========================================================
//	函数名称：	Air724_Status
//	函数功能：	Air724状态判断
//	入口参数：	无
//	返回参数：	1-连接状态     0-断开状态
//==========================================================
_Bool Air724_Status(void)
{
	unsigned int timeOut;
	/* IP查询 */
	Usart_SendString((unsigned char *)"AT+CIPSTATUS\r\n", strlen((const char *)"AT+CIPSTATUS\r\n"));
	timeOut = 50;
	while(timeOut--)
	{
		if(Air724_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)Air724_buf, "STATE: IP STATUS") != NULL)		//如果检索到关键词
			{
				Air724_Clear();									//清空缓存
				return 1;	
			}
      else
      {
        Air724_Clear();									//清空缓存
        return 0;
			}
		}
		DelayXms(10);
	}
	Air724_Clear();
	return 1;
}
//==========================================================
//	函数名称：	Air724_init
//	函数功能：	主函数初始化函数
//	入口参数：	无
//	返回参数：	无
//==========================================================
void  Air724_init(void)
{
  HAL_UART_Receive_IT(&Huart_4G, &uart4G_value, 1);
  if(Air724_Status()!=1)
  {
    Air724LinkAp();
    Air724LinkloT();
  }
}
//==========================================================
//	函数名称：	Air724_Get_Time
//	函数功能：	获取网络时间
//	入口参数：	无
//	返回参数：	无
//==========================================================
Time_Get Time;
Time_Get Air724_Get_Time(char ch)
{
	char *Start;

  if(ch==1)
	{
    ch=0;
    Usart_SendString((unsigned char *)"AT+CCLK?\r\n", strlen((const char *)"AT+CCLK?\r\n"));
  }
  Start = strstr((const char *)Air724_buf, "+CCLK");
  if(Start!= NULL)		//如果检索到关键词
  {
    Start = strstr((const char *)Air724_buf, "\"")+ 1;
    Time.year = 2000+((*Start)-'0')*10+(*(Start+1))-'0';//获取年
    Start = strstr((const char *)Air724_buf, "/") + 1;
    Time.month = ((*Start)-'0')*10+(*(Start+1))-'0';
    Start = Start + 3;
    Time.day = ((*Start)-'0')*10+(*(Start+1))-'0';
    Start = strstr((const char *)Air724_buf, ",") + 1;
    Time.hour = ((*Start)-'0')*10+(*(Start+1))-'0';
    Start = Start + 3;
    Time.minute = ((*Start)-'0')*10+(*(Start+1))-'0';
    Start = Start + 3;
    Time.second = ((*Start)-'0')*10+(*(Start+1))-'0';
    Air724_Clear();									//清空缓存
  }
	return Time;
}


//==========================================================
//	函数名称：	Air724sendMesEn
//	函数功能：	发送英文短信
//	入口参数：	无
//	返回参数：	无
//==========================================================
void Air724sendMesEn(char MessEn[])
{
	while(Air724_SendCmd("AT+CMGF=1\r\n", "OK"))
    DelayXms(1000);
  DelayXms(500);
  while(Air724_SendCmd("AT+CSMP?\r\n", "+CSMP"))
    DelayXms(1000);
  DelayXms(500);
	u4g_printf("AT+CSMP=17,11,0,0\r\n");	
	DelayXms(500);
  u4g_printf("AT+CMGS=\"%s\"\r\n",Tell);	
	DelayXms(500);																							
	u4g_printf("%s",MessEn);
  u4g_printf("%c",0x1A);
  DelayXms(500);
}

//==========================================================
//	函数名称：	Air724sendMesCh
//	函数功能：	发送中文短信
//	入口参数：	无
//	返回参数：	无
//==========================================================
void Air724sendMesCh(char MessCh[])
{
	while(Air724_SendCmd("AT+CMGF=1\r\n", "OK"))
    DelayXms(1000);
  DelayXms(200);
  while(Air724_SendCmd("AT+CSMP?\r\n", "+CSMP"))
    DelayXms(1000);
  DelayXms(200);
  u4g_printf("AT+CSMP=17,167,0,8\r\n");	
	DelayXms(200);
  u4g_printf("AT+CMGS=\"%s\"\r\n",Tell);	
	DelayXms(200);																							
	u4g_printf("%s",MessCh);
  u4g_printf("%c",0x1A);
  DelayXms(200);
}
//==========================================================
//	函数名称：	Air724Call
//	函数功能：	打电话函数
//	入口参数：	无
//	返回参数：	无
//==========================================================
void Air724Call(void)
{
	u4g_printf("AT+SETVOLTE=1\r\n");
	DelayXms(200);
	u4g_printf("ATD%s;\r\n", Tell);
}
