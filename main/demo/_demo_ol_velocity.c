#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "common/common_macro.h"
#include "utils/x_utils.h"
#include "motor/x_motor.h"
#include "sensor/as5600.h"
#include <sys/time.h>


/*
开双速度DEMO
*/


static float voltage_power_supply=12.6;
static float shaft_angle=0,open_loop_timestamp=0;
static float zero_electric_angle=0,Ualpha,Ubeta=0,Ua=0,Ub=0,Uc=0,dc_a=0,dc_b=0,dc_c=0;

// 电角度求解
static float demo_olv_electricalAngle(float shaft_angle, int pole_pairs) 
{
  return (shaft_angle * pole_pairs);
}

// 设置PWM到控制器输出
static void demo_olv_setPwm(float Ua, float Ub, float Uc) 
{

  // 计算占空比
  // 限制占空比从0到1
  dc_a = _constrain(Ua / voltage_power_supply, 0.0f , 1.0f );
  dc_b = _constrain(Ub / voltage_power_supply, 0.0f , 1.0f );
  dc_c = _constrain(Uc / voltage_power_supply, 0.0f , 1.0f );

  //写入PWM到PWM 0 1 2 通道
  x_motor_set_duty(x_motor_0, dc_a, dc_b, dc_c);
}

static void demo_olv_setPhaseVoltage(float Uq,float Ud, float angle_el) {
  angle_el = _normalizeAngle(angle_el + zero_electric_angle);
  // 帕克逆变换
  Ualpha =  -Uq*sin(angle_el); 
  Ubeta =   Uq*cos(angle_el); 

  // 克拉克逆变换
  Ua = Ualpha + voltage_power_supply/2;
  Ub = (sqrt(3)*Ubeta-Ualpha)/2 + voltage_power_supply/2;
  Uc = (-Ualpha-sqrt(3)*Ubeta)/2 + voltage_power_supply/2;
  demo_olv_setPwm(Ua,Ub,Uc);
}

//开环速度函数
static float demo_olv_velocityOpenloop(float target_velocity)
{
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);

  int64_t now_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;  //获取从开启芯片以来的微秒数，它的精度是 4 微秒。 micros() 返回的是一个无符号长整型（unsigned long）的值
  
  //计算当前每个Loop的运行时间间隔
  float Ts = (now_us - open_loop_timestamp) * 1e-6f;

  //由于 micros() 函数返回的时间戳会在大约 70 分钟之后重新开始计数，在由70分钟跳变到0时，TS会出现异常，因此需要进行修正。如果时间间隔小于等于零或大于 0.5 秒，则将其设置为一个较小的默认值，即 1e-3f
  if(Ts <= 0 || Ts > 0.5f) Ts = 1e-3f;
  

  // 通过乘以时间间隔和目标速度来计算需要转动的机械角度，存储在 shaft_angle 变量中。在此之前，还需要对轴角度进行归一化，以确保其值在 0 到 2π 之间。
  shaft_angle = _normalizeAngle(shaft_angle + target_velocity*Ts);
  //以目标速度为 10 rad/s 为例，如果时间间隔是 1 秒，则在每个循环中需要增加 10 * 1 = 10 弧度的角度变化量，才能使电机转动到目标速度。
  //如果时间间隔是 0.1 秒，那么在每个循环中需要增加的角度变化量就是 10 * 0.1 = 1 弧度，才能实现相同的目标速度。因此，电机轴的转动角度取决于目标速度和时间间隔的乘积。

  // 使用早前设置的voltage_power_supply的1/3作为Uq值，这个值会直接影响输出力矩
  // 最大只能设置为Uq = voltage_power_supply/2，否则ua,ub,uc会超出供电电压限幅
  float Uq = voltage_power_supply/3;
  
  demo_olv_setPhaseVoltage(Uq,  0, demo_olv_electricalAngle(shaft_angle, 7));
  
  open_loop_timestamp = now_us;  //用于计算下一个时间间隔

  return Uq;
}

void demo_ol_velocity(void)
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


    int val = 0;
    int dir = 0;
    while (1)
    {
        demo_olv_velocityOpenloop(val);
        vTaskDelay(X_MS_TO_TICK(10));
        if(val>=100)
        {
            dir = 1;
        }
        else if(val<=0)
        {
            dir = 0;
        }

         if(dir==0)
        {
            val++;
        }
        else
        {
            val--;
        }

    }
    
}
