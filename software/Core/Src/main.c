/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "ssd1306.h"
#include "fonts.h"
#include "test.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define UP_BUTON HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)
#define DOWN_BUTON HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11)
#define ESC_BUTON HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12)
#define SET_BUTON HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)
#define MAX_VALUE_INDEX 5
#define MAX_ROW_INDEX 15
#define PAGE_VALUE 4
#define MAX_SIZE 8
#define MAX_BYTE 20
#define SIZE 2
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */
/*
 * DEGISKENLER
 * */

typedef struct menu_variable{
	char index[15];
	int menu_id;
	int adc_sensor_value;
}menu;

typedef enum menu_titles{
	mainmenu=0,
	DEVICE_SETTINGS,
	TEMP_SETTINGS,
	LIMIT_SETTINGS,
	CO2_SETTINGS,
	ABOUT,
	device_adress,
	device_calibration,
	device_reset,
	device_factory_reset,
	temp_offset,
	limits_temp,
	limits_co2,
	co2_temp_offset,
	co2_altitute,
	co2_calibration,
	co2_offsett,

}title_menu;


int main_menu_flag = 0;
int page_memory;
int buton_up_flag=0;
int buton_down_flag=0;
int buton_esc_flag=0;
int buton_set_flag=0;
/*MENULER*/

//ANA MENU
menu main_menu[6]={{"Device Settings"},{"Temp Settings"},{"Limit Settings"},{"Co2 Settings"},{"About"}};

//DEVICE SETTINGS
menu device_menu[5]={{"DEVICE SETTINGS"},{"Address"},{"4-20 Calibration"},{"Factory Reset"},{"Reset"}};
menu device_menu_address[5]={{"device ADDRESS","1"}};
menu device_menu_calibration[5]={{"111 ppm"}};
menu device_factory[5]={{"111 ppm"}};
menu device_menu_reset[6]={{"resetADDRESS","1"}};


//TEMP SETTINGS
menu Temp_menu[6]={{"TEMP SETTINGS"},{"Offset"},{5}};
menu temp_menu_offset[6]={{"TEMP OFSET","0.0C"},{6}};


//LIMIT SETTINGS
menu limits_menu[6]={{"LIMITS"},{"Temp Limits"},{"Co2 Limits"},{7}};
menu limits_menu_temp_limits[6]={{"TEMP LIMITS"},{"LOW:123"},{"HIGH:123"},{8}};
menu limits_menu_co2_limits[6]={{"CO2 LIMITS"},{"LOW:123"},{"HIGH:123"},{9}};

//CO2 SETTINGS
menu co2_settings_menu[6]={{"CO2 SETTINGS"},{"Temp Offset"},{"Altitute"},{"Calibration"},{"Offset"},{10}};
menu co2_settings_menu_temp_offset[6]={{"CO2 TEMP."},{"11"},{11}};
menu co2_settings_menu_altitute[5]={{"ALTITUDE","11"}};
menu co2_settings_menu_calibration[5]={{"CO2 CALIB.","400"}};
menu co2_settings_menu_offset[5]={{"OFFSET","400"}};

//ABOUT
menu about_menu[5]={{"CANTEK VER 1.0"}};
int page_count = 0;
int menu_index = 0;
int button_flag=0;
int buton_select_flag=0;

int buton_time_flag = 0;
uint8_t crossIndexLine[6]={0,10,20,30,40,50};
int time_buton=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define LCD_WIDTH 128
#define LCD_HEIGHT 64
#define PIXEL LCD_HEIGHT*LCD_HEIGHT

//İlk kelimenin boyutunu bulma
int length(char *s){
    if(*s != '\0' && *s !=' ')
    {
        return (1+length(s+1));
    }
    else{
        return 0;
    }
}

void submenus_set(int menuindex_lower_limit,int page_count_lower_limit,int total_number_of_menus)
{
	if(buton_set_flag)
	{
		if(menu_index == menuindex_lower_limit)
		{
			page_count=page_count_lower_limit;
			buton_set_flag=0;
		}

		else{
			menuindex_lower_limit++;
			page_count_lower_limit++;
			total_number_of_menus--;
			if(total_number_of_menus>0)
			{
				submenus_set(menuindex_lower_limit, page_count_lower_limit, total_number_of_menus);
			}

		}
	}

}

void submenus_esc(int back_to_page)
{
	if(buton_esc_flag)
	{
		page_count=back_to_page;
		buton_esc_flag=0;
	}
}






void lcd_print(menu p[])
{
    for(int i = 0;i<5;i++){

    	SSD1306_GotoXY(10,crossIndexLine[i]);
        SSD1306_Puts(p[i].index,&Font_7x10,1);
        SSD1306_UpdateScreen();

    }
}


void page_buffer_choice(int choice_menu)
{

	if( buton_set_flag)
	{
		menu_choice(choice_menu);
	}
	else if(buton_esc_flag)
	{
		menu_choice(choice_menu);
	}
}


