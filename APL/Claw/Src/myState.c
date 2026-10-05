#include "myState.h"

SM_StateMachine Claw_SM; // 爪子状态机
SM_Event ClawEvent;      // 爪子触发事件

volatile SmallArmPos small_arm_pos = {0};
volatile BigArmPos big_arm_pos = {0};
volatile LiftHeight lift_height = {0};
volatile RotationPos rotation_pos = {0};
volatile uint8_t getsky_phase = 0;
volatile uint8_t putsky_phase = 0;
volatile uint8_t spinsky_phase = 0;

SM_Transition SMTranList[MAX_TRANSITION_NUM] = {0}; // 状态转移情况列表

SM_State IdleState = {State_IDLE, NULL, NULL}; // 父状态，遥控事件放在父状态

SM_State GetSkyState = {State_GetSky, &IdleState, NULL};
SM_State PutSkyState = {State_PutSky, &IdleState, NULL};
SM_State SpinSkyState = {State_SpinSky, &IdleState, NULL};

SM_State MotivateState = {State_Motivate, &IdleState, NULL};
SM_State NotMotivateState = {State_NotMotivate, &IdleState, NULL};
SM_State ZeroState = {State_Zero, &IdleState, NULL};
SM_State ResetState = {State_Reset, &IdleState, NULL};

void MySM_Init(void)
{
    SM_Init(&Claw_SM, &IdleState);
    Claw_Init();
}

// void StateEventUpdate()
// {
// }

void ClawEventEcho()
{
    if (SM_ProcessTransitions(&Claw_SM, SMTranList, TRANSITION_NUM, &ClawEvent) != 0)
    {
        // TODO 错误处理
        while (1)
        {
            // BEEP_Alarm(1); // 蜂鸣器报警
        }
    }
}

SM_State *State_IDLE(SM_StateMachine *stateMachine, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_ENTRY:
        ClawEvent.type = SM_EVENT_IDLE;
        // 进入空闲状态时的操作
        break;
    case SM_EVENT_EXIT:
        // 退出操作
        break;
    case SM_EVENT_IDLE:
        return &IdleState;
    case SM_EVENT_GETSKY:
        SM_TransitionTo(stateMachine, &GetSkyState);
        break;
    case SM_EVENT_PUTSKY:
        SM_TransitionTo(stateMachine, &PutSkyState);
        break;
    case SM_EVENT_SPINSKY:
        SM_TransitionTo(stateMachine, &SpinSkyState);
        break;

    case SM_EVENT_MOTIVATE:    
    SM_TransitionTo(stateMachine, &MotivateState);
    break;

    case SM_EVENT_NOTMOTIVATE:
    SM_TransitionTo(stateMachine, &NotMotivateState);
    break;

    case SM_EVENT_ZERO:
    SM_TransitionTo(stateMachine, &ZeroState);
    break;

    case SM_EVENT_RESET:
    SM_TransitionTo(stateMachine, &ResetState);
    break;

    default:
        ClawEvent.type = SM_EVENT_IDLE;
        break;
    }
    return NULL;
}