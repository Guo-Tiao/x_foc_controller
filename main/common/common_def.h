#pragma once
/*
驱动板配置
 */


/*****************************电机参数*****************************/
#define X_MOTOR_PP                      7               //电机极对数
#define X_MOTOR_VOL                   12.6          //电机电压

#define X_MOTOR_ENABLE_NUM     12           //电机使能引脚


/*M0电机和传感器*/
 //PWM引脚
#define X_M0_PWM_A  32     
#define X_M0_PWM_B  33      
#define X_M0_PWM_C  25      

//电机通道
#define X_M0_CHANNEL_A      0  
#define X_M0_CHANNEL_B      1
#define X_M0_CHANNEL_C      2

//传感器方向
#define X_M0_Sensor_DIR  -1   

//磁编传感器
#define X_M0_AS_I2C_NUM             0
#define X_M0_AS_I2C_FREZ             400000
#define X_M0_AS_SDA                     19
#define X_M0_AS_SCL                      18

//电流传感器
#define X_M0_CURR_PINA 39
#define X_M0_CURR_PINB 36




/*M1电机和传感器*/
 //PWM引脚
#define X_M1_PWM_A  26     
#define X_M1_PWM_B  27      
#define X_M1_PWM_C  14      

//电机通道
#define X_M1_CHANNEL_A      3
#define X_M1_CHANNEL_B      4
#define X_M1_CHANNEL_C      5

//传感器方向
#define X_M1_Sensor_DIR  1   

//磁编传感器
#define X_M1_AS_I2C_NUM             1
#define X_M1_AS_I2C_FREZ             400000
#define X_M1_AS_SDA                     23
#define X_M1_AS_SCL                      5
//电流传感器
#define X_M1_CURR_PINA 35
#define X_M1_CURR_PINB 34

/*****************************电机参数*****************************/

