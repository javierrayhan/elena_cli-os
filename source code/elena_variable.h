#ifndef ELENA_VARIABLE_H
#define ELENA_VARIABLE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_VARS 8
#define NAME_LEN 12
#define STR_LEN 8

typedef enum { TYPE_INT, TYPE_FLOAT, TYPE_BOOL, TYPE_STRING } VarType;

typedef struct {
    char name[NAME_LEN];
    VarType type;
    union {
        int i_val;
        float f_val;
        int b_val;       // 0 = false, 1 = true
        char s_val[STR_LEN];
    } value;
    int is_used;
} Variable;

/// Variable Table
extern Variable vars[MAX_VARS];

/// -------- Utility Functions --------
int find_var(const char* name);
int find_empty_slot(void);
float eval_expr(const char* expr);

/// -------- Command Handlers --------
void cmd_var(char* args);
void cmd_set(char* args);
void cmd_print(char* args);

/// -------- Main Interpreter --------
void interpret(char* cmd);

#endif
