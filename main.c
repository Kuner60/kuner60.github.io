#include "stm32f10x.h"
#include "led.h"
#include "beep.h"
#include "usart.h"
#include "usart2.h"
#include "delay.h"
#include "jdq.h"
#include "ds18b20.h"
#include "LDR.h"
#include "oled.h"
#include "key.h"
#include "Modules.h"
#include "TIM2.h"
#include "TIM3.h"
#include "adcx.h"
#include "flash.h"
#include "fan.h"
#include "bump.h"
#include "water.h"
#include "ph.h"

**********************BEGIN***********************/

#define KEY_Long1	11

#define KEY_1	1
#define KEY_2	2
#define KEY_3	3
#define KEY_4	4

#define FLASH_START_ADDR	0x0801f000	//写入的起始地址

//传感器变量声明
u16 lux;//光照值变量
u16 soil_temp;//土壤温度变量
u16 soil_Humi;//土壤湿度变量
u32 dat;
u16 co2Data;//二氧化碳变量
uint16_t time_num = 0;								//10ms计时


SensorModules sensorData;								//声明传感器数据结构体变量
SensorThresholdValue Sensorthreshold;		//声明传感器阈值结构体变量
DriveModules driveData;									//声明驱动器状态结构体变量
static uint8_t app_cmd = 0;   // 0 无指令  3 开  4 关
uint8_t mode = 0;	//系统模式  1自动  2手动  3设置

//系统静态变量
static uint8_t count_a = 1;  //自动模式按键数
 uint8_t count_m = 1;  //手动模式按键数
static uint8_t count_s = 1;	 //设置模式按键数

extern uint8_t usart2_buf[256];				//串口2接收数组
char display_buf[16];					        //显示数组

/**
  * @brief  显示菜单内容
  * @param  无
  * @retval 无
  */
enum 
{
	AUTO_MODE = 0,
	MANUAL_MODE,
	SETTINGS_MODE
	
}MODE_PAGES;

/**
  * @brief  显示菜单1的固定内容
  * @param  无
  * @retval 无
  */
void OLED_autoPage1(void)		//自动模式菜单第一页
{	
	//显示“光照强度：  Lux”
	OLED_ShowChinese(0,0,3,16,1); //光
	OLED_ShowChinese(16,0,4,16,1);//照
	OLED_ShowChinese(32,0,5,16,1);//强
	OLED_ShowChinese(48,0,2,16,1);//度
	OLED_ShowChar(64,0,':',16,1);
	
	//显示“水域温度：  ℃
	OLED_ShowChinese(0,16,54,16,1); //水
	OLED_ShowChinese(16,16,64,16,1);//域
	OLED_ShowChinese(32,16,0,16,1);//温
	OLED_ShowChinese(48,16,2,16,1);//度
	OLED_ShowChar(64,16,':',16,1);
	
	//显示“PH值： %”
	OLED_ShowString(0,32,"PH",16,1);//PH
	OLED_ShowChinese(24,32,17,16,1);//值
	OLED_ShowChar(64,32,':',16,1);
	
	//显示“CO2浓度： %”
	OLED_ShowString(0,48,"CO2",16,1);//CO2
	OLED_ShowChinese(32,48,8,16,1);	//浓
	OLED_ShowChinese(48,48,2,16,1);	//度
	OLED_ShowChar(64,48,':',16,1);
	
	OLED_Refresh();
	
}

void OLED_autoPage2(void)   //自动模式菜单第二页
{
	
	//显示“水位： ”
	OLED_ShowChinese(0,0,54,16,1); //水
	OLED_ShowChinese(16,0,59,16,1);//位
	OLED_ShowChar(64,0,':',16,1);
	
	
	OLED_Refresh();
}

