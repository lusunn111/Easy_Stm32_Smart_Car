# 代码说明

## 1.代码结构

1. 代码结构如下：

```
├── README.md
├── Project
│   ├── Start
│   ├── Library
│   ├── System
        ├── Delay.c
        ├── Delay.h
        ├── Timer.c
        ├── Timer.h
│   ├── Hardware
        ├── LED.c
        ├── LED.h
        ├── Key.c
        ├── Key.h
        ├── OLED.c
        ├── OLED.h
        ├── PWM.c
        ├── PWM.h
        ├── Motor.c
        ├── Motor.h
        ├── CountCensor.c
        ├── CountCensor.h
│   ├── User
        ├── User.c
        ├── User.h

```
## 2.代码说明


### 2.1 System

### 2.1.1 Delay.c


- 功能：延时函数

- 中断：无

- TIM占用：无

- 包含函数：`Delay_ms`,`Delay_us`,`Delay_us`

### 2.1.2 Timer.c

- 功能：计时器函数

- 中断：内部计时器作为时钟的中断

- TIM占用：TIM2中断

- 包含函数：`Timer_Init`,`TIM2_IRQHandler`

### 2.2 Hardware

### 2.2.1 LED.c

功能：LED驱动函数

中断：无

TIM占用：无

包含函数：`LED_Init`,`LED1_ON`,`LED1_OFF`,`LED1_Turn`,`LED2_ON`,`LED2_OFF`,`LED2_Turn`

### 2.2.2 Key.c

功能：按键驱动函数

中断：

### 2.2.3 OLED.c

        

### 2.1.2 Timer.c

### 2.2 Hardware

### 2.2.1 LED.c

### 2.2.2 Key.c

### 2.2.3 OLED.c

### 2.2.4 PWM.c

### 2.2.5 Motor.c

### 2.2.6 CountCensor.c

### 2.3 User

### 2.3.1 User.c

