#include "lcd.h"

#include "open32f3_lcd_utils.h"

int __lcd_write_register(void *user_data, uint16_t address, uint16_t value) {
  (void)user_data;

  OPEN32F3_LCD_CLEAR_CS();

  // Address send
  OPEN32F3_LCD_CLEAR_RS();
  OPEN32F3_LCD_DATA_WRITE(address);
  OPEN32F3_LCD_CLEAR_WR();
  OPEN32F3_LCD_SET_WR();
  OPEN32F3_LCD_SET_RS();

  // Data send
  OPEN32F3_LCD_DATA_WRITE(value);
  OPEN32F3_LCD_CLEAR_WR();
  OPEN32F3_LCD_SET_WR();

  OPEN32F3_LCD_SET_CS();
}

int __lcd_write_words(void *user_data, uint16_t address, uint16_t *data,
                      size_t size) {
  (void)user_data;

  OPEN32F3_LCD_CLEAR_CS();

  // Address send
  OPEN32F3_LCD_CLEAR_RS();
  OPEN32F3_LCD_DATA_WRITE(address);
  OPEN32F3_LCD_CLEAR_WR();
  OPEN32F3_LCD_SET_WR();
  OPEN32F3_LCD_SET_RS();

  // Data send
  while (size--) {
    uint16_t value = *data;
    data++;

    OPEN32F3_LCD_DATA_WRITE(value);
    OPEN32F3_LCD_CLEAR_WR();
    OPEN32F3_LCD_SET_WR();
  }

  OPEN32F3_LCD_SET_CS();
}

int __lcd_read_register(void *user_data, uint16_t address, uint16_t *value) {
  (void)user_data;

  OPEN32F3_LCD_CLEAR_CS();

  // Address send
  OPEN32F3_LCD_CLEAR_RS();
  OPEN32F3_LCD_DATA_WRITE(address);
  OPEN32F3_LCD_CLEAR_WR();
  OPEN32F3_LCD_SET_WR();
  OPEN32F3_LCD_SET_RS();

  // Data read
  OPEN32F3_LCD_DATA_SET_INPUT_MODE();
  OPEN32F3_LCD_CLEAR_RD();
  OPEN32F3_LCD_SET_RD();
  *value = OPEN32F3_LCD_DATA_READ();
  OPEN32F3_LCD_DATA_SET_OUTPUT_MODE();

  OPEN32F3_LCD_SET_CS();
}

lcd_ili93xx_driver_t make_lcd_driver(int *err) {
  lcd_ili93xx_driver_t driver;
  lcd_ili93xx_init_clear(&driver);
  driver.user_data = NULL;
  driver.reset = open32f3_lcd_reset;
  driver.read_reg = __lcd_read_register;
  driver.write_reg = __lcd_write_register;
  driver.write_words = __lcd_write_words;
  driver.delay = open32f3_lcd_delay;

  int res = lcd_ili93xx_init(&driver);
  if (res && err) {
    *err = res;
  }

  return driver;
}