void SensorDataDisplay1(void)		//传感器数据显示第一页
{
	uint8_t ph_point = sensorData.ph*10                                                                                                                                                                         ;
	//显示光照强度数据
	OLED_ShowNum(90,0,sensorData.lux,3,16,1);
	//显示土壤温度数据
	OLED_ShowNum(90,16, sensorData.temp,2,16,1);	
	//显示PH值数据
	OLED_ShowNum(90,32,sensorData.ph,1,16,1);
	OLED_ShowString(98,32,".",16,1);//"."
	OLED_ShowNum(106,32,ph_point%10,1,16,1);
  //显示二氧化碳数据
	OLED_ShowNum(90,48,sensorData.co2Data,4,16,1);
}

void SensorDataDisplay2(void)		//传感器数据显示第二页
{
	//显示水位数据
	if(sensorData.water==0)
	{
		//缺水
		OLED_ShowChinese(90,0,62,16,1);
		OLED_ShowChinese(106,0,63,16,1);
	}
	else
	{
		//正常
		OLED_ShowChinese(90,0,60,16,1);
		OLED_ShowChinese(106,0,61,16,1);
	}
}

/**
  * @brief  显示手动模式设置界面1
  * @param  无
  * @retval 无
  */
void OLED_manualPage1(void)
{
	//显示“灯光：”
	OLED_ShowChinese(16,0,18,16,1);	
	OLED_ShowChinese(32,0,19,16,1);	
	OLED_ShowChar(64,0,':',16,1);

	//显示“风扇：”
	OLED_ShowChinese(16,16,20,16,1);	
	OLED_ShowChinese(32,16,21,16,1);	
	OLED_ShowChar(64,16,':',16,1);
	
	//显示“加热：”
	OLED_ShowChinese(16,32,43,16,1);	
	OLED_ShowChinese(32,32,44,16,1);	
	OLED_ShowChar(64,32,':',16,1);
	
	//显示“加水：”
	OLED_ShowChinese(16,48,43,16,1);	
	OLED_ShowChinese(32,48,22,16,1);	
	OLED_ShowChar(64,48,':',16,1);
}

/**
  * @brief  显示手动模式设置界面2
  * @param  无
  * @retval 无
  */
void OLED_manualPage2(void)
{
	//显示“中和：”
	OLED_ShowChinese(16,0,68,16,1);	
	OLED_ShowChinese(32,0,69,16,1);		
	OLED_ShowChar(64,0,':',16,1);
}


/**
  * @brief  显示手动模式设置参数界面1
  * @param  无
  * @retval 无
  */
void ManualSettingsDisplay1(void)
{
	if(driveData.LED_Flag)
	{
		OLED_ShowChinese(96,0,24,16,1); 	//开
	}
	else
	{
		OLED_ShowChinese(96,0,26,16,1); 	//关
	}
	
	if(driveData.Fan_Flag)
	{
		OLED_ShowChinese(96,16,24,16,1); 	//开
	}
	else
	{
		OLED_ShowChinese(96,16,26,16,1); 	//关
	}
	
	if(driveData.Jdq_Flag)
	{
		OLED_ShowChinese(96,32,24,16,1); 	//开
	}
	else
	{
		OLED_ShowChinese(96,32,26,16,1); 	//关
	}
		if(driveData.Bump1_Flag)
	{
		OLED_ShowChinese(96,48,24,16,1); 	//开
	}
	else
	{
		OLED_ShowChinese(96,48,26,16,1); 	//关
	}
}

/**
  * @brief  显示手动模式设置参数界面2
  * @param  无
  * @retval 无
  */
void ManualSettingsDisplay2(void)
{
	
	if(driveData.Bump2_Flag)
	{
		OLED_ShowChinese(96,0,24,16,1); 	//开
	}
	else
	{
		OLED_ShowChinese(96,0,26,16,1); 	//关
	}
}

/**
  * @brief  显示系统阈值设置界面1
  * @param  无
  * @retval 无
  */
