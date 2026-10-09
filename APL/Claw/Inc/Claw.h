#ifndef __CLAW_H__
#define __CLAW_H__

#include "djmotor.h"
#include "zdrive.h"

#define CLAW_SOLENOID_CHANNEL 3U // 后续可能需要修改，和main.c中的 solenoid_init(3) 保持一致

typedef struct
{
    Zdrive *big_arm_motor;          // 大臂电机
    DJMotorPointer small_arm_motor; // 小臂电机
    DJMotorPointer rotation_motor;  // 旋转电机，使用现有 ZDrive 接口
    DJMotorPointer lift_motor;      // 升降电机

} Claw_struct;

typedef struct
{
    float Current_Height;  // 当前高度（仅升降机构使用）
    float Current_Pos;     // 当前转角（仅升降机构不使用）
    float Target_deg;      // 目标角度（输出到DJ/zdrive模板中的valSet角度）
    float Degree_Per_Unit; // 每升高1mm/转1°电机所需转动的角度（规定逆时针为正方向）
} MotorDeg;

typedef struct
{
    const void *Motor;    // 轨迹对应的电机指针
    float Target_HP;      // 目标高度/转角
    float Target_Degree;  // 电机的目标角度
    float Coefficient[6]; // 五次多项式中的6个系数
    float Position;       // 五次多项式中的位移
    float Velocity;       // 五次多项式中的速度
    float Acceleration;   // 五次多项式中的加速度
    uint32_t Start_Tick;  // 沿轨迹运动的开始时间
    uint32_t Time_ms;     // 轨迹运动的总时长
    bool Active;          // 轨迹运动使能/失能
} ClawTrajectory;

extern Claw_struct Claw;
extern volatile uint32_t Claw_Move_Time_ms[4];
void Claw_Init(void);
void Motor_Move(const void *Motor, float Target_HP);
void Claw_TrajectoryUpdate(void);
void Claw_Delay(uint32_t Ticks);

#endif /* __CLAW_H__ */