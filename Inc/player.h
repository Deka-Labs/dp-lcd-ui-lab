#pragma once

#include <stdint.h>

typedef uint32_t NoteType;           /// Type for note frequency
typedef struct Player Player;        /// Main type for sounds player
typedef struct MusicInfo MusicInfo;  /// Information about music

struct MusicInfo {
  const NoteType *notes;
  const uint16_t *times;
  uint32_t notes_len;

  uint8_t pwm_started;
  uint16_t countdown;
};

typedef union PlayerInternalData {
  MusicInfo music;
  NoteType note;
} PlayerInternalData;

struct Player {
  PlayerInternalData data;

  void (*init)(Player *);
  void (*update)(Player *);
  void (*destroy)(Player *);
  int (*ended)(Player *);
};

Player MakeNoPlayer();

/// Creates Music player
Player MakeMusicPlayer(const NoteType *notes, const uint16_t *times,
                       uint32_t notes_len);

void Player_Init(Player *p);
void Player_Update(Player *p);
void Player_Destroy(Player *p);
int Player_Ended(Player* p);
