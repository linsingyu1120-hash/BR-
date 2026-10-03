#ifndef __MYOSTASKS_H
#define __MYOSTASKS_H

#include "cmsis_os2.h"
#include "main.h"
#include "Led.h"
#include "Beep.h"

extern uint8_t BeepAlarmTimes;

void LedWaterTask(void *argument);
void BeepAlarmTask(void *argument);
void Claw_Echo_Func(void *argument);
void Claw_Update_Func(void *argument);

#endif // __MYOSTASKS_H