#pragma once
#include "esp_err.h"
#include "driver/mcpwm_prelude.h"

#define X_MCPWM_GPIO_COUNT_MAX     3    //PWM引脚数量
//MCPWM配置
typedef struct x_mcpwm_config_t
{
    int group_id;   //组ID
    int gpio_num[X_MCPWM_GPIO_COUNT_MAX];    //GPIO引脚号
    uint32_t freq_hz;   //频率
    uint32_t period_ticks;   //周期
}x_mcpwm_config;

//MCPWM上下文
typedef struct x_mcpwm_context_t
{
    mcpwm_timer_handle_t timer; //定时器
    mcpwm_oper_handle_t operators[X_MCPWM_GPIO_COUNT_MAX];  //操作器
    mcpwm_gen_handle_t generators[X_MCPWM_GPIO_COUNT_MAX];  //生成器
    mcpwm_cmpr_handle_t comparators[X_MCPWM_GPIO_COUNT_MAX]; // 比较器
} x_mcpwm_context;

//MCPWM配置初始化
esp_err_t x_mcpwm_init(const x_mcpwm_config *config, x_mcpwm_context *ret_ctx);
//MCPWM开启停止
esp_err_t x_mcpwm_start_stop(x_mcpwm_context *ctx, mcpwm_timer_start_stop_cmd_t cmd);
//MCPWM设置占空比
esp_err_t x_mcpwm_set_duty(x_mcpwm_context *ctx, uint16_t a,uint16_t b,uint16_t c);
//MCPWM反初始化
esp_err_t x_mcpwm_uninit(x_mcpwm_context *ctx);