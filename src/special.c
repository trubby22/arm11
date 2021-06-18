#include <string.h>
#include <stdint.h>
#include "data_processing_2.h"
#include "global_vars_2.h"
#include "special.h"

extern char** operands;
extern char* mnemonic;

uint32_t andeq(void) {
	return 0;	
}

uint32_t special_lsl(void) {
	char* num = operands[1];
	operands[1] = operands[0];
	operands[2] = mnemonic;
	operands[3] = num;
	return data_processing();
}

uint32_t special(void) {
	if (strcmp(mnemonic, "andeq") == 0) {
		return andeq();
	}
	if (strcmp(mnemonic, "lsl") == 0) {
		return special_lsl();
	}
	return 0; // should not happen
}
