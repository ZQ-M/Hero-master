/**
 * @file gyroscope_task.c
 * @brief 板载陀螺仪解析任务
 * TODO: Kalman Filter
 * @version 0.1
 * @date 2021-10-17
 * @copyright Copyright (c) 2021
 */
/* 因使用外置陀螺仪数据，所以暂时注释陀螺仪任务函数内容，后续不需要就删除整个任务 */

#include "gyroscope_task.h"
#include "quaternion.h"

void StartGyroscopeTask(void const *argument)
{
  osDelay(100);

  Mpu_Device_Init();
  Init_Quaternion();

  for (;;)
  {
    //初始化及更新数据
    /*Mpu6050_Get_Data();
    Imu_AHRS_Update();

    Imu_Attitude_Update(); */
    
    ///<检查是否需要数据检查函数
    //  imu_date = Get_imu_date_now;

    osDelay(10);
  }
}
