/* USER CODE BEGIN Header */
#include "../Inc/main.h"
#ifndef TB_DEF
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "api.h"
#include "lwip/tcp.h"
#include "ethernetif.h"
#include "params_func.h"
#include "cpu_clock_count.h"
#include "usart.h"
#else        
#endif
#include "string.h"
#include "stdio.h"
#include "math.h"
#include "../Inc/global.h"
#include "../Inc/server.h"
#include "../Inc/tools.h"
#include "../Inc/commands_handler.h"

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

device_struct mcs_storage;

QueueHandle_t link_queue;
QueueHandle_t int_can_mess_queue;
QueueHandle_t indication_queue;
QueueHandle_t adc_data_queue;
QueueHandle_t tcp_rx_data_queue;
QueueHandle_t tcp_tx_data_queue;
QueueHandle_t exti_alarm_queue;

uint8_t uart1_rx_byte;

/* USER CODE END Variables */
osThreadId main_taskHandle;
uint32_t main_taskBuffer[ 2048 ];
osStaticThreadDef_t main_taskControlBlock;
osThreadId indi_taskHandle;
uint32_t indi_taskBuffer[ 256 ];
osStaticThreadDef_t indi_taskControlBlock;
osThreadId server_taskHandle;
uint32_t server_taskBuffer[ 2048 ];
osStaticThreadDef_t server_taskControlBlock;
osThreadId toolsTaskHandle;
osMutexId CanMutexHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */



int dev_refreshtask_state = 0;
osThreadId dev_refreshtaskHandle;
void h_dev_refreshtask ( void const *argument ) ;
//---------------------------------------------

//osThreadDef ( dev_refreshtask , h_dev_refreshtask, osPriorityNormal, 1, 256);

/* USER CODE END FunctionPrototypes */

void h_main_task(void const * argument);
void h_indi_task(void const * argument);
void h_server_task(void const * argument);
void h_tools(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
#ifndef TB_DEF
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
	*ppxIdleTaskStackBuffer = &xIdleStack[0];
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
	/* place for user code */
}
#endif
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

	read_last_config_data ( &mcs_storage.config ) ;
  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of CanMutex */
  osMutexDef(CanMutex);
  CanMutexHandle = osMutexCreate(osMutex(CanMutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	link_queue         = xQueueCreate(5,sizeof(uint8_t));
	indication_queue   = xQueueCreate(20, sizeof(uint8_t));
	tcp_rx_data_queue  = xQueueCreate(TCP_QUEUE_BUF_SIZE, sizeof(char));

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of main_task */
  osThreadStaticDef(main_task, h_main_task, osPriorityNormal, 0, 2048, main_taskBuffer, &main_taskControlBlock);
  main_taskHandle = osThreadCreate(osThread(main_task), NULL);

  /* definition and creation of indi_task */
  osThreadStaticDef(indi_task, h_indi_task, osPriorityNormal, 0, 256, indi_taskBuffer, &indi_taskControlBlock);
  indi_taskHandle = osThreadCreate(osThread(indi_task), NULL);

  /* definition and creation of server_task */
  osThreadStaticDef(server_task, h_server_task, osPriorityIdle, 0, 2048, server_taskBuffer, &server_taskControlBlock);
  server_taskHandle = osThreadCreate(osThread(server_task), NULL);

  /* definition and creation of toolsTask */
  osThreadDef(toolsTask, h_tools, osPriorityIdle, 0, 512);
  toolsTaskHandle = osThreadCreate(osThread(toolsTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
	osThreadSuspend(server_taskHandle);
	osThreadSuspend(indi_taskHandle);
	osThreadSuspend(toolsTaskHandle);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_h_main_task */
/* USER CODE END Header_h_main_task */
void h_main_task(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN h_main_task */
	device_struct *mcs = &mcs_storage;

	char debug_buffer[1000];
	char tcp_buffer[1000];
	char resp[1000];
	long tcp_buffer_ind = 0;

	protection_set_state(&mcs->state, true);
	protection_err_clr(mcs);

	osThreadDef(dev_refreshtask, h_dev_refreshtask, osPriorityNormal, 1, 256);
	dev_refreshtaskHandle = osThreadCreate(osThread(dev_refreshtask), NULL);

	osThreadResume(server_taskHandle);
	osThreadResume(indi_taskHandle);
	osThreadResume(toolsTaskHandle);

	while (true) {

		HAL_UART_Receive_IT(&huart1, &uart1_rx_byte, 1);

		if (xQueueReceive(tcp_rx_data_queue, &tcp_buffer[tcp_buffer_ind], 0) == pdPASS) {

			if (tcp_buffer[tcp_buffer_ind] == '\r'
					|| tcp_buffer[tcp_buffer_ind] == '\n') {

				if (tcp_buffer_ind > 2) {
					tcp_buffer[tcp_buffer_ind + 1] = '\0';
					CommandsHandler(mcs, resp, debug_buffer, tcp_buffer, 0);
					resp[0] = 0;
				}

				tcp_buffer[tcp_buffer_ind] = '\0';
				tcp_buffer_ind = 0;

			} else if (tcp_buffer_ind < TCP_QUEUE_BUF_SIZE - 2) {

				if (tcp_buffer[tcp_buffer_ind] == '\b' && tcp_buffer_ind > 0)
					tcp_buffer_ind--;
				else
					tcp_buffer_ind++;

			} else {
				tcp_buffer_ind = 0;
			}

		} else {
			osDelay(1);
		}
		mcs->current_interface = get_interface_in();
	}
  /* USER CODE END h_main_task */
}

/* USER CODE BEGIN Header_h_indi_task */
/* USER CODE END Header_h_indi_task */
void h_indi_task(void const * argument)
{
  /* USER CODE BEGIN h_indi_task */
	indication_handler(&indication_queue,&mcs_storage);
  /* USER CODE END h_indi_task */
}

/* USER CODE BEGIN Header_h_server_task */
/* USER CODE END Header_h_server_task */
void h_server_task(void const * argument)
{
  /* USER CODE BEGIN h_server_task */
	server_handler(&tcp_rx_data_queue,&indication_queue);
  /* USER CODE END h_server_task */
}

/* USER CODE BEGIN Header_h_tools */
/* USER CODE END Header_h_tools */
void h_tools(void const * argument)
{
  /* USER CODE BEGIN h_tools */
	buttons_init ();
	/* Infinite loop */
	for (;;) {
		handler_alarm(&mcs_storage);
		handler_buttons (&mcs_storage);
		osDelay(1);
	}
  /* USER CODE END h_tools */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void HAL_UART_RxCpltCallback ( UART_HandleTypeDef *huart ){
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if( huart == &huart1 ){
		if ( xQueueSendFromISR ( tcp_rx_data_queue, ( &uart1_rx_byte ), &xHigherPriorityTaskWoken ) == pdPASS )
			set_interface_in(INTERFACE_USB_UART1) ;
		HAL_UART_Receive_IT ( &huart1 , &uart1_rx_byte , 1 );
	}
}

void h_dev_refreshtask (void const *argument ){
	dev_refreshtask_state = 1;
	while(1){
	  osDelay(10);
	}
	dev_refreshtask_state = 0;
	osThreadTerminate(NULL);
}

/* USER CODE END Application */
