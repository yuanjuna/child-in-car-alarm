/**********************************
包含头文件
**********************************/
#include "./HAL/KEY/key.h"

/**********************************
变量定义
**********************************/
uint8_t chiclet_keyboard_num = 0;				//键值变量

/**********************************
函数定义
**********************************/
/****
*******独立按键扫描函数
*******返回值：键值
*****/
uint8_t Chiclet_Keyboard_Scan(void)
{
	if(K1 == 0)													//按键K1为低电平
	{
		HAL_Delay(2);								//2ms消抖
		if(K1 == 0)												//按键K1依然为低电平，此时确认是K1按下
		{
			chiclet_keyboard_num = 1;				//键值设置成1
		}
		while(!K1);												//while死循环，直到抬手跳出
		return chiclet_keyboard_num;			//返回键值
	}

	if(K2 == 0)													//按键K2为低电平
	{
		HAL_Delay(2);								//2ms消抖
		if(K2 == 0)												//按键K2依然为低电平，此时确认是K2按下
		{
			chiclet_keyboard_num = 2;				//键值设置成2
		}
		while(!K2);												//while死循环，直到抬手跳出
		return chiclet_keyboard_num;			//返回键值
	}

	if(K3 == 0)													//按键K3为低电平
	{
		HAL_Delay(2);								//2ms消抖
		if(K3 == 0)												//按键K3依然为低电平，此时确认是K3按下
		{
			chiclet_keyboard_num = 3;				//键值设置成3
		}
		while(!K3);												//while死循环，直到抬手跳出
		return chiclet_keyboard_num;			//返回键值
	}
	if(K4 == 0)													//按键K3为低电平
	{
		HAL_Delay(2);								//2ms消抖
		if(K4 == 0)												//按键K4依然为低电平，此时确认是K4按下
		{
			chiclet_keyboard_num = 4;				//键值设置成4
		}
		while(!K4);												//while死循环，直到抬手跳出
		return chiclet_keyboard_num;			//返回键值
	}
	return 0;
}
