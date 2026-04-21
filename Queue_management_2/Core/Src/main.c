
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "queue.h"

#include "string.h"
#include "LED_Driver.h"



UART_HandleTypeDef huart2;
void SystemClock_Config(void);
static void MX_GPIO_Init(void);


void usart_config(void);
void print_uart(char* msg);








typedef enum{
	ON,
	OFF,
	TOGGLE,
	BLINK,
	STOP
}LED_STATE_t;

typedef struct LED_COMMAND{
	LED_STATE_t state;
	uint16_t blink_ms;
}LED_COMMAND_t;


void Consumer_Handler(void* params);
TaskHandle_t ConsumerHandle = NULL;

void Producer_Handler(void* params);
TaskHandle_t ProducerHandle = NULL;


QueueHandle_t producer_consumer_queue;
QueueHandle_t isr_producer_queue;



volatile char RX_BUFFER[30];
volatile uint8_t rx_index = 0;




int main(void)
{



	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();

  usart_config();
  LED_config();

  DWT->CTRL |= (1UL << 0);		// DWT aktif edildi
  SEGGER_SYSVIEW_Conf();




  /*
   * SİSTEM: amaç uart üzerinden gelen komutları işleyerek buna göre led kontrolünü yapmak.
   * Bunun için 2 task kullanılacak, producer ve consumer task. Biri uart üzerinden verileri
   * alıp , kontrol edip gerekli yapılması gereken bilgileri içeren bir struct oluşturup
   * 2 task arasında iletişimi sağlayan queue ya aktaracak. Consumer task ise bu queue üzerinden
   * gelen verileri alarak led üzerinde yapılacak işlemleri gerçekleştirecek.
   *
   * Ayrıca uart üzerinden gelen veriler uart rx ISR içinden alınacağı için bu interrupt ile
   * producer task arasında da bir queue yapısı kullanılması gerekiyor ki verileri producer
   * içinde işlenebilsin.
   *
   *
   *
   */


  print_uart("Sistem Başlatılıyor...\r\n");
  print_uart("KOMUTLAR: ON , OFF, TOGGLE, BLINK:100, BLINK:500, STOP\r\n");




  producer_consumer_queue = xQueueCreate(5,sizeof(LED_COMMAND_t));
  if(producer_consumer_queue == NULL){
      // Queue oluşturulamadı = heap yetmedi
      while(1);  // burda takılırsa heap sorunu kesindir

  }

	isr_producer_queue = xQueueCreate(5,sizeof(char[30]));
	if(isr_producer_queue == NULL){
		// Queue oluşturulamadı = heap yetmedi
		while(1);  // burda takılırsa heap sorunu kesindir

	}



	// TASK OLUŞTURULURKEN priority değeri freertosconfig.h dosyası içinden kontrol edilmelidir.
	// Bu max değerden küçük olmak zorundadırlar.

  BaseType_t status1 = xTaskCreate(    Consumer_Handler,				// Task'ın çalıştıracağı fonksiyon
  				  "Consumer Task",						// Task'ın ismi, debug için kullanılır
  				  configMINIMAL_STACK_SIZE ,	// Task için ayrılacak stack boyutu, task a göre farklı olabilir
  				  NULL,							// Task'a gönderilecek parametre
  				  4,							// Task önceliği, büyük sayı daha önceliklidir
  				  &ConsumerHandle);





  BaseType_t status2 = xTaskCreate(    Producer_Handler,				// Task'ın çalıştıracağı fonksiyon
  				  "Producer Task",						// Task'ın ismi, debug için kullanılır
  				  configMINIMAL_STACK_SIZE ,	// Task için ayrılacak stack boyutu, task a göre farklı olabilir
  				  NULL,							// Task'a gönderilecek parametre
  				  4,							// Task önceliği, büyük sayı daha önceliklidir
  				  &ProducerHandle);






  if(status1 != pdPASS)
  {
      print_uart("Consumer task create FAIL\r\n");
      while(1);
  }

  if(status2 != pdPASS)
  {
      print_uart("Producer task create FAIL\r\n");
      while(1);
  }



  vTaskStartScheduler();		// RTOS çalışmaya başlar

  SEGGER_SYSVIEW_Start();





  while (1)
  {


  }
  /* USER CODE END 3 */
}




