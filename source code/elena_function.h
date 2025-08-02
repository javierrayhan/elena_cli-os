#ifndef ELENA_FUNCTION_H
#define ELENA_FUNCTION_H

#include <stdlib.h>
#include <Wire.h>
#include <math.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "elena_sound.h"
#include "elena_user.h"
#include "e_time.h"

virtuabotixRTC myRTC(PIN_PD2, PIN_PD3, PIN_PD4); // Sesuai wiring: CLK, DAT, RST
RealTimeManager timeManager(&myRTC);

bool soundEN;

// ===== Pharsing String ====== 
// String getSubstring(String str, int start, int len) {
//   if (start < 0 || len <= 0 || start >= str.length()) return "";
//   int end = start + len;
//   if (end > str.length()) end = str.length();
//   return str.substring(start, end);
// }

// ===== Serial Input Function ======
String readSerialLine() {
  String input = "";
  while (true) {
    if (Serial.available()) {
      char ch = Serial.read();
      if (ch == '\n' || ch == '\r') {
        if (input.length() > 0) break; // End input on Enter
      } else {
        input += ch;
      }
    }
  }
  return input;
}

// ===== Sub Functional ===== 
void sound(const Sound& soundToPlay) {
  bool soundEN = eeprom_bool_read(33);
  if (soundEN) {
    playSound(soundToPlay);
  } else if (!soundEN){

  } else {
    playSound(soundToPlay);
  }
}


extern int __heap_start, *__brkval;
int freeMemory() {
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

int getUsedEEPROM() {
  int used = 0;
  for (uint16_t i = 0; i <= E2END; i++) {
    if (eeprom_read_byte((uint8_t*)i) != 0xFF) used++;
  }
  return used;
}

void softwareReset() {
  wdt_enable(WDTO_15MS); 
  while (1);             
}

float evaluateExpression(const char* expr) {
  float result = 0;
  float current = 0;
  char op = '+';  
  bool error = false;

  while (*expr) {
    while (*expr == ' ') expr++;  

    float value = 0;
    char* endPtr;

    if (strncmp(expr, "sqrt", 4) == 0) {
      expr += 4; 
      value = strtod(expr, &endPtr);
      if (value < 0) {
        Serial.println(F("[!] sqrt() of negative number!"));
        playSound(errorSound);
        return NAN;
      }
      value = sqrt(value);
    } else {
      value = strtod(expr, &endPtr);
    }

    expr = endPtr;

    switch (op) {
      case '+': result += value; break;
      case '-': result -= value; break;
      case '*': result *= value; break;
      case '/':
        if (value == 0) {
          Serial.println(F("[X] Divide by zero!"));
          playSound(errorSound );
          return NAN;
        }
        result /= value;
        break;
      case '^':
        result = pow(result, value);
        break;
      default:
        Serial.print(F("[X] Invalid operator: "));
        Serial.println(op);
        playSound(errorSound);
        return NAN;
    }

    while (*expr == ' ') expr++;

    op = *expr;
    if (*expr) expr++;
  }

  return result;
}

void printAllPinStatus() {
  for (char portChar = 'A'; portChar <= 'D'; portChar++) {
    volatile uint8_t* pin;

    switch (portChar) {
      case 'A': pin = &PINA; break;
      case 'B': pin = &PINB; break;
      case 'C': pin = &PINC; break;
      case 'D': pin = &PIND; break;
      default: continue;
    }

    Serial.println(F(""));

    for (int pinNum = 0; pinNum < 8; pinNum++) {
      if (portChar == 'D' && (pinNum == 0 || pinNum == 1)) continue;

      bool state = (*pin & (1 << pinNum));
      Serial.print(F("[GPIO] P"));
      Serial.print(portChar);
      Serial.print(pinNum);
      Serial.print(F(": "));
      Serial.println(state ? "HIGH [1]" : "LOW  [0]");
    }
  }
}

String formatUptime(unsigned long ms) {
  unsigned long seconds = ms / 1000;
  unsigned int minutes = seconds / 60;
  unsigned int hours = minutes / 60;
  seconds %= 60;
  minutes %= 60;

  char buffer[20];
  sprintf(buffer, "%02u:%02u:%02u", hours, minutes, seconds);
  return String(buffer);
}

void scanI2CDevices() {
  byte error, address;
  uint8_t found = 0;
  uint8_t err = 0;

  Serial.println(F("[i] Scanning I2C bus..."));

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print(F("[I2C] Found at 0x"));
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      found++;
    } else if (error == 4) {
      Serial.print(F("[!] Unknown error at 0x"));
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      err++;
    }
  }

  Serial.println();
  Serial.print(F("[S] Found: "));
  Serial.print(found);
  Serial.print(F(" device(s), "));
  Serial.print(F("Err: "));
  Serial.print(err);
  Serial.println(F(" address(es) failed."));
  Serial.println(F("[i] Scan complete."));
  sound(notifySound);
}

