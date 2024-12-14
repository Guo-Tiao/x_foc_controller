#pragma once
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#define X_OS_TRUE                         pdTRUE                  
#define X_OS_FALSE                       pdFALSE
#define X_WAIT_FOR_INFINITE       portMAX_DELAY            //永久等待
#define X_TICK_TO_MS(tick)           pdTICKS_TO_MS(tick)       //tick转换为ms
#define X_MS_TO_TICK(ms)            pdMS_TO_TICKS(ms)       //ms转换为tick
#define X_OS_SLEEP(ms)                 vTaskDelay(X_MS_TO_TICK(ms));    //休眠MS

#define Deg_To_Angle_Conff              57.29579143313326        //弧度转换为角度乘以系数
#define X_PI 3.1415926
#define X_2PI 6.28318530718
#define X_3PI_2 4.71238898038
#define X_1_SQRT3 0.57735026919
#define X_2_SQRT3 1.15470053838