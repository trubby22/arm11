#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define RAM_SIZE 65536
#define PC_REGISTER 15
#define CPSR_REGISTER 16
#define CPSR_N 31
#define CPSR_Z 30
#define CPSR_C 29
#define CPSR_V 28

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
		//TODO: should be changed to false when not debugging
		return true;
	}
}

void dataProcessing() {
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
}

uint32_t lsl(uint32_t identifier, uint32_t value) {
	uint32_t carry;
	if (value == 0) {
		carry = 0;
	}
	else {
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
	}
	else {
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
	}
	else {
		carry = ((identifier >> (value - 1)) & 0b1);
	}
	// set C bit in CPSR identifier
	Registers[16] |= carry << 29;
	highBit = identifier >> 31;
	result = identifier >> value;
	if (highBit == 1) {
		for (uint32_t i = 0; i < value; i++) {
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
	for (uint32_t i = 0; i < value; i++) {
		identifier = rorOnce(identifier);
	}
	return identifier;
}

uint32_t shift(uint32_t offset) { {}
uint8_t rm = offset & 0b1111;
bool variable = (offset >> 4) & 0b1;
uint8_t shiftType = (offset >> 5) & 0b11;
uint8_t value = (offset >> 7) & 0b11111;

if (variable) {
	uint8_t rs = (value >> 1) & 0b1111;
	assert(rs != PC_REGISTER && "shift register cannot be PC_REGISTER");
	value = Registers[rs] & 0xff;
}

switch (shiftType) {
case 0b00:
	return lsl(rm, value);
case 0b01:
	return lsr(rm, value);
case 0b10:
	return asr(rm, value);
default:
	return ror(rm, value);
}
}

void singleDataTransfer(uint32_t instruction) {
	bool immediateOffset = (instruction >> 25) & 0b1;
	bool pIndexing = (instruction >> 24) & 0b1;
	bool up = (instruction >> 23) & 0b1;
	bool load = (instruction >> 20) & 0b1;
	uint8_t baseRegister = (instruction >> 16) & 0b1111;
	uint8_t sourceRegister = (instruction >> 12) & 0b1111;
	uint32_t offset = instruction & 0xfff;
	uint32_t baseRegisterUp;

	if (immediateOffset)
		offset = shift(offset);

	if (!up)
		baseRegisterUp = Registers[baseRegister] - offset;
	else
		baseRegisterUp = Registers[baseRegister] + offset;

	if (pIndexing)
		Registers[baseRegister] = baseRegisterUp;

	if (load)
		Ram[baseRegister] = Registers[sourceRegister];
	else
		Registers[sourceRegister] = Ram[baseRegister];

	if (!pIndexing)
		Registers[baseRegister] = baseRegisterUp;
}

void branch() {

}

uint32_t fetch() {
	if (Registers[PC_REGISTER] >= RAM_SIZE) return 0;

	uint32_t fetched = *((uint32_t*)(Ram + Registers[PC_REGISTER]));
	Registers[PC_REGISTER] += 4;
	return fetched;
}

void print_state(uint16_t program_size) {
	printf("Registers\n");
	uint16_t i;
	for (i = 0; i <= 9; i++)
		printf("$%d  :\t %d (0x%08x)\n", i, Registers[i], Registers[i]);
	for (i = 10; i <= 12; i++)
		printf("$%d :\t %d (0x%08x)\n", i, Registers[i], Registers[i]);

	printf("PC  :\t %d (0x%08x)\n", Registers[PC_REGISTER], Registers[PC_REGISTER]);
	printf("CPSR:\t %d (0x%08x)\n", Registers[CPSR_REGISTER], Registers[CPSR_REGISTER]);
	printf("Non-zero memory: \n");
	for (i = 0; i <= program_size; i += 4) {
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
				singleDataTransfer(execute);
				break;

			case 0b10:
				branch();
				break;

			default:
				break;
			}
		}
		execute = decoded;
		decoded = fetched;
		fetched = fetch();
	}

	print_state(program_size);

	free(Ram);
	return EXIT_SUCCESS;
}