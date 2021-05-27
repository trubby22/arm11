#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define RAM_SIZE 65536
#define PC_REGISTER 15
#define CPSR_REGISTER 16
#define CPSR_N 31
#define CPSR_Z 30
#define CPSR_C 29
#define CPSR_V 28

enum code {eq, ne, ge, lt, gt, le, al, wrong_code};

uint8_t* Ram;
uint32_t Registers[17] = { 0 };

void set_bit(uint32_t* number, uint8_t bit, bool set) {
	if (set) {
		*number |= (1 << bit);
		return;
	}
	*number &= ~(1 << bit);
}

bool cond(uint8_t code) {
	bool n = (Registers[CPSR_REGISTER] >> CPSR_N) & 1;
	bool z = (Registers[CPSR_REGISTER] >> CPSR_Z) & 1;
	bool c = (Registers[CPSR_REGISTER] >> CPSR_C) & 1;
	bool v = (Registers[CPSR_REGISTER] >> CPSR_V) & 1;

	switch (code) {
	case 0b0000:
		return z;
	case 0b0001:
		return !z;
	case 0b1010:
		return n == v;
	case 0b1011:
		return n != v;
	case 0b1100:
		return !z && (n == v);
	case 0b1101:
		return z | (n != v);
	case 0b1110:
		return true;
	default:
		return true;
	}
}

void dataProcessing() {
	printf("data processed\n");
}

void multiply(uint32_t instruction) {
	bool accumulate = (instruction >> 21) & 1;
	bool set_condition = (instruction >> 20) & 1;
	uint8_t register_d = (instruction >> 16) & 0b1111;
	uint8_t register_n = (instruction >> 12) & 0b1111;
	uint8_t register_s = (instruction >> 8) & 0b1111;
	uint8_t register_m = instruction & 0b1111;

	uint32_t result = Registers[register_m] * Registers[register_s];

	if (accumulate)
		result += Registers[register_n];

	Registers[register_d] = result;

	if (set_condition) {
		set_bit(Registers + CPSR_REGISTER, CPSR_N, (result >> 31) & 1);
		set_bit(Registers + CPSR_REGISTER, CPSR_Z, result == 0);
	}

	printf("multiplied\n");
}

uint32_t lsl(uint32_t identifier, uint32_t value) {
	uint32_t carry;
	if (value == 0) {
		carry = 0;
	} else {
		carry = ((identifier << (value - 1)) >> 31);
	}
	// set C bit in CPSR identifier
	Registers[16] |= carry << 29;
	return identifier << value;
}

uint32_t lsr(uint32_t identifier, uint32_t value) {
	uint32_t carry;
	if (value == 0) {
		carry = 0;
	} else {
		carry = ((identifier >> (value - 1)) & 0b1);
	}
	// set C bit in CPSR identifier
	Registers[16] |= carry << 29;
	return identifier >> value;
}

uint32_t asr(uint32_t identifier, uint32_t value) {
	uint32_t carry, highBit, result;
	if (value == 0) {
		carry = 0;
	} else {
		carry = ((identifier >> (value - 1)) & 0b1);
	}
	// set C bit in CPSR identifier
	Registers[16] |= carry << 29;
	highBit = identifier >> 31;
	result = identifier >> value;
	if (highBit == 1) {
		for (int i = 0; i < value; i++) {
			result |= highBit << (31 - i);
		}
	}
	return result;
}

uint32_t rorOnce(uint32_t identifier) {
	uint32_t lowBit = identifier & 0b1;
	uint32_t result = identifier >> 1;
	result |= lowBit << 31;
	Registers[16] |= lowBit << 29;
	return result;
}

uint32_t ror(uint32_t identifier, uint32_t value) {
	for (int i = 0; i < value; i++) {
		identifier = rorOnce(identifier);
	}
	return identifier;
}

uint32_t shift(uint32_t offset) {
	assert(offset <= 0xfff && "Offset should be 12 bits only");
	uint32_t shift, rm, shiftType, integer;
	shift = offset >> 4;
	rm = offset & 0xf;
	if ((shift & 0b1) == 0) {
		shiftType = (shift >> 1) & 0b11;
		integer = shift >> 3;
	} else {
		// optional, maybe TODO later
	}
	switch (shiftType) {
		case 0b00:
			return lsl(rm, integer);
		case 0b01:
			return lsr(rm, integer);
		case 0b10:
			return asr(rm, integer);
		case 0b11:
			return ror(rm, integer);
		default:
			printf("Incorrect shift type");
			// how to throw an exception here?
			return 0;
	}
}

