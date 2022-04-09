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
#include <string.h>

#include "keyboard.h"
#include "lab_5_icons.h"
#include "lcd.h"
#include "touch.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define AVG_BUFFER_SIZE 3
#define TOUCH_TRIGGER_VALUE 3800
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define CURSOR_DEFAULT cursor_6_image
#define CURSOR_ON_TOUCH cursor_5_image
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
int background_color = LCD_ILI93XX_COLOR_FUCHSIA;
int cursor_color = LCD_ILI93XX_COLOR_GRAY;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void KeyboardHandleEvent(KeyEvent e) {
  if (e.state != PRESSED) {
    return;
  }

  switch (e.key) {
    case 0:
      background_color = LCD_ILI93XX_COLOR_FUCHSIA;
      break;

    case 1:
      background_color = LCD_ILI93XX_COLOR_SILVER;
      break;

    case 2:
      background_color = LCD_ILI93XX_COLOR_MAROON;
      break;

    case 3:
      background_color = LCD_ILI93XX_COLOR_NAVY;
      break;

    default:
      break;
  }
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  int err = 0;

  lcd_xpt2046_driver_t touch_drv = make_touch_driver(&err);
  if (err) {
    while (1) {
      __NOP();
    }
  }

  lcd_ili93xx_driver_t lcd_drv = make_lcd_driver(&err);
  if (err) {
    while (1) {
      __NOP();
    }
  }

  Keyboard_Init();
  Keyboard_SetCallback(KeyboardHandleEvent);

  int16_t width = 0;
  int16_t height = 0;
  lcd_ili93xx_get_width(&lcd_drv, &width);
  lcd_ili93xx_get_height(&lcd_drv, &height);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int touched = 0;
  int touch_x, touch_y;

  while (1) {
    // Grab touch data
    int x[AVG_BUFFER_SIZE] = {0};
    int y[AVG_BUFFER_SIZE] = {0};
    int z1[AVG_BUFFER_SIZE] = {0};
    int z2[AVG_BUFFER_SIZE] = {0};

    for (int i = 0; i < AVG_BUFFER_SIZE; i++) {
      int err = 0;
      err |= lcd_xpt2046_measure(&touch_drv, XPT2046_CMD_MEASURE_X, &x[i]);
      err |= lcd_xpt2046_measure(&touch_drv, XPT2046_CMD_MEASURE_Y, &y[i]);
      err |= lcd_xpt2046_measure(&touch_drv, XPT2046_CMD_MEASURE_Z1, &z1[i]);
      err |= lcd_xpt2046_measure(&touch_drv, XPT2046_CMD_MEASURE_Z2, &z2[i]);

      if (err) {
        __NOP();
      }
    }
    // Average data
    int avg_x = 0, avg_y = 0, avg_z1 = 0, avg_z2 = 0;
    for (int i = 0; i < AVG_BUFFER_SIZE; i++) {
      avg_x += x[i];
      avg_y += y[i];
      avg_z1 += z1[i];
      avg_z2 += z2[i];
    }
    avg_x /= AVG_BUFFER_SIZE;
    avg_y /= AVG_BUFFER_SIZE;
    avg_z1 /= AVG_BUFFER_SIZE;
    avg_z2 /= AVG_BUFFER_SIZE;

    // Touch detection
    if (ABS(avg_z2 - avg_z1) < TOUCH_TRIGGER_VALUE) {
      // Touched
      touched = 1;
      touch_x = avg_x * 275 / 4096 - 25;
      touch_y = avg_y * 375 / 4096 - 25;
    } else {
      touched = 0;
    }

    // Draw
    int cached_back_color = background_color;
    int cached_front_color = cursor_color;

    for (int x = 0; x < 240; x++) {
      uint16_t color_line[320];
      for (int i = 0; i < 320; i++) {
        color_line[i] = cached_back_color;
      }

      if (touch_x <= x && x < touch_x + 16) {
        int x_in_img = x - touch_x;
        for (int y_in_img = 0; y_in_img < 16; y_in_img++) {
          int mask = CURSOR_DEFAULT[x_in_img][y_in_img];
          if (touched) {
            mask = CURSOR_ON_TOUCH[x_in_img][y_in_img];
          }

          if (mask) {
            color_line[touch_y + y_in_img] = cached_front_color;
          }
        }
      }

      lcd_ili93xx_fill_area(&lcd_drv, x, 0, x, 320, color_line);
    }

    // lcd_ili93xx_fill_area_color(&lcd_drv, 0, 0, width, height,
    //                             cached_back_color);

    // uint16_t color_data[16][16] = {0};

    // for (int x = 0; x < 16; x++) {
    //   for (int y = 0; y < 16; y++) {
    //     int mask = CURSOR_DEFAULT[x][y];
    //     if (touched) {
    //       mask = CURSOR_ON_TOUCH[x][y];
    //     }

    //     if (mask) {
    //       color_data[x][y] = cached_front_color;
    //     } else {
    //       color_data[x][y] = cached_back_color;
    //     }
    //   }
    // }

    // lcd_ili93xx_fill_area(&lcd_drv, touch_x, touch_y, touch_x + 16,
    //                       touch_y + 16, (int16_t*)color_data);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_Delay(1);
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void) {
  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC,
                    TOUCH_CS_Pin | KEYBOARD_LINE_0_Pin | KEYBOARD_LINE_1_Pin |
                        KEYBOARD_LINE_2_Pin | KEYBOARD_LINE_3_Pin |
                        GPIO_PIN_10 | GPIO_PIN_11,
                    GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE,
                    LD4_Pin | LD3_Pin | LD5_Pin | LD7_Pin | LD9_Pin | LD10_Pin |
                        LD8_Pin | LD6_Pin,
                    GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_8 | GPIO_PIN_9,
                    GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD,
                    GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                        GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 |
                        GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                        GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,
                    GPIO_PIN_SET);

  /*Configure GPIO pin : TOUCH_CS_Pin */
  GPIO_InitStruct.Pin = TOUCH_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TOUCH_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : KEYBOARD_COL_0_Pin KEYBOARD_COL_1_Pin
   * KEYBOARD_COL_2_Pin KEYBOARD_COL_3_Pin */
  GPIO_InitStruct.Pin = KEYBOARD_COL_0_Pin | KEYBOARD_COL_1_Pin |
                        KEYBOARD_COL_2_Pin | KEYBOARD_COL_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : KEYBOARD_LINE_0_Pin KEYBOARD_LINE_1_Pin
   * KEYBOARD_LINE_2_Pin KEYBOARD_LINE_3_Pin */
  GPIO_InitStruct.Pin = KEYBOARD_LINE_0_Pin | KEYBOARD_LINE_1_Pin |
                        KEYBOARD_LINE_2_Pin | KEYBOARD_LINE_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD7_Pin
                           LD9_Pin LD10_Pin LD8_Pin LD6_Pin */
  GPIO_InitStruct.Pin = LD4_Pin | LD3_Pin | LD5_Pin | LD7_Pin | LD9_Pin |
                        LD10_Pin | LD8_Pin | LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB11 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD10 PD11
                           PD12 PD13 PD14 PD15
                           PD0 PD1 PD2 PD3
                           PD4 PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                        GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 |
                        GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                        GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PC10 PC11 */
  GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 4);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 4);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_TSC_IRQn, 0, 4);
  HAL_NVIC_EnableIRQ(EXTI2_TSC_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 4);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
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
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
