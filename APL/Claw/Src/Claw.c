#include "Claw.h"
#include "cmsis_os2.h"

Claw_struct Claw;
static ClawTrajectory Claw_Trajectory[4];

volatile MotorDeg motor_deg[4] =
    {
        {0.0f, 0.0f, 0.0f, 2.68f},         // big_arm_motor: Current_Height, Current_Pos, Target_deg, Degree_Per_Unit
        {0.0f, 0.0f, 0.0f, 1.363636364f},  // small_arm_motor: Current_Height, Current_Pos, Target_deg, Degree_Per_Unit
        {0.0f, 0.0f, 0.0f, 2.0f},          // rotation_motor: Current_Height, Current_Pos, Target_deg, Degree_Per_Unit
        {0.0f, 0.0f, 0.0f, 3.157894737f}}; // lift_motor: Current_Height, Current_Pos, Target_deg, Degree_Per_Unit

volatile uint32_t Claw_Move_Time_ms[4] = {2000U, 2000U, 2000U, 2000U}; // big->small->rotation->lift

void Claw_Init(void)
{
    Claw.big_arm_motor = &Zmotor[0];
    Claw.small_arm_motor = &DJmotor[0];
    Claw.rotation_motor = &DJmotor[1];
    Claw.lift_motor = &DJmotor[2];
}

static void Claw_TrajectorySample(ClawTrajectory *Trajectory, uint32_t Tick) //五次多项式轨迹规划
{
    if (!Trajectory->Active)
    {
        return;
    }

    uint32_t Elapsed = Tick - Trajectory->Start_Tick;
    if (Elapsed >= Trajectory->Time_ms)
    {
        Trajectory->Position = Trajectory->Target_Degree;
        Trajectory->Velocity = 0.0f;
        Trajectory->Acceleration = 0.0f;
        Trajectory->Active = false;
        return;
    }

    float u = (float)Elapsed / (float)Trajectory->Time_ms;
    float Time = (float)Trajectory->Time_ms * 0.001f;
    const float *c = Trajectory->Coefficient;
    Trajectory->Position = c[0] + u * (c[1] + u * (c[2] + u * (c[3] + u * (c[4] + u * c[5]))));
    Trajectory->Velocity = (c[1] + u * (2.0f * c[2] + u * (3.0f * c[3] + u * (4.0f * c[4] + u * 5.0f * c[5])))) / Time;
    Trajectory->Acceleration = (2.0f * c[2] + u * (6.0f * c[3] + u * (12.0f * c[4] + u * 20.0f * c[5]))) / (Time * Time);
}

