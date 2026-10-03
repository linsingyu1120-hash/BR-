#include "myState.h"

SM_StateMachine Claw_SM; // 爪子状态机
SM_Event ClawEvent;      // 爪子触发事件

static bool Guard(SM_StateMachine *sm, const SM_Event *event);
static void Action(SM_StateMachine *sm, const SM_Event *event);
static void TranitionInit(void);

SM_Transition SMTranList[MAX_TRANSITION_NUM] = {0}; // 状态转移情况列表

SM_State IdleState = {State_IDLE, NULL, NULL}; // 父状态，遥控事件放在父状态
SM_State GetSkyState = {State_GetSky, &IdleState, NULL};
SM_State PutSkyState = {State_PutSky, &IdleState, NULL};
SM_State SpinSkyState = {State_SpinSky, &IdleState, NULL};

void MySM_Init(void)
{
    SM_Init(&Claw_SM, &IdleState);
    TranitionInit();
}

void TransitionInit()
{
    //    SM_AddTransition(SMTranList, MAX_TRANSITION_NUM, &ChassisRunTrajState, SM_EVENT_TRAJ,
    //                     &ChassisLockPointState, Guard, Action);
    SM_AddTransition(SMTranList, MAX_TRANSITION_NUM, &ChassisRunTrajState, SM_EVENT_TRAJ,
                     &ChassisIdleState, Guard, Action);
    SM_AddTransition(SMTranList, MAX_TRANSITION_NUM, &SaveStaffState, SM_EVENT_SAVESTAFF,
                     &IdleState, Guard, Action);
}

void StateEventUpdate()
{
    if (ChassisEvent.type != SM_EVENT_REMOTE || RobotEvent.type != SM_EVENT_REMOTE)
    {
        Laser_RX_Count++;
    }
    if (Laser_RX_Count > 20)
    {
        ChassisEvent.type = SM_EVENT_REMOTE;
        RobotEvent.type = SM_EVENT_REMOTE;
        BoolButton.CHASSISBUTTON.Chassis_Traj = 0;
    }
    Deal_Bool_RxMsg(&RxMsgPack, &Chassis);
    Robot_Update(&SMR1_Robot);
    if (RobotEvent.type == SM_EVENT_SAVESTAFF)
    {
        SMR1_Robot.region = 1;
        SMR1_Robot.last_region = 1;
    }
    TxMsgPack.shorts[4] = SMR1_Robot.task_mode;
    Deal_BLE_TxMsg(&TxMsgPack, &Chassis);
#if VOFA_TX_MODE == VOFA_TX_MODE_STATE_EVENT
    VOFA_DealTxMsg(&VofaTxPack);
#endif
    // SMTajParam.p_now = Chassis.ChassisPostureReal.Dis;
    if (SMR1_Robot.task_mode == MODE_IDLE && (BoolButton.CHASSISBUTTON.LockPoint == 0) && (BoolButton.CHASSISBUTTON.Chassis_Traj == 0) && !BoolButton.AutoAct)
    {
        ChassisEvent.type = SM_EVENT_REMOTE;
        RobotEvent.type = SM_EVENT_REMOTE;
        BoolButton.CHASSISBUTTON.Chassis_Traj = 0;
    }
    if (SMR1_Robot.mode != ROBOT_MODE_REMOTE)
    {
        //		if (SMR1_Robot.task_mode == MODE_COMBARM && SMR1_Robot.region == 1) {
        //			RobotEvent.type = SM_EVENT_COMBARM;
        //		}
        if (SMR1_Robot.task_mode == SMR1_Robot.task_mode_pre || (ChassisEvent.type != SM_EVENT_REMOTE && ChassisEvent.type != SM_EVENT_CROSSBRAKE) || RobotEvent.type != SM_EVENT_REMOTE)
        {
            return;
        }
        else
        {
            SMR1_Robot.task_mode_pre = SMR1_Robot.task_mode;
            // 用 task_mode * (region + 10) 作为唯一键路由到对应事件：
            // region=1 时乘 11, region=2 乘 12, region=3 乘 13，避免不同 region
            // 的相同 task_mode 发生冲突
            switch (SMR1_Robot.task_mode * (SMR1_Robot.region + 10))
            {
            case MODE_IDLE:
                ChassisEvent.type = SM_EVENT_REMOTE;
                RobotEvent.type = SM_EVENT_REMOTE;
                break;
            case MODE_GETARM * 11:
                RobotEvent.type = SM_EVENT_GETSTAFF;
                break;
            case MODE_COMBARM * 11:
                RobotEvent.type = SM_EVENT_COMBARM;
                break;
            case MODE_SAVESTAFF * 11:
                RobotEvent.type = SM_EVENT_SAVESTAFF;
                break;
            case MODE_EXIT1 * 11:
                RobotEvent.type = SM_EVENT_EXIT1;
                break;
            case MODE_GET_S1 * 12:
                RobotEvent.type = SM_EVENT_GETSCROLL1;
                break;
            case MODE_GET_S2 * 12:
                RobotEvent.type = SM_EVENT_GETSCROLL2;
                break;
            case MODE_GET_S3 * 12:
                RobotEvent.type = SM_EVENT_GETSCROLL3;
                break;
            case MODE_FINAL2 * 12:
                RobotEvent.type = SM_EVENT_EXIT2;
                break;
            case MODE_UPHILL * 13:
                RobotEvent.type = SM_EVENT_UPHILL;
                break;
            case MODE_PUTBLOCK * 13:
                RobotEvent.type = SM_EVENT_PUTBLOCK;
                break;
            case 14: // 1 * 14, 四区R2配合锁点
                RobotEvent.type = SM_EVENT_R2LIFT;
                break;
            case 15: // 1 * 15, 五区背取双块
                RobotEvent.type = SM_EVENT_ZONE5BACK;
                break;
            case 16: // 1 * 16, 六区背取单块
                RobotEvent.type = SM_EVENT_ZONE6BACK;
                break;
            default:
                ChassisEvent.type = SM_EVENT_REMOTE;
                RobotEvent.type = SM_EVENT_REMOTE;
                break;
            }
        }
    }
    else
    {
        if (RobotEvent.type != SM_EVENT_REMOTE)
        {
            ChassisEvent.type = SM_EVENT_REMOTE; // 确保底盘也退出自动模式，解除阻塞while循环
            SM_TransitionTo(&Robot_SM, &IdleState);
        }
        RobotEvent.type = SM_EVENT_REMOTE;
    }
}

