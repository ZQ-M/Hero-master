#include "quaternion.h"

extern Imu_t imu;
volatile float q0 = 1.0f;
volatile float q1 = 0.0f;
volatile float q2 = 0.0f;
volatile float q3 = 0.0f;
static volatile float gx, gy, gz, ax, ay, az, mx, my, mz;
volatile uint32_t last_update, now_update; ///< Sampling cycle count, ubit ms
volatile float exInt, eyInt, ezInt;        ///< error integral
#define Kp 2.0f                            /*                                           \
                                            * proportional gain governs rate of         \
                                            * convergence to accelerometer/magnetometer \
                                            */
#define Ki 0.01f                           /*                                 \
                                            * integral gain governs rate of   \
                                            * convergence of gyroscope biases \
                                            */
																						extern Imu_t imu;

/**
 * @brief               初始化四元数
 * @param[in]           void
 * @retval              void
 */
void Init_Quaternion(void)
{
    float hx, hy; ///< hz;

    hx = imu.mx;
    hy = imu.my;
    ///< hz = imu.mz;

#ifdef BOARD_DOWN
    if (hx < 0 && hy < 0)
    {
        if (fabs(hx / hy) >= 1)
        {
            q0 = -0.005;
            q1 = -0.199;
            q2 = 0.979;
            q3 = -0.0089;
        }
        else
        {
            q0 = -0.008;
            q1 = -0.555;
            q2 = 0.83;
            q3 = -0.002;
        }
    }
    else if (hx < 0 && hy > 0)
    {
        if (fabs(hx / hy) >= 1)
        {
            q0 = 0.005;
            q1 = -0.199;
            q2 = -0.978;
            q3 = 0.012;
        }
        else
        {
            q0 = 0.005;
            q1 = -0.553;
            q2 = -0.83;
            q3 = -0.0023;
        }
    }
    else if (hx > 0 && hy > 0)
    {
        if (fabs(hx / hy) >= 1)
        {
            q0 = 0.0012;
            q1 = -0.978;
            q2 = -0.199;
            q3 = -0.005;
        }
        else
        {
            q0 = 0.0023;
            q1 = -0.83;
            q2 = -0.553;
            q3 = 0.0023;
        }
    }
    else if (hx > 0 && hy < 0)
    {
        if (fabs(hx / hy) >= 1)
        {
            q0 = 0.0025;
            q1 = 0.978;
            q2 = -0.199;
            q3 = 0.008;
        }
        else
        {
            q0 = 0.0025;
            q1 = 0.83;
            q2 = -0.56;
            q3 = 0.0045;
        }
    }
#else
    if (hx < 0 && hy < 0)
    {
        if (fabs(hx / hy) >= 1)
        {
            q0 = 0.195;
            q1 = -0.015;
            q2 = 0.0043;
            q3 = 0.979;
        }
        else
        {
            q0 = 0.555;
            q1 = -0.015;
            q2 = 0.006;
            q3 = 0.829;
        }
    }
    else if (hx < 0 && hy > 0)
    {
        if (fabs(hx / hy) >= 1)
        {
            q0 = -0.193;
            q1 = -0.009;
            q2 = -0.006;
            q3 = 0.979;
        }
        else
        {
            q0 = -0.552;
            q1 = -0.0048;
            q2 = -0.0115;
            q3 = 0.8313;
        }
    }
    else if (hx > 0 && hy > 0)
    {
        if (fabs(hx / hy) >= 1)
        {
            q0 = -0.9785;
            q1 = 0.008;
            q2 = -0.02;
            q3 = 0.195;
        }
        else
        {
            q0 = -0.9828;
            q1 = 0.002;
            q2 = -0.0167;
            q3 = 0.5557;
        }
    }
    else if (hx > 0 && hy < 0)
    {
        if (fabs(hx / hy) >= 1)
        {
            q0 = -0.979;
            q1 = 0.0116;
            q2 = -0.0167;
            q3 = -0.195;
        }
        else
        {
            q0 = -0.83;
            q1 = 0.014;
            q2 = -0.012;
            q3 = -0.556;
        }
    }
#endif
}

/**
 * @brief           根据陀螺仪测的数据更新四元数
 * @param[in]       void
 * @retval          void
 */
