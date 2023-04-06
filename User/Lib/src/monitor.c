#include "monitor.h"

/**
 * @brief 模块状态初始化
 * @param {Module_status_t*} module 模块结构体的指针
 * @param {uint8_t} cnt_num 模块超时时间（实际时间为 cnt_num*任务延时时间）
 * @param {void} Online_Callback 模块上线的回调函数
 * @param {void} Offline_Callback 模块离线的回调函数
 * @return {*}
 */
void Module_Status_Init(Module_status_t* module, uint8_t cnt_num, void Online_Callback(void), void Offline_Callback(void))
{
	module->count = 0;
	module->reload_cnt = cnt_num;
	
	module->time_out_flag = 1;
	module->old_time_out_flag = 1;
	
	module->Module_Online_Callback = Online_Callback;
	module->Module_Offline_Callback = Offline_Callback;
}

/**
 * @brief 模块监听，在定时器或任务中定时监听
 * @param {Module_status_t*} module 模块结构体的指针
 * @return {*}
 */
void Module_Status_Listen(Module_status_t* module)
{
	module->old_time_out_flag = module->time_out_flag;
	
	if(module->count > 0)
	{
		module->count--;
		module->time_out_flag=0;
		if(module->Module_Online_Callback!=NULL && module->old_time_out_flag == 1)
		{
			module->Module_Online_Callback();
		}
	}
	else if(module->count == 0)
	{
		module->time_out_flag=1;
		if(module->Module_Offline_Callback!=NULL && module->old_time_out_flag == 0)
		{
			module->Module_Offline_Callback();
		}
	}
}

/**
 * @brief 得到模块状态
 * @param {Module_status_t*} module 模块结构体的指针
 * @return {*}
 */
uint8_t Get_Module_Status(Module_status_t* module)
{
	//return module->time_out_flag;
	if(module->time_out_flag == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief 模块状态重置（例如在模块收到数据时调用）
 * @param {Module_status_t*} module 模块结构体的指针
 * @return {*}
 */
void Module_Status_Reload(Module_status_t* module)
{
	module->count = module->reload_cnt;
}

