#include "myState.h"
#include "cmsis_os2.h"
#include "solenoid.h"

/*爪子任务静态变量*/
volatile BigArmPos big_arm_pos =
    {
        .GetSky = 190.0f,
        .PutSky = 170.0f,
        .OutMachine = 180.0f,
        .InMachine = 0.0f};

volatile SmallArmPos small_arm_pos =
    {
        .GetSky = -10.0f,
        .PutSky = 10.0f};

volatile RotationPos rotation_pos =
    {
        .GetSky = 0.0f,
        .PutSky = 0.0f,
        .SpinSky = 180.0f};

volatile LiftHeight lift_height =
    {
        .GetSky = 0.0f,
        .PutSky = 700.0f,
        .SpinSkyUp = 720.0f};

volatile uint8_t getsky_phase = 0;
volatile uint8_t putsky_phase = 0;
volatile uint8_t spinsky_phase = 0;

/*取传递区天空块*/
SM_State *State_GetSky(SM_StateMachine *stateMachine, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_ENTRY:
        ClawEvent.type = SM_EVENT_GETSKY;
        break;
    case SM_EVENT_EXIT: // 退出状态时的操作
        ClawEvent.type = SM_EVENT_IDLE;
        break;

    case SM_EVENT_GETSKY:
        switch (getsky_phase)
        {
        case 1: // 爪子下降到指定高度
            Change_HP_to_Degree(Claw.lift_motor, lift_height.GetSky);
            break;

        case 2: // 爪子翻转，大臂/小臂调整角度，开爪
            solenoid_on(CLAW_SOLENOID_CHANNEL, 1);
            Change_HP_to_Degree(Claw.big_arm_motor, big_arm_pos.GetSky);
            osDelay(100);
            Change_HP_to_Degree(Claw.small_arm_motor, small_arm_pos.GetSky);
            break;

        case 3: // 关爪夹取天空块
            solenoid_on(CLAW_SOLENOID_CHANNEL, 0);
            break;

        case 4: // 爪子翻转，存入机构体内，开爪
            Change_HP_to_Degree(Claw.big_arm_motor, big_arm_pos.InMachine);
            solenoid_on(CLAW_SOLENOID_CHANNEL, 1);
            break;

        default:
            break;
        }
        break;
    default:
        return State_IDLE(stateMachine, event);
    }
    return NULL;
}

/*建塔放天空块*/
SM_State *State_PutSky(SM_StateMachine *stateMachine, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_ENTRY:
        ClawEvent.type = SM_EVENT_PUTSKY;
        break;

    case SM_EVENT_EXIT: // 退出状态时的操作
        ClawEvent.type = SM_EVENT_IDLE;
        break;

    case SM_EVENT_PUTSKY:
        switch (putsky_phase)
        {
        case 1: // 关爪夹取体内天空块，爪子翻转出体外
            solenoid_on(CLAW_SOLENOID_CHANNEL, 0);
            osDelay(100);
            Change_HP_to_Degree(Claw.big_arm_motor, big_arm_pos.PutSky);
            break;

        case 2: // 爪子上升到指定高度
            Change_HP_to_Degree(Claw.lift_motor, lift_height.PutSky);
            break;

        case 3: // 小臂调整角度，开爪
            solenoid_on(CLAW_SOLENOID_CHANNEL, 1);
            Change_HP_to_Degree(Claw.small_arm_motor, small_arm_pos.PutSky);
            break;

        case 4: // 爪子下降到指定高度，翻转进体内
            Change_HP_to_Degree(Claw.lift_motor, lift_height.GetSky);
            osDelay(100);
            Change_HP_to_Degree(Claw.big_arm_motor, big_arm_pos.InMachine);
            break;

        default:
            break;
        }
        break;
    default:
        return State_IDLE(stateMachine, event);
    }
    return NULL;
}