void Imu_AHRS_Update(void)
{
    float norm;
    float hx, hy, hz, bx, bz;
    float vx, vy, vz, wx, wy, wz;
    float ex, ey, ez, halfT;
    float tempq0, tempq1, tempq2, tempq3;

    float q0q0 = q0 * q0;
    float q0q1 = q0 * q1;
    float q0q2 = q0 * q2;
    float q0q3 = q0 * q3;
    float q1q1 = q1 * q1;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q3q3 = q3 * q3;

    gx = imu.wx;
    gy = imu.wy;
    gz = imu.wz;
    ax = imu.ax;
    ay = imu.ay;
    az = imu.az;
    mx = imu.mx;
    my = imu.my;
    mz = imu.mz;

    now_update = HAL_GetTick(); ///< ms
    halfT = ((float)(now_update - last_update) / 2000.0f);
    last_update = now_update;

    ///< Fast inverse square-root 
    norm = Inv_Sqrt(ax * ax + ay * ay + az * az);
    ax = ax * norm;
    ay = ay * norm;
    az = az * norm;

#ifdef IST8310
    norm = Inv_Sqrt(mx * mx + my * my + mz * mz);
    mx = mx * norm;
    my = my * norm;
    mz = mz * norm;
#else
    mx = 0;
    my = 0;
    mz = 0;
#endif
    ///< compute reference direction of flux 
    hx = 2.0f * mx * (0.5f - q2q2 - q3q3) + 2.0f * my * (q1q2 - q0q3) + 2.0f * mz * (q1q3 + q0q2);
    hy = 2.0f * mx * (q1q2 + q0q3) + 2.0f * my * (0.5f - q1q1 - q3q3) + 2.0f * mz * (q2q3 - q0q1);
    hz = 2.0f * mx * (q1q3 - q0q2) + 2.0f * my * (q2q3 + q0q1) + 2.0f * mz * (0.5f - q1q1 - q2q2);
    bx = sqrt((hx * hx) + (hy * hy));
    bz = hz;

    ///< estimated direction of gravity and flux (v and w) 
    vx = 2.0f * (q1q3 - q0q2);
    vy = 2.0f * (q0q1 + q2q3);
    vz = q0q0 - q1q1 - q2q2 + q3q3;
    wx = 2.0f * bx * (0.5f - q2q2 - q3q3) + 2.0f * bz * (q1q3 - q0q2);
    wy = 2.0f * bx * (q1q2 - q0q3) + 2.0f * bz * (q0q1 + q2q3);
    wz = 2.0f * bx * (q0q2 + q1q3) + 2.0f * bz * (0.5f - q1q1 - q2q2);

    /* 
	 * error is sum of cross product between reference direction 
	 * of fields and direction measured by sensors 
	 */
    ex = (ay * vz - az * vy) + (my * wz - mz * wy);
    ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
    ez = (ax * vy - ay * vx) + (mx * wy - my * wx);

    ///< PI 
    if (ex != 0.0f && ey != 0.0f && ez != 0.0f)
    {
        exInt = exInt + ex * Ki * halfT;
        eyInt = eyInt + ey * Ki * halfT;
        ezInt = ezInt + ez * Ki * halfT;

        gx = gx + Kp * ex + exInt;
        gy = gy + Kp * ey + eyInt;
        gz = gz + Kp * ez + ezInt;
    }

    tempq0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * halfT;
    tempq1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * halfT;
    tempq2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * halfT;
    tempq3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * halfT;

    ///< normalise quaternion 
    norm = Inv_Sqrt(tempq0 * tempq0 + tempq1 * tempq1 + tempq2 * tempq2 + tempq3 * tempq3);
    q0 = tempq0 * norm;
    q1 = tempq1 * norm;
    q2 = tempq2 * norm;
    q3 = tempq3 * norm;
}

/**
  * @brief          更新陀螺仪数据
  * @param          void
  * @retval         void
  */
void Imu_Attitude_Update(void)
{
    ///< yaw    -pi----pi 
    imu.yaw = -atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2 * q2 - 2 * q3 * q3 + 1) * 57.3;
    ///< pitch  -pi/2----pi/2 
    imu.pit = -asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3;
    ///< roll   -pi----pi  
    imu.rol = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3;
    // Console.print("yaw %.2f, pitch %.2f, rol %.2f \r\n", imu.yaw, imu.pit, imu.rol);
}
