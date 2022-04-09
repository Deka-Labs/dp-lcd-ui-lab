
set(LCD_SOURCES 
    ${CMAKE_SOURCE_DIR}/lcd_libs/lab_5_icons.c
    ${CMAKE_SOURCE_DIR}/lcd_libs/lcd_ili93xx_driver.c
    ${CMAKE_SOURCE_DIR}/lcd_libs/open32f3_lcd_touch_utils.c
    ${CMAKE_SOURCE_DIR}/lcd_libs/open32f3_lcd_utils.c
)

add_library(lcd_ll STATIC ${LCD_SOURCES})
target_include_directories(lcd_ll PUBLIC ${CMAKE_SOURCE_DIR}/lcd_libs)
target_include_directories(lcd_ll PRIVATE ${CMAKE_SOURCE_DIR}/Inc)
target_link_libraries(lcd_ll PRIVATE 
    HAL::STM32::${STM32_FAMILY}::RCC
    HAL::STM32::${STM32_FAMILY}::RCCEx
    HAL::STM32::${STM32_FAMILY}::GPIO
    HAL::STM32::${STM32_FAMILY}::CORTEX
    HAL::STM32::${STM32_FAMILY}::TIM
    HAL::STM32::${STM32_FAMILY}::TIMEx
    CMSIS::STM32::F303xC
)