/*翻转对方的天空块*/
SM_State *State_SpinSky(SM_StateMachine *stateMachine, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_ENTRY:
        ClawEvent.type = SM_EVENT_SPINSKY;
        break;

    case SM_EVENT_EXIT: // 退出状态时的操作
        ClawEvent.type = SM_EVENT_IDLE;
        break;

    case SM_EVENT_SPINSKY:
        switch (spinsky_phase)
        {
        case 1: // 爪子翻转出体外
            Change_HP_to_Degree(Claw.big_arm_motor, big_arm_pos.PutSky);
            break;
        case 2: // 爪子上升到指定高度
            Change_HP_to_Degree(Claw.lift_motor, lift_height.PutSky);
            break;
        case 3: // 根据对方天空块的摆放情况，调整大臂/小臂/手腕角度
            solenoid_on(CLAW_SOLENOID_CHANNEL, 1);
            Change_HP_to_Degree(Claw.small_arm_motor, small_arm_pos.PutSky);
            break;
        case 4: // 关爪夹取天空块，爪子抬升一小段距离
            solenoid_on(CLAW_SOLENOID_CHANNEL, 0);
            osDelay(100);
            Change_HP_to_Degree(Claw.lift_motor, lift_height.SpinSkyUp);
            break;
        case 5: // 爪子旋转
            Change_HP_to_Degree(Claw.rotation_motor, rotation_pos.SpinSky);
            break;
        case 6: // 开爪放天空块
            solenoid_on(CLAW_SOLENOID_CHANNEL, 1);

            break;
        case 7: // 关爪，爪子下降到指定高度
            solenoid_on(CLAW_SOLENOID_CHANNEL, 0);
            osDelay(100);
            Change_HP_to_Degree(Claw.lift_motor, lift_height.GetSky);

            break;
        case 8: // 爪子翻转进体内
            Change_HP_to_Degree(Claw.big_arm_motor, big_arm_pos.InMachine);
            break;
        default:
            break;
        }
        break;
    default:
        return State_IDLE(stateMachine, event);
    }
    return NULL;
}

SM_State *State_Motivate(SM_StateMachine *stateMachine, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_ENTRY:
        ClawEvent.type = SM_EVENT_MOTIVATE;
        Claw.big_arm_motor->Begin = true;
        Claw.small_arm_motor->Begin = true;
        Claw.rotation_motor->Begin = true;
        Claw.lift_motor->Begin = true;

        Claw.big_arm_motor->mode = Zdrive_Postion;
        Claw.small_arm_motor->MODE_Set = DJ_Position;
        Claw.lift_motor->MODE_Set = DJ_Position;
        Claw.rotation_motor->MODE_Set = DJ_Position;
        break;
    case SM_EVENT_EXIT:
        ClawEvent.type = SM_EVENT_IDLE;
        break;
    case SM_EVENT_MOTIVATE:
        break;
    default:
        return State_IDLE(stateMachine, event);
    }
    return NULL;
}

SM_State *State_NotMotivate(SM_StateMachine *stateMachine, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_ENTRY:
        ClawEvent.type = SM_EVENT_NOTMOTIVATE;
        Claw.small_arm_motor->Begin = false;
        Claw.big_arm_motor->Begin = false;
        Claw.lift_motor->Begin = false;
        Claw.rotation_motor->Begin = false;
        break;
    case SM_EVENT_EXIT:
        ClawEvent.type = SM_EVENT_IDLE;
        break;
    case SM_EVENT_NOTMOTIVATE:
        break;
    default:
        return State_IDLE(stateMachine, event);
    }
    return NULL;
}

SM_State *State_Zero(SM_StateMachine *stateMachine, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_ENTRY:
        ClawEvent.type = SM_EVENT_ZERO;
        break;
    case SM_EVENT_EXIT:
        ClawEvent.type = SM_EVENT_IDLE;
        break;
    case SM_EVENT_ZERO:
        break;
    default:
        return State_IDLE(stateMachine, event);
    }
    return NULL;
}

SM_State *State_Reset(SM_StateMachine *stateMachine, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_ENTRY:
        ClawEvent.type = SM_EVENT_RESET;
        __set_FAULTMASK(1);
        NVIC_SystemReset();
        break;
    case SM_EVENT_EXIT:
        ClawEvent.type = SM_EVENT_IDLE;
        break;
    case SM_EVENT_RESET:
        break;
    default:
        return State_IDLE(stateMachine, event);
    }
    return NULL;
}