void interactiveSetTime() {
  char input[32];
  int dd, mm, yyyy, hh, min, ss;

  Serial.println(F("[MANUAL TIME SET MODE]"));

  // Ambil DAY
  while (true) {
    Serial.println(F("DAY: "));
    readLine(input, sizeof(input));
    if (sscanf(input, "%d", &dd) == 1 && dd >= 1 && dd <= 31) break;
    Serial.println(F("[!] Invalid day. Masukkan antara 1–31."));
  }

  // Ambil MONTH
  while (true) {
    Serial.println(F("MONTH: "));
    readLine(input, sizeof(input));
    if (sscanf(input, "%d", &mm) == 1 && mm >= 1 && mm <= 12) break;
    Serial.println(F("[!] Invalid month. 1–12."));
  }

  // Ambil YEAR
  while (true) {
    Serial.println(F("YEAR: "));
    readLine(input, sizeof(input));
    if (sscanf(input, "%d", &yyyy) == 1 && yyyy >= 2000 && yyyy <= 2099) break;
    Serial.println(F("[!] Invalid year. 2000–2099."));
  }

  // Ambil JAM
  while (true) {
    Serial.println(F("HH: "));
    readLine(input, sizeof(input));
    if (sscanf(input, "%d", &hh) == 1 && hh >= 0 && hh <= 23) break;
    Serial.println(F("[!] Invalid hour. 0–23."));
  }

  // Ambil MENIT
  while (true) {
    Serial.println(F("MM: "));
    readLine(input, sizeof(input));
    if (sscanf(input, "%d", &min) == 1 && min >= 0 && min <= 59) break;
    Serial.println(F("[!] Invalid minute. 0–59."));
  }

  // Ambil DETIK
  while (true) {
    Serial.println(F("SS: "));
    readLine(input, sizeof(input));
    if (sscanf(input, "%d", &ss) == 1 && ss >= 0 && ss <= 59) break;
    Serial.println(F("[!] Invalid second. 0–59."));
  }

  // Eksekusi set waktu
  timeManager.setTimeManual(dd, mm, yyyy, hh, min, ss);
  Serial.println(F("[S] Time set successful!"));
}

void handlePrintCommand(const char* input) {
  char message[64];
  int count = 1;

  if (sscanf(input, "print \"%63[^\"]\" for %d", message, &count) == 2) {
    unsigned long startTime = millis(); 
    for (int i = 0; i < count; i++) {
      Serial.println(message);
    }
    unsigned long endTime = millis();  
    float totalTime = (endTime - startTime) / 1000.0;

    Serial.print(F("[i] Finished in "));
    Serial.print(totalTime, 3);
    Serial.println(F(" seconds."));
  } else if (sscanf(input, "print \"%63[^\"]\"", message) == 1) {
    Serial.println(message);
  } else {
    Serial.println(F("[!] Invalid format. Use: print \"text\" or print \"text\" for N"));
    sound(errorSound);
  }
}

void delayFunc(const char* input) {
  unsigned long delayMs = 0;

  // Coba format "delay <angka>"  (ada spasi)
  if (sscanf(input, "delay %lu", &delayMs) == 1 ||
      sscanf(input, "delay%lu",  &delayMs) == 1) {
    delay(delayMs);                     // Arduino delay = milidetik
  } else {
    Serial.println(F("[!] Invalid syntax. Try: delay 500"));
  }
}


void showHelp() {
  Serial.println(F("[i] Available Commands:"));
  Serial.println(F(" ┌────────────────────┐"));
  Serial.println(F(" │ help         → Show all commands"));
  Serial.println(F(" │ login        → Login with username"));
  Serial.println(F(" │ logout       → Logout from acount"));
  Serial.println(F(" │ stat         → Show sys&hw status"));
  Serial.println(F(" │ rns acc      → Remove & set new account"));
  Serial.println(F(" │ reboot       → Reboot the system"));
  Serial.println(F(" │ whoami       → Display logged in user"));
  Serial.println(F(" │ time         → Display current time"));
  Serial.println(F(" │ set time     → Setting time"));
  Serial.println(F(" │ sound on/off → Setting sound"));
  Serial.println(F(" │ calc         → Calculator"));
  Serial.println(F(" │ print        → Print something"));
  Serial.println(F(" │ delay        → Delay"));
  Serial.println(F(" │ gpio s/w/r   → GPIO function")); 
  Serial.println(F(" │ i2c scan     → Scan all i2c devices"));
  Serial.println(F(" └────────────────────┘"));
}

