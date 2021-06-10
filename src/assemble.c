#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_SIZE 511

// swap used to convert from big endian to little endian
uint32_t swap(uint32_t num) {
	uint32_t swapped = ((num >> 24) & 0xff) | // move byte 3 to byte 0
		((num << 8) & 0xff0000) | // move byte 1 to byte 2
		((num >> 8) & 0xff00) | // move byte 2 to byte 1
		((num << 24) & 0xff000000); // byte 0 to byte 3
	return swapped;
}

// splits line into label, opcode, operands and operand count
// returns true when it is a label, false otherwise
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
  
	char str_org[MAX_LINE_SIZE];
	memcpy(str_org, str, MAX_LINE_SIZE);
 
	token = strtok(str, ":");
	//printf("token check: %s\n", token);
	//printf("strcmp: %d\n", strcmp(str, token));

	// strings are different
	if (strcmp(str_org, token) != 0) {
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

bool has_equals(char* operand) {
	return (*operand == '=');
}

bool has_sq_brackets(char* operand) {
	return (*operand == '[');
}

bool has_hashtag(char* operand) {
	return (*operand == '#');
}

char* remove_equals(char* operand) {
	if (has_equals(operand)) {
		operand += sizeof(char);
	}
	return operand;
}

char* remove_sq_brackets(char* operand) {
	if (has_sq_brackets(operand)) {
		operand += sizeof(char);
		int i;
		for (i = 0; operand[i] != ']'; i++) {
			
		}
		operand[i] = '\0';
	}
	return operand;
}

char* remove_hashtag(char* operand) {
	if (has_hashtag(operand)) {
		operand += sizeof(char);
	}
	return operand;
}

char* remove_special_chars(char* operand) {
	operand = remove_equals(operand);
	operand = remove_sq_brackets(operand);
	operand = remove_hashtag(operand);
	return operand;
}

// takes operand string and returns either decimal register number or decimal immediate value
uint32_t get_operand_value(char* operand) {
	// register
	if (is_register(operand)) {
		operand += sizeof(char);
		return strtol(operand, NULL, 10);
	}
	operand += sizeof(char);
	// hex immediate
	if (*operand == 'x') {
		operand += sizeof(char);
		return strtol(operand, NULL, 16);
	}
	// decimal immediate
	operand -= sizeof(char);
	return strtol(operand, NULL, 10);
}

//structs for opcode
//TODO: move to top where it should be

struct entry {
	char* str;
	int n;
};

struct entry opcode[] = {
	{"and", 0},
	{"eor", 1},
	{"sub", 2},
	{"rsb", 3},
	{"add", 4},
	{"orr", 5},
	{"mov", 6},
	{"tst", 7},
	{"teq", 8},
	{"cmp", 9},
	{"beq", 10},
	{"bne", 11},
	{"bge", 12},
	{"blt", 13},
	{"bgt", 14},
	{"ble", 15},
	{"bal", 16},
    {"b",   17}
};

int string_to_opcode(char* key)
{
	int i = 0;
	char* name = opcode[i].str;
	while (name) {
		if (strcmp(name, key) == 0)
			return opcode[i].n;
		name = opcode[++i].str;
	}
	return 0;
}

uint32_t data_processing(char* mnemonic, char** operands) {
	uint8_t opcode = string_to_opcode(mnemonic);
	uint8_t register_d = 0;
	uint8_t register_n = 0;
	uint16_t operand = 0;

	uint32_t result = 0;

	if (opcode <= 5) {
		register_d = get_operand_value(operands[0]);
		register_n = get_operand_value(operands[1]);
		operand = get_operand_value(operands[2]);
	}
	else if (opcode == 6) {
		register_d = get_operand_value(operands[0]);
		operand = get_operand_value(operands[1]);
	}
	else if (opcode >= 7) {
		result |= 1 << 20;
		register_n = get_operand_value(operands[0]);
		operand = get_operand_value(operands[1]);
	}

	result |= opcode << 21;
	result |= register_n << 16;
	result |= register_d << 12;
	result |= operand;

	return result;
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


// the target address passed in to this function is either a specific 32-bit 
// address or a label.
uint32_t branch(char* mnemonic, const int current_address, const int target_address) {
	uint32_t opcode, result;
	int32_t offset;
	const uint32_t unchanged_bits = 0xa << 24;

	// opcode = b if mnemonic length is 1 
	opcode = strlen(mnemonic) == 1 ? 0xe : string_to_opcode(mnemonic) % 10;
	if (opcode > 1) {
		// opcode = bge, blt, bgt, ble, OR bal
		opcode |= 0x8;
	}

	offset = target_address - current_address - 8;
	offset &= 0x3ffffff; // make offset 26 bits in length
	offset >>= 2;
	offset &= 0xffffff; // store the lower 24 bits
	
	result = (opcode << 28) | unchanged_bits | offset;
	return result;
}

uint32_t andeq(void) {
	return 0;	
}

uint32_t special_lsl(char* mnemonic, char** operands) {
	char* num = operands[1];
	operands[1] = operands[0];
	operands[2] = mnemonic;
	operands[3] = num;
	return data_processing(mnemonic, operands);
}

//void second_pass(FILE* fp, Symbol_table* table) {
    // write binary to file
	// For each instruction:
	// 	1. Map it exactly to one 32-bit word.
	// 	2. Read in its opcode mnemonic and operand field(s).
	// 	3. Generate the corresponding binary encoding.
	//	4. Write the binary encoding to the binary file.
	// useful: size_t fwrite()
//}

//void two_pass_assembly(FILE* fp, FILE* fp_2) {
	//Symbol_table* table = first_pass(fp);
	//second_pass(fp_2, table);
//}

// we assert the string ends with a \n
void remove_newline(char* str) {
	int i;
	for (i = 0; str[i] != '\n'; i++) {		

	}
	str[i] = '\0';
}

char* alloc_label (void) {
	char* label = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
	if (!label) {
		printf("Error!\n");
	}
	return label;
}

char* alloc_mnemonic (void) {
	char* mnemonic = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
	if (!mnemonic) {
		printf("Error!\n");
	}
	return mnemonic;
}

char** alloc_operands (void) {
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
	return operands;
}

char* alloc_line (void) {
	char* line = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
	if (!line) {
		printf("Error!\n");
	}
	return line;
}

int main(int argc, char** argv) {
	char* label = alloc_label();
	char* mnemonic = alloc_mnemonic();
	char** operands = alloc_operands();
	char* line = alloc_line();
	uint32_t num_operands = 0;
	char labels[100][100];
	uint8_t memory_addresses[100];
	bool label_present = false;	
	int i = 0;
	uint8_t mem = 0;

	assert(argc == 3 && "Enter two arguments");

	FILE* fptr = fopen(argv[1], "r"); // "r" - read
	assert(fptr != NULL && "Could not open file");

	FILE* fptr_2 = fopen(argv[2], "w"); // "w" - write
	assert(fptr != NULL && "Could not open file");

	//two_pass_assembly(fptr, fptr_2);		

	while (fgets(line, MAX_LINE_SIZE, fptr)) {
		remove_newline(line);
		printf("%s\n", line);
		label_present = tokenizer(line, label, mnemonic, operands, &num_operands);
		if (label_present) {
			printf("label after function check: %s\n", label);
			strcpy(labels[i], label);
			memory_addresses[i] = mem;
			i++;
		} else {
			printf("mnemonic after function check: %s\n", mnemonic);
			printf("num_operands: %d\n", num_operands);
			for (int i = 0; i < num_operands; i++) {
				printf("operands[%d]: %s", i, operands[i]);
				operands[i] = remove_special_chars(operands[i]);
				if (is_register(operands[i])) {
					printf(" is a register");
				} else {
					printf(" isn't a register");
				}
				printf(" with value %u", get_operand_value(operands[i]));
				printf("\n");
			}
		}	
		if (!label_present && (strcmp(mnemonic, "mul") == 0 || strcmp(mnemonic, "mla") == 0)) {
			printf("binary instr.: 0x%8x\n", swap(multiply(mnemonic, operands)));
		}
	mem++;
	}

	printf("number of lines read: %d\n", mem);
	printf("labels found: %d\n", i);    
	for (int j = 0; j < i; j++) {
		printf("Label: %s, has memory address %d\n", labels[j], memory_addresses[j]);
	}
	
	free(label);
	free(mnemonic);
	free(operands[0]);
	free(operands);

	fclose(fptr);
	fclose(fptr_2);

	return EXIT_SUCCESS;
}
