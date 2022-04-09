#pragma once

#include <stdint.h>

typedef enum {
  RELEASED = 0,
  PRESSED,
} KeyState;

struct KeyEvent {
  int key;                 /// Key ID
  uint32_t press_duration; /// how long is key pressed
  KeyState state;          /// State
};

typedef struct KeyEvent KeyEvent;

/// Function type for handling key events from keyboard sybsystem
typedef void (*KeyboardCallback)(KeyEvent);

/// Initialization of keyboard subsystem.
/// - Checks button states on init
/// - Disable callback for events
void Keyboard_Init();
/// Set output callback for key events
void Keyboard_SetCallback(KeyboardCallback callback);
/// Emulate button press for key
void Keyboard_PressKey(int key);
/// Emulate button release for key
void Keyboard_ReleaseKey(int key);

/// Handles EXTI interupts for keyboard.
/// If EXTI not supported it returns -1 otherwise - key number
int Keyboard_HandlePinEvent(uint16_t pin);
