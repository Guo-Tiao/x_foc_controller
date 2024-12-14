#include "x_motor.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "common/common_def.h"

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

//配置PWM
static void x_motor_ledc_config(int gpio_num,ledc_channel_t ledc_channel,uint32_t freq_hz,ledc_timer_bit_t duty_resolution)
{

    // 将LEDC通道与GPIO引脚关联
    ledc_set_pin( gpio_num,LEDC_HIGH_SPEED_MODE,ledc_channel);
    
     // 配置LEDC通道（这里我们假设使用高速模式，定时器0，分辨率默认为13位，但后面会覆盖）
    ledc_channel_config_t ledc_channel_conf = {
        .speed_mode = LEDC_HIGH_SPEED_MODE, // 或者LEDC_LOW_SPEED_MODE
        .channel = ledc_channel,
        .timer_sel = LEDC_TIMER_0, // 选择定时器0
        .intr_type = LEDC_INTR_DISABLE, // 禁用中断
        .gpio_num = X_M0_PWM_A, // GPIO引脚
    };
    // 注意：这里的duty_resolution在后面的ledc_timer_config中会被实际设置的值所覆盖，
    // 但为了保持结构完整性，这里仍然需要初始化。
 
    // 配置LEDC定时器
    ledc_timer_config_t ledc_timer_conf = {
        .speed_mode = LEDC_HIGH_SPEED_MODE, // 必须与ledc_channel_conf中的speed_mode匹配
        .timer_num = LEDC_TIMER_0, // 必须与ledc_channel_conf中的timer_sel匹配
        .freq_hz = 30000, // 设置频率为30kHz
        .duty_resolution = LEDC_TIMER_8_BIT, // 设置分辨率为8位
        .clk_cfg = LEDC_AUTO_CLK // 或者LEDC_APB_CLK, 根据需要选择
    };
 
    // 应用LEDC通道配置
    ledc_channel_config(&ledc_channel_conf);
    // 应用LEDC定时器配置
    ledc_timer_config(&ledc_timer_conf);
}

void x_motor0_init(void)
{
    //配置GPIO
    gpio_config_t motor_cfg =
    {
            .intr_type = GPIO_INTR_DISABLE, //配置中断类型
            .mode = GPIO_MODE_OUTPUT,    //配置输入输出模式
            .pin_bit_mask = ((1ULL<<X_M0_PWM_A) | (1ULL<<X_M0_PWM_B) | (1ULL<<X_M0_PWM_C))  , //配置引脚
            .pull_down_en = GPIO_PULLDOWN_DISABLE,//下拉电阻是否使能
            .pull_up_en = GPIO_PULLDOWN_DISABLE    //上拉电阻是否使能
    };
    gpio_config(&motor_cfg);

}

void x_motor1_init(void)
{

}

