/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for LedTask */
osThreadId_t LedTaskHandle;
const osThreadAttr_t LedTask_attributes = {
  .name = "LedTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for BeepTask */
osThreadId_t BeepTaskHandle;
const osThreadAttr_t BeepTask_attributes = {
  .name = "BeepTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ClawEcho_Task */
osThreadId_t ClawEcho_TaskHandle;
const osThreadAttr_t ClawEcho_Task_attributes = {
  .name = "ClawEcho_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal7,
};
/* Definitions for ClawUpdate_Task */
osThreadId_t ClawUpdate_TaskHandle;
const osThreadAttr_t ClawUpdate_Task_attributes = {
  .name = "ClawUpdate_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void LedWaterTask(void *argument);
void BeepAlarmTask(void *argument);
void Claw_Echo_Func(void *argument);
void Claw_Update_Func(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of LedTask */
  LedTaskHandle = osThreadNew(LedWaterTask, NULL, &LedTask_attributes);

  /* creation of BeepTask */
  BeepTaskHandle = osThreadNew(BeepAlarmTask, NULL, &BeepTask_attributes);

  /* creation of ClawEcho_Task */
  ClawEcho_TaskHandle = osThreadNew(Claw_Echo_Func, NULL, &ClawEcho_Task_attributes);

  /* creation of ClawUpdate_Task */
  ClawUpdate_TaskHandle = osThreadNew(Claw_Update_Func, NULL, &ClawUpdate_Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_LedWaterTask */
/**
  * @brief  Function implementing the LedTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_LedWaterTask */
__weak void LedWaterTask(void *argument)
{
  /* USER CODE BEGIN LedWaterTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END LedWaterTask */
}

/* USER CODE BEGIN Header_BeepAlarmTask */
/**
* @brief Function implementing the BeepTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_BeepAlarmTask */
__weak void BeepAlarmTask(void *argument)
{
  /* USER CODE BEGIN BeepAlarmTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END BeepAlarmTask */
}

/* USER CODE BEGIN Header_Claw_Echo_Func */
/**
* @brief Function implementing the ClawEcho_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Claw_Echo_Func */
__weak void Claw_Echo_Func(void *argument)
{
  /* USER CODE BEGIN Claw_Echo_Func */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Claw_Echo_Func */
}

/* USER CODE BEGIN Header_Claw_Update_Func */
/**
* @brief Function implementing the ClawUpdate_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Claw_Update_Func */
__weak void Claw_Update_Func(void *argument)
{
  /* USER CODE BEGIN Claw_Update_Func */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Claw_Update_Func */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