void showDangerousHelp() {
  Serial.println(F("\n[i] Available Dangerous Commands:"));
  Serial.println(F(" ┌────────────────────┐"));
  Serial.println(F(" │ eeprom wipe  → Wipe all internal EEPROM data "));
  Serial.println(F(" └────────────────────┘"));
}

void printHelp() {
  Serial.println(F("[ Print Command Help ]"));
  Serial.println(F("Format:"));
  Serial.println(F("  print \"text\""));
  Serial.println(F("  print \"text\" for N"));
  Serial.println(F("Example:"));
  Serial.println(F("  print \"hello\"        →  1x hello"));
  Serial.println(F("  print \"hello\" for 5  →  5x hello"));
}

void gpioWriteHelp(){

}

void gpioReadeHelp(){
  
}

// ===== Command Functions =====
void cmd_time(){
  // Serial.println(F("ELENA> Aww, i'm sorry, this feature is under development :( "));
  Serial.print(F("DATE: ")); timeManager.showDate();
  Serial.print(F("TIME: ")); timeManager.showTime();
  sound(notifySound);
}

void cmd_set_time() {
  interactiveSetTime();
  Serial.println(F("ELENA> Aww, i'm sorry, this feature is under development :("));
  sound(notifySound);
}

void cmd_calc() {
  char input[32];
  Serial.println(F("Calculator Mode. Type a math expression (e.g. 8 + 2 * sqrt25 - 4 / 2 ^ 2) or 'exit'."));

  while (true) {
    Serial.print(F("calc>> "));
    readLine(input, sizeof(input));

    if (strcmp(input, "exit") == 0) {
      Serial.println(F("[X] Exit from Calculator Mode."));
      break;
    }

    // Eksekusi ekspresi
    float result = evaluateExpression(input);
    Serial.print(F("Result: "));
    Serial.println(result, 6); 
  }
}

void cmd_stat(bool logged) {
  Serial.println(F("\n--------- System Status ---------"));

  Serial.print(F("[T] ")); timeManager.showDate();
  Serial.print(F("[T] ")); timeManager.showTime();
  // 🧑‍💻 Login status
  if (logged) {
    printCurrentUser();
  } else {
    Serial.println(F("[U] Not logged in"));
  }

  // 🧠 MCU info
    Serial.print(F("[M] MCU: "));
  #if defined(__AVR_ATmega32__)
    Serial.println(F("ATmega32"));
  #else
    Serial.println(F("Unknown MCU"));
  #endif

  // ⚙️ CPU Freq
  Serial.print(F("[C] CPU Freq: "));
  Serial.print(F_CPU / 1000000);
  Serial.println(F(" MHz"));

  // 📦 EEPROM usage
  int usedEEPROM = getUsedEEPROM();
  int totalEEPROM = E2END + 1;
  Serial.print(F("[E] EEPROM Used: "));
  Serial.print(usedEEPROM);
  Serial.print(F(" / "));
  Serial.print(totalEEPROM);
  Serial.println(F(" Bytes"));

  // 💾 RAM usage
  int free = freeMemory();
  int totalRAM = RAMEND - 0x60 + 1;
  int used = totalRAM - free;
  Serial.print(F("[R] RAM Used: "));
  Serial.print(used);
  Serial.print(F(" / "));
  Serial.print(totalRAM);
  Serial.println(F(" Bytes"));

  // 🕓 Uptime
  Serial.print(F("[T] Uptime: "));
  Serial.println(formatUptime(millis()));

  Serial.println(F("---------------------------------"));
}

void cmd_resetandset() {
  if (!loggedIn) {
    Serial.println(F("[!] You must login to reset your account!"));
    sound(notifySound);
    if (elenaUserStartup()) {
      registerUser();  
    } else {
      Serial.println(F("[X] Register cancelled because login failed."));
      sound(errorSound);
    }
  } else {
    registerUser(); 
    Serial.println(F("[S] Reset account success, you can relog to your new account!"));
    sound(notifySound);
    loggedIn = false;
  }
}

