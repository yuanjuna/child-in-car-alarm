/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "./HAL/key/key.h"
#include "./HAL/OLED/OLED_NEW.H"
#include "./HAL/ds18b20/ds18b20.h"
#include "./HAL/delay/delay.h"
#include "./HAL/Air724/Air724.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
void Key_function(void);								//按键函数
void Monitor_function(void);						//监测函数
void Display_function(void);						//显示函数
void Manage_function(void);							//处理函数
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define BEEP(a) (a?HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET)) 

#define RELAY(a) (a?HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET)) 

uint8_t USART1_TX_BUF[200];
#define u1_printf(...)  HAL_UART_Transmit(&huart1,USART1_TX_BUF,sprintf((char *)USART1_TX_BUF,__VA_ARGS__),0xffff)
                                                                                                                                                                        

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t key_num,flag_display;         //按键与显示变量
uint16_t time_1ms,time_500ms;         //计时变量1ms,500ms
uint16_t time_ms,time_1000ms;         //计时变量1ms,1000ms


//串口1的数据获取
uint8_t uart1_value;      //串口传的单个数据
//串口的储存数组，串口的接收时间，串口存值的数量
uint8_t uart1_buf[36],uart1_time,uart1_num;
uint8_t uart1_rx_flag;//串口的获取值的标志位

uint16_t temp;																						                //温度
uint8_t display_buf[20];                                                  //显示温度数组
uint16_t temp_init;                                                       //温度中间变量
uint8_t door_flag;                                                        //车门状态标志位
uint8_t window_flag;                                                      //车窗状态标志位
uint8_t people_flag;                                                      //车内状态标志位
uint16_t adc_value,light_value;
uint16_t SG90_1=500;                                                      //舵机角度控制变量（500-2500 0-180°）
uint8_t alarm;
uint8_t temp_yu=35,CO2_yu=50;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/****
*******按键设置函数
*****/
void Key_function(void)
{
	key_num = Chiclet_Keyboard_Scan();	//按键扫描
	if(key_num != 0)										//有按键按下
	{
		switch(key_num)
		{
			case 1:													//按键1，切换模式
			flag_display++;
			if(flag_display>=3)
				flag_display=0;
			OLED_Clear();									//按一下，清屏一次
			break;

			case 2:													//按键2
			if(flag_display==1)
			{
				if(temp_yu<99)
					temp_yu++;
			}
			if(flag_display==2)
			{
				if(CO2_yu<99)
					CO2_yu++;
			}
			break;

			case 3:													//按键3
			if(flag_display==0)
				window_flag=1;
			if(flag_display==1)
			{
				if(temp_yu>1)
					temp_yu--;
			}
			if(flag_display==2)
			{
				if(CO2_yu>1)
					CO2_yu--;
			}			
			break;
			
			case 4:													//按键4，切换模式
			window_flag=0;
				
			break;

			default:
			break;
		}
	}
}

/****
*******监测函数
*****/
void Monitor_function(void)
{
	if(time_500ms == 1)
	{
		time_500ms = 0;
		temp_init = DS18B20_Read_Temp_With_CRC();
		if(temp_init != 0x7FFF)
			temp = temp_init;			//获取温度
		 //单通道数据获取
		HAL_ADC_Start(&hadc1);
		if(HAL_ADC_PollForConversion(&hadc1, 999) == HAL_OK)
			adc_value = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
		light_value = (adc_value/4096.0)*100;			//获取二氧化碳值  
	}
  if(time_1000ms == 1)
	{
		time_1000ms = 0;
    Ali_MQTT_Publish();
  }
}


/****
*******显示函数
*****/
void Display_function(void)
{
	switch(flag_display)								//根据不同的显示模式标志位，显示不同
	{
		case 0:														
      Oled_ShowCHinese(0,0,(uint8_t *)"车门状态：");
      if(door_flag==0)
      {
        Oled_ShowCHinese(80,0,(uint8_t *)"关闭");
      }
      else
      {
        Oled_ShowCHinese(80,0,(uint8_t *)"开启");
      }      
      Oled_ShowCHinese(0,2,(uint8_t *)"车窗状态：");
      if(window_flag==0)
      {
        Oled_ShowCHinese(80,2,(uint8_t *)"关闭");
      }
      else
      {
        Oled_ShowCHinese(80,2,(uint8_t *)"开启");
      }
      
      Oled_ShowCHinese(0,4,(uint8_t *)"车内状态：");
      if(people_flag==1)
      {
        Oled_ShowCHinese(80,4,(uint8_t *)"有人");  
      }
      else
      {
        Oled_ShowCHinese(80,4,(uint8_t *)"无人");  
      }
//      sprintf((char *)display_buf,"%.1f  ",temp/10.0);//显示温度
//      Oled_ShowString(80,6,display_buf); 
		OLED_Show_Temp(80,6,temp);		
      OLED_ShowNum(16,6,light_value,2);               //显示二氧化碳浓度
		break;

		case 1:															
			Oled_ShowCHinese(16,1,(uint8_t *)"设置温度阈值"); 
			OLED_ShowNum(56,4,temp_yu,2);
		break;

		case 2:	
			Oled_ShowCHinese(0,1,(uint8_t *)"设置二氧化碳阈值"); 
			OLED_ShowNum(56,4,CO2_yu,2);														
		break;
	}
}


/****
*******处理函数
*****/
void Manage_function(void)
{
	if(HL==0)//车门开启
  {
    door_flag=1;
  }
  else
  {
    door_flag=0;
  }
  if(IN_F==1)//检测到有人
  {
    people_flag=1;
    
  }
  else
  {
      people_flag=0;
  }
  if(window_flag==1)
  {
    SG90_1=500+window_flag*1000;//打开窗户，舵机转动90度
  }
  if(window_flag==0)
  {
    SG90_1=500+window_flag*1000;//关闭窗户   
  }
  if(temp>temp_yu*10||light_value>CO2_yu)//检测温度过高或者二氧化碳浓度过高蜂鸣器报警，5秒后自动开窗
  {
    alarm=1;
    BEEP(1);
//    HAL_Delay(5000);
    window_flag=1;
  }
  else
  {
    alarm=0;
    BEEP(0);
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_UART_Receive_IT(&Huart_4G, &uart4G_value, 1);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);       //定时器3pwm初始化

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  OLED_Init();
  OLED_Clear();
  Ds18b20_GPIO_Init();
  Air724_init();
  
  while (1)
  {
		Key_function();							//按键函数
		Monitor_function();					//监测函数
		Display_function();					//显示函数
		Manage_function();					//处理函数
		Ali_MQTT_Recevie();
    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,SG90_1);    //舵机的角度控制
    
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == htim1.Instance)		//定时器1触发中断
	{
		time_1ms++;
		if(time_1ms>= 500)
		{
			time_1ms= 0;
			time_500ms = 1;
		}
    
		time_ms++;
		if(time_ms>= 1000)
		{
			time_ms= 0;
			time_1000ms = 1;
		}
		
    
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == Huart_4G.Instance)//串口触发中断
	{
    if(huart->Instance == Huart_4G.Instance)//串口触发
    {
      HAL_UART_Receive_IT(&Huart_4G, &uart4G_value, 1);
      if(Air724_cnt >= sizeof(Air724_buf))	Air724_cnt = 0; //防止串口被刷爆
      Air724_buf[Air724_cnt++] = uart4G_value;	
    }
	}

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
