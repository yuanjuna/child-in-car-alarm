/**********************************
作者：特纳斯电子
网站：https://www.mcude.com
联系方式：46580829(QQ)
淘宝店铺：特纳斯电子
**********************************/

#ifndef _KEY_H_
#define _KEY_H_


/**********************************
包含头文件
**********************************/
#include "main.h"


/**********************************
PIN口定义
**********************************/
#define K1 HAL_GPIO_ReadPin(K1_GPIO_Port,K1_Pin)
#define K2 HAL_GPIO_ReadPin(K2_GPIO_Port,K2_Pin)
#define K3 HAL_GPIO_ReadPin(K3_GPIO_Port,K3_Pin)
#define K4 HAL_GPIO_ReadPin(K4_GPIO_Port,K4_Pin)
/**********************************
函数声明
**********************************/
uint8_t Chiclet_Keyboard_Scan(void);			//独立按键扫描函数


#endif

