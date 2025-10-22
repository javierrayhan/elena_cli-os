#include <avr/pgmspace.h>
#include "elena_function.h"

void setup() {
  wdt_disable();
  Wire.begin();
  Serial.begin(19200);
  delay(500);

  showIntro();
  sound(bootSound);
}

void loop() {
  printPrompt();

  char inputBuffer[64];
  if (readSerialLine(inputBuffer, sizeof(inputBuffer))) {
    Serial.println(inputBuffer);   
    handleCommand(inputBuffer);    
  }

  delay(10);
}

void printPrompt() {
  Serial.print(loggedIn ? String(usernow) : String(F("guest")));
  Serial.print(F("> "));
}

void showIntro() {
  Serial.println(F("                                                    "));
  Serial.println(F("==================================================  "));
  Serial.println(F("                                                    "));
  Serial.println(F(" /$$$$$$$$ /$$       /$$$$$$$$ /$$   /$$  /$$$$$$   "));
  Serial.println(F("| $$_____/| $$      | $$_____/| $$$ | $$ /$$__  $$  "));
  Serial.println(F("| $$      | $$      | $$      | $$$$| $$| $$  | $$  "));
  Serial.println(F("| $$$$$   | $$      | $$$$$   | $$ $$ $$| $$$$$$$$  "));
  Serial.println(F("| $$__/   | $$      | $$__/   | $$  $$$$| $$__  $$  "));
  Serial.println(F("| $$      | $$      | $$      | $$   $$$| $$  | $$  "));
  Serial.println(F("| $$$$$$$$| $$$$$$$$| $$$$$$$$| $$    $$| $$  | $$  "));
  Serial.println(F("|________/|________/|________/|__/  /__/|__/  |__/  "));
  Serial.println(F("                                                    "));
  Serial.println(F("                 ELENA CLI OS v1.6                  "));
  Serial.println(F("                Build: 22 Sept 2025                 "));
  Serial.println(F("                by TwinWorks System                 "));
  Serial.println(F("                                                    "));
  Serial.println(F("==================================================  "));
  Serial.println(F("                                                    "));
  Serial.println(F("[i] Type 'help' to see available commands.          "));
  Serial.println(F("                                                    "));
}


