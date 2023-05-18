# RoboMaster HLL战队2022英雄程序
###TIPS:ZQ-M:队内拿过来移植用于2023年省赛和中部邀请赛，非公开库
---

## 硬件

0. Robomaster开发板（A型）
1. 底盘4个3508电机连接C620电调连接CAN1
2. 云台2个GM6020电机和发射机构波轮6002电机连接C610电调连接连接CAN2
3. 弹舱盖舵机和摩擦轮两个电机连接C615电调连接定时器2的通道1、3、4
4. 串口3用作调试输出
5. 串口1经过电平反向器连接至RM遥控器接收机
6. 串口6连接NUC获取自瞄数据
7. 串口8连接裁判系统
8. 串口7连接WT61C陀螺仪（未使用板载陀螺仪）

---

## 开发环境

* Keil MDK5
* STM32 CubeMx

---

## 功能介绍

* ### 板载LED（1红+1绿+8LED阵列）
    * 绿灯每秒闪烁2次，可通过闪烁状态判断系统是否正常运行

    * LED整列（8颗LED）用于模块状态指示。LED亮代表模块在线，否则代表模块离线。LED对应的模块如下。
        1. 遥控器接收机
        2. 底盘4个电机
        3. 云台2个电机
        4. 发射机构波轮电机
        5. 自瞄NUC
        6. 裁判系统
        7. 超级电容
        8. 陀螺仪

* ### 串口指令（Shell）
    * 串口3支持串口指令，输入?可查看所有指令
    * 串口指令不需要回车（"\r\n"）结尾
    * 可通过相应指令进行PID的参数调整
    * ` //参考自 https://gitee.com/somebug/atomlib/tree/master `

* ### 遥控器操作逻辑
    * 左摇杆y轴控制前进后退
    * 左摇杆x轴控制左右平移
    * 右摇杆y轴控制云台俯仰
    * 右摇杆x轴控制云台左右
    * 左上角开关从中间位置往上波切换操作设备（遥控器，键鼠）
    * 左上角开关从中间位置往下波切换机器人模式
    * 右上角开关从中间位置往上波切换发射模式（单发、三连发、连发）
    * 右上角开关从中间位置往下波控制摩擦轮和弹仓盖
    * 左上角波动摇杆是发射按钮

* ### 键鼠操作逻辑
    1. WASD前后左右
    2. 鼠标滑动控制云台移动
    3. 鼠标左键 连发
    4. 鼠标右键 三连发
    5. Shift 按住加速
    6. Ctrl切换底盘运动模式(跟隨、小陀螺)
    7. R开关弹舱
    8. E开关摩擦轮
    9. G开关自瞄
    10. Z特殊模式

## 代码规范

**整体采用Unix风格**

- 变量名全部小写；
- 宏定义全部大写；
- 函数名首字母大写并用下划线进行隔开；
- 缩进等采用vscode配置的LLVM格式化模式；

## Hll 2022 Super Hero 工程日志

- [x] 模块检测任务
- [x] 串口 1 DMA 双缓冲接收
- [x] 遥控器任务完成
- [x] 增加机器人模式
- [x] 增加遥控器进一步的数据解析
- [x] 改进 printf 使用 Mutex 打印
- [x] 云台角度控制
- [x] can 发送队列
- [x] 蜂鸣器任务
- [x] 云台发射电机速度初步设置
- [x] 底盘基本运动控制
- [x] 陀螺仪数据读取四元数解析输出三轴角度
- [x] 底盘完整的模式切换
- [x] 裁判系统数据读取解析
- [ ] 结构体等统一添加memset的初始化
- [ ] 卡尔曼滤波数据解析，以及遥控器和电机数据的低通滤波或斜坡函数（频率不一致，低频的数据在高频使用中会有阶跃和延迟问题）
- [x] 超级电容管理(finish,待测试)
- [x] 底盘完整的模式切换
- [x] 裁判系统数据读取解析
- [x] 功率控制（纯算法实现功率控制）
- [ ] 基于裁判系统的射速射频控制
- [ ] 超级电容使用逻辑
- [ ] 继续测试发射
- [ ] 专门配合英雄或者步兵的 QT 上位机界面
- [x] pitch轴自稳定设计（原Yaw上电的初始模式具有保持绝对空间稳定的作用，即无输入状态的IMU云台模式）

## 待改进（加入）功能
1. 客户端增加gui显示
2. 模糊PID
3. 显示屏（不急）
4. 陀螺仪（不急）
