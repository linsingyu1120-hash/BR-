#include "myState.h"
#include "cmsis_os2.h"
#include "solenoid.h"

/*爪子任务静态变量*/
volatile MotorDeg motor_deg[4] =
    {
        {0.0f, 0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, 0.0f, -1.0f}};

static void Change_HP_to_Degree(const void *Motor, float Target_HP)
{
    static float Reference_HP[4];
    static float Reference_Degree[4];
    static bool Initialized[4];
    uint8_t Motor_Index;
    float Current_deg;
    volatile float *Target_Degree;

    if (Motor == Claw.small_arm_motor)
    {
        Motor_Index = 0;
        Current_deg = Claw.small_arm_motor->valNow.angle_deg;
        Target_Degree = &Claw.small_arm_motor->valSet.angle_deg;
    }
    else if (Motor == Claw.big_arm_motor)
    {
        Motor_Index = 1;
        Current_deg = Claw.big_arm_motor->valNow.angle_deg;
        Target_Degree = &Claw.big_arm_motor->valSet.angle_deg;
    }
    else if (Motor == Claw.lift_motor)
    {
        Motor_Index = 2;
        Current_deg = Claw.lift_motor->valNow.angle_deg;
        Target_Degree = &Claw.lift_motor->valSet.angle_deg;
    }
    else if (Motor == Claw.rotation_motor)
    {
        Motor_Index = 3;
        Current_deg = Claw.rotation_motor->valReal.pos_deg;
        Target_Degree = &Claw.rotation_motor->valSetNow.pos_deg;
    }
    else
    {
        return;
    }

    float Degree_Per_Unit = motor_deg[Motor_Index].Degree_Per_Unit;
    if (Degree_Per_Unit == 0.0f)
    {
        return;
    }

    volatile float *Current_HP = Motor_Index == 2 ? &motor_deg[Motor_Index].Current_Height : &motor_deg[Motor_Index].Current_Pos;
    if (!Initialized[Motor_Index])
    {
        Reference_HP[Motor_Index] = *Current_HP;
        Reference_Degree[Motor_Index] = Current_deg;
        Initialized[Motor_Index] = true;
    }

    motor_deg[Motor_Index].Target_deg = Reference_Degree[Motor_Index] + (Target_HP - Reference_HP[Motor_Index]) * Degree_Per_Unit;
    *Target_Degree = motor_deg[Motor_Index].Target_deg;
    *Current_HP = Reference_HP[Motor_Index] + (Current_deg - Reference_Degree[Motor_Index]) / Degree_Per_Unit;
}

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

        case 2: // 爪子翻转，小臂/手腕调整角度，开爪
            Change_HP_to_Degree(Claw.big_arm_motor, big_arm_pos.OutMachine);
            osDelay(100);
            Change_HP_to_Degree(Claw.small_arm_motor, small_arm_pos.GetSky);
            Change_HP_to_Degree(Claw.rotation_motor, rotation_pos.GetSky);
            solenoid_on(CLAW_SOLENOID_CHANNEL, 1);
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
            Change_HP_to_Degree(Claw.big_arm_motor, big_arm_pos.OutMachine);
            break;

        case 2: // 爪子上升到指定高度
            Change_HP_to_Degree(Claw.lift_motor, lift_height.PutSky);
            break;

        case 3: // 大臂/小臂/手腕调整角度，开爪
            solenoid_on(CLAW_SOLENOID_CHANNEL, 1);
            Change_HP_to_Degree(Claw.big_arm_motor, big_arm_pos.PutSky);
            Change_HP_to_Degree(Claw.small_arm_motor, small_arm_pos.PutSky);
            Change_HP_to_Degree(Claw.rotation_motor, rotation_pos.PutSky);
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
            Change_HP_to_Degree(Claw.big_arm_motor, big_arm_pos.OutMachine);
            break;
        case 2: // 爪子上升到指定高度
            Change_HP_to_Degree(Claw.lift_motor, lift_height.SpinSky);
            break;
        case 3: // 根据对方天空块的摆放情况，调整大臂/小臂/手腕角度
            solenoid_on(CLAW_SOLENOID_CHANNEL, 1);
            Change_HP_to_Degree(Claw.big_arm_motor, big_arm_pos.SpinSky);
            Change_HP_to_Degree(Claw.small_arm_motor, small_arm_pos.SpinSky);
            Change_HP_to_Degree(Claw.rotation_motor, rotation_pos.SpinSky);
            break;
        case 4: // 关爪夹取天空块，爪子抬升一小段距离
            solenoid_on(CLAW_SOLENOID_CHANNEL, 0);
            osDelay(100);
            Change_HP_to_Degree(Claw.lift_motor, lift_height.SpinSkyUp);
            break;
        case 5: // 爪子旋转
            Change_HP_to_Degree(Claw.rotation_motor, rotation_pos.SpinSkyEnd);
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
    if (event->type != SM_EVENT_ENTRY)
    {
        if (event->type == SM_EVENT_EXIT)
        {
            return NULL;
        }
        return State_IDLE(stateMachine, event);
    }

    Claw.small_arm_motor->Begin = true;
    Claw.big_arm_motor->Begin = true;
    Claw.lift_motor->Begin = true;
    Claw.rotation_motor->Begin = true;

    Claw.small_arm_motor->MODE_Set = DJ_Position;
    Claw.big_arm_motor->MODE_Set = DJ_Position;
    Claw.lift_motor->MODE_Set = DJ_Position;
    Claw.rotation_motor->mode = Zdrive_Postion;
    ClawEvent.type = SM_EVENT_IDLE;

    return NULL;
}

SM_State *State_NotMotivate(SM_StateMachine *stateMachine, const SM_Event *event)
{
    if (event->type != SM_EVENT_ENTRY)
    {
        if (event->type == SM_EVENT_EXIT)
        {
            return NULL;
        }
        return State_IDLE(stateMachine, event);
    }

    Claw.small_arm_motor->Begin = false;
    Claw.big_arm_motor->Begin = false;
    Claw.lift_motor->Begin = false;
    Claw.rotation_motor->Begin = false;
    ClawEvent.type = SM_EVENT_IDLE;

    return NULL;
}

SM_State *State_Zero(SM_StateMachine *stateMachine, const SM_Event *event)
{
    if (event->type == SM_EVENT_ENTRY)
    {
        // 机械寻零方向和限位尚未标定，保留原来的空实现。
        ClawEvent.type = SM_EVENT_IDLE;
    }
    else if (event->type != SM_EVENT_EXIT)
    {
        return State_IDLE(stateMachine, event);
    }
    return NULL;
}

SM_State *State_Reset(SM_StateMachine *stateMachine, const SM_Event *event)
{
    if (event->type == SM_EVENT_ENTRY)
    {
        __set_FAULTMASK(1);
        NVIC_SystemReset();
    }
    else if (event->type != SM_EVENT_EXIT)
    {
        return State_IDLE(stateMachine, event);
    }
    return NULL;
}