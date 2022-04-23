#pragma once

typedef struct LVGLDriver LVGLDriver;

LVGLDriver *LVGLDriver_new();
void LVGLDriver_destroy(LVGLDriver *drv);
void LVGLDriver_update(LVGLDriver *drv);
