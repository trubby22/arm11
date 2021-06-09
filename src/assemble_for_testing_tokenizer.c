#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_SIZE 511
#define MAX_LABEL_SIZE 0
#define MAX_MNEMONIC_SIZE 5
#define MAX_OPERAND_SIZE 0
#define MAX_NUM_OPERANDS 4

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

uint32_t multiply(char* mnemonic, char** operands, uint32_t operand_count) {
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
}

void two_pass_assembly(FILE* fp, FILE* fp_2) {
	Symbol_table* table = first_pass(fp);
	second_pass(fp_2, table);
}

int main(int argc, char** argv) {
	char str[80] = "mov r3,#0x100";
	char* ptr = str;
	char* label = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
	if (!label) {
		printf("Error!\n");
	}	

	char* mnemonic = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
	if (!mnemonic) {
		printf("Error!\n");
	}

	char** operands = (char**) malloc(MAX_LINE_SIZE * sizeof(char*));
	if (!operands) {
		printf("Error!\n");
	}
	operands[0] = (char*) malloc(MAX_LINE_SIZE * MAX_LINE_SIZE * sizeof(char));
	if (!operands[0]) {
		free(operands);
		printf("Error!\n");
	}
	for (int i = 1; i < MAX_LINE_SIZE; i++) {
		operands[i] = operands[i - 1] + MAX_LINE_SIZE;
	}

	uint32_t num_operands = 0;
	bool label_present = tokenizer(ptr, label, mnemonic, operands, &num_operands);

/*
	printf("test\n");
	num_operands = 2;
	char hello[] = "hello";
	char world[] = "world";
	char* hello_2 = &(hello[0]);
	char* world_2 = &(world[0]);
	memcpy(operands[0], hello_2, 6);
	for (int i = 0; i < 6; i++) {
		operands[0][i] = hello[i];
	}
	printf("test 2\n");
	
	memcpy(operands[1], world_2, 6);
	printf("test 3\n");
*/
	if (label_present) {
		printf("label after function check: %s\n", label);
	} else {
		printf("mnemonic after function check: %s\n", mnemonic);
		printf("num_operands: %d\n", num_operands);
		for (int i = 0; i < num_operands; i++) {
			printf("operands[%d]: %s", i, operands[i]);
			if (is_register(operands[i])) {
				printf(" is a register");
			} else {
				printf(" isn't a register");
			}
			printf(" with value %u", get_operand_value(operands[i]));
			printf("\n");
		}
	}

	free(label);
	free(mnemonic);
	free(operands[0]);
	free(operands);

/*
	assert(argc == 3 && "Enter two arguments");

	FILE* fptr = fopen(argv[1], "r"); // "r" - read
	assert(fptr != NULL && "Could not open file");

	FILE* fptr_2 = fopen(argv[2], "w"); // "w" - write
	assert(fptr != NULL && "Could not open file");

	two_pass_assembly(fptr, fptr_2);

	fclose(fptr);
	fclose(fptr_2);
*/
	return EXIT_SUCCESS;
}

