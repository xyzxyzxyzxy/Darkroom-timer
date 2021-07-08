/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <math.h>
#include "7seg.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define C1_PORT GPIOD
#define C1_PIN GPIO_PIN_11
#define C2_PORT GPIOD
#define C2_PIN GPIO_PIN_9
#define C3_PORT GPIOB
#define C3_PIN GPIO_PIN_15
#define C4_PORT GPIOB
#define C4_PIN GPIO_PIN_13
#define R1_PORT GPIOB
#define R1_PIN GPIO_PIN_11
#define R2_PORT GPIOE
#define R2_PIN GPIO_PIN_15
#define R3_PORT GPIOE
#define R3_PIN GPIO_PIN_13
#define R4_PORT GPIOE
#define R4_PIN GPIO_PIN_11

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

void (*writeDigit[4])() = {&write_D1, &write_D2, &write_D3, &write_D4};

GPIO_TypeDef* Cx_PORT[4] = {C1_PORT, C2_PORT, C3_PORT, C4_PORT};
uint16_t Cx_PIN[4] = {C1_PIN, C2_PIN, C3_PIN, C4_PIN};
GPIO_TypeDef* Rx_PORT[4] = {R1_PORT, R2_PORT, R3_PORT, R4_PORT};
uint16_t Rx_PIN[4] = {R1_PIN, R2_PIN, R3_PIN, R4_PIN};

char keypad[5][5] = {{'\0', '\0', '\0', '\0', '\0'},
		{'\0', '1', '2', '3', 'A'},
		{'\0', '4', '5', '6', 'B'},
		{'\0', '7', '8', '9', 'C'},
		{'\0', '*', '0', '#', 'D'}};

uint32_t t0;
static float num;
static char key;
static char msg[50];
static int rn;
static int cn;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
// char readKey(void);
void displayNumber(float);
void displayChar(char);
void displayWait(void);
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
  key = '\0';
  num = 0;
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
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  sprintf(msg, "%c[2K", 27);
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, sizeof(msg), HAL_MAX_DELAY);
  HAL_Delay(200);
  t0 = HAL_GetTick();

  while (1)
  {
	 if (num > 0) {
		 displayNumber(num);
	 } else {
		 displayWait();
	 }
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 999;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 2499;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

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
  huart2.Init.BaudRate = 38400;
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE11 PE13 PE15 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PB11 PB4 PB5 PB6
                           PB7 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB13 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD9 PD11 */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD1 PD2 PD3
                           PD4 PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

void keyCheck(char key) {
	if ((int)key <= 58 && (int)key >= 48) { //a number between 0 and 9
		  for (int i = 0; i < 50; ++i) {
					  msg[i] = '\0';
		  }
		  num = num * 10 + (int)key - 48;
		  if (num > 999) {
			  num = (int)key-48;
		  }
		  sprintf(msg, "Key pressed: %f                   \r", num);
		  HAL_UART_Transmit(&huart2, (uint8_t*)msg, sizeof(msg), HAL_MAX_DELAY);
	  }
	  else if ((int)key > 0) { // not \0
		  for (int i = 0; i < 50; ++i) {
				msg[i] = '\0';
		  }
		  if (key == 'C' && num > 0) {
			  displayChar(key);
			  HAL_Delay(500);
			  key = '\0';

			  HAL_TIM_Base_Start_IT(&htim6);
			  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
			  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
			  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

		  } else if (key == 'D' && num > 0) {
			  if (num < 10) {
				  num = 0;
			  }
			  num /= 10;

		  } else if (key == '#' && num > 0) {
			  num += 0.5;
		  }

		  sprintf(msg, "Key pressed: %c                   \r", key);
		  HAL_UART_Transmit(&huart2, (uint8_t*)msg, sizeof(msg), HAL_MAX_DELAY);
	  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
	if (HAL_GetTick()-t0 > 115) {
		rn = 1;
		if (GPIO_Pin == GPIO_PIN_11) {
			cn = 1; //fixed DETERMINED BY GPIO_PIN

			HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);

			while(!HAL_GPIO_ReadPin(C1_PORT, C1_PIN) && rn <= 4) {
				rn++;
				HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);
			}
			HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_RESET);
		}
		else if (GPIO_Pin == GPIO_PIN_9) {
			cn = 2;
			HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);

			while(!HAL_GPIO_ReadPin(C2_PORT, C2_PIN) && rn <= 4) {
				rn++;
				HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);
			}
			HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_RESET);
		}
		else if (GPIO_Pin == GPIO_PIN_15) {
			cn = 3;
			HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);
			while(!HAL_GPIO_ReadPin(C3_PORT, C3_PIN) && rn <= 4) {
				rn++;
				HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);
			}
			HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_RESET);
		}
		else if (GPIO_Pin == GPIO_PIN_13) {
			cn = 4;
			HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);

			while(!HAL_GPIO_ReadPin(C4_PORT, C4_PIN) && rn <= 4) {
				rn++;
				HAL_GPIO_WritePin(Rx_PORT[rn-1], Rx_PIN[rn-1], GPIO_PIN_SET);
			}
			HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_RESET);
		}
		key = keypad[rn][cn];
		keyCheck(key);
	}
	t0 = HAL_GetTick();
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
}


