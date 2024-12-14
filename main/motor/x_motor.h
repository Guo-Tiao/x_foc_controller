#pragma once
#include "esp_err.h"
#include "stdbool.h"
//电机供电初始化
void x_motor_power_init(void);
//电机供电使能
void x_motor_power_enable(bool enable);



//电机0初始化
void x_motor0_init(void);
//电机1初始化
void x_motor1_init(void);