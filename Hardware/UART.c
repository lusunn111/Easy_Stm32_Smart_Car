/*

Author:lusunn111

Function:GET PWM AND FLAG

*/
#include "stm32f10x.h"
#include "Motor.h"
#include "LED.h"
#include "Delay.h"
// 初始化UART的函数
void UART_Init(void) {
    USART_InitTypeDef USART_InitStructure; // 定义用于初始化USART的结构体
    GPIO_InitTypeDef GPIO_InitStructure; // 定义用于初始化GPIO的结构体
    NVIC_InitTypeDef NVIC_InitStructure; // 定义用于初始化中断控制器的结构体

    // 1. 使能时钟
    // 启用GPIOA和USART1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 2. 配置USART1 Tx (PA.09) 和 Rx (PA.10)的GPIO
    // 设置PA9作为USART1的Tx（发送）引脚
     // 设置GPIO速度

    // 设置PA10作为USART1的Rx（接收）引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PA.10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 设置为浮空输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 应用设置

    // 3. 配置USART1
    // 设置USART1的参数：波特率、数据位、停止位、校验位等
    USART_InitStructure.USART_BaudRate = 115200; // 波特率9600
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 数据位8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx; // 启用接收和发送
    USART_Init(USART1, &USART_InitStructure); // 应用USART设置

    // 4. 配置USART1中断
    // 设置USART1中断的优先级并使能
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; // USART1中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断通道
    NVIC_Init(&NVIC_InitStructure); // 应用中断设置

    // 5. 使能USART1接收中断
    // 使能USART1的接收中断（接收到数据时触发）
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // 6. 使能USART1
    // 启用USART1模块
    USART_Cmd(USART1, ENABLE);
}

void UART_STOP(void){
	    // 1. 禁用 USART1 接收和发送
    USART_Cmd(USART1, DISABLE);

    // 2. 关闭 USART1 的中断
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    NVIC_DisableIRQ(USART1_IRQn);

    // 3. 将 USART1 寄存器复位到默认值
    USART_DeInit(USART1);  // 适用于标准外设库
    // 或使用 HAL_UART_DeInit(&huart1);  // 如果您使用的是 HAL 库

    // 4. (可选) 禁用 USART1 相关的 GPIO 引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 设置为浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 5. (可选) 关闭 USART1 的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
}
void UART_START(void){
	UART_Init();
}	
// 接收缓冲区和索引
volatile int8_t rx_buffer[7]; // 定义用于存储接收数据的缓冲区
volatile int rx_index = 0; // 接收数据的当前索引

// 用于存储解析后的数据的外部变量
extern volatile int8_t left_wheel_value;//左轮PWMB
extern volatile int8_t right_wheel_value;//右轮PWMA 
extern volatile int8_t stop_command;
extern int8_t QR_flag;

// 处理接收到的数据的函数
// 全局变量用于存储QR_flag的状态

void ProcessData(volatile int8_t data[]) {
    left_wheel_value = data[2]; // 解析左轮的值
    right_wheel_value = data[3]; // 解析右轮的值
    if (!QR_flag) {
        QR_flag = (data[5]); // 右移6位后与0x03进行与操作，得到前两位
    }
    if(!stop_command)stop_command = data[4]; // 右移5位后与0x01进行与操作，得到第三位
		Motor_SetSpeed(1,right_wheel_value);
		Motor_SetSpeed(0,left_wheel_value);
//		LED2_ON();
//		Delay_ms(2);
//		LED2_OFF();
}


// USART1中断服务例程
void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        // 从USART接收数据
        uint8_t data = USART_ReceiveData(USART1);

        // 按照预定的协议解析数据
        if (rx_index == 0 && data == 0x2C) { // 检查帧头的第一个字节
            rx_buffer[rx_index++] = data;
        } else if (rx_index == 1 && data == 0x12) { // 检查帧头的第二个字节
            rx_buffer[rx_index++] = data;
        } else if (rx_index >= 2 && rx_index < 6) { // 接收数据部分
            rx_buffer[rx_index++] = data;
        } else if (rx_index == 6 && data == 0x5B) { // 检查帧尾
            rx_buffer[rx_index] = data;
            ProcessData(rx_buffer); // 处理接收到的完整数据帧
            rx_index = 0; // 重置索引，准备接收下一帧数据
        } else {
            rx_index = 0; // 如果数据不符合预期，重置索引
        }
    }
}