void OLED_settingsPage1(void)
{
	//显示“光照阈值”
	OLED_ShowChinese(16,0,3,16,1);	
	OLED_ShowChinese(32,0,4,16,1);	
	OLED_ShowChinese(48,0,16,16,1);	
	OLED_ShowChinese(64,0,17,16,1);	
	OLED_ShowChar(80,0,':',16,1);
	
	//显示“水温上限”
	OLED_ShowChinese(16,16,22,16,1);	
	OLED_ShowChinese(32,16,0,16,1);	
	OLED_ShowChinese(48,16,65,16,1);	
	OLED_ShowChinese(64,16,66,16,1);	
	OLED_ShowChar(80,16,':',16,1);
	
	//显示“水温下限”
	OLED_ShowChinese(16,32,22,16,1);	
	OLED_ShowChinese(32,32,0,16,1);	
	OLED_ShowChinese(48,32,67,16,1);	
	OLED_ShowChinese(64,32,66,16,1);	
	OLED_ShowChar(80,32,':',16,1);
	
	//显示“CO2阈值”
	OLED_ShowString(16,48,"CO2",16,1);
	OLED_ShowChinese(48,48,16,16,1);	
	OLED_ShowChinese(64,48,17,16,1);	
	OLED_ShowChar(80,48,':',16,1);
	
}

/**
  * @brief  显示系统阈值设置界面2
  * @param  无
  * @retval 无
  */
void OLED_settingsPage2(void)
{
	//显示“PH上限”
	OLED_ShowString(16,0,"PH",16,1);
	OLED_ShowChinese(32,0,65,16,1);	
	OLED_ShowChinese(48,0,66,16,1);	
	OLED_ShowChar(80,0,':',16,1);
	
	//显示“PH下限”
	OLED_ShowString(16,16,"PH",16,1);
	OLED_ShowChinese(32,16,67,16,1);	
	OLED_ShowChinese(48,16,66,16,1);	
	OLED_ShowChar(80,16,':',16,1);
	
}


void SettingsThresholdDisplay1(void)
{
	//显示光照强度阈值数值
	OLED_ShowNum(90, 0, Sensorthreshold.luxValue, 3,16,1);
	//显示水温上限数值
	OLED_ShowNum(90, 16, Sensorthreshold.tempValue_H , 2,16,1);
	//显示水温下限数值
	OLED_ShowNum(90, 32, Sensorthreshold.tempValue_L, 2,16,1);
	//显示CO2浓度阈值数值
	OLED_ShowNum(90, 48, Sensorthreshold.co2DataValue, 3,16,1);
}
void SettingsThresholdDisplay2(void)
{
	//显示PH上限数值
		OLED_ShowNum(90, 0, Sensorthreshold.PHValue_H,2,16,1);
	//显示PH下限数值
	OLED_ShowNum(90, 16, Sensorthreshold.PHValue_L,2,16,1);
}

/**
  * @brief  记录自动模式界面下按KEY2的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetAuto(void)  
{
	if(KeyNum == KEY_2)
	{
		KeyNum = 0;
		count_a++;
		if (count_a > 2)
		{
			count_a = 1;
		}
		OLED_Clear();
	}
	return count_a;
}


/**
  * @brief  记录手动模式界面下按KEY2的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetManual(void)  
{

	if(KeyNum == KEY_2)
	{
		KeyNum = 0;
		count_m++;
		if (count_m == 5)
		{
			OLED_Clear();
		}
		if (count_m > 5)  		//一共可以控制的外设数量
		{
			OLED_Clear();
			count_m = 1;
		}
	}
	return count_m;
}

/**
  * @brief  记录阈值界面下按KEY2的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetSelection(void)
{
	if(KeyNum == KEY_2)
	{
		KeyNum = 0;
		count_s++;
		if (count_s == 5)
		{
			OLED_Clear();
		}
		if (count_s > 6)  		//一共可以控制的外设数量
		{
			OLED_Clear();
			count_s = 1;
		}
	}
	return count_s;
}


/**
  * @brief  显示手动模式界面的选择符号
  * @param  num 为显示的位置
  * @retval 无
  */
