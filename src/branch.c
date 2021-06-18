#include <stdint.h>
#include <stdbool.h>
#include "global_vars.h"
#include "branch.h"

extern uint32_t Registers[];

void branch(uint32_t instruction) {

	uint32_t offset = (instruction & 0xffffff) << 2;
	bool negative = (offset >> 25) & 0x1;
	if (negative) {
		offset = (~offset) + 1;
		offset &= 0xffffff;
		Registers[PC_REGISTER] -= offset;
	} else {
		Registers[PC_REGISTER] += offset;
	}
}
