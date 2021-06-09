#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_SIZE 511

// maps labels to memory addresses and expected operand count
// could use function pointers with the Symbol_table
typedef struct Symbol_table {
	uint32_t* addresses;
	uint32_t* operand_count;
	char** labels;
	//char* opcodes[];
	//function* functions[];
} Symbol_table;

// an attempt at function pointers - probably should look different
//(function*)func_from_opcode(uint32_t opcode) {
	//return (function*)ptr;
//}

// swap used to convert from big endian to little endian

uint32_t swap(uint32_t num) {
	uint32_t swapped = ((num >> 24) & 0xff) | // move byte 3 to byte 0
		((num << 8) & 0xff0000) | // move byte 1 to byte 2
		((num >> 8) & 0xff00) | // move byte 2 to byte 1
		((num << 24) & 0xff000000); // byte 0 to byte 3
	return swapped;
}

// splits line into label, opcode, operands and operand count
// returns true when a label, false otherwise
bool tokenizer(char* line, char* label, char* mnemonic, char** operands, uint32_t* num_operands) {
	char* token = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
	if (!token) {
		printf("Error!\n");
	}
	char str[MAX_LINE_SIZE];
	for (int i = 0; i < MAX_LINE_SIZE; i++) {
		str[i] = *line;
		line += sizeof(char);
	}
	//printf("str check: %s\n", str);
   
	token = strtok(str, ":");
	//printf("token check: %s\n", token);
	//printf("strcmp: %d\n", strcmp(str, token));

	// strings are different
	if (strcmp(str, token) != 0) {
		// we get a label
		memcpy(label, token, MAX_LINE_SIZE);
		return true;
	}

	token = strtok(str, " ");

	//printf("%s\n", token);
	// we get a mnemonic
	memcpy(mnemonic, token, MAX_LINE_SIZE);

	memcpy(str, token, MAX_LINE_SIZE);
	int n = 0;
	token = strtok(NULL, ",");

	while (token != NULL) {
		//printf("1st loop check: %s\n", token);
		memcpy(operands[n], token, MAX_LINE_SIZE * sizeof(char));
		token = strtok(NULL, ",");
		n++;
	}

	*num_operands = n;

	free(token);
	return false;
}

// is_register and get_operand_value are useful in the 4 instruction functions: DP, SDT, multiply, branch

bool is_register(char* operand) {
	if (operand[0] == 'r') {
		return true;
	}
	return false;
}

// takes operand string and returns either decimal register number or decimal immediate value
uint32_t get_operand_value(char* operand) {
	if (is_register(operand)) {
		operand += sizeof(char);
		return strtol(operand, NULL, 10);
	} else {
		operand += 3 * sizeof(char);
		return strtol(operand, NULL, 16);
	}
}

// returns relevant binary instruction
uint32_t dataProcessing(char* label, int* opcode, uint32_t** operands, uint32_t operand_count) {
	return 0;
}

uint32_t multiply(char* mnemonic, char** operands) {

	uint32_t rd = get_operand_value(operands[0]);
	uint32_t rm = get_operand_value(operands[1]);
	uint32_t rs = get_operand_value(operands[2]);
	uint32_t rn;
	uint32_t result = 0;
	// sets cond field
	result = result | 0xe0000000;
	result = result | (rd << 16);
	result = result | (rs << 8);
	result = result | rm;
	result = result | (0x9 << 4);

	if (strcmp(mnemonic, "mla") == 0) {
		// sets bit A
		result = result | 0x00200000;	
		rn = get_operand_value(operands[3]);
		result = result | (rn << 12);
	}

	return result;
}

uint32_t singleDataTransfer(char* label, int* opcode, uint32_t** operands, uint32_t operand_count) {
	return 0;
}

uint32_t branch(char* label, int* opcode, uint32_t** operands, uint32_t operand_count) {
	return 0;
}

Symbol_table* first_pass(FILE* fp) {
	//fill in symbol table
	//return (Symbol_table*)table;
	return NULL;
}

void second_pass(FILE* fp, Symbol_table* table) {
    // write binary to file
	// For each instruction:
	// 	1. Map it exactly to one 32-bit word.
	// 	2. Read in its opcode mnemonic and operand field(s).
	// 	3. Generate the corresponding binary encoding.
	//	4. Write the binary encoding to the binary file.
	// useful: size_t fwrite()
}

void two_pass_assembly(FILE* fp, FILE* fp_2) {
	Symbol_table* table = first_pass(fp);
	second_pass(fp_2, table);
}

int main(int argc, char** argv) {
	assert(argc == 3 && "Enter two arguments");

	FILE* fptr = fopen(argv[1], "r"); // "r" - read
	assert(fptr != NULL && "Could not open file");

	FILE* fptr_2 = fopen(argv[2], "w"); // "w" - write
	assert(fptr != NULL && "Could not open file");

	two_pass_assembly(fptr, fptr_2);

	fclose(fptr);
	fclose(fptr_2);
	return EXIT_SUCCESS;
}