void OLED_manualOption(uint8_t num)
{
	switch(num)
	{
		case 1:	
			OLED_ShowChar(0, 0,'>',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 2:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,'>',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 3:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,'>',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 4:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,'>',16,1);
			break;
		case 5:	
			OLED_ShowChar(0, 0,'>',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		default: break;
	}
}

/**
  * @brief  显示阈值界面的选择符号
  * @param  num 为显示的位置
  * @retval 无
  */
void OLED_settingsOption(uint8_t num)
{
	switch(num)
	{
		case 1:	
			OLED_ShowChar(0, 0,'>',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 2:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,'>',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 3:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,'>',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 4:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,'>',16,1);
			break;
		case 5:	
			OLED_ShowChar(0, 0,'>',16,1);
			OLED_ShowChar(0,16,' ',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		case 6:	
			OLED_ShowChar(0, 0,' ',16,1);
			OLED_ShowChar(0,16,'>',16,1);
			OLED_ShowChar(0,32,' ',16,1);
			OLED_ShowChar(0,48,' ',16,1);
			break;
		default: break;
	}
}

/**
  * @brief  自动模式控制函数
  * @param  无
  * @retval 无
  */
void AutoControl(void)
{
	if(sensorData.lux<Sensorthreshold.luxValue)
		driveData.LED_Flag = 1;
	else
		driveData.LED_Flag = 0;
	if(sensorData.temp>Sensorthreshold.tempValue_H || sensorData.co2Data>Sensorthreshold.co2DataValue)
	{
		driveData.Fan_Flag = 1;
		driveData.Beep_Flag = 1;
	}
	else
	{
		driveData.Fan_Flag = 0;
		driveData.Beep_Flag = 0;
	}
	if(sensorData.temp<Sensorthreshold.tempValue_L)
		driveData.Jdq_Flag = 1;
	else
		driveData.Jdq_Flag = 0;
	if(sensorData.water)
		driveData.Bump1_Flag = 0;
	else
		driveData.Bump1_Flag = 1;
	if(sensorData.ph<Sensorthreshold.PHValue_L || sensorData.ph>Sensorthreshold.PHValue_H)
		driveData.Bump2_Flag = 1;
	else
		driveData.Bump2_Flag = 0;
}

/**
  * @brief  手动模式控制函数
  * @param  无
  * @retval 无
  */
void ManualControl(uint8_t num)
{
	if(app_cmd)
    {
        KeyNum  = app_cmd;   // 伪造一次按键值
        app_cmd = 0;         // 立即清掉，只执行一次
    }
		
	switch(num)
	{
		case 1:	
			//显示外设开关
			if(KeyNum == KEY_3)
				driveData.LED_Flag = 1;
			if(KeyNum == KEY_4)
				driveData.LED_Flag = 0;
			break;
		case 2:	
			if(KeyNum == KEY_3)
				driveData.Fan_Flag = 1 ;
			if(KeyNum == KEY_4)
				driveData.Fan_Flag = 0;
			break;
		case 3:	
			if(KeyNum == KEY_3)
				driveData.Jdq_Flag = 1;
			if(KeyNum == KEY_4)
				driveData.Jdq_Flag = 0;
			break;
		case 4:	
			if(KeyNum == KEY_3)
				driveData.Bump1_Flag = 1;
			if(KeyNum == KEY_4)
				driveData.Bump1_Flag = 0;
			break;
	  case 5:	
			if(KeyNum == KEY_3)
				driveData.Bump2_Flag = 1;
			if(KeyNum == KEY_4)
				driveData.Bump2_Flag = 0;
			break;
		default: break;
	}

}

/**
  * @brief  控制函数
  * @param  无
  * @retval 无
  */
void Control_Manager(void)
{
	if(driveData.LED_Flag)
	{
		LED_On();
	}
	else
	{
		LED_Off();
	}
	if(driveData.Fan_Flag)
	{
		FAN_ON;
	}
	else
	{
		FAN_OFF;
	}
	
	if(driveData.Bump1_Flag)
	{
		BUMP1_ON;
	}
	else
	{
		BUMP1_OFF;
	}
	if(driveData.Bump2_Flag)
	{
		BUMP2_ON;
	}
	else
	{
		BUMP2_OFF;
	}
	
	if(driveData.Jdq_Flag)
	{
		JDQ_ON;
	}
	else
	{
		JDQ_OFF;
	}
	
	if(driveData.Beep_Flag)
	{
		BEEP_On();
	}
	else
	{
		BEEP_Off();
	}

}

/**
  * @brief  阈值设置函数
  * @param  无
  * @retval 无
  */
void ThresholdSettings(uint8_t num)
{
	switch (num)
	{	
		case 1:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				Sensorthreshold.luxValue += 10;
				if (Sensorthreshold.luxValue > 200)
				{
					Sensorthreshold.luxValue = 200;
				}
			}
			else if (KeyNum == KEY_4)
			{
				KeyNum = 0;
				Sensorthreshold.luxValue -= 10;
				if (Sensorthreshold.luxValue < 10)
				{
					Sensorthreshold.luxValue = 10;
				}				
			}	
			break;
			
		case 2:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				Sensorthreshold.tempValue_H += 1;
				if (Sensorthreshold.tempValue_H > 50)
				{
					Sensorthreshold.tempValue_H = 50;
				}
			}
			else if (KeyNum == KEY_4)
			{
				KeyNum = 0;
				if (Sensorthreshold.tempValue_H > (Sensorthreshold.tempValue_L+1))
				{
					Sensorthreshold.tempValue_H -= 1;
				}				
			}
			break;
			
		case 3:
			if (KeyNum == KEY_3)
			{
				KeyNum = 0;
				if (Sensorthreshold.tempValue_L < (Sensorthreshold.tempValue_H-1))
				{
					Sensorthreshold.tempValue_L+= 1;
				}
			}
			else if (KeyNum == KEY_4)
			{
				KeyNum = 0;
				Sensorthreshold.tempValue_L -= 1;
				if (Sensorthreshold.tempValue_L < 5)
				{
					Sensorthreshold.tempValue_L = 5;
				}				
			}
			break;
			
			case 4:
			if (KeyNum == KEY_3)
			{ 
				KeyNum = 0;
				Sensorthreshold.co2DataValue += 20;
				if (Sensorthreshold.co2DataValue > 999)
				{
					Sensorthreshold.co2DataValue = 999;
				}
			}
			else if (KeyNum == KEY_4)
			{
				KeyNum = 0;
				Sensorthreshold.co2DataValue -= 20;
				if (Sensorthreshold.co2DataValue < 300)
				{
					Sensorthreshold.co2DataValue = 300;
				}				
			}
			break;
			case 5:
			if (KeyNum == KEY_3)
			{ 
				KeyNum = 0;
				Sensorthreshold.PHValue_H += 1;
				if (Sensorthreshold.PHValue_H> 12)
				{
					Sensorthreshold.PHValue_H = 12;
				}
			}
			else if (KeyNum == KEY_4)
			{
				KeyNum = 0;
				Sensorthreshold.PHValue_H -= 1;
				if (Sensorthreshold.PHValue_H <8)
				{
					Sensorthreshold.PHValue_H = 8;
				}				
			}
			break;
			case 6:
			if (KeyNum == KEY_3)
			{ 
				KeyNum = 0;
				Sensorthreshold.PHValue_L += 1;
				if (Sensorthreshold.PHValue_L> 6)
				{
					Sensorthreshold.PHValue_L = 6;
				}
			}
			else if (KeyNum == KEY_4)
			{
				KeyNum = 0;
				Sensorthreshold.PHValue_L -= 1;
				if (Sensorthreshold.PHValue_L <1)
				{
					Sensorthreshold.PHValue_L = 1;
				}				
			}
			break;
		default: break;		
	}
}

