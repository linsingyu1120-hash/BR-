#include "Claw.h"

Claw_struct Claw; 

volatile MotorDeg motor_deg[4] =
    {
        {0.0f, 0.0f, 0.0f, -1.0f},  // small_arm_motor: Current_Height, Current_Pos, Target_deg, Degree_Per_Unit
        {0.0f, 0.0f, 0.0f, -1.0f},  // big_arm_motor: Current_Height, Current_Pos, Target_deg, Degree_Per_Unit
        {0.0f, 0.0f, 0.0f, -1.0f},  // lift_motor: Current_Height, Current_Pos, Target_deg, Degree_Per_Unit
        {0.0f, 0.0f, 0.0f, -1.0f}}; // rotation_motor: Current_Height, Current_Pos, Target_deg, Degree_Per_Unit

void Claw_Init(void)
{
    Claw.small_arm_motor = &DJmotor[0];
    Claw.big_arm_motor = &DJmotor[1];
    Claw.lift_motor = &DJmotor[2];
    Claw.rotation_motor = &Zmotor[3];

    Claw.small_arm_motor->Begin = true;
    Claw.big_arm_motor->Begin = true;
    Claw.lift_motor->Begin = true;
    Claw.rotation_motor->Begin = true;

    Claw.small_arm_motor->MODE_Set = DJ_Position;
    Claw.big_arm_motor->MODE_Set = DJ_Position;
    Claw.lift_motor->MODE_Set = DJ_Position;
    Claw.rotation_motor->mode = Zdrive_Postion; 
}

void Change_HP_to_Degree(const void *Motor, float Target_HP) // 选择的电机型号；目标位置->对于升降机构来说是目标高度，对于其他机构来说是目标转角
{
    static float Reference_HP[4];     // 四个机构各自的当前高度/当前转角
    static float Reference_Degree[4]; // 对应的电机的输出角度，上述两个参考值是电机第一次被调用后保存的当前值，之后不会发生改变（Initialized = true）
    static bool Initialized[4];       // 判断是否保存当前值（各个电机初始化后不再重复初始化，以第一轮的参考值进行高度转角换算）
    uint8_t Motor_Index;              // 电机索引
    float Current_deg;                // 当前角度（对应电机的valNow角度）
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

    float Degree_Per_Unit = motor_deg[Motor_Index].Degree_Per_Unit;

    volatile float *Current_HP = Motor_Index == 2 ? &motor_deg[Motor_Index].Current_Height
                                                  : &motor_deg[Motor_Index].Current_Pos;
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