#include "control_data.h"

static void Robot_Rc_Mode_Change_Control(Rc_Ctrl_t *rc_data, Rc_Ctrl_t *last_rc_data, Robot_control_data_t *robot_control_data);
static void Wasd_Key_To_Virtual_Rocker(Rc_Ctrl_t *rc_data, Robot_control_data_t *robot_control_data);
static void Mouse_To_Virtual_Rocker(Rc_Ctrl_t *rc_data, Robot_control_data_t *robot_control_data);
static void Switch_Mouse_Key_Change(Rc_Ctrl_t *rc_data, Rc_Ctrl_t *last_rc_data, Robot_control_data_t *robot_control_data);
static void Shoot_Key_Control(Rc_Ctrl_t *rc_data, Rc_Ctrl_t *last_rc_data, Robot_control_data_t *robot_control_data);

/**
 *@brief		设置机器人初始时的模式
 @param			控制数据结构体地址
 @retval		void
  */
void Control_Data_Init(Robot_control_data_t *robot_control_data)
{
	robot_control_data->mode.control_device = mouse_keyboard_device_ENUM;				///< 鼠标控制
	robot_control_data->mode.rc_motion_mode = rc_special_mode_ENUM;						///< 特殊模式（遥控器模式）
	robot_control_data->mode.mouse_keyboard_chassis_mode = mk_chassis_follow_mode_ENUM; ///< 底盘跟随 （键鼠模式）
	robot_control_data->mode.mouse_keyboard_gimbal_mode = mk_manual_aim_mode_ENUM;		///< 手动瞄准模式（云台自由运动）
	robot_control_data->mode.gyro_direction = gyro_positive_ENUM;						///< 小陀螺方向：正向
	robot_control_data->mode.fric_cover_mode = fric_cover_off_mode_ENUM;				///< 关闭摩擦轮
	robot_control_data->mode.shoot_mode = on_shoot_mode_ENUM;							///< 单发模式
	robot_control_data->mode.shoot_key = 0;												///< 关闭射击开关

#define MOUSE_X_MAX_VALUE 5000	 ///< X方向限幅
#define MOUSE_Y_MAX_VALUE 5000	 ///< Y方向限幅
#define MOUSE_X_SENSITIVITY 0.8f ///< X灵敏度
#define MOUSE_Y_SENSITIVITY 0.8f

	robot_control_data->parameter.mouse_x_max_value = MOUSE_X_MAX_VALUE;
	robot_control_data->parameter.mouse_y_max_value = MOUSE_Y_MAX_VALUE;
	robot_control_data->parameter.mouse_x_sensitivity = MOUSE_X_SENSITIVITY;
	robot_control_data->parameter.mouse_y_sensitivity = MOUSE_Y_SENSITIVITY;
	robot_control_data->parameter.chassis_acceleration = 10;
	robot_control_data->parameter.chassis_max_speed = 660;

#undef MOUSE_X_MAX_VALUE
#undef MOUSE_Y_MAX_VALUE
#undef MOUSE_X_SENSITIVITY
#undef MOUSE_Y_SENSITIVITY

	robot_control_data->virtual_rocker.ch0 = 0;
	robot_control_data->virtual_rocker.ch1 = 0;
	robot_control_data->virtual_rocker.ch2 = 0;
	robot_control_data->virtual_rocker.ch3 = 0;
}
/**
 * @brief 解析机器人控制模式
 * @param[in] {rc_data}   					本次解析的遥控器数据
 * @param[in] {last_rc_data}   				上次解析的遥控器数据
 * @param[in] {uint16_t}					机器人模式配置数据
 * @retval    {uint8_t}						1 : 数据解析并校验成功
 * 											0 : 数据解析校验失败
 */
void Parse_Robot_Control_Data(Rc_Ctrl_t *rc_data, Rc_Ctrl_t *last_rc_data, Robot_control_data_t *robot_control_data)
{
	Robot_Rc_Mode_Change_Control(rc_data, last_rc_data, robot_control_data); ///< 机器人模式变换响应
	Wasd_Key_To_Virtual_Rocker(rc_data, robot_control_data);				 ///< WASD虚拟为摇杆
	Mouse_To_Virtual_Rocker(rc_data, robot_control_data);					 ///< 鼠标虚拟为摇杆
	Switch_Mouse_Key_Change(rc_data, last_rc_data, robot_control_data);		 ///< 响应键盘控制
	Shoot_Key_Control(rc_data, last_rc_data, robot_control_data);			 ///< 射击按钮
}

