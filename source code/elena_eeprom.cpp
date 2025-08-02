#include "elena_eeprom.h"

#include <Arduino.h>
#include <EEPROM.h>

#define MAX_USER_LEN 16
#define MAX_PASS_LEN 16
#define EEPROM_USER_ADDR 0
#define EEPROM_PASS_ADDR (EEPROM_USER_ADDR + MAX_USER_LEN)

char username_now[MAX_USER_LEN + 1];
char password_now[MAX_PASS_LEN + 1]; 

void saveUserToEEPROM(const char* user, const char* pass) {
  for (uint8_t i = 0; i < MAX_USER_LEN; i++) {
    EEPROM.write(EEPROM_USER_ADDR + i, user[i]);
  }
  for (uint8_t i = 0; i < MAX_PASS_LEN; i++) {
    EEPROM.write(EEPROM_PASS_ADDR + i, pass[i]);
  }
}

void loadUserFromEEPROM(char* user, char* pass) {
  for (uint8_t i = 0; i < MAX_USER_LEN; i++) {
    user[i] = EEPROM.read(EEPROM_USER_ADDR + i);
  }
  user[MAX_USER_LEN] = '\0';

  for (uint8_t i = 0; i < MAX_PASS_LEN; i++) {
    pass[i] = EEPROM.read(EEPROM_PASS_ADDR + i);
  }
  pass[MAX_PASS_LEN] = '\0';
}

void printCurrentUser() {
  loadUserFromEEPROM(username_now, password_now);
  Serial.printf("[U] Logged in as: %s", username_now);
  Serial.println(F(""));
}

bool isUserSet() {
  byte first = EEPROM.read(EEPROM_USER_ADDR);
  return first != 0xFF && first != '\0';
}

void eeprom_write(uint16_t address, uint8_t data) {
  EEPROM.write(address, data); 
}

void eeprom_bool_write(uint16_t address, bool data) {
  EEPROM.write(address, data); 
}

uint8_t eeprom_read(uint16_t address) {
  return EEPROM.read(address);
}

bool eeprom_bool_read(uint16_t address) {
  return EEPROM.read(address);
}