/****
*******蓝牙控制函数
*****/
void Bluetooth_Control(void)
{
		if(time_num % 10 == 0)							//发送数据
		{
			UsartPrintf(USART2,"光照：%dLux\r\n",sensorData.lux);
			UsartPrintf(USART2,"水温：%dC\r\n",sensorData.temp);
			UsartPrintf(USART2,"PH值：%.1f\r\n",sensorData.ph);
			UsartPrintf(USART2,"CO2：%dppm\r\n",sensorData.co2Data);
			if(sensorData.water)
				UsartPrintf(USART2,"水位：正常\r\n");
			else
				UsartPrintf(USART2,"水位：偏低\r\n");
		}
		if(USART2_WaitRecive() == 0)				//如果接收到蓝牙数据
		{
			if(usart2_buf[0] == 'A')
			{
				mode = !mode;
				if(mode)
				{
					count_m = 1;
					OLED_Clear();
				}
				else
				{
					count_a = 1;
					OLED_Clear();				
				}
				USART2_Clear();
			}
			
			if(mode == 1)  //手动模式下
			{	
				switch(usart2_buf[0])
				{
					case('B'):
						count_m = 1;					
						OLED_Clear();
						if(driveData.LED_Flag)
							 app_cmd = 4;
						else
							 app_cmd = 3;  
					break;
					
					case('C'):
						count_m = 2;					
						OLED_Clear();
						if(driveData.Fan_Flag)
							 app_cmd = 4;
						else
							 app_cmd = 3;
					break;
					case('D'):	
						count_m = 3;					
						OLED_Clear();
						if(driveData.Jdq_Flag)
							 app_cmd = 4;
						else
							 app_cmd = 3;
					break;
					
					case('E'):
						count_m = 4;					
						OLED_Clear();
						if(driveData.Bump1_Flag)
							 app_cmd = 4;
						else
							 app_cmd = 3;
					break;
					
					case('F'):	
						count_m = 5 ;					
						OLED_Clear();
						if(driveData.Bump2_Flag)
							 app_cmd = 4;
						else
							 app_cmd = 3;
					break;
					default:
					break;
				}	
			}
		}
}
/**
  * @brief  flash阈值读取函数
  * @param  无
  * @retval 无
  */