static void Robot_Rc_Mode_Change_Control(Rc_Ctrl_t *rc_data, Rc_Ctrl_t *last_rc_data, Robot_control_data_t *robot_control_data)
{
#define ROBOT_MODE robot_control_data->mode
#define S1_VALUE rc_data->rc.s1
#define S2_VALUE rc_data->rc.s2
#define OLD_S1_VALUE last_rc_data->rc.s1
#define OLD_S2_VALUE last_rc_data->rc.s2
#define S1_CHANGED_TO(a, b) ((OLD_S1_VALUE == (a)) && (S1_VALUE == (b)))
#define S2_CHANGED_TO(a, b) ((OLD_S2_VALUE == (a)) && (S2_VALUE == (b)))

	/* 操控设备选择 */
	if (S2_CHANGED_TO(3, 2))
	{
		ROBOT_MODE.control_device++;
		if (ROBOT_MODE.control_device == 3)
			ROBOT_MODE.control_device = 1;
		Set_Beep_Time(ROBOT_MODE.control_device, 1000, 55);
	}

	/* 底盘云台模式 */
	if (S2_CHANGED_TO(3, 1))
	{
		ROBOT_MODE.rc_motion_mode++;
		if (ROBOT_MODE.rc_motion_mode == 6)
			ROBOT_MODE.rc_motion_mode = 1;
		Set_Beep_Time(ROBOT_MODE.rc_motion_mode, 1200, 50);
	}

	if (ROBOT_MODE.control_device != 2)
	{
		return;
	}

	/* 摩擦轮速度模式，舱盖开关 */
	if (S1_CHANGED_TO(3, 2))
	{
		ROBOT_MODE.fric_cover_mode++;
		if (ROBOT_MODE.fric_cover_mode == 4)
			ROBOT_MODE.fric_cover_mode = 0;
		Set_Beep_Time(ROBOT_MODE.fric_cover_mode + 1, 800, 60);
	}

	/* 射击模式选择器 */
	if (S1_CHANGED_TO(3, 1))
	{
		ROBOT_MODE.shoot_mode++;
		if (ROBOT_MODE.shoot_mode == 3)
			ROBOT_MODE.shoot_mode = 1;
		Set_Beep_Time(ROBOT_MODE.shoot_mode, 600, 65);
	}

#undef ROBOT_MODE
#undef S1_VALUE
#undef S2_VALUE
#undef OLD_S1_VALUE
#undef OLD_S2_VALUE
#undef S1_CHANGED_TO
#undef S2_CHANGED_TO
}
/**
 * @brief               					键盘数据解析为虚拟摇杆
 * @param[in] {Rc_Ctrl_t*}   				遥控器数据缓冲区
 * @param[in] {Robot_control_data_t*}   	遥控器数据指针结构体
 * @retval    {uint8_t}						无
 */
