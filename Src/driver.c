#include "driver.h"

#include "lcd.h"
#include "lvgl.h"
#include "stm32f3xx.h"
#include "touch.h"
#include <memory.h>
#include <stdlib.h>

#define AVG_BUFFER_SIZE 3
#define TOUCH_TRIGGER_VALUE 3800
#define ABS(x) ((x) < 0 ? -(x) : (x))

struct LVGLDriver {
  lcd_ili93xx_driver_t lcd_driver;
  lcd_xpt2046_driver_t touch_driver;

  size_t buf_size;
  lv_color_t *buf;
  lv_disp_draw_buf_t lv_draw_buf;
  lv_disp_drv_t lv_driver;

  lv_disp_t *disp;

  lv_indev_drv_t indev_drv;
  lv_indev_t *indev;
  int last_x_position, last_y_position;
};

static void lvgl_display_flush_cb(lv_disp_drv_t *disp_drv,
                                  const lv_area_t *area, lv_color_t *color_p) {

  LVGLDriver *drv = disp_drv->user_data;
  lcd_ili93xx_fill_area(&drv->lcd_driver, area->x1, area->y1, area->x2,
                        area->y2, (uint16_t *)color_p);

  lv_disp_flush_ready(disp_drv);
}

static void lvgl_display_touch_sensor_read_cb(lv_indev_drv_t *disp_drv,
                                              lv_indev_data_t *data) {
  LVGLDriver *drv = disp_drv->user_data;

  int touch_x;
  int touch_y;
  int touched;

  // Grab touch data
  int x[AVG_BUFFER_SIZE] = {0};
  int y[AVG_BUFFER_SIZE] = {0};
  int z1[AVG_BUFFER_SIZE] = {0};
  int z2[AVG_BUFFER_SIZE] = {0};

  for (int i = 0; i < AVG_BUFFER_SIZE; i++) {
    int err = 0;
    err |=
        lcd_xpt2046_measure(&drv->touch_driver, XPT2046_CMD_MEASURE_X, &x[i]);
    err |=
        lcd_xpt2046_measure(&drv->touch_driver, XPT2046_CMD_MEASURE_Y, &y[i]);
    err |=
        lcd_xpt2046_measure(&drv->touch_driver, XPT2046_CMD_MEASURE_Z1, &z1[i]);
    err |=
        lcd_xpt2046_measure(&drv->touch_driver, XPT2046_CMD_MEASURE_Z2, &z2[i]);

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

  if (touched) {
    drv->last_x_position = touch_x;
    drv->last_y_position = touch_y;
  }
  data->point.x = drv->last_x_position;
  data->point.y = drv->last_y_position;
  if (touched) {
    data->state = LV_INDEV_STATE_PR;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

LVGLDriver *LVGLDriver_new() {

  lv_init();

  LVGLDriver *drv = malloc(sizeof(LVGLDriver));
  if (!drv) {
    return NULL;
  }

  // Touch driver init
  int err = 0;
  drv->touch_driver = make_touch_driver(&err);
  if (err) {
    LVGLDriver_destroy(drv);
    return NULL;
  }

  // LCD driver init
  drv->lcd_driver = make_lcd_driver(&err);
  if (err) {
    LVGLDriver_destroy(drv);
    return NULL;
  }

  int16_t width = 0;
  int16_t height = 0;
  lcd_ili93xx_get_width(&drv->lcd_driver, &width);
  lcd_ili93xx_get_height(&drv->lcd_driver, &height);

  drv->buf_size = 30 * width;
  drv->buf = malloc(sizeof(lv_color_t) * drv->buf_size);
  if (!drv->buf) {
    LVGLDriver_destroy(drv);
    return NULL;
  }

  lv_disp_draw_buf_init(&drv->lv_draw_buf, drv->buf, NULL, drv->buf_size);

  lv_disp_drv_init(&drv->lv_driver);

  drv->lv_driver.hor_res = width;
  drv->lv_driver.ver_res = height;
  drv->lv_driver.draw_buf = &drv->lv_draw_buf;
  drv->lv_driver.user_data = drv;
  drv->lv_driver.flush_cb = lvgl_display_flush_cb;
  drv->disp = lv_disp_drv_register(&drv->lv_driver);

  if (!drv->disp) {
    LVGLDriver_destroy(drv);
    return NULL;
  }

  lv_indev_drv_init(&drv->indev_drv);
  drv->indev_drv.type = LV_INDEV_TYPE_POINTER;
  drv->last_x_position = 0;
  drv->last_y_position = 0;
  drv->indev_drv.user_data = drv;
  drv->indev_drv.disp = drv->disp;
  drv->indev_drv.read_cb = lvgl_display_touch_sensor_read_cb;
  drv->indev = lv_indev_drv_register(&drv->indev_drv);
  if (!drv->indev) {
    LVGLDriver_destroy(drv);
    return NULL;
  }

  return drv;
}

void LVGLDriver_destroy(LVGLDriver *drv) {
  if (drv) {
    if (drv->buf) {
      free(drv->buf);
    }

    free(drv);
  }
}

void LVGLDriver_update(LVGLDriver *drv) {
  if (!drv) {
    return;
  }

  lv_timer_handler();
}

lv_obj_t* LVGLDriver_GetScreen(LVGLDriver *drv) {
return lv_disp_get_scr_act(drv->disp);
}
