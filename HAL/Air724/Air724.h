
#ifndef _Air724_H_
#define _Air724_H_

#include "main.h"
//C库
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

/* ------------------------------------------------------------------
 * 阿里云 IoT 接入凭据
 *
 * 真实凭据不写入本文件，避免随源码泄露。
 * 请在同目录新建 Air724_config_local.h（已加入 .gitignore），内容示例见
 * Air724_config.example.h；本文件在缺少该配置时使用占位值，可正常编译，
 * 但无法连上阿里云 IoT。
 * ------------------------------------------------------------------ */
#if defined(__has_include)
#  if __has_include("Air724_config_local.h")
#    include "Air724_config_local.h"
#  endif
#endif

#ifndef ALIYUN_PRODUCT_KEY
#define ALIYUN_PRODUCT_KEY   "YOUR_PRODUCT_KEY"
#endif
#ifndef ALIYUN_DEVICE_NAME
#define ALIYUN_DEVICE_NAME   "YOUR_DEVICE_NAME"
#endif
#ifndef ALIYUN_DEVICE_SECRET
#define ALIYUN_DEVICE_SECRET "YOUR_DEVICE_SECRET"
#endif
#ifndef ALIYUN_TIMESTAMP
#define ALIYUN_TIMESTAMP     "0"
#endif
#ifndef TELL_PHONE_NUMBER
#define TELL_PHONE_NUMBER    ""
#endif

#define clientId     ALIYUN_PRODUCT_KEY "." ALIYUN_DEVICE_NAME "|securemode=2,signmethod=hmacsha256,timestamp=" ALIYUN_TIMESTAMP "|"
#define username     ALIYUN_DEVICE_NAME "&" ALIYUN_PRODUCT_KEY
#define passwd       ALIYUN_DEVICE_SECRET
#define mqttHostUrl  ALIYUN_PRODUCT_KEY ".iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define port         "1883"

#define ProductKey   ALIYUN_PRODUCT_KEY
#define DeviceName   ALIYUN_DEVICE_NAME

#define Tell         TELL_PHONE_NUMBER

#define Huart_4G   huart1

#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志

#define DelayXms(x) HAL_Delay(x)

extern unsigned char Air724_buf[512];
extern unsigned short Air724_cnt;

extern uint8_t uart4G_value;				//串口2接收缓存变量

typedef struct{			//时间结构体
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t week;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}Time_Get;

void Air724_init(void);
void Ali_MQTT_Publish(void);		    //上报消息
void Ali_MQTT_Recevie(void);        //接收消息
void Air724sendMesEn(char MessEn[]);//发送英文短信
void Air724sendMesCh(char MessCh[]);//发送中文短信
void Air724Call(void);				//打电话函数
_Bool Air724_Status(void);          //1-连接状态    0-断开状态
Time_Get Air724_Get_Time(char ch);

#endif

//  if(huart->Instance == Huart_4G.Instance)//串口触发中断
//	{
//    if(huart->Instance == Huart_4G.Instance)//串口触发
//    {
//      HAL_UART_Receive_IT(&Huart_4G, &uart4G_value, 1);
//      if(Air724_cnt >= sizeof(Air724_buf))	Air724_cnt = 0; //防止串口被刷爆
//      Air724_buf[Air724_cnt++] = uart4G_value;	
//    }
//	}


//获取时间
/*

extern Time_Get Time;                          //时间
unsigned char second,minute,hour,month,day,week;
unsigned int year,ms;



              Time = Air724_Get_Time(1);
              year = Time.year; month = Time.month; day = Time.day; hour = Time.hour;
              minute = Time.minute; second = Time.second; week = Time.week;

监测函数
  Air724_Get_Time(0);
*/




