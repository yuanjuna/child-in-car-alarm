#include "./HAL/OLED/OLED_NEW.H"
#include "./HAL/OLED/OLED_TAB.H"

/**********************************
作者：特纳斯电子
网站：https://www.mcude.com
联系方式：46580829(QQ)
淘宝店铺：特纳斯电子
  * 函数功能: OLED初始化引脚函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void OLED_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  //引脚时钟使能
  OLED_SCL_GPIO_CLK_ENABLE();
  OLED_SDA_GPIO_CL_ENABLE();

  /* GPIO引脚配置以及初始化 */
  GPIO_InitStruct.Pin   = OLED_SDA_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OLED_SDA_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin   = OLED_SCL_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OLED_SCL_PORT, &GPIO_InitStruct);
}

void ys(unsigned int i)
{
   while(i--);
}
/**********************************
作者：特纳斯电子
网站：https://www.mcude.com
联系方式：46580829(QQ)
淘宝店铺：特纳斯电子
  * 函数功能: OLED IIC  开始函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void OLED_IIC_Start()
{
   scl(1);
  ys(4);
   sda(1);
  ys(4);
   sda(0);
  ys(4);
   scl(0);

}

/**********************************************
//IIC Stop
**********************************************/
void OLED_IIC_Stop()
{
   sda(0);
  ys(4);
   scl(1);
  ys(4);
   sda(1);
}
/**********************************************
// IIC Write byte
**********************************************/
unsigned char OLED_Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char Ack_Bit;                    //应答信号
	for(i=0;i<8;i++)		
	{
		if(IIC_Byte & 0x80)		//1?0?
		{sda(1);}
		else
		{
		sda(0);
		}
		 ys(4);
		scl(1);
    	 ys(4);
		scl(0);
		ys(4);
		IIC_Byte<<=1;			//loop
	}
	 sda(1);		                //释放IIC SDA总线为主器件接收从器件产生应答信号	
	 ys(4);
	scl(1);                     //第9个时钟周期
	 ys(4);
	Ack_Bit = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13);		            //读取应答信号
	scl(0);
	return Ack_Bit;	
}  
void OLED_write_iic_com(unsigned char IIC_Command)
{
   OLED_IIC_Start();
   OLED_Write_IIC_Byte(0x78);            //Slave addre.ss,SA0=0
   OLED_Write_IIC_Byte(0x00);			//write command
   OLED_Write_IIC_Byte(IIC_Command); 
   OLED_IIC_Stop();
}


void OLED_write_iic_dat(unsigned char IIC_Data)
{
   OLED_IIC_Start();
   OLED_Write_IIC_Byte(0x78);			
   OLED_Write_IIC_Byte(0x40);			//write data
   OLED_Write_IIC_Byte(IIC_Data);
   OLED_IIC_Stop(); 
}

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{
	OLED_write_iic_com(0xb0+y);
	OLED_write_iic_com(((x&0xf0)>>4)|0x10);
	OLED_write_iic_com((x&0x0f)|0x01);
}
void OLED_Clear(void)  
{  
	unsigned char i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_write_iic_com (0xb0+i);
		OLED_write_iic_com (0x00);     
		OLED_write_iic_com (0x10);        
		for(n=0;n<128;n++)OLED_write_iic_dat(0); 
	}
} 

