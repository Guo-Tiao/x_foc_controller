#include "as5600.h"
#include "driver/i2c.h"
#include "common/common_def.h"
#include "common/common_macro.h"

AS5600 as5600_m0;  //电机M0的传感器
AS5600 as5600_m1;  //电机M1的传感器

#define AS5600_Addr                          0X36
#define AS5600_RawAngle                  0X0C
#define AS5600_TRAN_COFF              0.087890625    //传感器转换系数(360/4096)

#define AS5600_I2C_NUM(num) (num==0?X_M0_AS_I2C_NUM:X_M1_AS_I2C_NUM)
#define AS5600_GET_OBJ(num) (num==0?&as5600_m0:&as5600_m1)

AS5600 *as5600_get(as5600_num num)
{
    return  AS5600_GET_OBJ(num);;
}

void as5600_i2c_init(void)
{
    //初始化M0的AS5600的I2C通讯
    i2c_config_t m0_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = X_M0_AS_SDA,
        .scl_io_num = X_M0_AS_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = X_M0_AS_I2C_FREZ,
    };
    i2c_param_config(X_M0_AS_I2C_NUM, &m0_conf);
    i2c_driver_install(X_M0_AS_I2C_NUM, m0_conf.mode, 0, 0, 0);


    //初始化M1的AS5600的I2C通讯
    i2c_config_t m1_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = X_M1_AS_SDA,
        .scl_io_num = X_M1_AS_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = X_M1_AS_I2C_FREZ,
    };
    i2c_param_config(X_M1_AS_I2C_NUM, &m1_conf);
    i2c_driver_install(X_M1_AS_I2C_NUM, m1_conf.mode, 0, 0, 0);
}
//计算相对角度
static void as5600_calc_relative_angle(AS5600 *as5600)
{
	//相对角度获取，顺时针为正
	double  relative_angle = as5600->raw_angle - as5600->init_angle; //没变动则为0
	if(relative_angle < 0) //把相对角度转换为0-360度表示
	{
		  relative_angle = relative_angle + 360; 
	}
	//把0-360角度转换为-180到+180表示
	if(relative_angle > 180)
    {
		  relative_angle = relative_angle - 360;
	}
    as5600->relative_angle = relative_angle;
}
//计算象限和总角度
void as5600_calc_quadrant(AS5600 *as5600)
{
    //用象限来计算圈数，顺时针转为正
      /*
      //Quadrants:
      4  |  1
      ---|---
      3  |  2
      */
      //Quadrant 1
    double relative_angle = as5600->relative_angle;
    int last_quadrant = as5600->last_quadrant;
    int prev_quadrant = as5600->prev_quadrant;
    if (relative_angle > 0 && relative_angle <= 90)
    {
        last_quadrant = 1;
    }
    
      //Quadrant 2
    if(relative_angle > 90 && relative_angle <=180)
    {
        last_quadrant = 2;
    }
    
      //Quadrant 3
    if(relative_angle > -180 && relative_angle <=-90)
    {
        last_quadrant = 3;
    }
    
      //Quadrant 4
    if(relative_angle > -90 && relative_angle <0)
    {
      last_quadrant = 4;
    }
    as5600->last_quadrant = last_quadrant;
    if(last_quadrant != prev_quadrant) //如果象限改变
    {
        if(last_quadrant == 1 && prev_quadrant == 4)
        {
          as5600->full_rotations++; // 现在象限为1而上次为4，则顺时针旋转过0一次，圈数加1
        }

        if(last_quadrant == 4 && prev_quadrant == 1)
        {
          as5600->full_rotations--; // 现在象限为4而上次为1，则逆时针旋转过0一次，圈数减1
        }
        as5600->prev_quadrant = last_quadrant;  //更新旧象限
    }
    as5600->total_angle = (as5600->full_rotations *360) + as5600->relative_angle; //number of turns (+/-) plus the actual angle within the 0-360 range
}

//获取原始角度
static double as5600_get_raw_angle(as5600_num num)
{
    uint8_t angle_high = 0;
    uint8_t angle_low = 0;
    uint16_t result = 0;
    float angle=0;
    i2c_cmd_handle_t cmd;

    //读取HighAddr
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AS5600_Addr << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, AS5600_RawAngle, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(AS5600_I2C_NUM(num), cmd, X_MS_TO_TICK(1000));
    i2c_cmd_link_delete(cmd);
    X_OS_SLEEP(1);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AS5600_Addr << 1 | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &angle_high, 0); 
    i2c_master_read_byte(cmd, &angle_low, 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(AS5600_I2C_NUM(num), cmd,  X_MS_TO_TICK(1000));
    i2c_cmd_link_delete(cmd);

    result=(uint16_t)(angle_high<<8|angle_low); //一共就11位 注意
    angle=((int) result & 0b0000111111111111)*AS5600_TRAN_COFF;

    return angle;
}

void as5600_data_init(void)
{
    //更新M0角度
    as5600_m0.relative_angle = 0;
    as5600_m0.total_angle = 0;
    as5600_m0.full_rotations = 0;
    as5600_m0.last_quadrant = 0;
    as5600_m0.prev_quadrant = 0;
    as5600_m0.init_angle=as5600_get_raw_angle(AS5600_M0);//更新初始角度
    X_OS_SLEEP(1);
    as5600_m0.raw_angle= as5600_get_raw_angle(AS5600_M0);
    as5600_calc_relative_angle(&as5600_m0);

    //更新M1角度
    as5600_m1.relative_angle = 0;
    as5600_m1.total_angle = 0;
    as5600_m1.full_rotations = 0;
    as5600_m1.last_quadrant = 0;
    as5600_m1.prev_quadrant = 0;
    as5600_m1.init_angle=as5600_get_raw_angle(AS5600_M1);//更新初始角度
    X_OS_SLEEP(1);
    as5600_m1.raw_angle= as5600_get_raw_angle(AS5600_M1);
    as5600_calc_relative_angle(&as5600_m1);
}

void as5600_update(as5600_num num)
{
    AS5600 *as5600 = AS5600_GET_OBJ(num);
    float raw_angle = as5600_get_raw_angle(num);
    as5600->raw_angle = raw_angle;
    as5600_calc_relative_angle(as5600);
    as5600_calc_quadrant(as5600);
}

void as5600_test(as5600_num num)
{
    AS5600 *as5600 = as5600_get(num);
    as5600_update(num);
    printf("init_angle:%.2f  raw_angle:%.2f  relative_angle:%.2f  total_angle:%.2f  full_rotations:%d last_quadrant:%d prev_quadrant:%d\r\n",
           as5600->init_angle,
           as5600->raw_angle,
           as5600->relative_angle,
           as5600->total_angle,
           as5600->full_rotations,
           as5600->last_quadrant,
           as5600->prev_quadrant);
}
