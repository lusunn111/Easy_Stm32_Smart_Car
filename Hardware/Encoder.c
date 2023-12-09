#include "stm32f10x.h" 
#include "OLED.h"

extern volatile int32_t round_cnt;
const int N = 26;
int8_t cnted = 0;

void TIM3_Encoder_Init(void) {
    // ����GPIOA��TIM3ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // ����GPIOA�ĵ�6�ţ�PA6����ΪTIM3������
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // ��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*ʱ����Ԫ��ʼ��*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;            // ����Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���
    TIM_TimeBaseStructure.TIM_Period = 10*N-1;              // ARRֵ��Ϊ12
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // ���ñ������ӿ�
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    // ���벶������
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICFilter = 0xF; // �������벶���˲���
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    // �����ж�
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM3_Encoder_Start(void) {
    TIM_Cmd(TIM3, ENABLE);
		TIM_SetCounter(TIM3, 0);
}

void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
				if(cnted){
				  round_cnt++;
					OLED_ShowString(4,1,"Round:");
					OLED_ShowNum(4,7,round_cnt,6);
				}else{
					++cnted;
				}

        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}
