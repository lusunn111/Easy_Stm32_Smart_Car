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
int8_t flag;//二维码传递的位置
int8_t stop_flag;//是否停止，由K210给出
int8_t time_du;//运行时间，TIM3控制
volatile int8_t left_wheel_value;
volatile int8_t right_wheel_value;
volatile int8_t stop_command;
int8_t fl = 1;
volatile int32_t round_cnt = -1;

int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	Motor_Init();		//直流电机初始化
	Key_Init();			//按键初始化
	LED_Init();//LED初始化
	CountSensor_Init();//金属传感器初始化
	Timer_Init();//定时器初始化
	UART_Init();
	TIM3_Encoder_Init();
	TIM3_Encoder_Start();
	/*显示静态字符串*/
	OLED_ShowString(1, 1, "Speed:+000+000");		//1行1列显示字符串Speed
	OLED_ShowString(2 ,1, "Cnt:");
	OLED_ShowNum(2, 5, CountSensor_Get(), 3);
	OLED_ShowString(3,1,"Time:000");
	OLED_ShowString(4,1,"Round:000000");
	while (1)
	{
		if(stop_flag==1||time_du>=60){
			Motor_SetSpeed(1,0);
			Motor_SetSpeed(0,0);
			Delay_ms(10);
			OLED_ShowString(3,1,"Time:");
			OLED_ShowNum(3,6,time_du,3);
			break;
		}
		KeyNum = Key_GetNum();				//获取按键键码
		if (KeyNum == 1)					//按键1按下
		{
			Speed += 20;					//速度变量自增20
			if (Speed > 100)				//速度变量超过100后
			{
				Speed = -100;				//速度变量变为-100
											//此操作会让电机旋转方向突然改变，可能会因供电不足而导致单片机复位
											//若出现了此现象，则应避免使用这样的操作
			}
		}

//		Speed+=fl;
//		if(Speed>100||Speed<-100)fl*=-1;
		Motor_SetSpeed(1,Speed);				//设置直流电机的速度为速度变量
		Motor_SetSpeed(0,Speed);
		
		//OLED显示速度变量
//		OLED_ShowNum(2, 7, CountSensor_Get(), 5);
	}
}