void Consumer_Handler(void* params){


	LED_COMMAND_t new_command;
	uint16_t delay_ms  ;
	uint8_t blink_active = 0;

	while(1){


		/* !!!
		 *
		 * Bu task ile queue dan alınacak veri başta sonsuz beklenir, eğer veri gelirse işlenir
		 * Eğer veri blink yaptırmak için LED toggle edildikten sonra bir süre beklemek
		 * gerekiyor. Bu bekleme delay süresini task içinda queue bekleme süresi olarak
		 * ayarlanması ile yapılıyor, yani blink komutunda istenen periyotu sağlamak için
		 * task için queue bekleme süresi değiştiriliyor, böylece blink periyodu boyunca
		 * task blocking durumuna geçerek sanki başka bir işi daha yapıyormuş gibi oluyor.
		 */


		TickType_t wait_time;

		if(blink_active){
			wait_time = pdMS_TO_TICKS(delay_ms);
		}else{
			wait_time = portMAX_DELAY;
		}


		if(xQueueReceive(producer_consumer_queue, &new_command, wait_time) == pdPASS){

			/*
			 * Buradaki while(1) içindeki if ile queue kontrolü polling gibi görünsede aslında
			 * polling değildir. QueueReceive() fonksiyonu bloklayıcı bir fonksiyondur, bu nedenle
			 * eğer veri gelmezse parametre olarak belirtilen wait_time kadar
			 * bloklanacaktır, bekleyecektir. Yani CPU burada oyalanmaz, yapı event-driven çalışır.
			 *
			 * 	Bu method ile her veri alındığında veri queue dan çıkarılır, silinir.
			 */

			if(new_command.state == ON){
				LED_ON();
				blink_active = 0;
				print_uart("LED -> ON\r\n");
			}
			else if(new_command.state == OFF){
				LED_OFF();
				blink_active = 0;
				print_uart("LED -> OFF\r\n");
			}
			else if(new_command.state == TOGGLE){
				LED_TOGGLE();
				blink_active = 0;
				print_uart("LED -> TOGGLE\r\n");
			}
			else if(new_command.state == BLINK && new_command.blink_ms == 100){

				//LED_BLINK(new_command.blink_ms);
				blink_active = 1;
				delay_ms = new_command.blink_ms;

				print_uart("LED -> BLINK: 100ms\r\n");

			}
			else if(new_command.state == BLINK && new_command.blink_ms == 500){

				//LED_BLINK(new_command.blink_ms);
				blink_active = 1;
				delay_ms = new_command.blink_ms;

				print_uart("LED -> BLINK: 500ms\r\n");

			}
			else if(new_command.state == STOP){
				LED_OFF();
				blink_active = 0;
				print_uart("LED -> STOP\r\n");
			}
			else{
				blink_active = 0;
				print_uart("BILINMEYEN KOMUT\r\n");
			}



		}else{

			if(blink_active){

				LED_TOGGLE();
				vTaskDelay(pdMS_TO_TICKS(delay_ms));

			}
		}


	}
}

