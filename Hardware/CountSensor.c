#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"
#include "Motor.h"
#include "LED.h"
uint16_t CountSensor_Count;				//ȫ�ֱ��������ڼ���

/**
  * ��    ����������������ʼ��
  * ��    ������
  * �� �� ֵ����
  */
extern int8_t flag; 
extern int8_t time_du;
void CountSensor_Init(void)
{
	/*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//����GPIOB��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//����AFIO��ʱ�ӣ��ⲿ�жϱ��뿪��AFIO��ʱ��
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//��PB14���ų�ʼ��Ϊ��������
	
	/*AFIOѡ���ж�����*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);//���ⲿ�жϵ�14����ӳ�䵽GPIOB����ѡ��PB14Ϊ�ⲿ�ж�����
	
	/*EXTI��ʼ��*/
	EXTI_InitTypeDef EXTI_InitStructure;						//����ṹ�����
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;					//ѡ�������ⲿ�жϵ�14����
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					//ָ���ⲿ�ж���ʹ��
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//ָ���ⲿ�ж���Ϊ�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//ָ���ⲿ�ж���Ϊ�½��ش���
	EXTI_Init(&EXTI_InitStructure);								//���ṹ���������EXTI_Init������EXTI����
	
	/*NVIC�жϷ���*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//����NVICΪ����2
																//����ռ���ȼ���Χ��0~3����Ӧ���ȼ���Χ��0~3
																//�˷������������������н������һ��
																//���ж���жϣ����԰Ѵ˴������main�����ڣ�whileѭ��֮ǰ
																//�����ö�����÷���Ĵ��룬���ִ�е����ûḲ����ִ�е�����
	
	/*NVIC����*/
	NVIC_InitTypeDef NVIC_InitStructure;						//����ṹ�����
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		//ѡ������NVIC��EXTI15_10��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ָ��NVIC��·ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//ָ��NVIC��·����ռ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//ָ��NVIC��·����Ӧ���ȼ�Ϊ1
	NVIC_Init(&NVIC_InitStructure);								//���ṹ���������NVIC_Init������NVIC����
}

/**
  * ��    ������ȡ�����������ļ���ֵ
  * ��    ������
  * �� �� ֵ������ֵ����Χ��0~65535
  */
uint16_t CountSensor_Get(void)
{
	return CountSensor_Count;
}

/**
  * ��    ����EXTI15_10�ⲿ�жϺ���
  * ��    ������
  * �� �� ֵ����
  * ע������˺���Ϊ�жϺ�����������ã��жϴ������Զ�ִ��
  *           ������ΪԤ����ָ�����ƣ����Դ������ļ�����
  *           ��ȷ����������ȷ���������κβ��죬�����жϺ��������ܽ���
  */
void EXTI15_10_IRQHandler(void)
{
	Delay_ms(30);
	if (EXTI_GetITStatus(EXTI_Line14) == SET)		//�ж��Ƿ����ⲿ�ж�14���ߴ������ж�
	{
		/*��������������������󣬿��ٴ��ж����ŵ�ƽ���Ա��ⶶ��*/
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
		{
			CountSensor_Count ++;					//����ֵ����һ��
			OLED_ShowString(2 ,1, "Cnt:");//����OLED��ʾ
			OLED_ShowNum(2, 5, CountSensor_Get(), 3);//
			if(CountSensor_Count<=3&&CountSensor_Count!=flag){//��������λ��
				for(int i = 1;i<=10;++i){
					LED1_ON();
					Delay_ms(40);
//					LED1_OFF();
//					Delay_ms(20);
				}
				LED1_OFF();
			}else if(CountSensor_Count==flag){//��ά���־����λ��
				Motor_SetSpeed(1,0);
				Motor_SetSpeed(0,0);
				for(int i = 1;i*20<=2000;++i){
					LED1_ON();
					Delay_ms(20);
				}
				LED1_OFF();
				time_du+=2;
				Motor_SetSpeed(1,100);
				Motor_SetSpeed(0,100);
			}else if(CountSensor_Count==4){//Bλ��
				Motor_SetSpeed(1,0);
				Motor_SetSpeed(0,0);
				for(int i = 1;i*50<=5000;++i){
					LED1_ON();
					Delay_ms(25);
					LED1_OFF();
					Delay_ms(25);
				}
				time_du+=5;
				Motor_SetSpeed(1,100);
				Motor_SetSpeed(0,100);
			}
		}
		Delay_ms(30);
		EXTI_ClearITPendingBit(EXTI_Line14);		//����ⲿ�ж�14���ߵ��жϱ�־λ
													//�жϱ�־λ�������
													//�����жϽ��������ϵش�����������������
	}
}
