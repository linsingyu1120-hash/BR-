#ifndef __MYSTATE_H__
#define __MYSTATE_H__

#include "StateMachine.h"
#include "Claw.h"
#include "djmotor.h"
#include "zdrive.h"

/* ------------------------ */
#define TRANSITION_NUM 2
#define MAX_TRANSITION_NUM 4



typedef struct
{
    float GetSky; //
    float PutSky;
    float SpinSky;
} SmallArmPos;

typedef struct
{
    float GetSky;
    float PutSky;
    float SpinSky;
    float OutMachine;
    float InMachine;

} BigArmPos;

typedef struct
{
    float GetSky;
    float PutSky;
    float SpinSky;
    float SpinSkyUp;

} LiftHeight;

typedef struct
{
    float GetSky;
    float PutSky;
    float SpinSky;
    float SpinSkyEnd;

} RotationPos;

/* --- 全局变量 extern 声明 --- */
extern SM_StateMachine Claw_SM;
extern SM_Event ClawEvent;
extern volatile SmallArmPos small_arm_pos;
extern volatile BigArmPos big_arm_pos;
extern volatile LiftHeight lift_height;
extern volatile RotationPos rotation_pos;
extern volatile MotorDeg motor_deg[4];
extern volatile uint8_t getsky_phase;
extern volatile uint8_t putsky_phase;
extern volatile uint8_t spinsky_phase;

/* --- 状态对象 extern 声明 --- */
extern SM_State IdleState;
extern SM_State GetSkyState;
extern SM_State PutSkyState;
extern SM_State SpinSkyState;

/* --- 状态函数声明（用于 SM_State 初始化 + 跨文件调用） --- */
SM_State *State_GetSky(SM_StateMachine *stateMachine, const SM_Event *event);
SM_State *State_PutSky(SM_StateMachine *stateMachine, const SM_Event *event);
SM_State *State_SpinSky(SM_StateMachine *stateMachine, const SM_Event *event);
SM_State *State_Motivate(SM_StateMachine *stateMachine, const SM_Event *event);
SM_State *State_NotMotivate(SM_StateMachine *stateMachine, const SM_Event *event);
SM_State *State_Zero(SM_StateMachine *stateMachine, const SM_Event *event);
SM_State *State_Reset(SM_StateMachine *stateMachine, const SM_Event *event);
SM_State *State_IDLE(SM_StateMachine *stateMachine, const SM_Event *event);

/* --- API 函数 --- */
void MySM_Init(void);
// void StateEvent_Update(void);
void ClawEventEcho(void);

#endif /* __MYSTATE_H__ */