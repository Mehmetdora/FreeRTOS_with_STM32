
#include "main.h"
#include "FreeRTOS.h"		// bu dosya diğer freeRTOS dosyalarından önce tanımlanmalı


#include "task.h"
#include "semphr.h"
#include "stdio.h"

#include "GPIO_Driver.h"
#include "MPU_6050_Driver.h"
#include "I2C_Driver.h"
#include "TimerDriver.h"



UART_HandleTypeDef huart2;
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);






/*
 *
 * Buton task, butonları kontrol edere , buna göre hangi butona basıldığını event olarak queue ile gönderir
 * butonlar 20 ms aralıklarla kontrol edilir .
 *
 *
 * State Manager TAsk, bu task da buton task dan gelen event'leri okur,buna göre state günceller
 * State bilgisini tutar , state değişime göre led blink kontrol eder.
 * Bu task içinde bir state machine olacak, sistemin off,referans,denge gibi durumlarda olup
 * olmadığını kontrol edecek
 *
 *
 *
 * BAlance control task, bu task belirlenen imu değerini korumak için servo açısını belirler,
 * imu dan veri okur, açıyı hesaplar,farkı hesaplar ve buna göre motoru kontrol eder.
 *
 * 10ms periyot ile çalışır.
 *
 * Mantık olarak bir if ile sürekli sistemin balane state de olduğunu kontrol eder, eğer bu state de ise
 * asıl işini yapar, yani direkt task içinde asıl işini yapmaz , eğer sistem balance state de
 * değilse bu sefer 50 ms bekler
 *
 */

void Task1_system_control_btns_Handler(void* params);
TaskHandle_t Task1Handle = NULL;

void Task2_state_manager_Handler(void* params);
TaskHandle_t Task2Handle = NULL;

void Task3_balance_control_Handler(void* params);
TaskHandle_t Task3Handle = NULL;

void Task4_LED_Blinker_Handler(void* params);
TaskHandle_t Task4Handle = NULL;




typedef enum{
	EVENT_ON,
	EVENT_OFF,
	EVENT_REF_START,
	EVENT_REF_STOP,
	EVENT_ERR
}SystemEvent_t;


typedef enum{
	SYS_OFF,
	SYS_REF,		// sistemin açıldığı an başladığı state bu
	SYS_BALANCE,
	SYS_ERR
}SystemState_t;




// Sistemin başlangıç durumları
SystemState_t System_State = SYS_OFF;
SystemEvent_t Current_Event = EVENT_OFF;
SystemEvent_t Next_Event = EVENT_OFF;
uint8_t is_led_blinking = 0;

float current_angle = 0.0;
float referans_angle = 0.0;
float error = 0.0;

#define SERVO_CENTER_ANGLE 90.0f
#define SERVO_MIN_SAFE     20.0f
#define SERVO_MAX_SAFE     160.0f
#define KP                 1.5f
#define SERVO_DIRECTION    1.0f
#define ANGLE_DEADBAND     0.7f
#define SERVO_MAX_STEP     1.0f


QueueHandle_t BtnEvents_Queue = NULL;



SemaphoreHandle_t BtnSmphr_sysOn = NULL;
SemaphoreHandle_t BtnSmphr_sysOff = NULL;
SemaphoreHandle_t BtnSmphr_sysRef_start = NULL;
SemaphoreHandle_t BtnSmphr_sysRef_stop = NULL;
SemaphoreHandle_t LedBlinker = NULL;

int main(void)
{



	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_USART2_UART_Init();




	DWT->CTRL |= (1UL << 0);		// DWT aktif edildi, task'ların zamansal olarak bilgilerinin kaydının yapılabilmesi için
	SEGGER_SYSVIEW_Conf();
	vSetVarulMaxPRIGROUPValue();	// Bu fonk segger kaydının buffer a kaydedilebilmesi için eklenmeli
	SEGGER_SYSVIEW_Start();


	/*
	 * FreeRTOS gibi RTOS sistemlerinde init işlemleri , ilk kalibrasyon ayarlamaları scheduler
	 * başlatılmadan önceki main() içinde tamamlanarak sonrasında scheduler başlatılır.
	 *
	 * Eğer uzun süreli kalibrasyon gerekiyorsa ayrıca bir task oluşturulabilir ama bu
	 * nadirdir
	 */





	BtnEvents_Queue = xQueueCreate(8,sizeof(SystemEvent_t));


	BtnSmphr_sysOn = xSemaphoreCreateBinary();
	BtnSmphr_sysOff = xSemaphoreCreateBinary();
	BtnSmphr_sysRef_start = xSemaphoreCreateBinary();
	BtnSmphr_sysRef_stop = xSemaphoreCreateBinary();
	LedBlinker = xSemaphoreCreateBinary();


	if(BtnEvents_Queue == NULL ||
	   BtnSmphr_sysOn == NULL ||
	   BtnSmphr_sysOff == NULL ||
	   BtnSmphr_sysRef_start == NULL ||
	   BtnSmphr_sysRef_stop == NULL)
	{

	    while(1);

	}


	// En son init fonk. , sistem freerTOS u set etmeden başlamasın diye
	gpio_setup();
	I2C1_init();
	MPU_init();
	PWMDriver_init();




	  xTaskCreate(    Task1_system_control_btns_Handler,				// Task'ın çalıştıracağı fonksiyon
					  "Task 1 - System btns control",						// Task'ın ismi, debug için kullanılır
					  256 ,	// Task için ayrılacak stack boyutu, task a göre farklı olabilir
					  NULL,							// Task'a gönderilecek parametre
					  3,							// Task önceliği, büyük sayı daha önceliklidir
					  &Task1Handle);				// Task'ın referansı , task ı durdurmak veya başlatmak için bu kullanılacak

	  xTaskCreate(    Task2_state_manager_Handler,				// Task'ın çalıştıracağı fonksiyon
					  "Task 2 - System State Control",						// Task'ın ismi, debug için kullanılır
					  256 ,	// Task için ayrılacak stack boyutu, task a göre farklı olabilir
					  NULL,							// Task'a gönderilecek parametre
					  3,							// Task önceliği, büyük sayı daha önceliklidir
					  &Task2Handle);				// Task'ın referansı , task ı durdurmak veya başlatmak için bu kullanılacak


	  xTaskCreate(    Task3_balance_control_Handler,				// Task'ın çalıştıracağı fonksiyon
					  "Task 2 - System balance with servo",						// Task'ın ismi, debug için kullanılır
					  512 ,	// Task için ayrılacak stack boyutu, task a göre farklı olabilir
					  NULL,							// Task'a gönderilecek parametre
					  4,							// Task önceliği, büyük sayı daha önceliklidir
					  &Task3Handle);

	  xTaskCreate(    Task4_LED_Blinker_Handler,				// Task'ın çalıştıracağı fonksiyon
	  					  "Task 4 - LED BLink",						// Task'ın ismi, debug için kullanılır
	  					  128 ,	// Task için ayrılacak stack boyutu, task a göre farklı olabilir
	  					  NULL,							// Task'a gönderilecek parametre
	  					  1,							// Task önceliği, büyük sayı daha önceliklidir
	  					  &Task4Handle);


	  vTaskStartScheduler();		// RTOS çalışmaya başlar

	  while (1)
	  {
	  }

}







