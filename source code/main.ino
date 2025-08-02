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
  String inputs = readSerialLine();
  Serial.print(inputs); Serial.println("");
  delay(100);
  handleCommand(inputs);
}

void printPrompt() {
  if (loggedIn) {
    Serial.print(usernow);
  } else {
    Serial.print(F("guest"));
  }
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
Serial.println(F("                 ELENA CLI OS v1.5                  "));
Serial.println(F("                Build: 12 June 2025                 "));
Serial.println(F("               by Externalize System                "));
Serial.println(F("                                                    "));
Serial.println(F("==================================================  "));
Serial.println(F("                                                    "));
Serial.println(F("[i] Type 'help' to see available commands.          "));
Serial.println(F("                                                    "));

}