void logout(){
  if (!loggedIn){
    Serial.println("[!] You already logging out! Execute failed");
    sound(errorSound);
  } else if (loggedIn){
    loggedIn = false;
    Serial.println("[S] Logging out success!");
    sound(notifySound);
  }
}

void cmd_reboot() {
  softwareReset();
}

void cmd_whoami() {
  if (loggedIn){
    printCurrentUser();
  } else {
    Serial.println(F("[i] Logged in as: guest"));
  }
}

void cmd_gpioWrite(const char* input) {
  char action[6]; // bisa "write" atau "w"
  char stateAll;

  // ======= Handle: gpio w all h/l =======
  if (sscanf(input, "gpio %5s all %c", action, &stateAll) == 2) {
    if (strcmp(action, "write") == 0 || strcmp(action, "w") == 0) {
      if (tolower(stateAll) != 'h' && tolower(stateAll) != 'l') {
        Serial.println(F("[!] Status invalid. Use 'h' or 'l' for gpio all."));
        sound(errorSound);
        return;
      }

      for (char portChar = 'A'; portChar <= 'D'; portChar++) {
        volatile uint8_t *ddr, *port;
        switch (portChar) {
          case 'A': ddr = &DDRA; port = &PORTA; break;
          case 'B': ddr = &DDRB; port = &PORTB; break;
          case 'C': ddr = &DDRC; port = &PORTC; break;
          case 'D': ddr = &DDRD; port = &PORTD; break;
        }

        for (uint8_t pinNum = 0; pinNum < 8; pinNum++) {
          if (portChar == 'D' && (pinNum == 0 || pinNum == 1)) continue;
          *ddr |= (1 << pinNum);
          if (tolower(stateAll) == 'h') *port |= (1 << pinNum);
          else *port &= ~(1 << pinNum);
        }
      }

      Serial.print(F("[S] All GPIO set to "));
      Serial.println(tolower(stateAll) == 'h' ? "HIGH." : "LOW (except PD0 & PD1).");
      sound(notifySound);
      return;
    }
  }

  // ======= Handle: gpio w pa1 h =======
  char portStr[3];
  uint8_t pinNum;
  char stateChar;

  if (sscanf(input, "gpio %5s %2s%d %c", action, portStr, &pinNum, &stateChar) == 4) {
    if (strcmp(action, "write") != 0 && strcmp(action, "w") != 0) {
      Serial.println(F("[!] Invalid command. Use 'gpio w' or 'gpio write'."));
      sound(errorSound);
      return;
    }

    if (tolower(portStr[0]) != 'p' || strlen(portStr) != 2) {
      Serial.println(F("[!] Invalid port format. Use 'pa', 'pb', etc."));
      sound(errorSound);
      return;
    }

    char portChar = toupper(portStr[1]);
    volatile uint8_t *ddr = nullptr;
    volatile uint8_t *port = nullptr;

    switch (portChar) {
      case 'A': ddr = &DDRA; port = &PORTA; break;
      case 'B': ddr = &DDRB; port = &PORTB; break;
      case 'C': ddr = &DDRC; port = &PORTC; break;
      case 'D': ddr = &DDRD; port = &PORTD; break;
      default:
        Serial.println(F("[!] Port invalid. Use PA, PB, PC, or PD."));
        sound(errorSound);
        return;
    }

    if (pinNum > 7) {
      Serial.println(F("[!] Pin number must be 0-7."));
      sound(errorSound);
      return;
    }

    if (portChar == 'D' && (pinNum == 0 || pinNum == 1)) {
      Serial.println(F("[!] Can't modify PD0 or PD1 (used for Serial)."));
      sound(notifySound);
      return;
    }

    *ddr |= (1 << pinNum);

    if (tolower(stateChar) == 'h') {
      *port |= (1 << pinNum);
      Serial.print(F("[S] GPIO P")); Serial.print(portChar); Serial.print(pinNum); Serial.println(F(" set HIGH."));
    } else if (tolower(stateChar) == 'l') {
      *port &= ~(1 << pinNum);
      Serial.print(F("[S] GPIO P")); Serial.print(portChar); Serial.print(pinNum); Serial.println(F(" set LOW."));
    } else {
      Serial.println(F("[!] Status invalid. Use 'h' or 'l'."));
      sound(errorSound);
    }
    return;
  }

  // Kalau format gak cocok sama dua di atas
  Serial.println(F("[!] Invalid format. Use 'gpio w all h' or 'gpio w pa1 l'."));
  sound(errorSound);
}

