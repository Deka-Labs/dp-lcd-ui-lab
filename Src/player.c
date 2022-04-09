#include "player.h"

#include <stddef.h>
#include <stm32f3xx_hal.h>

#define BUZZER_CHANNEL TIM_CHANNEL_2
#define UPDATE_CHANNEL TIM_CHANNEL_1
#define ABS(x) (x < 0 ? -x : x)

/// Externals
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

//// UTILS /////
uint32_t player_divide(uint32_t left, uint32_t right) {
  uint32_t a = left / right;
  uint32_t r = left % right;
  if (r >= (right / 2)) {
    a++;
  }
  return a;
}

void player_SetTimerFreq(TIM_HandleTypeDef *TIM, uint32_t freq) {
  /// Assume that all TIMers configured for 72MHz
  uint32_t tim_input_freq = 72 * 1000000;
  uint16_t start_arr = 49;

  uint16_t best_prescaler = 32768;
  uint16_t best_arr = start_arr;
  uint32_t max_freq_error =
      ABS(player_divide(tim_input_freq, (1 + best_prescaler) * (1 + best_arr)) -
          freq);

  /// Binary search

  for (uint16_t arr = start_arr; arr < 65535; arr += 1000) {
    uint16_t prescaler = 32768;
    uint32_t current_step = 32768 / 2;
    while (current_step > 0) {
      uint32_t real_freq =
          player_divide(tim_input_freq, (1 + prescaler) * (1 + arr));

      uint32_t freq_err = ABS(real_freq - freq);
      if (freq_err < max_freq_error) {
        max_freq_error = freq_err;
        best_prescaler = prescaler;
        best_arr = arr;
      }

      if (real_freq == freq) {
        /// We found required parameters; apply and exit
        __HAL_TIM_SET_AUTORELOAD(TIM, arr);
        __HAL_TIM_SET_PRESCALER(TIM, prescaler);
        return;
      }

      if (real_freq < freq) {
        prescaler -= current_step;
      } else {
        prescaler += current_step;
      }
      current_step /= 2;
    }

    if (prescaler <= 1) {
      // A target frequency is too high for current arr, select with minimal
      // error
      __HAL_TIM_SET_AUTORELOAD(TIM, best_arr);
      __HAL_TIM_SET_PRESCALER(TIM, best_prescaler);
      return;
    }
  }
  // If we are here. We cannot achive required frequency set
  // Set a Max values
  __HAL_TIM_SET_AUTORELOAD(TIM, 65535);
  __HAL_TIM_SET_PRESCALER(TIM, 65535);
}
////////////////

//// Member calls ////
void Player_Init(Player *p) {
  if (p && p->init) {
    (p->init)(p);
  }
}

void Player_Update(Player *p) {
  if (p && p->update) {
    (p->update)(p);
  }
}

void Player_Destroy(Player *p) {
  if (p && p->destroy) {
    (p->destroy)(p);
  }
}
//////////////////////

//// No Player ////
Player MakeNoPlayer() {
  Player out = {0};
  out.init = NULL;
  out.update = NULL;
  out.destroy = NULL;
  return out;
}
///////////////////

//// Note Player ////

/**
 * Init Note player's stuff
 *  - Init TIM3 for using freq as specified in p.data
 */
void NotePlayer_Init(Player *p) {
  player_SetTimerFreq(&htim3, p->data.note);
  __HAL_TIM_SET_COMPARE(&htim3, BUZZER_CHANNEL,
                        __HAL_TIM_GET_AUTORELOAD(&htim3) / 2);

  HAL_TIM_PWM_Start(&htim3, BUZZER_CHANNEL);
}

void NotePlayer_Destroy(Player *p) { HAL_TIM_PWM_Stop(&htim3, BUZZER_CHANNEL); }

Player MakeNotePlayer(NoteType note_freq) {
  Player out = {0};
  out.init = NotePlayer_Init;
  out.update = NULL;
  out.destroy = NotePlayer_Destroy;
  out.data.note = note_freq;
  return out;
}
/////////////////////

//// Music player ////

/**
 * Init music player
 *  - Start TIM4 for generating interrupts to switch next notes
 *  - Start TIM3 for playing current note
 */
void MusicPlayer_Init(Player *p) {
  if (p->data.music.notes_len == 0) {
    return;
  }

  Player_Update(p);
}

void MusicPlayer_Update(Player *p) {
  if (p->data.music.notes_len == 0) {
    Player_Destroy(p);  // Self destruct
    return;
  }

  if (p->data.music.countdown > 0) {
    p->data.music.countdown--;
    return;
  }

  NoteType note = *(p->data.music.notes);
  uint16_t time = *(p->data.music.times);
  // Convert time to freq
  p->data.music.notes++;
  p->data.music.times++;
  p->data.music.notes_len--;
  p->data.music.countdown = time;

  if (note == 0) {
    if (p->data.music.pwm_started) {
      HAL_TIM_PWM_Stop(&htim3, BUZZER_CHANNEL);
      p->data.music.pwm_started = 0;
    }
  } else {
    player_SetTimerFreq(&htim3, note);
    __HAL_TIM_SET_COMPARE(&htim3, BUZZER_CHANNEL,
                          __HAL_TIM_GET_AUTORELOAD(&htim3) / 2);

    if (!p->data.music.pwm_started) {
      HAL_TIM_PWM_Start(&htim3, BUZZER_CHANNEL);
      p->data.music.pwm_started = 1;
    }
  }
}

void MusicPlayer_Destroy(Player *p) {
  HAL_TIM_PWM_Stop(&htim3, BUZZER_CHANNEL);
}

Player MakeMusicPlayer(const NoteType *notes, const uint16_t *times,
                       uint32_t notes_len) {
  Player out = {0};
  out.init = MusicPlayer_Init;
  out.update = MusicPlayer_Update;
  out.destroy = MusicPlayer_Destroy;
  out.data.music.notes = notes;
  out.data.music.times = times;
  out.data.music.notes_len = notes_len;
  out.data.music.pwm_started = 0;
  out.data.music.countdown = 0;
  return out;
}

//////////////////////
