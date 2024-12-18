#include "x_foc_core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "common/common_macro.h"
#include "utils/x_utils.h"
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

    //初始化电机
    x_motor_init(x_motor_0);
    x_motor_init(x_motor_1);
    //启动电机
    x_motor_start(x_motor_0);
    x_motor_start(x_motor_1);
    printf("FOC控制器初始化完成\r\n");
    //延时
    vTaskDelay(X_MS_TO_TICK(1000));
}



void x_foc_run(void)
{
   
    while (1)
    {
        vTaskDelay(X_MS_TO_TICK(1000));
    }
    
}
