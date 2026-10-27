/**********************************
作者：特纳斯电子
网站：https://www.mcude.com
联系方式：46580829(QQ)
淘宝店铺：特纳斯电子
**********************************/
#ifndef __OLED_NEW_H__
#define __OLED_NEW_H__

#include "main.h"


/* 宏定义 -------------------------------------------------------------------*/
/***********************   OLED 连接引脚定义  **************************/
#define OLED_SCL_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()
#define OLED_SCL_PORT                           GPIOB
#define OLED_SCL_PIN                            GPIO_PIN_15

#define OLED_SDA_GPIO_CL_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()
#define OLED_SDA_PORT                           GPIOB
#define OLED_SDA_PIN                            GPIO_PIN_14

#define scl(a) (a?HAL_GPIO_WritePin(OLED_SCL_PORT,OLED_SCL_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(OLED_SCL_PORT,OLED_SCL_PIN,GPIO_PIN_RESET))
#define sda(a) (a?HAL_GPIO_WritePin(OLED_SDA_PORT,OLED_SDA_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(OLED_SDA_PORT,OLED_SDA_PIN,GPIO_PIN_RESET))

/* 函数 -------------------------------------------------------------------*/
/***********************   OLED  函数引用列表  **************************/
void OLED_Clear(void) ; 
void OLED_Set_Pos(unsigned char x, unsigned char y); 
void OLED_Init(void); 
void OLED_Clear2(void) ;
void point(unsigned char x,unsigned char y);
void OLED_Show_Time(uint8_t *TIME);
void OLED_Show_Humi(unsigned char hang, unsigned char add,unsigned int humi);
void OLED_Show_Temp(unsigned char hang, unsigned char add,unsigned int temp);
void Oled_ShowCHinese(unsigned char x,unsigned char y,unsigned char *p);
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len);
void Oled_ShowString(unsigned char x,unsigned char y,unsigned char *chr);
void OLED_Drwa_QRCode(void);
void OLED_Drwa_NUM_Plus(unsigned char x,unsigned char num);//数字显示函数（64X64）

#endif
