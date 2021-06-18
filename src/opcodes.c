#include <stdint.h>
#include <string.h>
#include "global_vars_2.h"
#include "opcodes.h"

struct entry {
	char* str;
	uint8_t n;
};

struct entry opcode[] = {
	{"and", 10},
	{"eor", 11},
	{"sub", 12},
	{"rsb", 13},
	{"add", 14},
	{"orr", 15},
	{"mov", 16},
	{"tst", 17},
	{"teq", 18},
	{"cmp", 19},
	{"mul", 20},
	{"mla", 21},
	{"ldr", 30},
	{"str", 31},
	{"beq", 40},
	{"bne", 41},
	{"bge", 42},
	{"blt", 43},
	{"bgt", 44},
	{"ble", 45},
	{"bal", 46},
	{"b", 47},
	{"lsl", 50},
	{"andeq", 51}
};


// represents the true opcode of data processing mnemonics
struct entry opcode_dp[] = {
	{"and", 0x0},
	{"eor", 0x1},
	{"sub", 0x2},
	{"rsb", 0x3},
	{"add", 0x4},
	{"orr", 0xc},
	{"mov", 0xd},
	{"tst", 0x8},
	{"teq", 0x9},
	{"cmp", 0xa}
};

uint8_t string_to_opcode(char* key) {
	for (uint8_t i = 0; i < NUM_OPCODES; i++) {
		if (strcmp(opcode[i].str, key) == 0) {
			return opcode[i].n;
		}
	}
	return 0;
}

// works just as string_to_opcode but using the opcode_dp struct
int string_to_opcode_dp(char* key) {
	int i = 0;
	char* name = opcode_dp[i].str;
	while (name) {
		if (strcmp(name, key) == 0)
			return opcode_dp[i].n;
		name = opcode_dp[++i].str;
	}
	return 0;
}
