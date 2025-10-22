#include "elena_user.h"
#include <Arduino.h>
#include "elena_eeprom.h"

bool loggedIn = false;
char usernow[MAX_USER_LEN + 1];   

void readLine(char* buffer, int maxLen) {
  int i = 0;
  while (i < maxLen - 1) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') break;
      buffer[i++] = c;
    }
  }
  buffer[i] = '\0';
}

bool loginUser() {
  char storedUser[MAX_USER_LEN + 1];
  char storedPass[MAX_PASS_LEN + 1];
  loadUserFromEEPROM(storedUser, storedPass);

  char inputUser[MAX_USER_LEN + 1];
  char inputPass[MAX_PASS_LEN + 1];

  Serial.println(F("[!] Login required."));
  Serial.print(F("Username: "));
  readLine(inputUser, MAX_USER_LEN);

  Serial.print(F("Password: "));
  readLine(inputPass, MAX_PASS_LEN);

  if (strcmp(inputUser, storedUser) == 0 && strcmp(inputPass, storedPass) == 0) {
    Serial.println(F("[S] Login successful. Welcome back!"));
    strncpy(usernow, storedUser, MAX_USER_LEN); 
    usernow[MAX_USER_LEN] = '\0';
    loggedIn = true;
    return true;
  } else {
    Serial.println(F("[!] Invalid username or password."));
    loggedIn = false;
    return false;
  }
}

void registerUser() {
  char username[MAX_USER_LEN + 1];
  char password[MAX_PASS_LEN + 1];

  Serial.println(F("\nCreate Username: "));
  readLine(username, MAX_USER_LEN);
  
  Serial.println(F("Create Password: "));
  readLine(password, MAX_PASS_LEN);

  saveUserToEEPROM(username, password);
  Serial.println(F("[S] User registered successfully."));
}

bool elenaUserStartup() {
  if (!isUserSet()) {
    Serial.println(F("[i] No user found. Please register."));
    registerUser();
    return true; 
  } else {
    Serial.print(F("[?] Do you want to "));
    Serial.println(loggedIn ? F("re-login? (y/n)") : F("login? (y/n)"));

    char input[10];
    readLine(input, sizeof(input));
    for (uint8_t i = 0; input[i]; i++) input[i] = tolower(input[i]);

    if (strcmp(input, "n") == 0) {
      Serial.println(F("[X] Skipping login..."));
      return false;
    } 
    if (strcmp(input, "y") == 0) {
      while (!loginUser()) {
        Serial.println(F("[i] Please try again.\n"));
        Serial.println(F("[?] Retry login? (y/n)"));
        readLine(input, sizeof(input));

        if (strcmp(input, "n") == 0) {
          Serial.println(F("[X] Login aborted."));
          return false;
        }
      }
      return true;
    }
    Serial.println(F("[X] Login aborted."));
    return false;
  }
}
