#pragma once

#include "lvgl.h"

typedef struct LVGLDriver LVGLDriver;

LVGLDriver *LVGLDriver_new();
void LVGLDriver_destroy(LVGLDriver *drv);
void LVGLDriver_update(LVGLDriver *drv);

lv_obj_t* LVGLDriver_GetScreen(LVGLDriver *drv);