void Producer_Handler(void* params){


	uint8_t received_command[30];

	while(1){


		if(xQueueReceive(isr_producer_queue, received_command, portMAX_DELAY) == pdPASS){

			LED_COMMAND_t command = {0};

			if(strcmp((char*)received_command, "ON") == 0){
				command.state = ON;
				command.blink_ms = 0;


			}else if(strcmp((char*)received_command, "OFF") == 0){
				command.state = OFF;
				command.blink_ms = 0;

			}else if(strcmp((char*)received_command, "TOGGLE") == 0){
				command.state = TOGGLE;
				command.blink_ms = 0;

			}else if(strcmp((char*)received_command, "BLINK:100") == 0){
				command.state = BLINK;
				command.blink_ms = 100;

			}else if(strcmp((char*)received_command, "BLINK:500") == 0){
				command.state = BLINK;
				command.blink_ms = 500;

			}else if(strcmp((char*)received_command, "STOP") == 0){
				command.state = STOP;
				command.blink_ms = 0;
			}else{
				command.state = OFF;
				command.blink_ms = 0;
			}


			// bu methodun dönüş değeri queue nın dolu olması durumu hakkında bilgi verir
			BaseType_t result = xQueueSend(producer_consumer_queue, &command , (TickType_t)0);
			if(result == errQUEUE_FULL){
				// Queue dolu, hata verdi -> duruma özel aksiyon alınabilir
			}





		}


	}
}

void usart_config(void){

	RCC->APB1ENR |= (1UL << 17);		// uart clock enable
	RCC->AHB1ENR |= (1UL << 0);			// GPIOA clock enable



	GPIOA->MODER |= (2UL << (2*2)) | (2UL << (2*3));	// PA2-PA3 AF modunda gelitirildi
	GPIOA->AFR[0] |= (7UL << (2*4)) | (7UL << (3*4));	// AF7 olarak ayarlandılar.

	USART2->CR1 &= ~(1UL << 12);	// Data boyutunun 8 bit olarak ayarlanması



	USART2->BRR = (22UL << 4) | (13UL);		// eski değerlerlerin kalmaması için direkt = kullan

	// Transmitter enable ve receiver enable yapmak

	USART2->CR1 |= (1UL << 2) | (1UL << 3);


	// USART2 RX interrupt enable
	USART2->CR1 |= (1UL << 5);		// Bu bit RXENIE ile RX için interruot a izin verir


	NVIC_SetPriority(USART2_IRQn, 6);
	NVIC_EnableIRQ(USART2_IRQn);	// Bu NVIC in bu interrupt için erişilebilmesini sağlar.


	USART2->CR1 |= (1UL << 13);		// Usart2 enable



}





void USART2_IRQHandler(void){


	if(USART2->SR & (1UL << 5)){

		/*
		 * RX interrupt enable ve
		 * RX den veri okunmaya hazırsa
		 */
		uint8_t byte = USART2->DR;

		if(byte == '\r'){

			if(rx_index == 0) return;
			RX_BUFFER[rx_index] = '\0';
			rx_index = 0;


			//	QUEUE MANAGEMENT FROM ISR

			BaseType_t xHigherPriorityTaskWoken = pdFALSE;

			// bu methodun dönüş değeri queue nın dolu olması durumu hakkında bilgi verir
			BaseType_t result = xQueueSendFromISR(isr_producer_queue, RX_BUFFER , &xHigherPriorityTaskWoken);
			if(result == errQUEUE_FULL){
				// Queue dolu, hata verdi -> duruma özel aksiyon alınabilir
			}

			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

			/*
			 * Buradaki xHigherPriorityTaskWoken değeri ISR sırasında bir task uyandırıldı mı
			 * bilgisini taşır. Bu değer QueueSendFromISR() fonksiyonuna verilir, eğer
			 * queue ya veri yazılması sonucu bir task aktif hale geçecekse fonksiyon
			 * sonunda pdTRUE olur. En sondaki YIELD_FROM_ISR() methodu ise bu değişkenin
			 * değerine göre ISR çıkışınca hemen aktif olan task ın başlatılmasını sağlar.
			 *
			 * QueueSendFromISR() methodu queue ya verinin yazılıp yazılamadığı hakkında
			 * sonucu return eder, bu sonuca göre işlemler uygulanabilir.
			 *
			 */





		}else{

			if(byte == '\n') return;

			if(rx_index < 29){
			    RX_BUFFER[rx_index++] = byte;
			} else {
			    rx_index = 0;
			}

		}



	}


}





void print_uart(char* msg){

	while(*msg){
		while( !(USART2->SR & (1UL << 7)) ){};
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
