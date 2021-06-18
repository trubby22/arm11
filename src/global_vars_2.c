#include <stdint.h>
#include "global_vars_2.h"

char* line;
char* label;
char* mnemonic;
char** operands;
uint32_t num_operands;
uint32_t constants[MAX_CONSTANTS_COUNT] = { 0 };
uint32_t consts_size;
uint32_t num_lines;
uint32_t curr_line;
uint32_t last_line;