void OLED_Init(void) 
{
	OLED_GPIO_Init();//引脚初始化

	OLED_write_iic_com(0xae);//--turn off oled panel
	OLED_write_iic_com(0x00);//---set low column address
	OLED_write_iic_com(0x10);//---set high column address
	OLED_write_iic_com(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_write_iic_com(0x81);//--set contrast control register
	OLED_write_iic_com(0xCF); // Set SEG Output Current Brightness
	OLED_write_iic_com(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_write_iic_com(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_write_iic_com(0xa6);//--set normal display
	OLED_write_iic_com(0xa8);//--set multiplex ratio(1 to 64)
	OLED_write_iic_com(0x3f);//--1/64 duty
	OLED_write_iic_com(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_write_iic_com(0x00);//-not offset
	OLED_write_iic_com(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_write_iic_com(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_write_iic_com(0xd9);//--set pre-charge period
	OLED_write_iic_com(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_write_iic_com(0xda);//--set com pins hardware configuration
	OLED_write_iic_com(0x12);
	OLED_write_iic_com(0xdb);//--set vcomh
	OLED_write_iic_com(0x40);//Set VCOM Deselect Level
	OLED_write_iic_com(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_write_iic_com(0x02);//
	OLED_write_iic_com(0x8d);//--set Charge Pump enable/disable
	OLED_write_iic_com(0x14);//--set(0x10) disable
	OLED_write_iic_com(0xa4);// Disable Entire Display On (0xa4/0xa5)
	OLED_write_iic_com(0xa6);// Disable Inverse Display On (0xa6/a7) 
	OLED_write_iic_com(0xaf);//--turn on oled panel
	OLED_Set_Pos(0,0);
} 

void OLED_Clear2(void)  
{  
	unsigned char i,n;		    
	for(i=4;i<8;i++)  
	{  
		OLED_write_iic_com (0xb0+i);
		OLED_write_iic_com (0x02);    
		OLED_write_iic_com (0x10);       
		for(n=0;n<128;n++)OLED_write_iic_dat(0); 
	} 
} 

/****
*******OLED显示单个字符函数
*****/
void Oled_ShowChar(unsigned char x,unsigned char y,unsigned int chr)
{      	
	unsigned int c=0,i=0;	
	c=chr-' ';			
	OLED_Set_Pos(x,y);	
	for(i=0;i<8;i++)
		OLED_write_iic_dat(F8X16[c*16+i]);
	OLED_Set_Pos(x,y+1);
	for(i=0;i<8;i++)
		OLED_write_iic_dat(F8X16[c*16+i+8]);
}

/****
*******OLED显示字符串函数
*****/
void Oled_ShowString(unsigned char x,unsigned char y,unsigned char *chr)
{
	unsigned int j=0;
	while (chr[j]!='\0')
	{		
		Oled_ShowChar(x,y,chr[j]);
		x+=8;
		if(x>120){x=0;y+=2;}
		j++;
	}
}
unsigned int oled_pow(unsigned char m,unsigned char n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~65525);	 		  
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len)
{         	
	unsigned char t,temp;
	unsigned char enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				Oled_ShowChar(x+(16/2)*t,y,'0');
				continue;
			}else enshow=1; 
		 	 
		}
	 	Oled_ShowChar(x+(16/2)*t,y,temp+'0'); 
	}
} 
/****
*******OLED显示中文函数
*****/
void Oled_ShowCHinese(unsigned char x,unsigned char y,unsigned char *p)
{      			    
	unsigned char t,wordNum;
	while(*p != '\0')
	{
		for(wordNum=0;wordNum<30;wordNum++)
		{
			if(Hzk[wordNum].Char[0]== *p && Hzk[wordNum].Char[1]== *(p+1))
			{
				OLED_Set_Pos(x,y);
				for(t=0;t<16;t++)
				{
					OLED_write_iic_dat(Hzk[wordNum].Hex[t]);
				}
				OLED_Set_Pos(x,y+1);
				for(t=0;t<16;t++)
				{
					OLED_write_iic_dat(Hzk[wordNum].Hex[t+16]);
				}
				break;
			}
		}
		p += 2;
		x+=16;
	}
}


void OLED_Show_Temp(unsigned char hang, unsigned char add,unsigned int temp)
{
	OLED_ShowNum(hang,add,temp/100%10+30,1);
	OLED_ShowNum(hang+8,add,temp/10%10+30,1);
	Oled_ShowString(hang+16,add,(uint8_t *)".");					//显示 .
	OLED_ShowNum(hang+24,add,temp/1%10+30,1);
	Oled_ShowCHinese(hang+32,add,(uint8_t *)"℃");					//显示 .
}

void OLED_Show_Humi(unsigned char hang, unsigned char add,unsigned int humi)
{
	OLED_ShowNum(hang,add,humi/10%10+30,1);
	OLED_ShowNum(hang+8,add,humi/1%10+30,1);
	Oled_ShowString(hang+16,add,(uint8_t *)"%");					//显示 .
}

void OLED_Show_Time(unsigned char *TIME)
{
	unsigned char nian,yue,ri,shi,fen,miao,xingqi;
	
	nian = TIME[6]/0x10*10 + TIME[6]%0x10;
	yue = TIME[4]/0x10*10 + TIME[4]%0x10;
	ri = TIME[3]/0x10*10 + TIME[3]%0x10;
	shi = TIME[2]/0x10*10 + TIME[2]%0x10;
	fen = TIME[1]/0x10*10 + TIME[1]%0x10;
	miao = TIME[0]/0x10*10 + TIME[0]%0x10;
	xingqi = TIME[5]/0x10*10 + TIME[5]%0x10;
	OLED_ShowNum(0,0,20,2);				//显示年
	OLED_ShowNum(16,0,nian,2);				//显示年
	Oled_ShowString(32,0,(unsigned char *)"-");					//显示 -
	OLED_ShowNum(40,0,yue,2);				//显示月
	Oled_ShowString(56,0,(unsigned char *)"-");					//显示 -
	OLED_ShowNum(64,0,ri,2);				//显示日

	OLED_ShowNum(0,2,shi,2);				//显示时
	Oled_ShowString(16,2,(unsigned char *)":");					//显示 :
	OLED_ShowNum(24,2,fen,2);				//显示分
	Oled_ShowString(40,2,(unsigned char *)":");					//显示 :
	OLED_ShowNum(48,2,miao,2);			//显示秒

	if(xingqi == 1)
	{
		Oled_ShowString(96,0,(unsigned char *)"Mon ");			//显示星期一英文缩写
	}
	else if(xingqi == 2)
	{
		Oled_ShowString(96,0,(unsigned char *)"Tues");			//显示星期二英文缩写
	}
	else if(xingqi == 3)
	{
		Oled_ShowString(96,0,(unsigned char *)"Wed ");			//显示星期三英文缩写
	}
	else if(xingqi == 4)
	{
		Oled_ShowString(96,0,(unsigned char *)"Thur");			//显示星期四英文缩写
	}
	else if(xingqi == 5)
	{
		Oled_ShowString(96,0,(unsigned char *)"Fri ");			//显示星期五英文缩写
	}
	else if(xingqi == 6)
	{
		Oled_ShowString(96,0,(unsigned char *)"Sat ");			//显示星期六英文缩写
	}
	else
	{
		Oled_ShowString(96,0,(unsigned char *)"Sun ");			//显示星期天英文缩写
	}
}

void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char *p)
{
 	unsigned char x,y;
 	if(y1%8==0) y=y1/8;      
 	else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
   	 	for(x=x0;x<x1;x++)
	    {      
	    	OLED_write_iic_dat(*p++);	    	
	    }
	}
}
void OLED_Drwa_QRCode(void)//二维码显示函数（64X64）
{
	uint16_t i, j;
	for(j=0; j<8; j++)
	{
		OLED_Set_Pos(32, j);
		for(i=0; i<64; i++)
		{
			OLED_write_iic_dat(*(unsigned char *)((QRCode_HEX)+i+j*64));
		}
	}
}
void OLED_Drwa_NUM_Plus(unsigned char x,unsigned char num)//数字显示函数（64X64）
{
	 unsigned char i, j;
	for(j=0; j<8; j++)
	{
		OLED_Set_Pos(x, j);
		for(i=0; i<32; i++)
			OLED_write_iic_dat(ZiMo_64X64[num][j*32 + i]);
	}
}

void point(unsigned char x,unsigned char y)
{
  unsigned char y1,y2;
   y1=y/8;
   y2=y%8;
	 OLED_Set_Pos(x+2,y1);
	 OLED_write_iic_dat(0x01<<y2);
}
