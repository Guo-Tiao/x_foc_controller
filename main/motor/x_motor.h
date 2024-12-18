#pragma once
#include "esp_err.h"
#include "stdbool.h"

#define X_MOTOR_MCPWM_FREQ                  1*1000*1000     // 1MHz, 1 tick = 1us
#define X_MOTOR_MCPWM_PERIOD              1*1000                // 1000 * 1us = 1000us = 1ms
//实际频率为 X_MOTOR_MCPWM_FREQ/X_MOTOR_MCPWM_PERIOD =1KHZ

//电机供电初始化
void x_motor_power_init(void);
//电机供电使能
void x_motor_power_enable(bool enable);

typedef enum x_motor_index_t
{
    x_motor_0=0,
    x_motor_1=1,

    x_motor_count=2,
}x_motor_index;
//电机初始化
void x_motor_init(x_motor_index index);
//电机启动
void x_motor_start(x_motor_index index);
//电机停止
void x_motor_stop(x_motor_index index);
//设置占空比(0.0f-1.0f)
void x_motor_set_duty(x_motor_index index, float a,float b,float c);
