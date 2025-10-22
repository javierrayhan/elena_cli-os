#include <Arduino.h>
#include "elena_variable.h"
/// -------- Variable Table --------
Variable vars[MAX_VARS];

/// -------- Utility Functions --------
int find_var(const char* name) {
    for (int i = 0; i < MAX_VARS; i++) {
        if (vars[i].is_used && strcmp(vars[i].name, name) == 0) {
            return i; // ketemu
        }
    }
    return -1;
}

int find_empty_slot() {
    for (int i = 0; i < MAX_VARS; i++) {
        if (!vars[i].is_used) return i;
    }
    return -1;
}

/// -------- Expression Evaluator --------
float eval_expr(const char* expr) {
    char left[16], op, right[16];
    if (sscanf(expr, "%s %c %s", left, &op, right) == 3) {
        float l_val, r_val;

        int idx = find_var(left);
        if (idx >= 0) {
            if (vars[idx].type == TYPE_INT) l_val = vars[idx].value.i_val;
            else if (vars[idx].type == TYPE_FLOAT) l_val = vars[idx].value.f_val;
            else return 0;
        } else l_val = atof(left);

        idx = find_var(right);
        if (idx >= 0) {
            if (vars[idx].type == TYPE_INT) r_val = vars[idx].value.i_val;
            else if (vars[idx].type == TYPE_FLOAT) r_val = vars[idx].value.f_val;
            else return 0;
        } else r_val = atof(right);

        switch (op) {
            case '+': return l_val + r_val;
            case '-': return l_val - r_val;
            case '*': return l_val * r_val;
            case '/': return r_val != 0 ? l_val / r_val : 0;
        }
    }
    return atof(expr);
}

/// -------- Command Handlers --------
void cmd_var(char* args) {
    char type[8], name[NAME_LEN], val[32];
    int matched = sscanf(args, "%s %s = %[^\n]", type, name, val);

    if (find_var(name) >= 0) {
        Serial.printf("Error: variable %s already exists\n", name);
        return;
    }

    int idx = find_empty_slot();
    if (idx < 0) {
        Serial.print(F("Error: variable table full\n"));
        return;
    }

    strcpy(vars[idx].name, name);
    vars[idx].is_used = 1;

    if (strcmp(type, "int") == 0) {
        vars[idx].type = TYPE_INT;
        vars[idx].value.i_val = (matched == 3) ? (int)eval_expr(val) : 0;
    } else if (strcmp(type, "float") == 0) {
        vars[idx].type = TYPE_FLOAT;
        vars[idx].value.f_val = (matched == 3) ? eval_expr(val) : 0.0;
    } else if (strcmp(type, "bool") == 0) {
        vars[idx].type = TYPE_BOOL;
        vars[idx].value.b_val = (matched == 3) ? atoi(val) : 0;
    } else if (strcmp(type, "string") == 0) {
        vars[idx].type = TYPE_STRING;
        if (matched == 3) strncpy(vars[idx].value.s_val, val, STR_LEN);
        else strcpy(vars[idx].value.s_val, "");
    }
}

void cmd_set(char* args) {
    char name[NAME_LEN], val[32];
    if (sscanf(args, "%s = %[^\n]", name, val) < 2) return;

    int idx = find_var(name);
    if (idx < 0) {
        Serial.printf("Error: variable %s not found\n", name);
        return;
    }

    switch (vars[idx].type) {
        case TYPE_INT: vars[idx].value.i_val = (int)eval_expr(val); break;
        case TYPE_FLOAT: vars[idx].value.f_val = eval_expr(val); break;
        case TYPE_BOOL: vars[idx].value.b_val = atoi(val); break;
        case TYPE_STRING: strncpy(vars[idx].value.s_val, val, STR_LEN); break;
    }
}

void cmd_print(char* args) {
    char name[NAME_LEN];
    if (sscanf(args, "%s", name) < 1) return;

    int idx = find_var(name);
    if (idx < 0) {
        Serial.printf("\nError: variable %s not found", name);
        return;
    }

    switch (vars[idx].type) {
        case TYPE_INT: Serial.printf("\n%s = %d\n", name, vars[idx].value.i_val); break;
        case TYPE_FLOAT: Serial.printf("\n%s = %f\n", name, vars[idx].value.f_val); break;
        case TYPE_BOOL: Serial.printf("\n%s = %s\n", name, vars[idx].value.b_val ? "true" : "false"); break;
        case TYPE_STRING: Serial.printf("\n%s = \"%s\"\n", name, vars[idx].value.s_val); break;
    }
}

/// -------- Main Interpreter --------
void interpret(char* cmd) {
    if (strncmp(cmd, "var ", 4) == 0) cmd_var(cmd + 4);
    else if (strncmp(cmd, "set ", 4) == 0) cmd_set(cmd + 4);
    else if (strncmp(cmd, "print var ", 10) == 0) cmd_print(cmd + 10);
    else Serial.println(F("[VX] Unknown command!"));
}
