#include "open32f3_lcd_utils.h"

int open32f3_lcd_delay(void *lcd_data, int ms) {
    HAL_Delay(ms);
    return 0;
}

int open32f3_lcd_reset(void *lcd_data) {
    // reset LCD
    HAL_GPIO_WritePin(OPEN32F3_LCD_RESET_PORT, OPEN32F3_LCD_RESET_PIN, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(OPEN32F3_LCD_RESET_PORT, OPEN32F3_LCD_RESET_PIN, GPIO_PIN_SET);
    return 0;
}
