#ifndef __CONTROL_DATA_H_
#define __CONTROL_DATA_H_

#include "buzzer_task.h"
#include "math2.h"
#include "remote.h"
#include "stm32f4xx.h"

enum controlDevice ///<�����豸ѡ��
{
	mouse_keyboard_device_ENUM = 1u, ///< �����豸Ϊ����
	remote_controller_device_ENUM	 ///< �����豸Ϊң����
};

enum rcMotionMode ///<ң����ģʽ�ĵ�����̨ģʽ
{
	rc_chassis_follow_mode_ENUM = 1u,	///< ���̸���+�ֶ���׼
	rc_chassis_gyro_mode_ENUM,			///< ����С����+�ֶ���׼
	rc_stable_chassis_follow_mode_ENUM, ///< ���̸���+������̨
	rc_stable_chassis_gyro_mode_ENUM,	///< ����С����+������̨
	rc_special_mode_ENUM				///< ����ģʽ
};

enum mouseKeyboardChassisMode ///<����ģʽ�ĵ���ģʽѡ��
{
	mk_chassis_follow_mode_ENUM = 1u, ///< ���̸���
	mk_chassis_gyro_mode_ENUM,		  ///< ����С����
	mk_chassis_special_mode_ENUM	  ///< ��������ģʽ
};

enum mouseKeyboardAimMode ///<����ģʽ�����ģʽѡ��
{
	mk_manual_aim_mode_ENUM = 1u, ///< �ֶ���׼
	mk_auto_aim_mode_ENUM,		  ///< �Զ���׼
	mk_special_aim_mode_ENUM	  ///< ��̨����ģʽ
};

enum gyroDirection ///<С����ѡ��
{
	gyro_positive_ENUM = 1, ///< ��������
	gyro_negative_ENUM = -1 ///< ���ݷ���
};

enum fricCoverMode ///<Ħ����ѡ��
{
	fric_cover_off_mode_ENUM = 0u, ///< Ħ���ֹر�
	fric_adaptive_speed_mode_ENUM, ///< Ħ��������Ӧ�ٶ�
	fric_high_speed_mode_ENUM,	   ///< Ħ��������ٶ��ٶ�
	cover_on_ENUM				   ///< Ħ���ֹر�
};

enum shooterMode ///<���ģʽѡ��
{
	on_shoot_mode_ENUM = 1u, ///< ����
	rapid_fire_max_mode_ENUM ///< ��������
};

typedef struct
{
	struct
	{
		uint8_t control_device;				 ///< �����豸 1����ģʽ(rc) 2ң����ģʽ(mouse_key)
		uint8_t rc_motion_mode;				 ///< ������̨ģʽ 1���̸��� 2С���� 3����+���̸��� 4����+���̸��� 5����
		uint8_t mouse_keyboard_chassis_mode; ///< ����ģʽ��1���� 2С���� 3����
		uint8_t mouse_keyboard_gimbal_mode;	 ///< ��̨ģʽ��1�����˶� 2������̨ģʽ 3����ģʽ
		int8_t gyro_direction;				 ///< С������ת����-1���ݷ��� 1��������
		uint8_t fric_cover_mode;			 ///< Ħ����ģʽ�� 0�ر� 1����Ӧ 2����� 3���ո�
		uint8_t shoot_mode;					 ///< ���ģʽ�� 1���� 2����������ٶȣ�
		uint8_t shoot_key;					 ///< ������أ� 0�ر�
	} mode;

	struct
	{
		//�������ٶ�
		int16_t mouse_x_max_value;
		int16_t mouse_y_max_value;

		//������ݱ���(������)
		float mouse_x_sensitivity;
		float mouse_y_sensitivity;

		//���̼��ٶ�
		int16_t chassis_acceleration;
		//��������ٶ�
		int16_t chassis_max_speed; //����ҡ�����ֵ
	} parameter;

	struct
	{
		int16_t ch0; //��Ӧ���x�����ƶ�
		int16_t ch1; //��Ӧ���y�����ƶ�
		int16_t ch2; //���� ����WS
		int16_t ch3; //ǰ�� ����AD
	} virtual_rocker;

	// ctrl�����л�
	//���ݿ��ء�����-1 0 1
	//�������ģʽ
	//���ߵ���һ��С���� ����
} Robot_control_data_t;
void Control_Data_Init(Robot_control_data_t *robot_control_data);
void Parse_Robot_Control_Data(Rc_Ctrl_t *rc_data, Rc_Ctrl_t *last_rc_data, Robot_control_data_t *robot_control_data);
///<������Ҫ�������ݷ��غ���

#endif // __CONTROL_DATA_H_
