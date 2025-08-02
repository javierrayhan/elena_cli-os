
#include "elena_sound.h"

#include <Arduino.h>
#include "pitches.h"

#define BUZZER_PIN PIN_PD7  

// struct Sound {
//   const int* notes;
//   const int* durations;
//   int length;
// };

void playSound(const Sound& sound) {
  for (int i = 0; i < sound.length; i++) {
    int durationMs = 1000 / sound.durations[i];
    tone(BUZZER_PIN, sound.notes[i], durationMs);
    delay(durationMs * 1.3);
    noTone(BUZZER_PIN);
  }
}

const int melodyNotes[] = { NOTE_C4, NOTE_D4, NOTE_G4 };
const int melodyDurations[] = { 7, 7, 4 };

const int errorNotes[] = { NOTE_CS3, NOTE_CS3 };
const int errorDurations[] = { 10, 10 };

const int notifyNotes[] = { NOTE_E3, NOTE_A3 };
const int notifyDurations[] = { 9, 9 };

Sound bootSound = { melodyNotes, melodyDurations, 3 };
Sound errorSound = { errorNotes, errorDurations, 2 };
Sound notifySound = { notifyNotes, notifyDurations, 2 };
