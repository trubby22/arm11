#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define RAM_SIZE 65536
#define PC_REGISTER 15

enum code {eq, ne, ge, lt, gt, le, al, wrong_code};

uint8_t* Ram;
uint32_t Registers[17] = { 0 };

// n, z, c, v use bit masking to determine whether respective bits in CPSR register are 0 or 1 and hence the values of N, Z, C, V flags

bool n(void) {
	uint32_t cpsr = Registers[16];
	uint32_t mask = pow(2, 31);
  return (mask & cpsr) == mask;
}

bool z(void) {
	uint32_t cpsr = Registers[16];
	uint32_t mask = pow(2, 30);
  return (mask & cpsr) == mask;
}

bool c(void) {
	uint32_t cpsr = Registers[16];
	uint32_t mask = pow(2, 29);
  return (mask & cpsr) == mask;
}

bool v(void) {
	uint32_t cpsr = Registers[16];
	uint32_t mask = pow(2, 28);
  return (mask & cpsr) == mask;
}

// read condition code from instruction

enum code readCondition(uint32_t instruction) {
	uint32_t cond = (instruction >> 28);
	switch (cond) {
		case 0:
			return eq;
		case 1:
			return ne;
		case 10:
			return ge;
		case 11:
			return lt;
		case 12:
			return gt;
		case 13:
			return le;
		case 14:
			return al;
		default:
			return wrong_code;
	}
}

// check if the flags in the CPSR register satisfy the Cond from the instruction

bool checkConditions(enum code c) {
	switch (c) {
		case eq:
			return z();
		case ne:
			return !z();
		case ge:
			return n() == v();
		case lt:
			return n() != v();
		case gt:
			return !z() && (n() == v());
		case le:
			return z() || (n() != v());
		case al:
			return true;
		default:
			printf("Enter a correct code");
			return false;
	}
}

void dataProcessing() {

}

void multiply() {

}

void singleDataTransfer() {
 
}

void branch() {

}

uint32_t fetch() {
	if (Registers[PC_REGISTER] >= RAM_SIZE) return 0;

	uint32_t fetched = *((uint32_t*)(Ram + Registers[PC_REGISTER]));
	Registers[PC_REGISTER] += 4;
	return fetched;
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

		uint8_t instruction = (execute >> 26) & 0b11;

		switch (instruction)
		{
		case 0b00:
			if ((execute >> 4) == 0b1001) multiply();
			else dataProcessing();
			break;

		case 0b01:
			singleDataTransfer();
			break;

		case 0b10:
			branch();
			break;

		default:
			// default statements
		}
		printf("%08X\t%01X\n", execute, instruction);
		execute = decoded;
		decoded = fetched;
		fetched = fetch();
	}

	free(Ram);
	return EXIT_SUCCESS;
}
