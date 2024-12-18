#include "x_motor.h"
#include "driver/gpio.h"

#include "drivers/x_mcpwm.h"
#include "common/common_def.h"

//电机句柄
static x_mcpwm_context x_motor_context[x_motor_count];

void x_motor_power_init(void)
{
    gpio_config_t power_cfg =
    {
            .intr_type = GPIO_INTR_DISABLE, //配置中断类型
            .mode = GPIO_MODE_OUTPUT,    //配置输入输出模式
            .pin_bit_mask = 1ULL << X_MOTOR_ENABLE_NUM, //配置引脚
            .pull_down_en = GPIO_PULLDOWN_DISABLE,//下拉电阻是否使能
            .pull_up_en = GPIO_PULLDOWN_DISABLE    //上拉电阻是否使能
    };
    //配置GPIO
    gpio_config(&power_cfg);
}

void x_motor_power_enable(bool enable)
{
    gpio_set_level(X_MOTOR_ENABLE_NUM, enable);
}

void x_motor_init(x_motor_index index)
{
    if(index>=x_motor_count)
    {
        return;
    }
    x_mcpwm_config config =
    {
        .group_id=index,
        .freq_hz=X_MOTOR_MCPWM_FREQ,
        .period_ticks=X_MOTOR_MCPWM_PERIOD,
        .gpio_num={GPIO_NUM_NC,GPIO_NUM_NC,GPIO_NUM_NC}
    };
    switch (index)
    {
    case x_motor_0:
        config.gpio_num[0] = X_M0_PWM_A;
        config.gpio_num[1] = X_M0_PWM_B;
        config.gpio_num[2] = X_M0_PWM_C;
        break;
    case x_motor_1:
        config.gpio_num[0] = X_M1_PWM_A;
        config.gpio_num[1] = X_M1_PWM_B;
        config.gpio_num[2] = X_M1_PWM_C;
        break;
    default:
        break;;
    }
    x_mcpwm_init(&config, &x_motor_context[index]);
}

void x_motor_start(x_motor_index index)
{
    x_mcpwm_start_stop(&x_motor_context[index], MCPWM_TIMER_START_NO_STOP);
}

void x_motor_stop(x_motor_index index)
{
    x_mcpwm_start_stop(&x_motor_context[index], MCPWM_TIMER_STOP_EMPTY);
}

void x_motor_set_duty(x_motor_index index, float a, float b, float c)
{
    x_mcpwm_set_duty(&x_motor_context[index], 
                                        X_MOTOR_MCPWM_PERIOD * a, 
                                        X_MOTOR_MCPWM_PERIOD * b, 
                                        X_MOTOR_MCPWM_PERIOD * c
                                    );
}