void FLASH_ReadThreshold()
{
	Sensorthreshold.luxValue	= FLASH_R(FLASH_START_ADDR);	//从指定页的地址读FLASH
	Sensorthreshold.tempValue_H = FLASH_R(FLASH_START_ADDR+2);	//从指定页的地址读FLASH
	Sensorthreshold.tempValue_L = FLASH_R(FLASH_START_ADDR+4);	//从指定页的地址读FLASH
	Sensorthreshold.co2DataValue = FLASH_R(FLASH_START_ADDR+6);	//从指定页的地址读FLASH
	Sensorthreshold.PHValue_H = FLASH_R(FLASH_START_ADDR+8);	//从指定页的地址读FLASH
	Sensorthreshold.PHValue_L = FLASH_R(FLASH_START_ADDR+10);	//从指定页的地址读FLASH
	if(Sensorthreshold.tempValue_H > 50 || Sensorthreshold.tempValue_L > 20 ||
		Sensorthreshold.luxValue > 200|| Sensorthreshold.co2DataValue > 800 || 
		Sensorthreshold.PHValue_H > 12 || Sensorthreshold.PHValue_L < 2)//flash读取乱码重置flash中的数值
	{
		FLASH_W(FLASH_START_ADDR,130,28,12,500,8,4);
	}
}

