#include <stdint.h>
#include <stdbool.h>
#include "global_vars.h"
#include "helper_tools.h"
#include "multiply.h"

extern uint32_t Registers[];

void multiply(uint32_t instruction) {
	bool accumulate = (instruction >> 21) & 0x1;
	bool set_condition = (instruction >> 20) & 0x1;
	uint8_t register_d = (instruction >> 16) & 0xf;
	uint8_t register_n = (instruction >> 12) & 0xf;
	uint8_t register_s = (instruction >> 8) & 0xf;
	uint8_t register_m = instruction & 0xf;

	uint32_t result = Registers[register_m] * Registers[register_s];

	if (accumulate) {
		result += Registers[register_n];
	}

	Registers[register_d] = result;

	if (set_condition) {
		set_bit(Registers + CPSR_REGISTER, CPSR_N, (result >> 31) & 0x1);
		set_bit(Registers + CPSR_REGISTER, CPSR_Z, result == 0);
	}
}