// method to fix the fact that registers 13 and 14 are absent

uint32_t actualRegister(uint32_t identifier) {
	if (identifier >= 13) {
		return identifier + 2;
	} else {
		return identifier;
	}
}

void singleDataTransfer(uint32_t instruction) {
	uint32_t immediateOffset, pIndexing, up, ipu, load, baseRegister, sourceRegister, offset, baseRegisterUp, memoryLocation;
	if (!cond(instruction >> 28)) {
		printf("Conditions not satisfied");
	} else {
		immediateOffset = (instruction >> 25) & 0b1;
		pIndexing = (instruction >> 24) & 0b1;
		up = (instruction >> 23) & 0b1;
		load = (instruction >> 20) & 0b1;
		baseRegister = (instruction >> 16) & 0b1111;
		sourceRegister = (instruction >> 12) & 0b1111;
		offset = instruction & 0xfff;
		if (immediateOffset == 1) {
			offset = shift(offset);
		}
		if (up == 0) {
			baseRegisterUp = Registers[actualRegister(baseRegister)] - offset;
		} else {
			baseRegisterUp = Registers[actualRegister(baseRegister)] + offset;
		}
		if (pIndexing == 0) {
			memoryLocation = Registers[actualRegister(baseRegister)];
		} else {
			memoryLocation = baseRegisterUp;
		}
		if (load == 0) {
			// to correct
			Ram[memoryLocation] = Registers[actualRegister(sourceRegister)];
		} else {
			// this too
			Registers[actualRegister(sourceRegister)] = Ram[memoryLocation];
		}
		if (pIndexing == 0) {
			Registers[actualRegister(baseRegister)] = baseRegisterUp;
		}
	}
}

void branch() {
	printf("branched\n");
}

uint32_t fetch() {
	if (Registers[PC_REGISTER] >= RAM_SIZE) return 0;

	uint32_t fetched = *((uint32_t*)(Ram + Registers[PC_REGISTER]));
	Registers[PC_REGISTER] += 4;
	return fetched;
}

void print_state() {
	printf("Registers\n");
	uint16_t i;
	for (i = 0; i <= 9; i++)
		printf("$%d  :\t %d (0x%08x)\n", i, Registers[i], Registers[i]);
	for (i = 10; i <= 12; i++)
		printf("$%d :\t %d (0x%08x)\n", i, Registers[i], Registers[i]);

	printf("PC  :\t %d (0x%08x)\n", Registers[13], Registers[13]);
	printf("CPSR:\t %d (0x%08x)\n", Registers[14], Registers[14]);
	printf("Non-zero memory: \n");
	for (i = 0; i <= Registers[PC_REGISTER]; i += 4) {
		printf("0x%08x:  0x%08x\n", i, *((uint32_t*)(Ram + i)));
	}
}

int main(int argc, char* argv[]) {
	assert(argc == 2 && "Enter one argument");

	FILE* fptr = fopen(argv[1], "rb");
	assert(fptr != NULL && "Could not open file");

	Ram = (uint8_t*)calloc(RAM_SIZE, sizeof(uint8_t));
	assert(Ram != NULL && "Could not claim memory");

	uint16_t program_size = fread(Ram, 1, RAM_SIZE, fptr);
	printf("Program has %u bytes\n", program_size);

	uint32_t execute = fetch();
	uint32_t decoded = fetch();
	uint32_t fetched = fetch();

	while (execute != 0) {
		uint8_t code = (execute >> 28) & 0b1111;
		if (cond(code)) {
			uint8_t instruction = (execute >> 26) & 0b11;

			switch (instruction)
			{
			case 0b00:
				if ((execute >> 4) == 0b1001)
					multiply(execute);
				else
					dataProcessing();
				break;

			case 0b01:
				singleDataTransfer(instruction);
				break;

			case 0b10:
				branch();
				break;

			default:
				printf("defaulted\n");
			}
		}
		execute = decoded;
		decoded = fetched;
		fetched = fetch();
	uint32_t result = 0;
	}

	print_state();

	free(Ram);
	return EXIT_SUCCESS;
}