void Task1_system_control_btns_Handler(void* params){

	while(1){

		// Kart üzerindeki pc13 deki buton referans ölçümünü başlatmak için,
		// diğer eklenecek iki buton(pc0 ve pc1) on ve off işlemleri için kullanılacak

		if(xSemaphoreTake(BtnSmphr_sysOff, 0) == pdTRUE)
		{
			Current_Event = EVENT_OFF;
			xQueueSend(BtnEvents_Queue, &Current_Event, 0);
		}

		if(xSemaphoreTake(BtnSmphr_sysOn, 0) == pdTRUE)
		{
			Current_Event = EVENT_ON;
			xQueueSend(BtnEvents_Queue, &Current_Event, 0);
		}

		if(xSemaphoreTake(BtnSmphr_sysRef_start, 0) == pdTRUE)
		{
			Current_Event = EVENT_REF_START;
			xQueueSend(BtnEvents_Queue, &Current_Event, 0);
		}

		if(xSemaphoreTake(BtnSmphr_sysRef_stop, 0) == pdTRUE)
		{
			Current_Event = EVENT_REF_STOP;
			xQueueSend(BtnEvents_Queue, &Current_Event, 0);
		}

		vTaskDelay(pdMS_TO_TICKS(20));


	}
}
void Task2_state_manager_Handler(void* params){

	while(1){

		if(xQueueReceive(BtnEvents_Queue, &Next_Event, portMAX_DELAY) == pdPASS){

			// Event queue üzerinden gönderilen event'lere next_event üzerinden erişilecek.
			// Yeni event gelidiği an bu task uyanacak


			if(Next_Event == EVENT_ON){
				System_State = SYS_REF;


				// LED yakılacak, referans başlatma butonuna basılması ile led blink yapacak
				gpio_set(5);
				portYIELD();
			}

			if(Next_Event == EVENT_REF_START && is_led_blinking == 0){
				// Bu event geldiği an LED blink yapmaya başlar. 100 hz ile
				is_led_blinking = 1;

			}


			if(Next_Event == EVENT_REF_STOP && is_led_blinking == 1){
				is_led_blinking = 0;
				gpio_set(5);
				System_State = SYS_BALANCE;

				// bu noktada belirlenen imu açısı referans olarak kaydedilecek

				referans_angle = MPU6050_GetPitchAngle();

			}



			if(Next_Event == EVENT_OFF){
				System_State = SYS_OFF;
				Servo_SetAngle(90);
				gpio_reset(5);
			}








		}

	}
}


void Task3_balance_control_Handler(void* params)
{
    TickType_t last_wake = xTaskGetTickCount();

    float servo_angle = SERVO_CENTER_ANGLE;
    float target_servo_angle = SERVO_CENTER_ANGLE;

    Servo_SetAngle(servo_angle);

    while(1)
    {
        if(System_State == SYS_BALANCE)
        {
            current_angle = MPU6050_GetPitchAngle();

            error = NormalizeAngle180(referans_angle - current_angle);

            if(error > -ANGLE_DEADBAND && error < ANGLE_DEADBAND)
            {
                error = 0.0f;
            }

            target_servo_angle = SERVO_CENTER_ANGLE +
                                 SERVO_DIRECTION * KP * error;

            target_servo_angle = clamp_float(
                target_servo_angle,
                SERVO_MIN_SAFE,
                SERVO_MAX_SAFE
            );

            servo_angle = limit_step(
                target_servo_angle,
                servo_angle,
                SERVO_MAX_STEP
            );

            Servo_SetAngle(servo_angle);

            vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(10));
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            last_wake = xTaskGetTickCount();
        }
    }
}


void Task4_LED_Blinker_Handler(void* params)
{
    TickType_t last_blink = xTaskGetTickCount();

    while(1)
    {
        if(System_State == SYS_REF && is_led_blinking == 1)
        {
            gpio_toggle(5);
            vTaskDelayUntil(&last_blink, pdMS_TO_TICKS(100));
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            last_blink = xTaskGetTickCount();
        }


    }
}









/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
