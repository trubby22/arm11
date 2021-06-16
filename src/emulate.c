/**

* File Name: emulate.c

* Authors: DANIEL ONG, MICHAEL MABO, PIOTR BLASZYK, TAHMID RAHMAN

* Date: 01/06/2021

* Description: A program that emulates an ARM11

* Input: Requires a binary file input

*/

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "emulate.h"
#include "global_vars.c"
#include "shift.c"
#include "helper_tools.c"
#include "data_processing.c"
#include "multiply.c"
#include "single_data_transfer.c"
#include "branch.c"

int main(int argc, char* argv[]) {
	assert(argc >= 2 && "Enter at least one argument"); //forces you to enter an argument

	FILE* fptr = fopen(argv[1], "rb"); // "rb" - read binary 
	assert(fptr != NULL && "Could not open file");

	Ram = (uint8_t*)calloc(RAM_SIZE, sizeof(uint8_t));
	assert(Ram != NULL && "Could not claim memory");

	start = 0;
	
	if (argc >= 3) {
		start = atoi(argv[2]);
	}

	uint16_t program_size = (uint16_t)fread(Ram + start, 1, RAM_SIZE, fptr);

	uint32_t execute = fetch();
	uint32_t decoded = fetch();
	uint32_t fetched = read_ram(start + Registers[PC_REGISTER]);

	while (execute != 0) {
		bool branch_present = false;
		uint8_t code = (execute >> 28) & 0xf;
		if (cond(code)) {
			uint8_t instruction = (execute >> 26) & 0x3;
			switch (instruction)
			{
			case 0x0:
				if (((execute >> 4) & 0xf) == 0x9) {
					multiply(execute);
				} else {
					dataProcessing(execute);
				}
				break;

			case 0x1:
				single_data_transfer(execute);
				break;

			case 0x2:
				branch_present = true;
				branch(execute);
				break;

			default:
				break;
			}
		}
		if (branch_present) {
			execute = read_ram(start + Registers[PC_REGISTER]);
			decoded = fetch_pre();
			fetched = fetch_pre();
		} else {
			execute = decoded;
			decoded = fetched;
			fetched = fetch_pre();
		}
	}

	print_state(program_size);

	free(Ram);
	fclose(fptr);
	return EXIT_SUCCESS;
}
