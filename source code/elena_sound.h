#ifndef ELENA_SOUND_H
#define ELENA_SOUND_H

#include "pitches.h"

#define BUZZER_PIN PIN_PD7  

struct Sound {
  const int* notes;
  const int* durations;
  int length;
};

void playSound(const Sound& sound);

extern const int melodyNotes[];
extern const int melodyDurations[];

extern const int errorNotes[];
extern const int errorDurations[];

extern const int notifyNotes[];
extern const int notifyDurations[];

extern const int odeNotes[];
extern const int odeDurations[];

extern Sound bootSound;
extern Sound errorSound;
extern Sound notifySound;
extern Sound odeSound;

#endif