void Motor_Move(const void *Motor, float Target_HP) // 选择的电机型号；目标位置->对于升降机构来说是目标高度，对于其他机构来说是目标转角
{
    static float Reference_HP[4];     // 四个机构各自的当前高度/当前转角
    static float Reference_Degree[4]; // 对应的电机的输出角度，上述两个参考值是电机第一次被调用后保存的当前值，之后不会发生改变（Initialized = true）
    static bool Initialized[4];       // 判断是否保存当前值（各个电机初始化后不再重复初始化，以第一轮的参考值进行高度转角换算）
    uint8_t Motor_Index;              // 电机索引
    float Current_deg;                // 当前角度（对应电机的valNow角度）
    volatile float *Target_Degree;

    if (Motor == NULL)
    {
        return;
    }

    if (Motor == Claw.big_arm_motor)
    {
        Motor_Index = 0;
        Current_deg = Claw.big_arm_motor->valReal.pos_deg;
        Target_Degree = &Claw.big_arm_motor->valSetNow.pos_deg;
    }
    else if (Motor == Claw.small_arm_motor)
    {
        Motor_Index = 1;
        Current_deg = Claw.small_arm_motor->valNow.angle_deg;
        Target_Degree = &Claw.small_arm_motor->valSet.angle_deg;
    }
    else if (Motor == Claw.rotation_motor)
    {
        Motor_Index = 2;
        Current_deg = Claw.rotation_motor->valNow.angle_deg;
        Target_Degree = &Claw.rotation_motor->valSet.angle_deg;
    }
    else if (Motor == Claw.lift_motor)
    {
        Motor_Index = 3;
        Current_deg = Claw.lift_motor->valNow.angle_deg;
        Target_Degree = &Claw.lift_motor->valSet.angle_deg;
    }

    else
    {
        return;
    }

    volatile float *Current_HP = Motor_Index == 3 ? &motor_deg[Motor_Index].Current_Height
                                                  : &motor_deg[Motor_Index].Current_Pos;
    if (!Initialized[Motor_Index])
    {
        Reference_HP[Motor_Index] = *Current_HP;
        Reference_Degree[Motor_Index] = Current_deg;
        Initialized[Motor_Index] = true;
    }

    float Final_Degree = Reference_Degree[Motor_Index] + (Target_HP - Reference_HP[Motor_Index]) * motor_deg[Motor_Index].Degree_Per_Unit; //轨迹运动结束后的最终位置
    ClawTrajectory *Trajectory = &Claw_Trajectory[Motor_Index];
    uint32_t Tick = HAL_GetTick();

    Claw_TrajectorySample(Trajectory, Tick); //先取得旧轨迹在当前时刻的状态
    if (Trajectory->Motor != Motor || Trajectory->Target_Degree != Final_Degree) //判断是否需要建立新轨迹
    {
        if (Trajectory->Motor != Motor)
        {
            Trajectory->Position = Current_deg;
            Trajectory->Velocity = 0.0f;
            Trajectory->Acceleration = 0.0f;
        }

        Trajectory->Motor = Motor;
        Trajectory->Target_Degree = Final_Degree;
        Trajectory->Time_ms = Claw_Move_Time_ms[Motor_Index];
        if (Trajectory->Time_ms == 0U)
        {
            Trajectory->Time_ms = 1U;
        }
        Trajectory->Start_Tick = Tick;
        Trajectory->Active = true;

        float Time = (float)Trajectory->Time_ms * 0.001f;
        float Distance = Final_Degree - Trajectory->Position;
        float *c = Trajectory->Coefficient;
        c[0] = Trajectory->Position;
        c[1] = Trajectory->Velocity * Time;
        c[2] = 0.5f * Trajectory->Acceleration * Time * Time;
        c[3] = 10.0f * Distance - 6.0f * c[1] - 3.0f * c[2];
        c[4] = -15.0f * Distance + 8.0f * c[1] + 3.0f * c[2];
        c[5] = 6.0f * Distance - 3.0f * c[1] - c[2];
    }

    Trajectory->Target_HP = Target_HP;
    motor_deg[Motor_Index].Target_deg = Final_Degree;  //保存最终目标
    *Target_Degree = Trajectory->Position;  //输出中间目标
    *Current_HP = Reference_HP[Motor_Index] + (Current_deg - Reference_Degree[Motor_Index]) / motor_deg[Motor_Index].Degree_Per_Unit;  //更新实际位置
}

void Claw_TrajectoryUpdate(void)
{
    for (uint8_t Motor_Index = 0; Motor_Index < 4U; Motor_Index++)
    {
        ClawTrajectory *Trajectory = &Claw_Trajectory[Motor_Index];
        if (Trajectory->Motor == NULL)
        {
            continue;
        }

        bool Enabled;
        if (Motor_Index == 0U)
        {
            Enabled = Claw.big_arm_motor->Begin && Claw.big_arm_motor->mode == Zdrive_Postion;
        }
        else
        {
            DJMotorPointer Motor = Motor_Index == 1U   ? Claw.small_arm_motor
                                   : Motor_Index == 2U ? Claw.rotation_motor
                                                       : Claw.lift_motor;
            Enabled = Motor->Begin && Motor->MODE_Set == DJ_Position;
        }

        if (!Enabled)
        {
            Trajectory->Motor = NULL;
            Trajectory->Active = false;
            continue;
        }

        Motor_Move(Trajectory->Motor, Trajectory->Target_HP);
    }
}

void Claw_Delay(uint32_t Ticks) // osDelay期间不执行更新轨迹，Claw_Delay是为了让原来的等待期间，五次轨迹也能持续更新。
{
    while (Ticks > 0U)
    {
        uint32_t Delay_Ticks = Ticks > 10U ? 10U
                                           : Ticks;
        osDelay(Delay_Ticks);
        Claw_TrajectoryUpdate();
        Ticks -= Delay_Ticks;
    }
}