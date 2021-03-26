/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "string.h" 
#include "stdio.h"
#include "lcd_1602.h"
#include "hx711.h.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define smalltimer TIM2
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
int i;//for loop

const float speedofsound=0.0343/2;
float distance;

bool GPIO_flag= false; 

char uartbuf[50];
uint8_t usart_buffer[20] = "LinXiuWei\r\n"; 
int average_weigh;
volatile int weigh =0;
volatile int myweigh =0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void smalldelay(unsigned int);
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
	unsigned int num_t;
	HX711 hx11;
  hx11.gpioSck = hx711sck_GPIO_Port;
  hx11.gpioData = hx711out_GPIO_Port;
  hx11.pinSck = hx711sck_Pin;
  hx11.pinData = hx711out_Pin;
  hx11.offset = 0;
  hx11.gain = 64;
	char weighinformation[20];
	
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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
 //Transmit without interrupt
 HAL_UART_Transmit(&huart2 , &usart_buffer[0] , strlen(&usart_buffer[0]) , 100);
 //Receive with interrupt
 HAL_UART_Receive_IT(&huart2 , &usart_buffer[0] , 1);
 ///////////////
 lcd_init();
 HX711_Init(hx11);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//HAL_Delay(1000);
		while(HAL_GPIO_ReadPin(PE_GPIO_Port,PE_Pin)!=GPIO_PIN_SET){
		lcd_clr();
		/////////////////hx711
		average_weigh=0;
		for(i=0;i<10;i++){
		weigh = HX711_Value(hx11);
		weigh =((weigh-8470500)/1500);
		average_weigh=average_weigh+weigh;
		}
		average_weigh=average_weigh/10;
		myweigh=average_weigh;	
	  //weigh = HX711_AverageValue(hx11, times);
	 
		sprintf(weighinformation,"Weigh is %d\r\n",myweigh);
	  HAL_UART_Transmit(&huart2 ,(unsigned char*)weighinformation, strlen(weighinformation) , 100);
		HAL_Delay(1000);
		}
		
		///////////////lcd
		lcd_clr();
		lcd_gotoxy(0,0);
		lcd_puts("Let start!!!");
		HAL_Delay(1000);
		
		while(HAL_GPIO_ReadPin(PL_GPIO_Port,PL_Pin)!=GPIO_PIN_SET){
		HAL_GPIO_WritePin(PE_GPIO_Port, PE_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(trig_GPIO_Port, trig_Pin, GPIO_PIN_RESET);
		smalldelay(3);
		HAL_GPIO_WritePin(trig_GPIO_Port, trig_Pin, GPIO_PIN_SET);
		smalldelay(10);
		HAL_GPIO_WritePin(trig_GPIO_Port, trig_Pin, GPIO_PIN_RESET);
		/////////////////measure echo pulse width
		num_t=0;
		while(HAL_GPIO_ReadPin(echo_GPIO_Port,echo_Pin)!=GPIO_PIN_SET);
		while(HAL_GPIO_ReadPin(echo_GPIO_Port,echo_Pin)==GPIO_PIN_SET)
		{
			num_t++;
			smalldelay(2);
		}
		//////////////////estimate distance
		distance=(num_t+0.0f)*7.3*speedofsound;
		////////////////////////for serial port
		sprintf(uartbuf,"distance is %.1f\r\n",distance);
		HAL_UART_Transmit(&huart2 ,(unsigned char*)uartbuf, strlen(uartbuf) , 100);
		
		/////////////////hx711
		average_weigh=0;
		for(i=0;i<10;i++){
		weigh = HX711_Value(hx11);
		weigh =((weigh-8470500)/1500);
		average_weigh=average_weigh+weigh;
		}
		average_weigh=average_weigh/10;
	 
		sprintf(weighinformation,"Weigh is %d\r\n",myweigh);
	  HAL_UART_Transmit(&huart2 ,(unsigned char*)weighinformation, strlen(weighinformation) , 100);
		
		///////////////////////Pudding control
			if(distance<15 || (weigh-myweigh)>5 || (weigh-myweigh)<-5){
				if((average_weigh-myweigh)>1 || (average_weigh-myweigh)<-1){
					lcd_clr();	
					lcd_gotoxy(5,0);
					lcd_puts("---    ---");
					lcd_gotoxy(6,1);
					lcd_puts("0      0");
					lcd_gotoxy(8,2);
					lcd_puts("0000");
					lcd_gotoxy(8,3);
					lcd_puts("0000");
					HAL_GPIO_WritePin(PE_GPIO_Port, PE_Pin, GPIO_PIN_SET);
				}
				else{
					lcd_clr();	
					lcd_gotoxy(5,0);
					lcd_puts("---    ---");
					lcd_gotoxy(6,1);
					lcd_puts(">      <");
					lcd_gotoxy(8,2);
					lcd_puts("0000");
					lcd_gotoxy(8,3);
					lcd_puts("0000");
				}
			}	
			else{
				lcd_clr();	
				lcd_gotoxy(6,1);
				lcd_puts(">      <");
				lcd_gotoxy(9,3);
				lcd_puts("VV");
			}
		HAL_Delay(1000);
		HAL_GPIO_WritePin(PE_GPIO_Port, PE_Pin, GPIO_PIN_RESET);}
		lcd_clr();
		lcd_gotoxy(0,0);
		lcd_puts("It's over!!!");
		HAL_Delay(1000);
		////////////////////////////////////////////////////////////////////helpful code	
		/*
		while(HAL_GPIO_ReadPin(PE_GPIO_Port,PE_Pin)!=GPIO_PIN_SET)lcd_clr();
		while(HAL_GPIO_ReadPin(PL_GPIO_Port,PL_Pin)!=GPIO_PIN_SET){
		HAL_GPIO_WritePin(trig_GPIO_Port, trig_Pin, GPIO_PIN_RESET);
		smalldelay(3);
		HAL_GPIO_WritePin(trig_GPIO_Port, trig_Pin, GPIO_PIN_SET);
		smalldelay(10);
		HAL_GPIO_WritePin(trig_GPIO_Port, trig_Pin, GPIO_PIN_RESET);
		/////////////////measure echo pulse width
		num_t=0;
		while(HAL_GPIO_ReadPin(echo_GPIO_Port,echo_Pin)!=GPIO_PIN_SET);
		while(HAL_GPIO_ReadPin(echo_GPIO_Port,echo_Pin)==GPIO_PIN_SET)
		{
			num_t++;
			smalldelay(2);
		}
		//////////////////estimate distance
		distance=(num_t+0.0f)*7.3*speedofsound;
		////////////////////////for serial port
		sprintf(uartbuf,"distance is %.1f\r\n",distance);
		HAL_UART_Transmit(&huart2 ,(unsigned char*)uartbuf, strlen(uartbuf) , 100);
		HAL_Delay(1000);}
		
		///////////////////////LCD control
			if(distance<10){
				lcd_clr();	
				lcd_gotoxy(5,0);
				lcd_puts("---    ---");
				lcd_gotoxy(6,1);
				lcd_puts("0      0");
				lcd_gotoxy(8,2);
				lcd_puts("0000");
				lcd_gotoxy(8,3);
				lcd_puts("0000");
			}	
			else{
				lcd_clr();	
				lcd_gotoxy(6,1);
				lcd_puts(">      <");
				lcd_gotoxy(9,3);
				lcd_puts("VV");
			}
		HAL_Delay(1000);	}
		
		///////////////SPEAKER
		HAL_GPIO_WritePin(PE_GPIO_Port, PE_Pin, GPIO_PIN_RESET);
		while(HAL_GPIO_ReadPin(REC_GPIO_Port,REC_Pin)!=GPIO_PIN_SET);
		while(HAL_GPIO_ReadPin(REC_GPIO_Port,REC_Pin)==GPIO_PIN_SET);
		HAL_GPIO_WritePin(PE_GPIO_Port, PE_Pin, GPIO_PIN_SET);
		HAL_Delay(2000);
		HAL_GPIO_WritePin(PE_GPIO_Port, PE_Pin, GPIO_PIN_RESET);
	
		
		/////////////////hx711
		while(HAL_GPIO_ReadPin(PE_GPIO_Port,PE_Pin)!=GPIO_PIN_SET)lcd_clr();
		while(HAL_GPIO_ReadPin(PL_GPIO_Port,PL_Pin)!=GPIO_PIN_SET){
		lcd_clr();
		lcd_gotoxy(0,0);
		lcd_puts("Let start!!!");	
		average_weigh=0;
		for(i=0;i<10;i++){
		weigh = HX711_Value(hx11);
		weigh =((weigh-8470500)/1000);
		average_weigh=average_weigh+weigh;
		}
		average_weigh=average_weigh/10;
	  //weigh = HX711_AverageValue(hx11,5);
		
		sprintf(weighinformation,"Weigh is %d\r\n",average_weigh);
	  HAL_UART_Transmit(&huart2 ,(unsigned char*)weighinformation, strlen(weighinformation) , 100);
		HAL_Delay(1000);}*/
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 84-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PE_GPIO_Port, PE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, trig_Pin|D4_Pin|D5_Pin|D6_Pin 
                          |RW_Pin|RS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, EN_Pin|hx711sck_Pin|D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PE_Pin */
  GPIO_InitStruct.Pin = PE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PL_Pin */
  GPIO_InitStruct.Pin = PL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : trig_Pin D4_Pin D5_Pin D6_Pin 
                           RW_Pin RS_Pin */
  GPIO_InitStruct.Pin = trig_Pin|D4_Pin|D5_Pin|D6_Pin 
                          |RW_Pin|RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : echo_Pin */
  GPIO_InitStruct.Pin = echo_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(echo_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : EN_Pin hx711sck_Pin D7_Pin */
  GPIO_InitStruct.Pin = EN_Pin|hx711sck_Pin|D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : hx711out_Pin */
  GPIO_InitStruct.Pin = hx711out_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(hx711out_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef*huart){
if(huart== &huart2){
HAL_UART_Transmit(&huart2 , &usart_buffer[0] , 1 , 100);//1.which hardware do you control 2.first char of string 3.how many information 4.max time for wait
HAL_UART_Receive_IT(&huart2 , &usart_buffer[0] , 1);}} 

void smalldelay(unsigned int smallsecond){
	if(smallsecond<2)smallsecond=2;
	smalltimer->ARR=smallsecond-1;
	smalltimer->EGR=1;
	smalltimer->SR &=~1;
	smalltimer->CR1 |=1;
	while((smalltimer->SR&0x0001)!=1);
	smalltimer->SR&=~(0x0001);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
