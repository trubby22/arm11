#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define RAM_SIZE 65536
#define PC_REGISTER 15

uint8_t* Ram;
uint32_t Registers[17] = { 0 };

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
