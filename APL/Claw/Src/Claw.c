#include "Claw.h"

Claw_struct Claw; 

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