void menu_choice(int index_choice)
{
	SSD1306_Clear();
	switch(index_choice)
	{
		case mainmenu:
			lcd_print(main_menu);
			submenus_set(0, 1, 5);

			break;
		case DEVICE_SETTINGS:
			lcd_print(device_menu);
			submenus_set(0, 6, 4);
			submenus_esc(0);

			break;
		case TEMP_SETTINGS:
			lcd_print(Temp_menu);
			submenus_set(0, 10, 1);
			submenus_esc(0);

			break;
		case LIMIT_SETTINGS:
			lcd_print(limits_menu);
			submenus_set(1, 11, 2);
			submenus_esc(0);
			break;
		case CO2_SETTINGS:
			lcd_print(co2_settings_menu);
			submenus_set(1, 13, 4);
			submenus_esc(0);
			break;
		case ABOUT:
			lcd_print(about_menu);
			submenus_esc(0);
			break;
		case device_adress:
			lcd_print(device_menu_address);
			submenus_esc(1);
			break;
		case device_calibration:
			lcd_print(device_menu_calibration);
			submenus_esc(1);
			break;
		case device_reset:
			lcd_print(device_menu_reset);
			submenus_esc(1);
			break;
		case device_factory_reset:
			lcd_print(device_factory);
			submenus_esc(1);
		case temp_offset:
			lcd_print(temp_menu_offset);
			submenus_esc(2);
			break;
		case limits_temp:
			lcd_print(limits_menu_temp_limits);
			submenus_esc(3);
			break;
		case limits_co2:
			lcd_print(limits_menu_co2_limits);
			submenus_esc(3);
			break;
		case co2_temp_offset:
			lcd_print(co2_settings_menu_temp_offset);
			submenus_esc(4);
			break;
		case co2_altitute:
			lcd_print(co2_settings_menu_altitute);
			submenus_esc(4);
			break;
		case co2_calibration:
			lcd_print(co2_settings_menu_calibration);
			submenus_esc(4);
			break;
		case co2_offsett:
			lcd_print(co2_settings_menu_offset);
			submenus_esc(4);
			break;
	}
}

void cross_clear(int index)
{
	 static int menu_index_memory;

	  SSD1306_GotoXY(0,crossIndexLine[index]);
	  SSD1306_Puts(">",&Font_7x10,1);
	  if(index!=menu_index_memory)
	  {
		  SSD1306_GotoXY(0,crossIndexLine[menu_index_memory]);
		  SSD1306_Puts(" ",&Font_7x10,1);
	  }
	  SSD1306_UpdateScreen();
	  menu_index_memory=index;


}


int buton_flag_function()
{
	if(!DOWN_BUTON)
	{
		buton_down_flag=1;
		return 1;
	}
	else if(!UP_BUTON)
	{
		buton_up_flag=1;
		return 2;
	}

	else if(!ESC_BUTON)
	{
		buton_esc_flag=1;
		return 3;
	}
	else if(!SET_BUTON)
	{
		buton_set_flag=1;

		return 4;
	}
	else {
		return 0;
	}
}


void buton_flag(){
	button_flag=buton_flag_function();

	if(button_flag==0)
	{

		if(buton_up_flag)
		{
			menu_index--;
			if(menu_index>MAX_VALUE_INDEX-1)
			{
				menu_index=0;
			}
			buton_up_flag=0;
		}
		else if(buton_down_flag)
		{
			menu_index++;
			if(menu_index>MAX_VALUE_INDEX-1)
			{
				menu_index=0;
			}
			buton_down_flag=0;
		}
		else if(buton_esc_flag)
		{
			buton_esc_flag = 0;
		}
		else if(buton_set_flag)
		{

			buton_set_flag = 0;
		}
		cross_clear(menu_index);
	}

}

/*int length(char *s){
    if(*s != '\0' && *s !=' ')
    {
        return (1+length(s+1));
    }
    else{
        return 0;
    }
}
*/

char str1 [MAX_SIZE][MAX_BYTE] ={"CO2 settings"};
char str2 [MAX_SIZE][MAX_BYTE] ={"TEMP about"};
void temp(char *str_1,char *str_2)
{
	int word_2_length=strlen(str2);
	   for(int i = 0;i<word_2_length;i++){
		   *(str_1+i)=*(str_2+i);
	   }
}

/*void temp(char *arr,char *arr2)
{
    char memory;
    int length1=strlen(arr)-1;
   int length2=strlen(arr+MAX_SIZE)-1;
   int length3=strlen(arr2)-1;
  int length4=strlen(arr2+MAX_SIZE)-1;


     memory=*(arr+length1);
    *(arr+length1)=*(arr+MAX_SIZE+length2);
    *(arr+MAX_SIZE+length2)=memory;
}
*/




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
  MX_I2C1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);
  SSD1306_Init();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	   if(main_menu_flag)
	  {
		  buton_flag();
		  page_buffer_choice(page_count);

	  }

	 // temp2(str1,str2);

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

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 3999;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : btn_up_Pin */
  GPIO_InitStruct.Pin = btn_up_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(btn_up_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : btn_down_Pin btn_esc_Pin btn_set_Pin */
  GPIO_InitStruct.Pin = btn_down_Pin|btn_esc_Pin|btn_set_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
int time_count = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(!SET_BUTON)
	{
		time_buton++;
	}
	else
	{
		time_buton=0;
		buton_set_flag=0;
	}

	if(time_buton == 5)
	{
		buton_set_flag=1;
		main_menu_flag=1;
		time_buton=0;
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
