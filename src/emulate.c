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

#define RAM_SIZE 65536
#define PC_REGISTER 15
#define CPSR_REGISTER 16
#define CPSR_N 31
#define CPSR_Z 30
#define CPSR_C 29
#define CPSR_V 28

uint8_t* Ram;
uint32_t Registers[17] = { 0 };

uint32_t read_ram(uint16_t address) {
	return *(uint32_t*)(Ram + address);
}

void write_ram(uint16_t address, uint32_t value) {
	*(uint32_t*)(Ram + address) = value;
}

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
	}
#ifdef __GNUC__
	#warning should be changed to false when not debugging
#else
#pragma	warning ("should be changed to false when not debugging")
#endif
	return true;
}

uint32_t arithmetic_right(uint32_t register_value, uint8_t shift_value) {
	bool negative = register_value >> 31 && 1;
	uint32_t result = register_value >> shift_value;

	if (negative)
		for (uint32_t i = 0; i < shift_value; i++)
			result |= 1 << (31 - i);

	return result;
}

uint32_t rotate_right(uint32_t register_value, uint8_t shift_value) {
	uint32_t shifted_value = register_value >> shift_value;
	uint32_t rotated_value = register_value << (32 - shift_value);
	return shifted_value | rotated_value;
}

uint32_t shift(uint32_t offset, bool set_condition) {
	uint8_t register_m = offset & 0b1111;
	bool variable = (offset >> 4) & 0b1;
	uint8_t shift_type = (offset >> 5) & 0b11;
	uint8_t shift_value = (offset >> 7) & 0b11111;

	uint32_t register_value = Registers[register_m];

	if (variable) {
		uint8_t register_s = (shift_value >> 1) & 0b1111;
		assert(register_s != PC_REGISTER && "shift register cannot be PC_REGISTER");
		shift_value = Registers[register_s] & 0xff;
	}

	if (set_condition && shift_value != 0) {
		if (shift_type == 0)
			set_bit(Registers + CPSR_REGISTER, CPSR_Z, (register_value >> (31 - shift_value)) & 1);
		else
			set_bit(Registers + CPSR_REGISTER, CPSR_Z, (register_value >> shift_value) & 1);
	}

	switch (shift_type) {
	case 0b00:
		return register_value << shift_value;
	case 0b01:
		return register_value >> shift_value;
	case 0b10:
		return arithmetic_right(register_value, shift_value);
	case 0b11:
		return rotate_right(register_value, shift_value);
	}

	return 0;
}

