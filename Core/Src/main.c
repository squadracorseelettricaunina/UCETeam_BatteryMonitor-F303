/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
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

/* USER CODE BEGIN PV */
ADC_HandleTypeDef* hADC[7] = {&hadc1, &hadc1, &hadc1, &hadc2, &hadc2, &hadc2, &hadc2};

double tensioni[7];			//Dove inserisco le tensioni medie dei vari canali
double temperature[7];
uint32_t conversione=0;
uint32_t sommaConversioni = 0;
double mediaConversioni = 0;
uint8_t output[1024];
uint8_t* pOutput = (uint8_t*) &output;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
double interpolazione (const double *v);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim2);
  __HAL_TIM_SetCounter(&htim2, 0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  for (int k = 0; k < 7; k++)
	  {
		  for (int i = 0; i<100; i++)	// 100 conversioni per canale
		  {
			  HAL_ADC_Start(hADC[k]);
			  HAL_ADC_PollForConversion(hADC[k], HAL_MAX_DELAY);
			  conversione = HAL_ADC_GetValue(hADC[k]);
			  sommaConversioni += conversione;
		  }

		  mediaConversioni = (double)sommaConversioni / (double)1000;
		  tensioni[k] = (mediaConversioni * 3.3) / 4095;
		  }

		  // Converto i valori del vettore tensioni in valori da inserire nel vettore temperature
		  for (int j = 0; j < 7; j++)
		  {
			  temperature[j] = interpolazione(&tensioni[j]);
		  }


		  // Attendo il trascorrere mezzo secondo (freq. tim1 = 8MHz)
		  while (__HAL_TIM_GetCounter(&htim2) < 4000000);
		  __HAL_TIM_SetCounter(&htim2, 0);


		  // Stampo l'array
		  snprintf((char*)pOutput, sizeof(output), "%f,%f,%f,%f,%f,%f,%f\r\n",
				  temperature[0], temperature[1], temperature[2], temperature[3], temperature[4], temperature[5], temperature[6]);

		//	  snprintf((char*)pOutput, sizeof(output), "%f,%f,%f,%f,%f,%f,%f\r\n",
		//	  			  tensioni[0], tensioni[1], tensioni[2], tensioni[3], tensioni[4], tensioni[5], tensioni[6]);
		  HAL_UART_Transmit(&huart2, pOutput,strlen((char*)output), HAL_MAX_DELAY);


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
double interpolazione (const double *v)
{
	double p1 = -616.9209;
	double p2 = 5.8773e+03;
	double p3 = -2.2351e+04;
	double p4 = 4.2418e+04;
	double p5 = -4.0250e+04;
	double p6 = 1.5358e+04;

	double temperatura = p1*pow(*v,5)+ p2*pow(*v,4) + p3*pow(*v,3) + p4*pow(*v,2) + p5*(*v) + p6;
	return temperatura;
}

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}
/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
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
