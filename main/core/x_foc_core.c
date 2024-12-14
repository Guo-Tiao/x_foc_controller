#include "x_foc_core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "common/common_macro.h"
#include "motor/x_motor.h"
#include "sensor/as5600.h"

void x_foc_init(void)
{
    //初始化电机电源控制
    x_motor_power_init();
    //使能，一定要放在校准电机前
    x_motor_power_enable(true);

    //初始化AS5600
    as5600_i2c_init();
    //AS5600数据初始化
    as5600_data_init();
}

void x_foc_run(void)
{

    while (1)
    {
        as5600_test(AS5600_M1);
        vTaskDelay(X_MS_TO_TICK(200));
    }
    
 
}
