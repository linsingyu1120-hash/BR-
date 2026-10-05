#ifndef __CLAW_H__
#define __CLAW_H__

#include "djmotor.h"
#include "zdrive.h"

#define CLAW_SOLENOID_CHANNEL 3U //后续可能需要修改，和main.c中的 solenoid_init(3) 保持一致

typedef struct
{
    DJMotorPointer small_arm_motor; // 小臂电机
    DJMotorPointer big_arm_motor;   // 大臂电机
    DJMotorPointer lift_motor;      // 升降电机
    Zdrive *rotation_motor;         // 旋转电机，使用现有 ZDrive 接口
} Claw_struct;

typedef struct
{
    float Current_Height;  // 当前高度（仅升降机构使用）
    float Current_Pos;     // 当前转角（仅升降机构不使用）
    float Target_deg;      // 目标角度（输出到DJ/zdrive模板中的valSet角度）
    float Degree_Per_Unit; // 每升高1mm/转1°电机所需转动的角度（规定逆时针为正方向）
} MotorDeg;

extern Claw_struct Claw;
void Claw_Init(void);
void Change_HP_to_Degree(const void *Motor, float Target_HP);

#endif /* __CLAW_H__ */