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

extern Claw_struct Claw;
void Claw_Init(void);

#endif /* __CLAW_H__ */