/**
 * Minimal LVGL 8.2 configuration.
 *
 * Note: if some sections/options are omitted, then default values from
 * "lv_conf_internal.h" are used.
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/

/*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#define LV_COLOR_DEPTH 16

/*=========================
   MEMORY SETTINGS
 *=========================*/

/*1: use custom or standard malloc/free, 0: use the built-in `lv_mem_alloc()`
 * and `lv_mem_free()`*/
#define LV_MEM_CUSTOM 1

/*====================
   HAL SETTINGS
 *====================*/

/*Default display refresh period. LVG will redraw changed areas with this period
 * time*/
#define LV_DISP_DEF_REFR_PERIOD 30 /*[ms]*/

/*Input device read period in milliseconds*/
#define LV_INDEV_DEF_READ_PERIOD 30 /*[ms]*/

/* Use STM32 HAL tick tick source. */
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM == 1
#define LV_TICK_CUSTOM_INCLUDE                                                 \
  "stm32f3xx_hal.h" /*Header for the system time function*/
#define LV_TICK_CUSTOM_SYS_TIME_EXPR                                           \
  (HAL_GetTick()) /*Expression evaluating to current system time in ms*/
#endif            /*LV_TICK_CUSTOM*/

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/

/*-------------
 * Drawing
 *-----------*/

/*Enable complex draw engine.
 *Required to draw shadow, gradient, rounded corners, circles, arc, skew lines,
 *image transformations or any masks*/
#define LV_DRAW_COMPLEX 0

/*-------------
 * Logging
 *-----------*/

/*Enable the log module*/
#define LV_USE_LOG 0
/*1: Print the log with 'printf';
 *0: User need to register a callback with `lv_log_register_print_cb()`*/
#define LV_LOG_PRINTF 1

/*-------------
 * Asserts
 *-----------*/

/*Enable asserts if an operation is failed or an invalid data is found.
 *If LV_USE_LOG is enabled an error message will be printed on failure*/
#define LV_USE_ASSERT_NULL                                                     \
  1 /*Check if the parameter is NULL. (Very fast, recommended)*/
#define LV_USE_ASSERT_MALLOC                                                   \
  1 /*Checks is the memory is successfully allocated or no. (Very fast,        \
       recommended)*/
#define LV_USE_ASSERT_STYLE                                                    \
  1 /*Check if the styles are properly initialized. (Very fast, recommended)*/
#define LV_USE_ASSERT_MEM_INTEGRITY                                            \
  0 /*Check the integrity of `lv_mem` after critical operations. (Slow)*/
#define LV_USE_ASSERT_OBJ                                                      \
  0 /*Check the object's type and existence (e.g. not deleted). (Slow)*/

/*-------------
 * Others
 *-----------*/

/*Change the built in (v)snprintf functions*/
#define LV_SPRINTF_CUSTOM 1

#define LV_SPRINTF_INCLUDE <stdio.h>
#define lv_snprintf snprintf
#define lv_vsnprintf vsnprintf

/* Custom data of user objects */
#define LV_USE_USER_DATA 1

/*==================
 *   FONT USAGE
 *===================*/

/*Montserrat fonts with ASCII range and some symbols using bpp = 4
 *https://fonts.google.com/specimen/Montserrat*/
#define LV_FONT_MONTSERRAT_8 0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 0
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 0
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 0
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0

/*Always set a default font*/
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/*=================
 *  TEXT SETTINGS
 *=================*/

/*==================
 *  WIDGET USAGE
 *================*/

/*Documentation of the widgets:
 * https://docs.lvgl.io/latest/en/html/widgets/index.html*/

#define LV_USE_ARC 1

#define LV_USE_ANIMIMG 1

#define LV_USE_BAR 1

#define LV_USE_BTN 1

#define LV_USE_BTNMATRIX 1

#define LV_USE_CANVAS 1

#define LV_USE_CHECKBOX 1

#define LV_USE_DROPDOWN 1 /*Requires: lv_label*/

#define LV_USE_IMG 1 /*Requires: lv_label*/

#define LV_USE_LABEL 1

#define LV_USE_LINE 1

#define LV_USE_ROLLER 1 /*Requires: lv_label*/

#define LV_USE_SLIDER 1 /*Requires: lv_bar*/

#define LV_USE_SWITCH 1

#define LV_USE_TEXTAREA 1 /*Requires: lv_label*/

#define LV_USE_TABLE 1

/*==================
 * EXTRA COMPONENTS
 *==================*/

/*-----------
 * Widgets
 *----------*/
#define LV_USE_CALENDAR 0

#define LV_USE_CHART 0

#define LV_USE_COLORWHEEL 0

#define LV_USE_IMGBTN 0

#define LV_USE_KEYBOARD 0

#define LV_USE_LED 0

#define LV_USE_LIST 0

#define LV_USE_MENU 0

#define LV_USE_METER 0

#define LV_USE_MSGBOX 0

#define LV_USE_SPINBOX 0

#define LV_USE_SPINNER 0

#define LV_USE_TABVIEW 0

#define LV_USE_TILEVIEW 0

#define LV_USE_WIN 0

#define LV_USE_SPAN 1

/*-----------
 * Themes
 *----------*/

/*A simple, impressive and very complete theme*/
#define LV_USE_THEME_DEFAULT 1
#define LV_THEME_DEFAULT_DARK 0
#define LV_THEME_DEFAULT_GROW 1

/*A very simple theme that is a good starting point for a custom theme*/
#define LV_USE_THEME_BASIC 0

/*A theme designed for monochrome displays*/
#define LV_USE_THEME_MONO 0

/*-----------
 * Layouts
 *----------*/

/*A layout similar to Flexbox in CSS.*/
#define LV_USE_FLEX 1

/*A layout similar to Grid in CSS.*/
#define LV_USE_GRID 1

/*--END OF LV_CONF_H--*/

#endif /*LV_CONF_H*/
