#ifndef __MYSTATE_H__
#define __MYSTATE_H__

#include "StateMachine.h"
#include "Claw.h"

/* --- 轨迹控制常量 --- */

/* ------------------------ */



/* --- 全局变量 extern 声明 --- */
extern SM_StateMachine Claw_SM;
extern SM_Event ClawEvent;

/* --- 状态对象 extern 声明 --- */
extern SM_State IdleState;
extern SM_State GetSkyState;
extern SM_State PutSkyState;
extern SM_State SpinSkyState;

/* --- 状态函数声明（用于 SM_State 初始化 + 跨文件调用） --- */
SM_State *State_GetSky(SM_StateMachine *stateMachine, const SM_Event *event);
SM_State *State_PutSky(SM_StateMachine *stateMachine, const SM_Event *event);
SM_State *State_SpinSky(SM_StateMachine *stateMachine, const SM_Event *event);
SM_State *State_IDLE(SM_StateMachine *stateMachine, const SM_Event *event);

/* --- 共享辅助函数声明 --- */


/* --- API 函数 --- */
void MySM_Init(void);
void StateEvent_Update(void);
void ClawEventEcho(void);

#endif /* __MYSTATE_H__ */