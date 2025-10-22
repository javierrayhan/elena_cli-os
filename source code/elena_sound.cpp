#include "elena_sound.h"
#include <Arduino.h>
#include "pitches.h"

#define BUZZER_PIN PIN_PD7  

void playSound(const Sound& sound) {
  for (int i = 0; i < sound.length; i++) {
    int note = pgm_read_word(&(sound.notes[i]));      
    int dur  = pgm_read_word(&(sound.durations[i]));  

    int durationMs = 1000 / dur;
    tone(BUZZER_PIN, note, durationMs);
    delay(durationMs * 1.3);
    noTone(BUZZER_PIN);
  }
}

// === Array masuk PROGMEM ===
const int melodyNotes[] PROGMEM    = { NOTE_C4, NOTE_D4, NOTE_G4 };
const int melodyDurations[] PROGMEM = { 7, 7, 4 };

const int errorNotes[] PROGMEM     = { NOTE_CS3, NOTE_CS3 };
const int errorDurations[] PROGMEM  = { 10, 10 };

const int notifyNotes[] PROGMEM     = { NOTE_E3, NOTE_A3 };
const int notifyDurations[] PROGMEM  = { 9, 9 };

const int odeNotes[] PROGMEM = {
  NOTE_E4, NOTE_E4, NOTE_F4, NOTE_G4,
  NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4,
  NOTE_E4, NOTE_D4, NOTE_D4,

  NOTE_E4, NOTE_E4, NOTE_F4, NOTE_G4,
  NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4,
  NOTE_D4, NOTE_C4, NOTE_C4,

  NOTE_D4, NOTE_D4, NOTE_E4, NOTE_C4,
  NOTE_D4, NOTE_E4, NOTE_F4, NOTE_E4, NOTE_C4,
  NOTE_D4, NOTE_E4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_D4, NOTE_G3,

  NOTE_E4, NOTE_E4, NOTE_F4, NOTE_G4,
  NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4,
  NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4,
  NOTE_D4, NOTE_C4, NOTE_C4
};

const int odeDurations[] PROGMEM = {
  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 2,

  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 2,

  4, 4, 4, 4,
  4, 8, 8, 4, 4,
  4, 8, 8, 4, 4,
  4, 4, 2,

  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 2
};

// === Sound structs ===
Sound bootSound   = { melodyNotes,  melodyDurations,  3 };
Sound errorSound  = { errorNotes,   errorDurations,   2 };
Sound notifySound = { notifyNotes,  notifyDurations,  2 };
Sound odeSound    = { odeNotes,     odeDurations,     sizeof(odeNotes) / sizeof(odeNotes[0]) };