static void Wasd_Key_To_Virtual_Rocker(Rc_Ctrl_t *rc_data, Robot_control_data_t *robot_control_data)
{
///<保证可移植性
#define KEY_W 0x01
#define KEY_A 0x04
#define KEY_S 0x02
#define KEY_D 0x08
#define RC_KEY rc_data->key.value
#define KEY_PRESSED(key) (RC_KEY & key)
#define VIRTUAL_ROCKER_STEP robot_control_data->parameter.chassis_acceleration ///<底盘加速度
#define VIRTUAL_ROCKER_MIN (-robot_control_data->parameter.chassis_max_speed)  ///<底盘反向最大速度
#define VIRTUAL_ROCKER_MAX robot_control_data->parameter.chassis_max_speed	   ///<底盘正向最大速度
#define VIRTUAL_ROCKER robot_control_data->virtual_rocker

	/* ch3 min max限制 */
	if (VIRTUAL_ROCKER.ch3 > VIRTUAL_ROCKER_MAX)
	{
		Int16_Minus_Limit(&VIRTUAL_ROCKER.ch3, VIRTUAL_ROCKER_STEP, VIRTUAL_ROCKER_MAX);
	}
	else if (VIRTUAL_ROCKER.ch3 < VIRTUAL_ROCKER_MIN)
	{
		Int16_Add_Limit(&VIRTUAL_ROCKER.ch3, VIRTUAL_ROCKER_STEP, VIRTUAL_ROCKER_MIN);
	}

	/*只按下W时，不按下S*/
	if (KEY_PRESSED(KEY_W) && !(KEY_PRESSED(KEY_S)))
	{
		if (VIRTUAL_ROCKER.ch3 < 0)
		{
			VIRTUAL_ROCKER.ch3 += VIRTUAL_ROCKER_STEP;
		}
		if (VIRTUAL_ROCKER.ch3 < VIRTUAL_ROCKER_MAX)
		{
			Int16_Add_Limit(&VIRTUAL_ROCKER.ch3, VIRTUAL_ROCKER_STEP, VIRTUAL_ROCKER_MAX);
		}
	}
	/*只按下S时，不按下W*/
	else if (KEY_PRESSED(KEY_S) && !(KEY_PRESSED(KEY_W)))
	{
		if (VIRTUAL_ROCKER.ch3 > 0)
		{
			VIRTUAL_ROCKER.ch3 -= VIRTUAL_ROCKER_STEP;
		}
		if (VIRTUAL_ROCKER.ch3 > VIRTUAL_ROCKER_MIN)
		{
			Int16_Minus_Limit(&VIRTUAL_ROCKER.ch3, VIRTUAL_ROCKER_STEP, VIRTUAL_ROCKER_MIN);
		}
	}
	/*W、S都未按下时*/
	else
	{
		if (VIRTUAL_ROCKER.ch3 < 0)
		{
			Int16_Add_Limit(&VIRTUAL_ROCKER.ch3, VIRTUAL_ROCKER_STEP, 0);
		}
		else if (VIRTUAL_ROCKER.ch3 > 0)
		{
			Int16_Minus_Limit(&VIRTUAL_ROCKER.ch3, VIRTUAL_ROCKER_STEP, 0);
		}
	}

	/* ch2 min max限制 */
	if (VIRTUAL_ROCKER.ch2 > VIRTUAL_ROCKER_MAX)
	{
		Int16_Minus_Limit(&VIRTUAL_ROCKER.ch2, VIRTUAL_ROCKER_STEP, VIRTUAL_ROCKER_MAX);
	}
	else if (VIRTUAL_ROCKER.ch2 < VIRTUAL_ROCKER_MIN)
	{
		Int16_Add_Limit(&VIRTUAL_ROCKER.ch2, VIRTUAL_ROCKER_STEP, VIRTUAL_ROCKER_MIN);
	}

	/*只按下D时*/
	if (KEY_PRESSED(KEY_D) && !(KEY_PRESSED(KEY_A)))
	{
		if (VIRTUAL_ROCKER.ch2 < 0)
		{
			VIRTUAL_ROCKER.ch2 += VIRTUAL_ROCKER_STEP;
		}
		if (VIRTUAL_ROCKER.ch2 < VIRTUAL_ROCKER_MAX)
		{
			Int16_Add_Limit(&VIRTUAL_ROCKER.ch2, VIRTUAL_ROCKER_STEP, VIRTUAL_ROCKER_MAX);
		}
	}
	/*只按下A时*/
	else if (KEY_PRESSED(KEY_A) && !(KEY_PRESSED(KEY_D)))
	{
		if (VIRTUAL_ROCKER.ch2 > 0)
		{
			VIRTUAL_ROCKER.ch2 -= VIRTUAL_ROCKER_STEP;
		}
		if (VIRTUAL_ROCKER.ch2 > VIRTUAL_ROCKER_MIN)
		{
			Int16_Minus_Limit(&VIRTUAL_ROCKER.ch2, VIRTUAL_ROCKER_STEP, VIRTUAL_ROCKER_MIN);
		}
	}
	/*A、D都未按下时*/
	else
	{
		if (VIRTUAL_ROCKER.ch2 < 0)
		{
			Int16_Add_Limit(&VIRTUAL_ROCKER.ch2, VIRTUAL_ROCKER_STEP, 0);
		}
		else if (VIRTUAL_ROCKER.ch2 > 0)
		{
			Int16_Minus_Limit(&VIRTUAL_ROCKER.ch2, VIRTUAL_ROCKER_STEP, 0);
		}
	}

#undef KEY_W
#undef KEY_A
#undef KEY_S
#undef KEY_D
#undef RC_KEY
#undef KEY_PRESSED
#undef VIRTUAL_ROCKER_STEP
#undef VIRTUAL_ROCKER_MIN
#undef VIRTUAL_ROCKER_MAX
#undef VIRTUAL_ROCKER
	/*相当于W,S,A,D,只是需要利用的中间值，利用他们生成虚拟摇杆数据，作为最终使用的数据*/
}
/** @brief								鼠标移动解析为虚拟摇杆通道值
 *  @param [in] Rc_Ctrl_t				接收的遥控器数据结构体指针
 *  @param [in] Robot_control_data_t	机器人控制数据结构体指针
 *
 */
