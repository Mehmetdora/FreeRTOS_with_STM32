
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"

UART_HandleTypeDef huart2;
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

void setup(void);
void usart_config(void);
void print_uart(char* msg);
char myMsg[200];
char UART_ACCESS_FLAG = 1;


void Task1_Handler(void* params);
void Task2_Handler(void* params);

TaskHandle_t Task1Handle = NULL;
TaskHandle_t Task2Handle = NULL;




int main(void)
{



	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_USART2_UART_Init();


  setup();
  usart_config();

  DWT->CTRL |= (1UL << 0);		// DWT aktif edildi
  SEGGER_SYSVIEW_Conf();




  xTaskCreate(    Task1_Handler,				// Task'ın çalıştıracağı fonksiyon
				  "Task 1",						// Task'ın ismi, debug için kullanılır
				  configMINIMAL_STACK_SIZE ,	// Task için ayrılacak stack boyutu, task a göre farklı olabilir
				  NULL,							// Task'a gönderilecek parametre
				  4,							// Task önceliği, büyük sayı daha önceliklidir
				  &Task1Handle);				// Task'ın referansı , task ı durdurmak veya başlatmak için bu kullanılacak

  xTaskCreate(    Task2_Handler,				// Task'ın çalıştıracağı fonksiyon
  				  "Task 2",						// Task'ın ismi, debug için kullanılır
  				  configMINIMAL_STACK_SIZE ,	// Task için ayrılacak stack boyutu, task a göre farklı olabilir
  				  NULL,							// Task'a gönderilecek parametre
  				  4,							// Task önceliği, büyük sayı daha önceliklidir
  				  &Task2Handle);

  vTaskStartScheduler();		// RTOS çalışmaya başlar



  SEGGER_SYSVIEW_Start();		// RTOS başlatıldıktan sonra başlatılmalı
  while (1)
  {



  }
  /* USER CODE END 3 */
}




void Task1_Handler(void* params){

	while(1){

		if(UART_ACCESS_FLAG == 1){					// flag kontrol edilir, 1 ise task çalışır
													// flag kapatılarak bu task bitene kadar diğer task çalışamaz
			print_uart("MERHABA MERHABA MERHABA");
			UART_ACCESS_FLAG = 0;					// bu task bittiği için flag açılır ve diğer task da aynı şekilde çalışabilir
			taskYIELD();							// taskYIELD() ile bu taskın bittiği söylenir ve bu task'tan çıkmak için kullanılır.
		}

	}
}

void Task2_Handler(void* params){

	while(1){
		if(UART_ACCESS_FLAG == 0){

			print_uart("hello_world hello_world");
			UART_ACCESS_FLAG = 1;
			taskYIELD();

		}
	}
}

void setup(void){

	RCC->AHB1ENR |= (1UL << 0);

	GPIOA->MODER |= (2UL << (2*2)) | (2UL << (2*3));	// PA2-PA3 AF modunda gelitirildi
	GPIOA->AFR[0] |= (7UL << (2*4)) | (7UL << (3*4));	// AF7 olarak ayarlandılar.


}

void usart_config(void){

	RCC->APB1ENR |= (1UL << 17);

	USART2->CR1 &= ~(1UL << 12);	// Data boyutunun 8 bit olarak ayarlanması



	USART2->BRR = (22UL << 4) | (13UL);		// eski değerlerlerin kalmaması için direkt = kullan




	// Transmitter enable ve receiver enable yapmak

	USART2->CR1 |= (1UL << 2) | (1UL << 3);


	// USART2 RX interrupt enable
	//USART2->CR1 |= (1UL << 5);		// Bu bit RXENIE ile RX için interruot a izin verir
	//NVIC_EnableIRQ(USART2_IRQn);	// Bu NVIC in bu interrupt için erişilebilmesini sağlar.


	USART2->CR1 |= (1UL << 13);		// Usart2 enable



}

void print_uart(char* msg){

	while(*msg){
		while( !(USART2->SR & (1UL << 6)) ){};
		USART2->DR = *msg++;
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