void cmd_gpioRead(const char* input) {
  // Set semua pin jadi input
  DDRA = 0x00;
  DDRB = 0x00;
  DDRC = 0x00;
  DDRD = 0x00;

  char portChar;
  uint8_t pinNum;

  // Contoh input valid: gpio r pa0
  if (sscanf(input, "gpio r p%c%d", &portChar, &pinNum) == 2) {
    volatile uint8_t* pin;

    switch (toupper(portChar)) {
      case 'A': pin = &PINA; break;
      case 'B': pin = &PINB; break;
      case 'C': pin = &PINC; break;
      case 'D': pin = &PIND; break;
      default:
        Serial.println(F("[!] Port invalid. Use PA, PB, PC, or PD."));
        sound(errorSound);
        return;
    }

    if (pinNum > 7) {
      Serial.println(F("[!] Pin number must be 0-7."));
      sound(errorSound);
      return;
    }

    bool state = (*pin & (1 << pinNum));
    Serial.print(F("[i] GPIO P"));
    Serial.print((char)toupper(portChar));
    Serial.print(pinNum);
    Serial.print(F(" is "));
    Serial.println(state ? "HIGH [1]" : "LOW  [0]");
    sound(notifySound);
    return;
  }

  Serial.println(F("[!] Format invalid. Try: 'gpio r pa0'"));
  sound(errorSound);
}

// ===== Dangerous Command =====
void cmd_eeprom_wipe() {
  Serial.println(F("[!] WARNING: This will erase ALL INTERNAL EEPROM data!"));
  sound(notifySound);

  if (!loggedIn){
    Serial.println(F("[i] You need to login first! Type 'login' to login."));
    sound(errorSound);
  } else {
    Serial.println(F("Type 'CONFIRM' to proceed:"));

    char input[16];
    readLine(input, sizeof(input)); 

    if (strcmp(input, "CONFIRM") != 0) {
      Serial.println(F("[X] Cancelled. EEPROM not erased."));
      sound(errorSound);
      return;
    }

    Serial.println(F("[DC] Erasing EEPROM..."));

    for (int i = 0; i < 1024; i++) {
      EEPROM.write(i, 0xFF); 
      if (i % 64 == 0) Serial.print("."); 
    }
    Serial.println(F(""));
    Serial.println(F("[S] EEPROM wiped successfully."));
    playSound(notifySound);
  }
}

// ===== Command Dispatcher =====
void handleCommand(String command) {
  command.trim();
  command.toLowerCase();

  if (command == "elena" || command == "hello" || command == "hi"){ //Easter egg 
    Serial.println(F("ELENA> Hi, are you called me?"));
  } 
  
  else if (command == "help") {
    showHelp();
  } 
  
  else if (command == "d help") {
    showDangerousHelp();
  } 
  
  else if (command == "login") {
    elenaUserStartup(); 
  } 
  
  else if (command == "logout") {
    logout();
  } 
  
  else if (command == "stat") {
    cmd_stat(loggedIn);
  } 
  
  else if (command == "rns acc") {
    cmd_resetandset();
  } 
  
  else if (command == "reboot") {
    cmd_reboot();
  } 
  
  else if (command == "whoami") {
    cmd_whoami();
  } 
  
  else if (command == "time") {
    cmd_time();
  } 
  
  else if (command == "set time") {
    interactiveSetTime();
  } 
  
  else if (command == "sound on") {
    eeprom_bool_write(33, true);
  } 
  
  else if (command == "sound off") {
    eeprom_bool_write(33, false);
  } 
  
  else if (command == "calc") {
    cmd_calc();
  } 
  
  else if (command.startsWith("print help")){
    printHelp();
  } else if (command.startsWith("print")) {
    handlePrintCommand(command.c_str());
  } 

  else if (command.startsWith("delay")){
    delayFunc(command.c_str());
  }
  
  else if (command == "gpio s") {
    printAllPinStatus();
  } 
  
  else if (command.startsWith("gpio w")) {
    cmd_gpioWrite(command.c_str());
  } 
  
  else if (command.startsWith("gpio r")) {
    cmd_gpioRead(command.c_str());
  } 
  
  else if (command == "i2c scan") {
    scanI2CDevices();
  } 
  
  else if (command == "eeprom wipe") {
    cmd_eeprom_wipe();
  } 
  
  else {
    Serial.print(F("[X] Unknown command with '"));
    Serial.print(command);
    Serial.println(F("'. Try 'help' to see all commands."));
    sound(errorSound);
  }
}

#endif
