cmake_minimum_required(VERSION 3.10)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/stm32_gcc.cmake)

project(lcd_lab ASM C)
set(STM32_FAMILY F3 CACHE STRING "Family of STM32 for searching CMSIS and HAL packages")
set(STM32_STARTUP_FILE ${CMAKE_SOURCE_DIR}/startup_stm32f303xc.s CACHE FILEPATH "Path to startup asm file")
set(STM32_LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F303VCTx_FLASH.ld CACHE FILEPATH "Path to linker script of MCU")


find_package(CMSIS COMPONENTS STM32${STM32_FAMILY} REQUIRED)
find_package(HAL COMPONENTS STM32${STM32_FAMILY} REQUIRED)

add_subdirectory(lcd_libs)

set(HEADERS 
    Inc/main.h
    Inc/stm32f3xx_hal_conf.h 
    Inc/stm32f3xx_it.h

    Inc/lcd.h
    Inc/touch.h
    Inc/keyboard.h
)

set(SOURCES 
    Src/main.c 
    Src/stm32f3xx_hal_msp.c
    Src/stm32f3xx_it.c

    Src/lcd.c
    Src/touch.c
    Src/keyboard.c
)

add_library(startup ${STM32_STARTUP_FILE})
add_executable(${CMAKE_PROJECT_NAME} ${HEADERS} ${SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE Inc)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE USE_HAL_DRIVER)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE 
    HAL::STM32::${STM32_FAMILY}::RCC
    HAL::STM32::${STM32_FAMILY}::RCCEx
    HAL::STM32::${STM32_FAMILY}::GPIO
    HAL::STM32::${STM32_FAMILY}::CORTEX
    HAL::STM32::${STM32_FAMILY}::UART
    HAL::STM32::${STM32_FAMILY}::UARTEx
    HAL::STM32::${STM32_FAMILY}::DMA
    HAL::STM32::${STM32_FAMILY}::TIM
    HAL::STM32::${STM32_FAMILY}::TIMEx
    HAL::STM32::${STM32_FAMILY}::SPI
    CMSIS::STM32::F303xC
    STM32::NoSys
    startup
    lcd_ll
)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -specs=nano.specs -T${STM32_LINKER_SCRIPT} )


stm32_print_size_of_target(${CMAKE_PROJECT_NAME})
stm32_generate_binary_file(${CMAKE_PROJECT_NAME})
stm32_generate_hex_file(${CMAKE_PROJECT_NAME})