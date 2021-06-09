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

// returns relevant binary instruction
uint32_t dataProcessing(char* label, int* opcode, uint32_t** operands, uint32_t operand_count) {
	return 0;
}

uint32_t multiply(char* label, int* opcode, uint32_t** operands, uint32_t operand_count) {
	return 0;
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

