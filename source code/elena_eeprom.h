#ifndef ELENA_EEPROM_H
#define ELENA_EEPROM_H

#include <EEPROM.h>

#define MAX_USER_LEN 16
#define MAX_PASS_LEN 16
#define EEPROM_USER_ADDR 0
#define EEPROM_PASS_ADDR (EEPROM_USER_ADDR + MAX_USER_LEN)

extern char username_now[MAX_USER_LEN + 1];
extern char password_now[MAX_PASS_LEN + 1]; 

void saveUserToEEPROM(const char* user, const char* pass);
void loadUserFromEEPROM(char* user, char* pass);
void printCurrentUser();
bool isUserSet();
void eeprom_write(uint16_t address, uint8_t data);
void eeprom_bool_write(uint16_t address, bool data);
uint8_t eeprom_read(uint16_t address);
bool eeprom_bool_read(uint16_t address);

#endif