void ClawEventEcho()
{
}

SM_State *State_IDLE(SM_StateMachine *stateMachine, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_ENTRY:
        // BEEP_Alarm(1);
        RobotEvent.type = SM_EVENT_REMOTE;
        // 进入空闲状态时的操作
        break;
    case SM_EVENT_EXIT:
        // 退出操作
        break;
    case SM_EVENT_REMOTE:
        ActuatorRemote(SMR1_Robot);
        break;
    case SM_EVENT_GETSKY:
        SM_TransitionTo(stateMachine, &GetSkyState);
        break;
    case SM_EVENT_PUTSKY:
        SM_TransitionTo(stateMachine, &PutSkyState);
        break;
    case SM_EVENT_SPINSKY:
        SM_TransitionTo(stateMachine, &SpinSkyState);
        break;

    default:
        RobotEvent.type = SM_EVENT_REMOTE;
        break;
    }
    return NULL;
}

void Enable_Check()
{
    if (BoolButton.CHASSISBUTTON.Protect && BoolButton.CHASSISBUTTON.Chassis_Enable)
    {
        if (!Chassis.enable)
            Master_ChassisEnable(&Chassis);
        IfChassis_Enable(&Chassis);
    }
    else if (!BoolButton.CHASSISBUTTON.Chassis_Enable)
    {
        ChassisEvent.type = SM_EVENT_REMOTE;
        BoolButton.CHASSISBUTTON.Protect = false;
        Master_ChassisDisable(&Chassis);
    }
    if (BoolButton.ACTBUTTON.Act_Enable && BoolButton.ACTBUTTON.Act_Protect)
    {
        if (!SMR1_Robot._arm[0]->enable || !SMR1_Robot._arm[1]->enable)
            ActComFunc(EN, 2, 'M', 1);
    }
    else if (!BoolButton.ACTBUTTON.Act_Enable)
    {
        ActComFunc(EN, 2, 'M', 0);
        SMR1_Robot._arm[0]->enable = 0;
        SMR1_Robot._arm[1]->enable = 0;
        BoolButton.ACTBUTTON.Act_Protect = false;
    }
}

static bool Guard(SM_StateMachine *sm, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_TRAJ:
        if (SMTajParam.run_time > (SMTajParam.BezierParam.TotalT - TranCheckPeriod) || GetTheDis2Robot((VECTOR_2D){
                                                                                           SMTajParam.BezierParam.BezierCtrlPoints[SMTajParam.BezierParam.rank].x,
                                                                                           SMTajParam.BezierParam.BezierCtrlPoints[SMTajParam.BezierParam.rank].y}) < 25.f * SMTajParam.EndSpeed)
        {
            BoolButton.CHASSISBUTTON.Chassis_Traj = false;
            return true;
        }
        return false;
    case SM_EVENT_COMBARM:
        return false;
    default:
        break;
    }
    return false;
}

static void Action(SM_StateMachine *sm, const SM_Event *event)
{
    switch (event->type)
    {
    case SM_EVENT_TRAJ:
        if (SMTajParam.IfCont == true)
        {
            // 衔接下一段：不减速
            ChassisEvent.type = SM_EVENT_NONE;
            SMTajParam.IfCont = false;
        }
        else
        {
            if (SMTajParam.BezierParam.IfLockPointAfterTraj)
            {
                ChassisEvent.type = SM_EVENT_LOCKPOINT;
                LockParam.angle = SMTajParam.BezierParam.EndAngle;
                LockParam.threshold.LOCKpoint_POSthreshold.LockAng_Threshold = 0.7f;
                LockParam.threshold.LOCKpoint_POSthreshold.LockPos_Threshold = 10.f;
                LockParam.LockPoint = SMTajParam.LockPoint;
                SMTajParam.BezierParam.IfLockPointAfterTraj = false;
                SMTajParam.traj.iflockpointafterTraj = false;
            }
            else
            {
                Chassis.ChassisPostureSet.Vel.x = 0.f;
                Chassis.ChassisPostureSet.Vel.y = 0.f;
                Chassis.ChassisPostureSet._AngW = 0.f;
                ChassisEvent.type = SM_EVENT_REMOTE;
            }
        }
        break;

    default:
        break;
    }
}