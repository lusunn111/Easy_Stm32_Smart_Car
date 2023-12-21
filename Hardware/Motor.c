/*

Author:lusunn111

Function:SET MORTOR

*/
#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "OLED.h"


extern int8_t Speed_A;
extern int8_t Speed_B;
void Motor_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//开启GPIOA的时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	PWM_Init();													//初始化直流电机的底层PWM
}


void Motor_SetSpeed(int8_t sta,int8_t x)//sta为1配置a0-PWMA,0配置a1-PWMB
{
	if(sta){
		Speed_A = x;
		if (Speed_A>= 0)							//如果设置正转的速度值
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_2);	//PA4置高电平
			GPIO_ResetBits(GPIOA, GPIO_Pin_3);	//PA5置低电平，设置方向为正转
			PWM_SetCompare1(Speed_A);			 	//PWM设置为速度值
		}
		else									//否则，即设置反转的速度值
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_2);	//PA4置低电平
			GPIO_SetBits(GPIOA, GPIO_Pin_3);	//PA5置高电平，设置方向为反转
			PWM_SetCompare1(-Speed_A);			//PWM设置为负的速度值，因为此时速度值为负数，而PWM只能给正数
		}

	}else{
		Speed_B = x;
		if (Speed_B>= 0)							//如果设置正转的速度值
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_4);	//PA4置高电平
			GPIO_ResetBits(GPIOA, GPIO_Pin_5);	//PA5置低电平，设置方向为正转
			PWM_SetCompare2(Speed_B);			 	//PWM设置为速度值
		}
		else									//否则，即设置反转的速度值
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_4);	//PA4置低电平
			GPIO_SetBits(GPIOA, GPIO_Pin_5);	//PA5置高电平，设置方向为反转
			PWM_SetCompare2(-Speed_B);			//PWM设置为负的速度值，因为此时速度值为负数，而PWM只能给正数
		}
		
	}
}
