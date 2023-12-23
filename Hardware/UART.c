/*

Author:lusunn111

Function:GET PWM AND FLAG

*/
#include "stm32f10x.h"
#include "Motor.h"
#include "LED.h"
#include "Delay.h"
// ��ʼ��UART�ĺ���
void UART_Init(void) {
    USART_InitTypeDef USART_InitStructure; // �������ڳ�ʼ��USART�Ľṹ��
    GPIO_InitTypeDef GPIO_InitStructure; // �������ڳ�ʼ��GPIO�Ľṹ��
    NVIC_InitTypeDef NVIC_InitStructure; // �������ڳ�ʼ���жϿ������Ľṹ��

    // 1. ʹ��ʱ��
    // ����GPIOA��USART1��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 2. ����USART1 Tx (PA.09) �� Rx (PA.10)��GPIO
    // ����PA9��ΪUSART1��Tx�����ͣ�����
     // ����GPIO�ٶ�

    // ����PA10��ΪUSART1��Rx�����գ�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PA.10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ����Ϊ��������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // Ӧ������

    // 3. ����USART1
    // ����USART1�Ĳ����������ʡ�����λ��ֹͣλ��У��λ��
    USART_InitStructure.USART_BaudRate = 115200; // ������9600
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // ����λ8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No; // ��У��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ��������
    USART_InitStructure.USART_Mode = USART_Mode_Rx; // ���ý��պͷ���
    USART_Init(USART1, &USART_InitStructure); // Ӧ��USART����

    // 4. ����USART1�ж�
    // ����USART1�жϵ����ȼ���ʹ��
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; // USART1�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // �����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // ʹ���ж�ͨ��
    NVIC_Init(&NVIC_InitStructure); // Ӧ���ж�����

    // 5. ʹ��USART1�����ж�
    // ʹ��USART1�Ľ����жϣ����յ�����ʱ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // 6. ʹ��USART1
    // ����USART1ģ��
    USART_Cmd(USART1, ENABLE);
}

void UART_STOP(void){
	    // 1. ���� USART1 ���պͷ���
    USART_Cmd(USART1, DISABLE);

    // 2. �ر� USART1 ���ж�
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    NVIC_DisableIRQ(USART1_IRQn);

    // 3. �� USART1 �Ĵ�����λ��Ĭ��ֵ
    USART_DeInit(USART1);  // �����ڱ�׼�����
    // ��ʹ�� HAL_UART_DeInit(&huart1);  // �����ʹ�õ��� HAL ��

    // 4. (��ѡ) ���� USART1 ��ص� GPIO ����
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ����Ϊ��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 5. (��ѡ) �ر� USART1 ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
}
void UART_START(void){
	UART_Init();
}	
// ���ջ�����������
volatile int8_t rx_buffer[7]; // �������ڴ洢�������ݵĻ�����
volatile int rx_index = 0; // �������ݵĵ�ǰ����

// ���ڴ洢����������ݵ��ⲿ����
extern volatile int8_t left_wheel_value;//����PWMB
extern volatile int8_t right_wheel_value;//����PWMA 
extern volatile int8_t stop_command;
extern int8_t QR_flag;

// ������յ������ݵĺ���
// ȫ�ֱ������ڴ洢QR_flag��״̬

void ProcessData(volatile int8_t data[]) {
    left_wheel_value = data[2]; // �������ֵ�ֵ
    right_wheel_value = data[3]; // �������ֵ�ֵ
    if (!QR_flag) {
        QR_flag = (data[5]); // ����6λ����0x03������������õ�ǰ��λ
    }
    if(!stop_command)stop_command = data[4]; // ����5λ����0x01������������õ�����λ
		Motor_SetSpeed(1,right_wheel_value);
		Motor_SetSpeed(0,left_wheel_value);
//		LED2_ON();
//		Delay_ms(2);
//		LED2_OFF();
}


// USART1�жϷ�������
void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        // ��USART��������
        uint8_t data = USART_ReceiveData(USART1);

        // ����Ԥ����Э���������
        if (rx_index == 0 && data == 0x2C) { // ���֡ͷ�ĵ�һ���ֽ�
            rx_buffer[rx_index++] = data;
        } else if (rx_index == 1 && data == 0x12) { // ���֡ͷ�ĵڶ����ֽ�
            rx_buffer[rx_index++] = data;
        } else if (rx_index >= 2 && rx_index < 6) { // �������ݲ���
            rx_buffer[rx_index++] = data;
        } else if (rx_index == 6 && data == 0x5B) { // ���֡β
            rx_buffer[rx_index] = data;
            ProcessData(rx_buffer); // ������յ�����������֡
            rx_index = 0; // ����������׼��������һ֡����
        } else {
            rx_index = 0; // ������ݲ�����Ԥ�ڣ���������
        }
    }
}