int main(void)
{ 
  SystemInit();//配置系统时钟为72M	
	delay_init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //配置中断优先分组
	
	//传感器初始化
	LDR_Init();//光敏电阻初始化
	DS18B20_Init();//ds18b20初始化
	WATER_Init();
	PH_Init();
	
	//执行器初始化
	LED_Init();//大功率LED灯初始化
	FAN_Init();//风扇初始化
	BUMP1_Init();//水泵1初始化
	BUMP2_Init();//水泵2初始化
	BEEP_Init();//蜂鸣器初始化
	JDQ_Init();//加热片初始化
	
	Key_Init();//按键初始化
	OLED_Init();//OLED初始化
	USART1_Config();//串口1初始化
	USART2_Config();//串口2初始化
	USART3_Config();//串口3初始化
  TIM2_Init(72-1,1000-1);//定时器2初始化
	FLASH_ReadThreshold();//阈值读取函数
////	TIM3_Int_Init(1000-1,72-1);		//定时1ms中断
	
	OLED_Clear();//清屏
  while (1)
  {
		SensorScan();	//获取传感器数据
		printf("当前水温%d\n",sensorData.temp);
		printf("当前光照%d\n",sensorData.lux);
		printf("当前PH值%.1f\n",sensorData.ph);
		printf("当前水位%d\n",sensorData.water);
		printf("当前CO2值%d\n",sensorData.co2Data);
		time_num++;												//计时变量+1
		if(time_num >= 5000)
		{
			time_num = 0;
		}
		Bluetooth_Control();//蓝牙控制函数
		
		switch(mode)
		{
			case AUTO_MODE://默认自动模式
					if(SetAuto() ==1 )//自动模式下按键2按下的次数等于1
				{
					OLED_autoPage1();	//显示主页面1固定信息
					SensorDataDisplay1();	//显示传感器1数据
				}
				else
				{
					OLED_autoPage2();	//显示主页面2固定信息
					SensorDataDisplay2();	//显示传感器2数据
				}
				AutoControl();//自动模式控制函数
				
				/*按键1按下时切换模式*/
				if (KeyNum == KEY_1)   //系统模式mode  1自动  2手动  3设置
				{
					KeyNum = 0;
					mode = MANUAL_MODE;//进入手动模式
					count_m = 1;//手动模式下按键2的次数置一
					OLED_Clear();
				}
				
				if (KeyNum == KEY_Long1)
				{
					KeyNum = 0;
					mode = SETTINGS_MODE;//进入阈值设置模式
					count_s = 1;//阈值设置模式下按键2按下的次数置一
					OLED_Clear();
				}
				
				Control_Manager();//执行器执行函数
				
				break;
				
			case MANUAL_MODE://手动模式下
				OLED_manualOption(SetManual());//手动模式光标显示函数
				ManualControl(SetManual());//手动模式控制函数
				if (SetManual() <= 4)		//手动模式下按键2按下的次数小于等于4次
				{	
					OLED_manualPage1();//手动模式界面1显示
					ManualSettingsDisplay1();//手动模式界面1的开关显示
				}
				else//手动模式下按键2按下的次数大于4次
				{
					OLED_manualPage2();//手动模式界面2显示
					ManualSettingsDisplay2();//手动模式界面2的开关显示
				}
				
				if (KeyNum == KEY_1)   //系统模式mode  0手动  1自动（默认）
				{
					KeyNum = 0;
					mode = AUTO_MODE;//进入自动模式
					count_a = 1;//自动模式下按键2按下的次数置一
					OLED_Clear();
				}
				Control_Manager();//执行器执行函数
				
				break;
				
			case SETTINGS_MODE://阈值设置函数

				OLED_settingsOption(SetSelection());	//实现阈值设置页面的选择功能
				ThresholdSettings(SetSelection());	//实现阈值调节功能	
					
				if (SetSelection() <= 4)	
				{
					OLED_settingsPage1();	//显示阈值设置界面1固定信息
					SettingsThresholdDisplay1();	//显示传感器阈值1数据	
				}
				else
				{
					OLED_settingsPage2();	//显示阈值设置界面2固定信息
					SettingsThresholdDisplay2();	//显示传感器阈值2数据
				}
					//判断是否退出阈值设置界面
					if (KeyNum == KEY_1)
					{
						KeyNum = 0;
						mode = AUTO_MODE;	//进入自动模式
						count_a = 1;//自动模式下按键2按下的次数置一
						OLED_Clear();	//清屏
						//存储修改的传感器阈值至flash内
						FLASH_W(FLASH_START_ADDR, Sensorthreshold.luxValue,
						Sensorthreshold.tempValue_H,Sensorthreshold.tempValue_L, Sensorthreshold.co2DataValue,
						Sensorthreshold.PHValue_H,Sensorthreshold.PHValue_L);
					}
				break;
				default: break;
		}
		
		
		
  }
}