static void Mouse_To_Virtual_Rocker(Rc_Ctrl_t *rc_data, Robot_control_data_t *robot_control_data)
{
	//鼠标上下限限制
	robot_control_data->virtual_rocker.ch0 = Int16_Limit(rc_data->mouse.x, -robot_control_data->parameter.mouse_x_max_value, robot_control_data->parameter.mouse_x_max_value);
	robot_control_data->virtual_rocker.ch1 = Int16_Limit(rc_data->mouse.y, -robot_control_data->parameter.mouse_y_max_value, robot_control_data->parameter.mouse_y_max_value);

	//鼠标数值*灵敏度
	robot_control_data->virtual_rocker.ch0 *= robot_control_data->parameter.mouse_x_sensitivity;
	robot_control_data->virtual_rocker.ch1 *= robot_control_data->parameter.mouse_y_sensitivity;
}

/* 响应键盘切换模式 */
static void Switch_Mouse_Key_Change(Rc_Ctrl_t *rc_data, Rc_Ctrl_t *last_rc_data, Robot_control_data_t *robot_control_data)
{
#define KEY_CTRL 0x20
#define KEY_SHIFT 0x10
#define KEY_Z 0x800
#define KEY_X 0x1000
#define KEY_C 0x2000
#define KEY_V 0x4000
#define KEY_B 0x8000
#define KEY_F 0x200
#define KEY_G 0x400
#define KEY_Q 0x40
#define KEY_E 0x80
#define KEY_R 0x100
#define KEY_VALUE rc_data->key.value
#define OLD_KEY_VALUE last_rc_data->key.value
#define KEY_PRESSED(key) (KEY_VALUE & key)
#define KEY_CLICKED(key) ((KEY_VALUE & key) && (!(OLD_KEY_VALUE & key)))

	//判断是不是键鼠模式
	if (robot_control_data->mode.control_device != 1)
	{
		return;
	}

	//按住shift底盘加速
	if (KEY_PRESSED(KEY_SHIFT))
	{
		robot_control_data->parameter.chassis_acceleration = 50;
		robot_control_data->parameter.chassis_max_speed = 660;
	}
	else
	{
		robot_control_data->parameter.chassis_acceleration = 16;
		robot_control_data->parameter.chassis_max_speed = 360;
	}

	//底盘模式(按下ctrl更改底盘模式1跟随模式2小陀螺模式)
	if (KEY_CLICKED(KEY_CTRL))
	{
		if (robot_control_data->mode.mouse_keyboard_chassis_mode == mk_chassis_follow_mode_ENUM)
		{
			robot_control_data->mode.mouse_keyboard_chassis_mode = mk_chassis_gyro_mode_ENUM;
			Set_Beep_Time(2, 1200, 50);
		}
		else
		{
			robot_control_data->mode.mouse_keyboard_chassis_mode = mk_chassis_follow_mode_ENUM;
			Set_Beep_Time(1, 1200, 50);
		}
	}

	//云台模式 G (手动控制 自瞄模式)
	if (KEY_CLICKED(KEY_G))
	{
		if (robot_control_data->mode.mouse_keyboard_gimbal_mode == mk_manual_aim_mode_ENUM)
		{
			robot_control_data->mode.mouse_keyboard_gimbal_mode = mk_auto_aim_mode_ENUM;
			Set_Beep_Time(4, 1200, 50);
		}
		else
		{
			robot_control_data->mode.mouse_keyboard_gimbal_mode = mk_manual_aim_mode_ENUM;
			Set_Beep_Time(3, 1200, 50);
		}
	}

	//特殊模式
	if (KEY_CLICKED(KEY_V))
	{
		robot_control_data->mode.mouse_keyboard_chassis_mode = mk_chassis_special_mode_ENUM;
		robot_control_data->mode.mouse_keyboard_gimbal_mode = mk_special_aim_mode_ENUM;
		Set_Beep_Time(5, 1200, 50);
	}

	//非特殊模式检查
	if (robot_control_data->mode.mouse_keyboard_chassis_mode == 3 && robot_control_data->mode.mouse_keyboard_gimbal_mode != 3)
	{
		robot_control_data->mode.mouse_keyboard_chassis_mode = 1;
	}
	if (robot_control_data->mode.mouse_keyboard_chassis_mode != 3 && robot_control_data->mode.mouse_keyboard_gimbal_mode == 3)
	{
		robot_control_data->mode.mouse_keyboard_gimbal_mode = 1;
	}

	//弹舱模式 R （开关弹舱盖）
	if (KEY_CLICKED(KEY_R))
	{
		if (robot_control_data->mode.fric_cover_mode == cover_on_ENUM)
		{
			robot_control_data->mode.fric_cover_mode = fric_cover_off_mode_ENUM;
			Set_Beep_Time(1, 800, 60);
		}
		else
		{
			robot_control_data->mode.fric_cover_mode = cover_on_ENUM;
			Set_Beep_Time(4, 800, 60);
		}
	}

	//摩擦轮模式 E
	if (KEY_CLICKED(KEY_E))
	{
		if (robot_control_data->mode.fric_cover_mode == fric_adaptive_speed_mode_ENUM)
		{
			robot_control_data->mode.fric_cover_mode = fric_cover_off_mode_ENUM;
			Set_Beep_Time(1, 800, 60);
		}
		else
		{
			robot_control_data->mode.fric_cover_mode = fric_adaptive_speed_mode_ENUM;
			Set_Beep_Time(2, 800, 60);
		}
	}

#undef KEY_CTRL
#undef KEY_SHIFT
#undef KEY_Z
#undef KEY_X
#undef KEY_C
#undef KEY_V
#undef KEY_B
#undef KEY_F
#undef KEY_G
#undef KEY_Q
#undef KEY_E
#undef KEY_R
#undef KEY_VALUE
#undef OLD_KEY_VALUE
#undef KEY_PRESSED
#undef KEY_CLICKED
}

/* 响应射击模式 */
static void Shoot_Key_Control(Rc_Ctrl_t *rc_data, Rc_Ctrl_t *last_rc_data, Robot_control_data_t *robot_control_data)
{
	//遥控器模式
	if (robot_control_data->mode.control_device == remote_controller_device_ENUM)
	{
		/* 射击按钮 */
		if (robot_control_data->mode.shoot_mode == on_shoot_mode_ENUM)
		{
			if (last_rc_data->rc.ch4 <= 120 && rc_data->rc.ch4 > 120)
			{
				robot_control_data->mode.shoot_key = on_shoot_mode_ENUM;
			}
		}
		else
		{
			if (rc_data->rc.ch4 > 120)
			{
				robot_control_data->mode.shoot_key = robot_control_data->mode.shoot_mode;
			}
			else
			{
				robot_control_data->mode.shoot_key = 0;
			}
		}
	}

	//键盘模式
	else if (robot_control_data->mode.control_device == 1)
	{
		if (rc_data->mouse.press_l && (!last_rc_data->mouse.press_l))
		{
			robot_control_data->mode.shoot_key = 1;
		}
	}
}
