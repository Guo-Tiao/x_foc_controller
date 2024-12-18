#include "x_mcpwm.h"
#include "esp_check.h"

#define X_MCPWM_TAG "X_MCPWM"


esp_err_t x_mcpwm_init(const x_mcpwm_config *config, x_mcpwm_context *ret_ctx)
{
    esp_err_t ret;
    ESP_RETURN_ON_FALSE(config && ret_ctx, ESP_ERR_INVALID_ARG, X_MCPWM_TAG, "无效参数");

    /* 初始化定时器 */
    mcpwm_timer_config_t timer_config = 
    {
        .group_id = config->group_id,  // 定时器组
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,  // 默认时钟源，默认为160MHz
        .resolution_hz = config->freq_hz,  // 频率
        .period_ticks = config->period_ticks,  // 周期
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,  // 向上计数
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &ret_ctx->timer));


     /* 初始化操作器 */
    mcpwm_operator_config_t operator_config = 
    {
        .group_id = config->group_id,  // 操作器组0
    };
    for (size_t i = 0; i < X_MCPWM_GPIO_COUNT_MAX; i++)
    {
        ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &ret_ctx->operators[i]));
        ESP_ERROR_CHECK(mcpwm_operator_connect_timer(ret_ctx->operators[i], ret_ctx->timer));
    }

    /* 初始化生成器 */
    for (size_t i = 0; i < X_MCPWM_GPIO_COUNT_MAX; i++)
    {
        mcpwm_generator_config_t generator_config = 
        {
        .   gen_gpio_num = config->gpio_num[i],  // 输出管脚
        };
         ESP_ERROR_CHECK(mcpwm_new_generator(ret_ctx->operators[i], &generator_config, &ret_ctx->generators[i]));
    }
    
     /* 初始化比较器 */
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,  // 计数器为0时更新比较器值
    };
    for (size_t i = 0; i < X_MCPWM_GPIO_COUNT_MAX; i++)
    {
        ESP_ERROR_CHECK(mcpwm_new_comparator(ret_ctx->operators[i], &comparator_config, &ret_ctx->comparators[i]));
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(ret_ctx->comparators[i], 0));  // 初始占空比0%
    }
    
    
     /* 初始化输出波形 */
    for (size_t i = 0; i < X_MCPWM_GPIO_COUNT_MAX; i++)
    {
        // 计数值为0时开始输出高电平
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(ret_ctx->generators[i],
                                                              MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
        // 比较事件发生时，输出低电平                                                      
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(ret_ctx->generators[i],
                                                                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, ret_ctx->comparators[i], MCPWM_GEN_ACTION_LOW)));
    }

    return ESP_OK;
}

esp_err_t x_mcpwm_start_stop(x_mcpwm_context *ctx, mcpwm_timer_start_stop_cmd_t cmd)
{
    ESP_RETURN_ON_FALSE(ctx, ESP_ERR_INVALID_ARG, X_MCPWM_TAG, "无效参数");

    if ((cmd != MCPWM_TIMER_STOP_EMPTY) && (cmd != MCPWM_TIMER_STOP_FULL)) 
    {
        ESP_RETURN_ON_ERROR(mcpwm_timer_enable(ctx->timer), X_MCPWM_TAG, "mcpwm timer enable failed");
    }
    ESP_RETURN_ON_ERROR(mcpwm_timer_start_stop(ctx->timer, cmd), X_MCPWM_TAG, "mcpwm timer start failed");
    return ESP_OK;
}

esp_err_t x_mcpwm_set_duty(x_mcpwm_context *ctx, uint16_t a, uint16_t b, uint16_t c)
{
    ESP_RETURN_ON_FALSE(ctx, ESP_ERR_INVALID_ARG, X_MCPWM_TAG, "无效参数");

    ESP_RETURN_ON_ERROR(mcpwm_comparator_set_compare_value(ctx->comparators[0], a), X_MCPWM_TAG, "pwm[A] set duty failed");
    ESP_RETURN_ON_ERROR(mcpwm_comparator_set_compare_value(ctx->comparators[1], b), X_MCPWM_TAG, "pwm[B] set duty failed");
    ESP_RETURN_ON_ERROR(mcpwm_comparator_set_compare_value(ctx->comparators[2], c), X_MCPWM_TAG, "pwm[C] set duty failed");
    return ESP_OK;
}

esp_err_t x_mcpwm_uninit(x_mcpwm_context *ctx)
{
    ESP_RETURN_ON_FALSE(ctx, ESP_ERR_INVALID_ARG, X_MCPWM_TAG, "无效参数");
    ESP_RETURN_ON_ERROR(mcpwm_timer_disable(ctx->timer), X_MCPWM_TAG, "mcpwm timer disable failed");
    for (int i = 0; i < X_MCPWM_GPIO_COUNT_MAX; i++) 
    {
        ESP_RETURN_ON_ERROR(mcpwm_del_generator(ctx->generators[i]), X_MCPWM_TAG, "free mcpwm positive generator failed");
        ESP_RETURN_ON_ERROR(mcpwm_del_generator(ctx->generators[i]), X_MCPWM_TAG, "free mcpwm negative generator failed");
        ESP_RETURN_ON_ERROR(mcpwm_del_comparator(ctx->comparators[i]), X_MCPWM_TAG, "free mcpwm comparator failed");
        ESP_RETURN_ON_ERROR(mcpwm_del_operator(ctx->operators[i]), X_MCPWM_TAG, "free mcpwm operator failed");
    }
    ESP_RETURN_ON_ERROR(mcpwm_del_timer(ctx->timer), X_MCPWM_TAG, "free mcpwm timer failed");
    return ESP_OK;
}
