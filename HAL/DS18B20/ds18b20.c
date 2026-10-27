/**********************************
包含头文件
**********************************/
#include "./HAL/ds18b20/ds18b20.h"
#include "./HAL/delay/delay.h"

unsigned char ds_cnt=0;//ds18b20 读取存在脉冲超时计数
/****
*******DS18B20引脚初始化函数
*****/
void Ds18b20_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	DQ_GPIO_CLK_ENABLE();
	
  /*Configure GPIO pin : DQ_Pin */
  GPIO_InitStruct.Pin = DQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DQ_GPIO_Port, &GPIO_InitStruct);
}

/****
*******DS18B20初始化函数
*****/
void DS_rest(void)
{
    HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_SET);//D=1;
	 delay_us(1);
	 HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_RESET);//D=0;
	 delay_us(750);
	 HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_SET);//D=1;
	 delay_us(100);
  while(HAL_GPIO_ReadPin(DQ_GPIO_Port,DQ_Pin))
 {
   delay_us(1);
   ds_cnt++;
   if(ds_cnt==200)
   {
     ds_cnt=0;
     break;
   }
 }
	delay_us(300);										//200us
}

/****
*******写ds18b20内的数据函数
*****/
void write_byte(uint8_t dat)
{
  unsigned char i;
	for(i=0;i<8;i++)
	{
		HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_SET);//D=1;
		delay_us(1);
		HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_RESET);//D=0;
		delay_us(2);
		HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin,(GPIO_PinState)(dat&0x01));//D=dat&0x01;
		delay_us(30);
		dat=dat>>1;
	}
}

/****
*******读取ds18b20内的数据函数
*****/
unsigned char read_byte(void)
{
 unsigned char i,dat,tmp=0;
	for(i=0;i<8;i++)
	{
		HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_SET);//D=1;
		delay_us(1);
		HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_RESET);//D=0;
		delay_us(2);
		HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_SET);//D=1;
		delay_us(2);
		dat=HAL_GPIO_ReadPin(DQ_GPIO_Port,DQ_Pin);//dat=D;
		tmp|=(dat<<i);
		delay_us(30);
	}
	return tmp;
}

// CRC校验函数
uint8_t DS18B20_Check_CRC(uint8_t *data, uint8_t len)
{
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i]; // 逐字节异或
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x01) {
                crc = (crc >> 1) ^ 0x8C; // 多项式异或
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/****
*******读取温度值函数
*******返回值：温度值(扩大10倍)
*****/
unsigned int Ds18b20_Read_Temp(void)
{
 unsigned char i,j;
	unsigned int tem=0;
	DS_rest();
	write_byte(0xcc);
	write_byte(0x44);
	DS_rest();
  write_byte(0xcc);
	write_byte(0xbe);
	i=read_byte();
	j=read_byte();
	tem|=j&0x0F;
	tem<<=8;
	tem|=i;
	tem=(tem*0.0625);
	return tem;
}

// 读取 DS18B20 暂存器数据
void DS18B20_Read_Scratchpad(uint8_t *data, uint8_t len)
{
    DS_rest();               // 复位 DS18B20
    write_byte(0xCC);       // 跳过 ROM 命令
    write_byte(0xBE);       // 读取暂存器命令

    // 逐字节读取暂存器数据
    for (uint8_t i = 0; i < len; i++) {
        data[i] = read_byte();
    }
}

// 读取DS18B20温度数据并校验
int16_t DS18B20_Read_Temp_With_CRC(void)
{
    uint8_t data[9]; // DS18B20的9字节暂存器
	
		Ds18b20_Read_Temp();
    DS18B20_Read_Scratchpad(data, 9); // 读取暂存器数据

    // 计算前8字节的CRC值
    uint8_t crc_calculated = DS18B20_Check_CRC(data, 8);

    // 比较计算出的CRC值与读取的CRC校验码
    if (crc_calculated == data[8]) {
        // CRC校验通过，返回温度值
        int16_t temp = ((data[1] << 8) | data[0])*10/16.0; // 前2字节为温度值
        return temp;
    } else {
        // CRC校验失败，返回错误值
        return 0x7FFF; // 自定义错误值
    }
}

