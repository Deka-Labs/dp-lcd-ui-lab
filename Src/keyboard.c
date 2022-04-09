#include "keyboard.h"

#include <stm32f3xx_hal.h>

#include "main.h"

#define KEYS_COUNT 16
#define KEY_COLUMNS 4
#define KEY_LINES 4

typedef struct keyboard {
  KeyState states[KEYS_COUNT];
  uint32_t time_pressed[KEYS_COUNT];

  KeyboardCallback callback;
} keyboard;

/// Global keyboard subsystem instance
keyboard g_Keyboard;
/// Port and pins mapping
GPIO_TypeDef *g_keyboard_column_gpios[KEY_COLUMNS] = {
    KEYBOARD_COL_0_GPIO_Port, KEYBOARD_COL_1_GPIO_Port,
    KEYBOARD_COL_2_GPIO_Port, KEYBOARD_COL_3_GPIO_Port};
uint16_t g_keyboard_column_pins[KEY_COLUMNS] = {
    KEYBOARD_COL_0_Pin, KEYBOARD_COL_1_Pin, KEYBOARD_COL_2_Pin,
    KEYBOARD_COL_3_Pin};

GPIO_TypeDef *g_keyboard_lines_gpios[KEY_COLUMNS] = {
    KEYBOARD_LINE_0_GPIO_Port, KEYBOARD_LINE_1_GPIO_Port,
    KEYBOARD_LINE_2_GPIO_Port, KEYBOARD_LINE_3_GPIO_Port};
uint16_t g_keyboard_lines_pins[KEY_COLUMNS] = {
    KEYBOARD_LINE_0_Pin, KEYBOARD_LINE_1_Pin, KEYBOARD_LINE_2_Pin,
    KEYBOARD_LINE_3_Pin};

/// returns column number by specified GPIO port and pin
int _keyboard_pin_nmb_to_column(uint16_t pin) {
  int result = -1;
  for (int i = 0; i < KEY_COLUMNS; i++) {
    if (pin == g_keyboard_column_pins[i]) {
      return i;
    }
  }
  return result;
}

/// Checks what button is pressed on column
/// If yes - returns key id
/// In errors returns negative values
int _keyboard_check_column(int col);

/// release all buttons on column
void _keyboard_column_released(int col);

void Keyboard_Init() {
  // Reset state
  for (int i = 0; i < KEYS_COUNT; i++) {
    g_Keyboard.states[i] = RELEASED;
    g_Keyboard.time_pressed[i] = 0;
  }
  g_Keyboard.callback = NULL;

  // Check current state of keys
  for (int i = 0; i < KEY_COLUMNS; i++) {
    int col_state = _keyboard_check_column(i);
    if (col_state >= 0) {
      Keyboard_PressKey(col_state);
    }
  }
}

void Keyboard_SetCallback(KeyboardCallback callback) {
  g_Keyboard.callback = callback;
}

void Keyboard_PressKey(int key) {
  if (g_Keyboard.states[key] == PRESSED) {
    return;
  }

  if ((HAL_GetTick() - g_Keyboard.time_pressed[key]) < 50) {
    return;
  }

  g_Keyboard.states[key] = PRESSED;
  g_Keyboard.time_pressed[key] = HAL_GetTick();

  if (g_Keyboard.callback) {
    KeyEvent e;
    e.key = key;
    e.press_duration = HAL_GetTick();
    e.state = PRESSED;
    (g_Keyboard.callback)(e);
  }
}

void Keyboard_ReleaseKey(int key) {
  if (g_Keyboard.states[key] == RELEASED) {
    return;
  }

  g_Keyboard.states[key] = RELEASED;
  uint32_t press_time = g_Keyboard.time_pressed[key];
  g_Keyboard.time_pressed[key] = HAL_GetTick();

  if (g_Keyboard.callback) {
    KeyEvent e;
    e.key = key;
    e.press_duration = HAL_GetTick() - press_time;
    e.state = RELEASED;
    (g_Keyboard.callback)(e);
  }
}

/// Handles EXTI interupts for keyboard.
/// If EXTI not supported it returns negative values
int Keyboard_HandlePinEvent(uint16_t pin) {
  int col = _keyboard_pin_nmb_to_column(pin);
  if (col < 0) {
    return -1;
  }

  int status = _keyboard_check_column(col);

  if (status < 0) {  // It is means that button released
    _keyboard_column_released(col);
    return 0;
  }

  // Otherwise button pressed
  // In case of unstable signal we can find pressed button while unpressing
  Keyboard_PressKey(status);

  return status;
}

/// Checks what button is pressed on column
/// If yes - returns key id
/// In errors returns negative values
int _keyboard_check_column(int col) {
  if (col < 0 || col > 3) {
    return -1;
  }

  uint16_t col_pin = g_keyboard_column_pins[col];
  GPIO_TypeDef *gpio = g_keyboard_column_gpios[col];

  if (HAL_GPIO_ReadPin(gpio, col_pin) == GPIO_PIN_RESET) {
    /// No buttons pressed
    return -1;
  }

  int key_nmb = -1;

  for (int line = 0; line < KEY_LINES; line++) {
    GPIO_TypeDef *line_gpio = g_keyboard_lines_gpios[line];
    uint16_t line_pin = g_keyboard_lines_pins[line];

    HAL_GPIO_WritePin(line_gpio, line_pin, GPIO_PIN_RESET);
    /// Add delay for switching gpios?

    if (HAL_GPIO_ReadPin(gpio, col_pin) == GPIO_PIN_RESET) {
      key_nmb = col + KEY_COLUMNS * line;
      HAL_GPIO_WritePin(line_gpio, line_pin, GPIO_PIN_SET);
      __HAL_GPIO_EXTI_CLEAR_IT(col_pin);
      break;
    }

    HAL_GPIO_WritePin(line_gpio, line_pin, GPIO_PIN_SET);
  }

  if (key_nmb < 0) {  /// Unsupported case: 2 or more button pressed
    return -1;
  }

  return key_nmb;
}

/// release all buttons on column
void _keyboard_column_released(int col) {
  for (int line = 0; line < KEY_LINES; line++) {
    int key_nmb = col + KEY_COLUMNS * line;
    Keyboard_ReleaseKey(key_nmb);
  }
}