void displayNumber(float number) {

	//For visualization purposes
	number *= 10;

	int n = (int)number;

	if (n >= 1) {
		int ind = 0;
		int n_digits = log10(n) + 1;

		int d[4] = {0, 0, 0, 0};
		uint8_t k = 0;

		d[0] = n % 10;
		n /= 10;
		k++;
		while (n > 0) {
			d[k] = n % 10;
			n = n / 10;
			k++;
		}

		//disp remaining digits as off
		for (ind = 0; ind < 4-n_digits; ++ind) {
			HAL_Delay(2);
			(*writeDigit[ind])();
			print_OFF();
		}

		for (int i = ind; i < 4; ++i) {
			HAL_Delay(2);
			(*writeDigit[i])();
			if (i == 2) {
				print_decimal();
				HAL_Delay(2);
			}
			switch (d[3-i]) {
			case 0:
				print_0();
				break;
			case 1:
				print_1();
				break;
			case 2:
				print_2();
				break;
			case 3:
				print_3();
				break;
			case 4:
				print_4();
				break;
			case 5:
				print_5();
				break;
			case 6:
				print_6();
				break;
			case 7:
				print_7();
				break;
			case 8:
				print_8();
				break;
			case 9:
				print_9();
				break;
			default:
				break;
			}
		}
	} else { //Number is < 1 log10 does not work.
		for (int i = 0; i < 3; ++i) {
			HAL_Delay(2);
			(*writeDigit[i])();
			print_OFF();
		}
	}
}

void displayChar(char c) {
	if ((int)c >= 65 && (int)c <= 68) {
		for (int i = 0; i < 2; ++i) {
			HAL_Delay(2);
			(*writeDigit[i])();
			print_OFF();
		}
		HAL_Delay(2);
		write_D4();
		switch(c) {
			case 'A':
				print_A();
				break;
			case 'B':
				print_b();
				break;
			case 'C':
				print_C();
				break;
			case 'D':
				print_d();
				break;
			default:
				break;
		}
	}
}

void displayWait() {
	for (int i = 0; i < 4; ++i) {
		HAL_Delay(2);
		(*writeDigit[i])();
		print_wait();
	}
}

//READ KEY POLLING VERSION

//char readKey(void) {
//	HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_SET);
//
//	if (!HAL_GPIO_ReadPin(C1_PORT, C1_PIN)) {
//
//				return '1';
//	}
//	if (!HAL_GPIO_ReadPin(C2_PORT, C2_PIN)) {
//
//				return '2';
//	}
//	if (!HAL_GPIO_ReadPin(C3_PORT, C3_PIN)) {
//
//				return '3';
//	}
//	if (!HAL_GPIO_ReadPin(C4_PORT, C4_PIN)) {
//
//				return 'A';
//	}
//
//	HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_SET);
//
//	if (!HAL_GPIO_ReadPin(C1_PORT, C1_PIN)) {
//
//				return '4';
//	}
//	if (!HAL_GPIO_ReadPin(C2_PORT, C2_PIN)) {
//
//				return '5';
//	}
//	if (!HAL_GPIO_ReadPin(C3_PORT, C3_PIN)) {
//
//				return '6';
//	}
//	if (!HAL_GPIO_ReadPin(C4_PORT, C4_PIN)) {
//
//				return 'B';
//	}
//
//	HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_SET);
//
//	if (!HAL_GPIO_ReadPin(C1_PORT, C1_PIN)) {
//
//				return '7';
//	}
//	if (!HAL_GPIO_ReadPin(C2_PORT, C2_PIN)) {
//
//				return '8';
//	}
//	if (!HAL_GPIO_ReadPin(C3_PORT, C3_PIN)) {
//
//				return '9';
//	}
//	if (!HAL_GPIO_ReadPin(C4_PORT, C4_PIN)) {
//
//				return 'C';
//	}
//
//	HAL_GPIO_WritePin(R1_PORT, R1_PIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(R2_PORT, R2_PIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(R3_PORT, R3_PIN, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(R4_PORT, R4_PIN, GPIO_PIN_RESET);
//
//	if (!HAL_GPIO_ReadPin(C1_PORT, C1_PIN)) {
//
//				return '*';
//	}
//	if (!HAL_GPIO_ReadPin(C2_PORT, C2_PIN)) {
//
//				return '0';
//	}
//	if (!HAL_GPIO_ReadPin(C3_PORT, C3_PIN)) {
//
//				return '#';
//	}
//	if (!HAL_GPIO_ReadPin(C4_PORT, C4_PIN)) {
//
//				return 'D';
//	}
//	return '\0';
//}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	num -= 0.1;
	if (num <= 0) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
		HAL_TIM_Base_Stop_IT(&htim6);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
		HAL_Delay(100);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
		HAL_Delay(100);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
		HAL_Delay(100);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
		HAL_Delay(100);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
		HAL_Delay(100);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
		for (int i = 0; i < 200; ++i) {
			write_D1();
			print_OFF();
			HAL_Delay(2);
			write_D2();
			print_E();
			HAL_Delay(2);
			write_D3();
			print_n();
			HAL_Delay(2);
			write_D4();
			print_d();
			HAL_Delay(2);
		}
		//RE enable interrupts from keyboard
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	}
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
