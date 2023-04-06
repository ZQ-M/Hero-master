/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "main.h"
#include "task.h"

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
osThreadId defaultTaskHandle;
uint32_t defaultTaskBuffer[512];
osStaticThreadDef_t defaultTaskControlBlock;
osThreadId shellTaskHandle;
uint32_t shellTaskBuffer[1024];
osStaticThreadDef_t shellTaskControlBlock;
osThreadId remoteTaskHandle;
uint32_t remoteTaskBuffer[512];
osStaticThreadDef_t remoteTaskControlBlock;
osThreadId monitorTaskHandle;
uint32_t monitorTaskBuffer[512];
osStaticThreadDef_t monitorTaskControlBlock;
osThreadId chassisTaskHandle;
uint32_t chassisTaskBuffer[512];
osStaticThreadDef_t chassisTaskControlBlock;
/* osThreadId parseCan1RxDataHandle;
uint32_t parseCanRxDataTBuffer[ 512 ]; */
osStaticThreadDef_t parseCanRxDataTControlBlock;
osThreadId gimbalTaskHandle;
uint32_t gimbalTaskBuffer[512];
osStaticThreadDef_t gimbalTaskControlBlock;
osThreadId buzzerTaskHandle;
uint32_t buzzerTaskBuffer[128];
osStaticThreadDef_t buzzerTaskControlBlock;
osThreadId shootTaskHandle;
uint32_t shootTaskBuffer[512];
osStaticThreadDef_t shootTaskControlBlock;
osThreadId gyroscopeTaskHandle;
uint32_t gyroscopeTaskBuffer[512];
osStaticThreadDef_t gyroscopeTaskControlBlock;
osThreadId waveWheelTaskHandle;
uint32_t waveWheelTaskBuffer[512];
osStaticThreadDef_t waveWheelTaskControlBlock;
osThreadId superCapacitorTHandle;
uint32_t superCapacitorTBuffer[ 512 ];
osStaticThreadDef_t superCapacitorTControlBlock;
osThreadId refereeSystemTaHandle;
uint32_t refereeSystemTaBuffer[ 512 ];
osStaticThreadDef_t refereeSystemTaControlBlock;
osThreadId ReferenceTaskHandle;
uint32_t ReferenceTaskBuffer[ 512 ];
osStaticThreadDef_t ReferenceTaskControlBlock;
osThreadId referenceGyroscHandle;
uint32_t referenceGyroscBuffer[ 512 ];
osStaticThreadDef_t referenceGyroscControlBlock;
osThreadId clientUiTaskHandle;
uint32_t clientUiTaskBuffer[ 512 ];
osStaticThreadDef_t clientUiTaskControlBlock;
osMessageQId buzzerQueueHandle;
uint8_t buzzerQueueBuffer[ 5 * 5 ];
osStaticMessageQDef_t buzzerQueueControlBlock;
osMutexId printfMutexHandle;
osSemaphoreId shellGetDataBinarySemHandle;
osStaticSemaphoreDef_t myBinarySem01ControlBlock;
osSemaphoreId remoteGetDataBinarySemHandle;
osStaticSemaphoreDef_t remoteGetDataBinarySemControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartShellTask(void const * argument);
void StartRemoteTask(void const * argument);
void StartMonitorTask(void const * argument);
void StartChassisTask(void const * argument);
void StartParseCan1RxDataTask(void const * argument);
void StartGimbalTask(void const * argument);
void StartBuzzerTask(void const * argument);
void StartShootTask(void const * argument);
void StartGyroscopeTask(void const * argument);
void StartWaveWheelTask(void const * argument);
void StartSuperCapacitorTask(void const * argument);
void StartRefereeSystemTask(void const * argument);
void StartReferenceTaskTask(void const * argument);
void StartextErnalGyroscopeParseTask(void const * argument);
void StartClientUiTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of printfMutex */
  osMutexDef(printfMutex);
  printfMutexHandle = osMutexCreate(osMutex(printfMutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of shellGetDataBinarySem */
  osSemaphoreStaticDef(shellGetDataBinarySem, &myBinarySem01ControlBlock);
  shellGetDataBinarySemHandle = osSemaphoreCreate(osSemaphore(shellGetDataBinarySem), 1);

  /* definition and creation of remoteGetDataBinarySem */
  osSemaphoreStaticDef(remoteGetDataBinarySem, &remoteGetDataBinarySemControlBlock);
  remoteGetDataBinarySemHandle = osSemaphoreCreate(osSemaphore(remoteGetDataBinarySem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of buzzerQueue */
  osMessageQStaticDef(buzzerQueue, 5, 5, buzzerQueueBuffer, &buzzerQueueControlBlock);
  buzzerQueueHandle = osMessageCreate(osMessageQ(buzzerQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadStaticDef(defaultTask, StartDefaultTask, osPriorityIdle, 0, 512, defaultTaskBuffer, &defaultTaskControlBlock);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of shellTask */
  osThreadStaticDef(shellTask, StartShellTask, osPriorityBelowNormal, 0, 1024, shellTaskBuffer, &shellTaskControlBlock);
  shellTaskHandle = osThreadCreate(osThread(shellTask), NULL);

  /* definition and creation of remoteTask */
  osThreadStaticDef(remoteTask, StartRemoteTask, osPriorityHigh, 0, 512, remoteTaskBuffer, &remoteTaskControlBlock);
  remoteTaskHandle = osThreadCreate(osThread(remoteTask), NULL);

  /* definition and creation of monitorTask */
  osThreadStaticDef(monitorTask, StartMonitorTask, osPriorityLow, 0, 512, monitorTaskBuffer, &monitorTaskControlBlock);
  monitorTaskHandle = osThreadCreate(osThread(monitorTask), NULL);

  /* definition and creation of chassisTask */
  osThreadStaticDef(chassisTask, StartChassisTask, osPriorityAboveNormal, 0, 512, chassisTaskBuffer, &chassisTaskControlBlock);
  chassisTaskHandle = osThreadCreate(osThread(chassisTask), NULL);

  /* definition and creation of gimbalTask */
  osThreadStaticDef(gimbalTask, StartGimbalTask, osPriorityAboveNormal, 0, 512, gimbalTaskBuffer, &gimbalTaskControlBlock);
  gimbalTaskHandle = osThreadCreate(osThread(gimbalTask), NULL);

  /* definition and creation of buzzerTask */
  osThreadStaticDef(buzzerTask, StartBuzzerTask, osPriorityNormal, 0, 128, buzzerTaskBuffer, &buzzerTaskControlBlock);
  buzzerTaskHandle = osThreadCreate(osThread(buzzerTask), NULL);

  /* definition and creation of shootTask */
  osThreadStaticDef(shootTask, StartShootTask, osPriorityNormal, 0, 512, shootTaskBuffer, &shootTaskControlBlock);
  shootTaskHandle = osThreadCreate(osThread(shootTask), NULL);

  /* definition and creation of gyroscopeTask */
  osThreadStaticDef(gyroscopeTask, StartGyroscopeTask, osPriorityNormal, 0, 512, gyroscopeTaskBuffer, &gyroscopeTaskControlBlock);
  gyroscopeTaskHandle = osThreadCreate(osThread(gyroscopeTask), NULL);

  /* definition and creation of waveWheelTask */
  osThreadStaticDef(waveWheelTask, StartWaveWheelTask, osPriorityAboveNormal, 0, 512, waveWheelTaskBuffer, &waveWheelTaskControlBlock);
  waveWheelTaskHandle = osThreadCreate(osThread(waveWheelTask), NULL);

  /* definition and creation of superCapacitorT */
  osThreadStaticDef(superCapacitorT, StartSuperCapacitorTask, osPriorityIdle, 0, 512, superCapacitorTBuffer, &superCapacitorTControlBlock);
  superCapacitorTHandle = osThreadCreate(osThread(superCapacitorT), NULL);

  /* definition and creation of ReferenceTask */
  osThreadStaticDef(ReferenceTask, StartReferenceTaskTask, osPriorityIdle, 0, 512, ReferenceTaskBuffer, &ReferenceTaskControlBlock);
  ReferenceTaskHandle = osThreadCreate(osThread(ReferenceTask), NULL);

  /* definition and creation of referenceGyrosc */
  osThreadStaticDef(referenceGyrosc, StartextErnalGyroscopeParseTask, osPriorityNormal, 0, 512, referenceGyroscBuffer, &referenceGyroscControlBlock);
  referenceGyroscHandle = osThreadCreate(osThread(referenceGyrosc), NULL);

  /* definition and creation of clientUiTask */
  osThreadStaticDef(clientUiTask, StartClientUiTask, osPriorityNormal, 0, 512, clientUiTaskBuffer, &clientUiTaskControlBlock);
  clientUiTaskHandle = osThreadCreate(osThread(clientUiTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
__weak void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartShellTask */
/**
* @brief Function implementing the shellTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartShellTask */
__weak void StartShellTask(void const * argument)
{
  /* USER CODE BEGIN StartShellTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartShellTask */
}

/* USER CODE BEGIN Header_StartRemoteTask */
/**
* @brief Function implementing the remoteTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRemoteTask */
__weak void StartRemoteTask(void const *argument)
{
  /* USER CODE BEGIN StartRemoteTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartRemoteTask */
}

/* USER CODE BEGIN Header_StartMonitorTask */
/**
* @brief Function implementing the monitorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMonitorTask */
__weak void StartMonitorTask(void const *argument)
{
  /* USER CODE BEGIN StartMonitorTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartMonitorTask */
}

/* USER CODE BEGIN Header_StartChassisTask */
/**
* @brief Function implementing the chassisTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartChassisTask */
__weak void StartChassisTask(void const *argument)
{
  /* USER CODE BEGIN StartChassisTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartChassisTask */
}

/* USER CODE BEGIN Header_StartParseCan1RxDataTask */
/**
* @brief Function implementing the parseCan1RxData thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartParseCan1RxDataTask */
__weak void StartParseCan1RxDataTask(void const *argument)
{
  /* USER CODE BEGIN StartParseCan1RxDataTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartParseCan1RxDataTask */
}

/* USER CODE BEGIN Header_StartGimbalTask */
/**
* @brief Function implementing the gimbalTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGimbalTask */
__weak void StartGimbalTask(void const *argument)
{
  /* USER CODE BEGIN StartGimbalTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartGimbalTask */
}

/* USER CODE BEGIN Header_StartBuzzerTask */
/**
* @brief Function implementing the buzzerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBuzzerTask */
__weak void StartBuzzerTask(void const *argument)
{
  /* USER CODE BEGIN StartBuzzerTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartBuzzerTask */
}

/* USER CODE BEGIN Header_StartShootTask */
/**
* @brief Function implementing the shootTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartShootTask */
__weak void StartShootTask(void const *argument)
{
  /* USER CODE BEGIN StartShootTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartShootTask */
}

/* USER CODE BEGIN Header_StartGyroscopeTask */
/**
* @brief Function implementing the gyroscopeTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGyroscopeTask */
__weak void StartGyroscopeTask(void const *argument)
{
  /* USER CODE BEGIN StartGyroscopeTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartGyroscopeTask */
}

/* USER CODE BEGIN Header_StartWaveWheelTask */
/**
* @brief Function implementing the waveWheelTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartWaveWheelTask */
__weak void StartWaveWheelTask(void const *argument)
{
  /* USER CODE BEGIN StartWaveWheelTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartWaveWheelTask */
}

/* USER CODE BEGIN Header_StartSuperCapacitorTask */
/**
* @brief Function implementing the superCapacitorT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSuperCapacitorTask */
__weak void StartSuperCapacitorTask(void const *argument)
{
  /* USER CODE BEGIN StartSuperCapacitorTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartSuperCapacitorTask */
}

/* USER CODE BEGIN Header_StartRefereeSystemTask */
/**
* @brief Function implementing the refereeSystemTa thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRefereeSystemTask */
__weak void StartRefereeSystemTask(void const *argument)
{
  /* USER CODE BEGIN StartRefereeSystemTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartRefereeSystemTask */
}

/* USER CODE BEGIN Header_StartReferenceTaskTask */
/**
* @brief Function implementing the ReferenceTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartReferenceTaskTask */
__weak void StartReferenceTaskTask(void const *argument)
{
  /* USER CODE BEGIN StartReferenceTaskTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartReferenceTaskTask */
}

/* USER CODE BEGIN Header_StartextErnalGyroscopeParseTask */
/**
* @brief Function implementing the referenceGyrosc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartextErnalGyroscopeParseTask */
__weak void StartextErnalGyroscopeParseTask(void const *argument)
{
  /* USER CODE BEGIN StartextErnalGyroscopeParseTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartextErnalGyroscopeParseTask */
}

/* USER CODE BEGIN Header_StartClientUiTask */
/**
* @brief Function implementing the clientUiTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartClientUiTask */
__weak void StartClientUiTask(void const *argument)
{
  /* USER CODE BEGIN StartClientUiTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartClientUiTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
