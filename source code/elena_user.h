#ifndef ELENA_USER_H
#define ELENA_USER_H

#include <Arduino.h>
#include "elena_eeprom.h"

extern bool loggedIn;
extern char usernow[MAX_USER_LEN + 1];

void readLine(char* buffer, int maxLen);
bool loginUser();
void registerUser();
bool elenaUserStartup();

#endif