void dataProcessing(uint32_t instruction) {
	uint32_t operand2 = instruction & 0xfff;
	uint8_t register_d = (instruction >> 12) & 0b1111;
	uint8_t register_n = (instruction >> 16) & 0b1111;
	bool set_condition = (instruction >> 20) & 1;
	uint8_t opcode = (instruction >> 21) & 0b1111;
	bool immediate_operand = (instruction >> 25) & 1;

	uint64_t result = 0;

	bool is_arithmetic = false;
	bool is_addition = false;
	bool write_result = false;

	if (immediate_operand) {
		// operand2 is an immediate constant
		uint8_t rotate = operand2 >> 8;
		uint8_t imm = operand2 & 0xff;
		operand2 = rotate_right(imm, rotate << 1); // Shifted 1 to the left to multiply by 2
	}
	else {
		operand2 = shift(operand2, set_condition); //operand2 is a shifted register
	}

	switch (opcode) {
	case 0b0000: //and
		result = Registers[register_n] & operand2;
		write_result = true;
		break;
	case 0b0001: //eor
		result = Registers[register_n] ^ operand2;
		write_result = true;
		break;
	case 0b0010: //sub
		result = Registers[register_n] - operand2;
		is_arithmetic = true;
		write_result = true;
		break;
	case 0b0011: //rsb
		result = operand2 - Registers[register_n];
		is_arithmetic = true;
		write_result = true;
		break;
	case 0b0100: //add
		result = (uint64_t)operand2 + Registers[register_n];
		is_addition = true;
		is_arithmetic = true;
		write_result = true;
		break;
	case 0b1000: //tst
		result = Registers[register_n] & operand2;
		break;
	case 0b1001: //teq
		result = Registers[register_n] ^ operand2;
		break;
	case 0b1010: //cmp
		result = Registers[register_n] - operand2;
		is_arithmetic = true;
		break;
	case 0b1100: //orr
		result = Registers[register_n] | operand2;
		write_result = true;
		break;
	case 0b1101: //mov
		result = operand2;
		write_result = true;
		break;
	}

	if (write_result) {
		Registers[register_d] = (uint32_t)result;
	}

	if (set_condition) {
		// For C bit: if logic operation -> set to carry out from any shift operation (result from barrel shifter)
		//            if arithmetic      -> set the the carry out from the ALU (1 for addition if there was a carry,
		//																		0 for subtraction if there was a borrow)
		if (is_arithmetic) {
			if (is_addition) {
				set_bit(Registers + CPSR_REGISTER, CPSR_C, result > 0xffffffff);
			} else {
				// result >= 0 --> no borrow; result < 0 --> borrow
				set_bit(Registers + CPSR_REGISTER, CPSR_C, result <= 0x80000000);
			}
		}

		// Z = 1 if the result is all 0s 
		if (result == 0) {
			set_bit(Registers + CPSR_REGISTER, CPSR_Z, 1);
		}
		// N = logical value of 31st bit of result
		set_bit(Registers + CPSR_REGISTER, CPSR_N, (result >> 31) & 1);
	}
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

void singleDataTransfer(uint32_t instruction) {
	bool immediate_offset = (instruction >> 25) & 1;
	bool pre_indexing = (instruction >> 24) & 1;
	bool up = (instruction >> 23) & 1;
	bool load = (instruction >> 20) & 1;
	uint8_t register_n = (instruction >> 16) & 0b1111; // holds memory address
	uint8_t register_m = (instruction >> 12) & 0b1111;  // holds value
	uint32_t offset = instruction & 0xfff;
	uint32_t memory = Registers[register_n];

	if (immediate_offset)
		offset = shift(offset, false);

	if (up)
		offset = Registers[register_n] + offset;
	else
		offset = Registers[register_n] - offset;

	if (pre_indexing)
		memory = offset;

	if (memory < 0 || memory >= RAM_SIZE) {
		printf("Error: Out of bounds memory access at address 0x%08x\n", memory);
		return;
	}

	if (load)
		Registers[register_m] = read_ram(memory);
	else
		write_ram(memory, Registers[register_m]);

	if (!pre_indexing)
		Registers[register_n] = offset;
}

void branch(uint32_t instruction) {

	uint32_t offset = (instruction & 0xffffff) << 2;
	bool negative = (offset >> 25) & 1;
	if (negative) {
		offset = (~offset) + 1;
		offset &= 0xffffff;
		Registers[PC_REGISTER] -= offset;
	}
	else {
		Registers[PC_REGISTER] += offset;
	}
}

uint32_t fetch() {
	if (Registers[PC_REGISTER] >= RAM_SIZE) return 0;

	uint32_t fetched = read_ram(Registers[PC_REGISTER]);
	Registers[PC_REGISTER] += 4;
	return fetched;
}

// works just as fetch but increments PC BEFORE reading memory

uint32_t fetch_pre() {
	if (Registers[PC_REGISTER] >= RAM_SIZE) return 0;

	Registers[PC_REGISTER] += 4;
	uint32_t fetched = read_ram(Registers[PC_REGISTER]);
	return fetched;
}

// swap used to convert from big endian to little endian

uint32_t swap(uint32_t num) {
	uint32_t swapped = ((num >> 24) & 0xff) | // move byte 3 to byte 0
		((num << 8) & 0xff0000) | // move byte 1 to byte 2
		((num >> 8) & 0xff00) | // move byte 2 to byte 1
		((num << 24) & 0xff000000); // byte 0 to byte 3
	return swapped;
}

void print_state(uint16_t program_size) {
	printf("Registers:\n");
	uint16_t i;
	for (i = 0; i <= 9; i++)
		printf("$%d  : %10d (0x%08x)\n", i, Registers[i], Registers[i]);
	for (i = 10; i <= 12; i++)
		printf("$%d : %10d (0x%08x)\n", i, Registers[i], Registers[i]);

	printf("PC  : %10d (0x%08x)\n", Registers[PC_REGISTER], Registers[PC_REGISTER]);
	printf("CPSR: %10d (0x%08x)\n", Registers[CPSR_REGISTER], Registers[CPSR_REGISTER]);
	printf("Non-zero memory:\n");
	for (i = 0; i <= 65532; i += 4) {
		if (read_ram(i) != 0)
			printf("0x%08x: 0x%08x\n", i, swap(read_ram(i))); // if you want to view numbers in regular big endian format, delete swap here
		if (i == 65532) {
			break;
		}
	}
}

int main(int argc, char* argv[]) {
	assert(argc == 2 && "Enter one argument"); //forces you to enter an argument

	FILE* fptr = fopen(argv[1], "rb"); // "rb" - read binary 
	assert(fptr != NULL && "Could not open file");

	Ram = (uint8_t*)calloc(RAM_SIZE, sizeof(uint8_t));
	assert(Ram != NULL && "Could not claim memory");

	uint16_t program_size = (uint16_t)fread(Ram, 1, RAM_SIZE, fptr);

	uint32_t execute = fetch();
	uint32_t decoded = fetch();
	uint32_t fetched = read_ram(Registers[PC_REGISTER]);

	while (execute != 0) {
		bool branch_present = false;
		//print_state(program_size);
		//printf("loop, instr.: 0x%x\n", execute);
		uint8_t code = (execute >> 28) & 0b1111;
		if (cond(code)) {
			uint8_t instruction = (execute >> 26) & 0b11;
			//printf("case: 0x%2x\n", instruction);
			switch (instruction)
			{
			case 0b00:
				if ((execute >> 4) == 0b1001) {
					//printf("execute\n");
					multiply(execute);
				} else {
					//printf("DP\n");
					dataProcessing(execute);
				}
				break;

			case 0b01:
				//printf("SDT\n");
				singleDataTransfer(execute);
				break;

			case 0b10:
				//printf("branch\n");
				branch_present = true;
				branch(execute);
				break;

			default:
				//printf("other\n");
				break;
			}
		}
		if (branch_present) {
			execute = read_ram(Registers[PC_REGISTER]);
			decoded = fetch_pre();
			fetched = fetch_pre();
		} else {
			execute = decoded;
			decoded = fetched;
			fetched = fetch_pre();
		}
	}

	//to account for one extra fetch
	//Registers[PC_REGISTER] -= 4; // no longer needed due to fetch_pre

	print_state(program_size);

	free(Ram);
	return EXIT_SUCCESS;
}
