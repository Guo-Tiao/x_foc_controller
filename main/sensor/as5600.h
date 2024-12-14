#pragma once
#include "esp_err.h"

typedef struct AS5600_t
{
    double init_angle;//初始角度
    double raw_angle;//原始角度(未换算)值:0-359.91
    double relative_angle; // 相对角度
    
    double total_angle; // 总角度
    int full_rotations; // 总圈数计数
    int last_quadrant;//当前象限
    int prev_quadrant;//上一次象限
}AS5600;

typedef enum as5600_num_t
{
    AS5600_M0=0,
    AS5600_M1=1,
} as5600_num;


//获取as5600
AS5600 *as5600_get(as5600_num num);
//AS5600 I2C 初始化
void as5600_i2c_init(void);
//AS5600初始化数据 
void as5600_data_init(void);
//更新数据
void as5600_update(as5600_num num);


/***---------------TEST---------------***/
void as5600_test(as5600_num num);
/***---------------TEST---------------***/

