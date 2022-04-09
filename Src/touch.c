#include "touch.h"

#include "main.h"

extern SPI_HandleTypeDef hspi1;

int __lcd_touch_callback(void *user_data, uint8_t *out_buf, uint8_t *in_buf,
                         size_t size) {
  // установить CS линию в 0 для начала приема/передачи
  HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_RESET);
  // передать/принять данные
  HAL_StatusTypeDef status =
      HAL_SPI_TransmitReceive(&hspi1, out_buf, in_buf, size, HAL_MAX_DELAY);
  // установить CS линию в 1 для конца приема/передачи
  HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET);
  return status == HAL_OK ? 0 : -1;
}

lcd_xpt2046_driver_t make_touch_driver(int *err) {
  HAL_Delay(100);
  lcd_xpt2046_driver_t driver;
  lcd_xpt2046_init_clear(&driver);

  driver.user_data = NULL;
  driver.communication_cb = __lcd_touch_callback;

  int res = lcd_xpt2046_init(&driver);
  if (res && err) {
    *err = res;
  }
  HAL_Delay(100);
  return driver;
}