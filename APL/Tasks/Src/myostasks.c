#include "myostasks.h"

uint8_t BeepAlarmTimes = 0;

void LedWaterTask(void *argument)
{
  for (;;)
  {
    Led_Water();
  }
}

void BeepAlarmTask(void *argument)
{
  for (;;)
  {
    uint8_t count = BeepAlarmTimes;
    BeepAlarmTimes = 0;
    for (uint8_t i = 0; i < count; i++)
    {
      BEEP_ON();
      osDelay(40);
      BEEP_OFF();
      osDelay(40);
    }

    osDelay(1);
  }
}

void Claw_Echo_Func(void *argument)
{
  for (;;)
  {
    ClawEventEcho();
    //UBaseType_t uxWaterMark = uxTaskGetStackHighWaterMark(RobotEcho_TaskHandle);
    osDelay(10);
  }
}

void Claw_Update_Func(void *argument)
{
  for (;;)
  {
    StateEvent_Update();
    osDelay(30);
  }
}
