/*

Author:lusunn111

Function:MAIN

*/
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"
#include "Key.h"
#include "LED.h"
#include "Timer.h"
#include "CountSensor.h"
#include "UART.h"
#include "Encoder.h"

uint8_t KeyNum;		//定义用于接收按键键码的变量
int8_t Speed_A;		//w定义速度变量
int8_t Speed_B;
int8_t Speed;
int8_t QR_flag;//二维码传递的位置
int8_t time_du;//运行时间，TIM3控制
volatile int8_t left_wheel_value;
volatile int8_t right_wheel_value;
volatile int8_t stop_command;
uint16_t CountSensor_Count;
int8_t fl = 1;
volatile int32_t round_cnt = -1;
int Flag1;
int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	Key_Init();			//按键初始化
	LED_Init();			//LED初始化

	while(1){
		KeyNum = Key_GetNum();	
		if(KeyNum){
			LED1_ON();
			Delay_ms(1000);
			LED1_OFF();
			KeyNum = 0;
			break;
		}
	}
	
	OLED_ShowString(1,1,"READAY!");
	
	UART_Init();
	
	while(1){
		if(QR_flag){
			UART_STOP();
			break;
		}
	}
//	UART_STOP();
	OLED_Clear();
	OLED_ShowString(1,1,"QR_READAY!");
	OLED_ShowNum(2,1,QR_flag,1);
	while(1){
		KeyNum = Key_GetNum();
		if(KeyNum){
			LED1_ON();
			Delay_ms(100);
			LED1_OFF();
			KeyNum = 0;
			time_du = 0;
			stop_command = 0;
			break;
		}
	}

	OLED_Init();		//OLED初始化
	Key_Init();			//按键初始化
	LED_Init();			//LED初始化
	UART_START();
	Motor_Init();		//直流电机初始化	
	CountSensor_Init();//金属传感器初始化
	Timer_Init();//定时器初始化
	TIM3_Encoder_Init();
	TIM3_Encoder_Start();
	OLED_NEW();
//	Motor_SetSpeed(1,75);
//	Motor_SetSpeed(0,75);
	Delay_ms(100);
	time_du = 0;
	CountSensor_Count = 0;
	stop_command = 0;
	
	while (1)  
	{
		
//		if(time_du>60||stop_command==2){//这个代码绝对不能修改！
		
		if(stop_command){//这个代码绝对不能修改！
			UART_STOP();
			Delay_ms(500);
//			OLED_NEW(); 
//			OLED_Clear();
			
			Motor_SetSpeed(1,0);
			Motor_SetSpeed(0,0);
			break;
		}
		//按键测试代码开始
//		KeyNum = Key_GetNum();				//获取按键键码
//		if (KeyNum == 1)					//按键1按下   
//		{
//			Speed += 20;					//速度变量自增20
//			if (Speed > 100)				//速度变量超过100后
//			{
//				Speed = -100;				//速度变量变为-100
//			}
//		}
//		Motor_SetSpeed(1,Speed);				//设置直流电机的速度为速度变量
//		Motor_SetSpeed(0,Speed);
		//按键测试代码结束
	}

}
