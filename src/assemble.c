#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_SIZE 511
#define NUM_OPCODES 24

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
		//memcpy(operands[n], token, MAX_LINE_SIZE * sizeof(char));
		strcpy(operands[n], token);
		token = strtok(NULL, ",");
		n++;
	}

	*num_operands = n;

	free(token);
	return false;
}

// is_register and get_operand_value are useful in the 4 instruction functions: DP, SDbbbT, multiply, branch

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

bool has_whitespace(char* operand) {
	return (*operand == ' ');
}

bool begins_with_sq_bracket(char* operand) {
	return (*operand == '[');
}

bool ends_with_sq_bracket(char* str) {
	int i;
	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] == ']') {
			return true;
		}
	}
	return false;
}

bool minus(char* str) {
	return (*str == '-');
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

char* remove_whitespace(char* operand) {
	if (has_whitespace(operand)) {
		operand += sizeof(char);
	}
	return operand;
}

char* remove_l_bracket(char* operand) {
	if (begins_with_sq_bracket(operand)) {
		operand += sizeof(char);
	}
	return operand;
}

char* remove_r_bracket(char* str) {
	int i;
	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] == ']') {
			str[i] = '\0';
		}
	}
	return str;
}

char* remove_minus(char* str) {
	if (minus(str)) {
		str += sizeof(char);
	}
	return str;
}

char* remove_special_chars(char* operand) {
	operand = remove_whitespace(operand);
	operand = remove_equals(operand);
	operand = remove_sq_brackets(operand);
	operand = remove_hashtag(operand);
	return operand;
}

// takes operand string and returns either decimal register number or decimal immediate value
uint32_t get_operand_value(char* operand) {
	// register
	operand = remove_special_chars(operand);
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
	uint8_t n;
};

struct entry opcode[] = {
	{"and", 10},
	{"eor", 11},
	{"sub", 12},
	{"rsb", 13},
	{"add", 14},
	{"orr", 15}, // orr = 12
	{"mov", 16}, // mov = 13
	{"tst", 17}, // tst = 8
	{"teq", 18}, // teq = 9
	{"cmp", 19}, // cmp = 10
	{"mul", 20},
	{"mla", 21},
	{"ldr", 30},
	{"str", 31},
	{"beq", 40},
	{"bne", 41},
	{"bge", 42},
	{"blt", 43},
	{"bgt", 44},
	{"ble", 45},
	{"bal", 46},
	{"b", 47},
	{"lsl", 50},
	{"andeq", 51}
};

struct entry opcode_dp[] = {
	{"and", 0x0},
	{"eor", 0x1},
	{"sub", 0x2},
	{"rsb", 0x3},
	{"add", 0x4},
	{"orr", 0xc},
	{"mov", 0xd},
	{"tst", 0x8},
	{"teq", 0x9},
	{"cmp", 0xa}
};

uint8_t string_to_opcode(char* key)
{
	for (uint8_t i = 0; i < NUM_OPCODES; i++) {
		if (strcmp(opcode[i].str, key) == 0) {
			return opcode[i].n;
		}
	}
	return 0;
}

int string_to_opcode_dp(char* key)
{
	int i = 0;
	char* name = opcode_dp[i].str;
	while (name) {
		if (strcmp(name, key) == 0)
			return opcode_dp[i].n;
		name = opcode_dp[++i].str;
	}
	return 0;
}

/*
char* remove_special_chars(char* operand) {
	char copy[MAX_LINE_SIZE];
	char* ptr = &(copy[0]);
	strcpy(ptr, operand);
	ptr = remove_whitespace(ptr);
	ptr = remove_equals(ptr);
	ptr = remove_sq_brackets(ptr);
	ptr = remove_hashtag(ptr);
	return ptr;
}
*/

uint32_t single_data_transfer(char* mnemonic, char** operands, uint32_t* load_count, uint32_t* constants, uint32_t* consts_size, uint32_t num_lines, uint32_t curr_line, uint32_t num_operands);

// bitwise circular left rotation for calculating rotate and operand fields of operand2 of 
uint32_t rotate_left(uint32_t value, uint8_t shift) {
	 if ((shift &= (sizeof(value) << 3) - 1) == 0)
      return value;
    return (value << shift) | (value >> ((sizeof(value) << 3) - shift));
}

uint32_t data_processing(char* mnemonic, char** operands, uint32_t* constants, uint32_t* consts_size, uint32_t num_lines, uint32_t curr_line, uint32_t num_operands) {
	printf("\n");
	printf("dp start, mnemonic: %s\n", mnemonic);
	uint8_t opcode = string_to_opcode(mnemonic) - 10;
	printf("operands[1] = %s\n", operands[1]);
	bool immediate = has_hashtag(remove_whitespace(operands[1])) || has_equals(operands[1]);
	if (opcode <= 5) {
		immediate = has_hashtag(remove_whitespace(operands[2]));
	}

/*
	if (strcmp(mnemonic, "mov") == 0 && immediate && get_operand_value(operands[1]) > 0xff) {
		printf("=== calling SDT ===\n");
		strcpy(mnemonic, "ldr");
		operands[1][0] = '=';
		printf("%s\n", operands[1]);
		return single_data_transfer(mnemonic, operands, 0, constants, consts_size, num_lines, curr_line, num_operands);
	}
*/

	uint8_t register_d = 0;
	uint8_t register_n = 0;
	uint32_t operand = 0;
	uint8_t rotate = 0;

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

	printf("before while loop: operand = %d\n",operand);
	
	// calculate the imm value and rotate value
	while (operand > 0xff) {
		operand = rotate_left(operand, 2);
		rotate++;
		if (rotate > 15) {
			printf("DP operand too big; error!\n");
			break;
		}
	}

	printf("after while loop: operand = %d and rotate = %d\n", operand, rotate);
	
	operand &= 0xff; // Imm value must be unsigned 8-bit
	operand = (rotate << 8) | operand;
	
	if (immediate) {
		result = result | 0x02000000; // operand2 is an immediate value
	}
	
	//printf("opcode is: 0x%1x\n", string_to_opcode_dp(mnemonic));

	result |= string_to_opcode_dp(mnemonic) << 21; // gets the actual opcode
	result |= register_n << 16;
	result |= register_d << 12;
	result |= operand;

	result = result | 0xe0000000; // cond field = 0111

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

bool has_expression(char* operand) {
	operand = remove_whitespace(operand);
	return has_equals(operand);
}

bool is_post(char* operand) { 
	//Looks like: [Rn],<#expression>
	for (int i = 0; operand[i] != '\0'; i++) {
		if (operand[i] == ']') {
			return true;
		}
		if (operand[i] == ',') {
			return false;
		}
	}
	return false;
}

// redundant
void add_sq_brackets(char* str, char* str_2, char* res) {
	res[0] = '[';
	int i;
	int j;
	for (i = 0; str[i] != '\0'; i++) {
		res[i + 1] = str[i];
	}
	for (j = 0; str_2[j] != '\0'; j++) {
		res[j + 1 + i] = str[j];
	}
	res[j + 1 + i] = ']';
	res[j + 2 + i] = '\0';
}

// redundant
void add_l_sq_bracket(char* str) {
	int size = 0;
	for (int i = 0; str[i] != '\0'; i++) {
		size++;
	}
	for (int i = size; i >= 0; i--) {
		str[i] = str[i-1];
		if (i == 0) {
			str[i] = '[';
		}
	}
}

// redundant
void add_r_sq_bracket(char* str) {
	int i;
	for (i = 0; str[i] != '\0'; i++) {
		
	}
	str[i] = ']';
	str[i + 1] = '\0';
}



uint32_t single_data_transfer(char* mnemonic, char** operands, uint32_t* load_count, uint32_t* constants, uint32_t* consts_size, uint32_t num_lines, uint32_t curr_line, uint32_t num_operands) {
	//TODO: remove print after debugging

	bool preindexing = true;

	if (num_operands == 3 && ends_with_sq_bracket(operands[1])) {
		preindexing = false;
	}
/*
	char temp[MAX_LINE_SIZE] = { '\0' };
	char* ptr = &(temp[0]);

	char temp_2[MAX_LINE_SIZE] = { '\0' };
	char* ptr_2 = &(temp_2[0]);
*/
		
	printf("Operands[0]: %s\nOperands[1]: %s\n", operands[0], operands[1]);
	if (num_operands == 3) {
		printf("operands[2]: %s\n", operands[2]);
	}
	bool immediate = true;
	//bool constant_present = has_expression(operands[1]);
	bool is_post;
	operands[1] = remove_whitespace(operands[1]);
	is_post = has_sq_brackets(operands[1]);
	uint32_t offset = 0;

	printf("%d\n", is_post);
	printf("check\n");

	//printf("operands[1] = %s\n", operands[1]);
	if (has_sq_brackets(operands[1])) {
		operands[1] = remove_sq_brackets(operands[1]);
		immediate = false;
	}

	if (num_operands == 3 && is_register(operands[2])) {
		immediate = true;
	}
	//
	printf("%s\n", operands[1]);	
	uint32_t address = get_operand_value(operands[1]);
	printf("check 2\n");
	printf("%s\n", operands[1]);

	if (strcmp(mnemonic, "ldr") == 0 && !is_register(operands[1])) { // If expression is a constant
		if (address <= 0xFF) {
			printf("returning dp\n");
			//operands[1][0] = '#'; 
			return data_processing("mov", operands, constants, consts_size, num_lines, curr_line, num_operands);
		} else {	
			constants[*consts_size] = get_operand_value(operands[1]);
			offset = ((num_lines + (*consts_size)) - curr_line) * 4 - 8;
			*consts_size = *consts_size + 1;
			strcpy(operands[1], "[r15");
			if (sprintf(operands[2], "#%d]", offset) < 0) {
				printf("Error!\n");
			}
			printf("big address, calling SDT\n");
			num_operands++;
			return single_data_transfer(mnemonic, operands, load_count, constants, consts_size, num_lines, curr_line, num_operands);
		}
	}

	// operand[1] is a register

	// if (preindexing && num_operands == 2) {
	// 	operands[1] = remove_l_bracket(operands[1]);
	// 	operands[2] = remove_r_bracket(operands[2]);
	// }	
	
	bool up = true;
	
	bool load = strcmp(mnemonic, "ldr") == 0;
	uint32_t register_n = get_operand_value(operands[1]);
	uint32_t register_d = get_operand_value(operands[0]);
	//offset = 0;
	uint32_t result = 0xe4000000;
	operands[2] = remove_hashtag(operands[2]);

	if (num_operands == 3 && minus(operands[2])) {
		up = false;
		operands[2] = remove_minus(operands[2]);
	}

	if (num_operands == 3) {
		offset = get_operand_value(operands[2]);
	}

/*
	if (constant_present) {
		offset = get_operand_value(operands[1]); 
		if (is_post) {
			preindexing = false;
		}
	}
*/

	result |= immediate << 25;
	result |= preindexing << 24;
	result |= up << 23;
	result |= load << 20;
	result |= register_n << 16;
	result |= register_d << 12;
	result |= offset;

	printf("no segfault yet\n");

	return result;
}

// the target address passed in to this function is either a specific 32-bit 
// address or a label.
uint32_t branch(char* mnemonic, const int current_address, const int target_address) {
	uint32_t  result, cond = string_to_opcode(mnemonic) % 10;
	int32_t offset;
	const uint32_t unchanged_bits = 0xa << 24;

	if (strcmp("b", mnemonic) == 0) {
		cond--;
	}
	if (cond > 1) {
		// cond = bge, blt, bgt, ble, OR bal
		cond |= 0x8;
	}

	offset = target_address - current_address - 8;
	offset &= 0x3ffffff; // make offset 26 bits in length
	offset >>= 2;
	offset &= 0xffffff; // store the lower 24 bits
	
	result = (cond << 28) | unchanged_bits | offset;
	//result = result & ~(0x10000000); // changes cond from 1111 to 1110
	return result;
}

uint32_t andeq(void) {
	return 0;	
}

uint32_t special_lsl(char* mnemonic, char** operands, uint32_t* constants, uint32_t* last_line, uint32_t num_lines, uint32_t curr_line, uint32_t num_operands) {
	char* num = operands[1];
	operands[1] = operands[0];
	operands[2] = mnemonic;
	operands[3] = num;
	return data_processing(mnemonic, operands, constants, last_line, num_lines, curr_line, num_operands);
}

uint32_t special(char* mnemonic, char** operands, uint32_t* constants, uint32_t* last_line, uint32_t num_lines, uint32_t curr_line, uint32_t num_operands) {
	if (strcmp(mnemonic, "andeq") == 0) {
		return andeq();
	}
	if (strcmp(mnemonic, "lsl") == 0) {
		return special_lsl(mnemonic, operands, constants, last_line, num_lines, curr_line, num_operands);
	}
	return 0; // should not happen
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
	uint32_t constants[100] = { 0 };
	uint32_t consts_size;
	uint32_t num_lines;

	assert(argc == 3 && "Enter two arguments");

	FILE* fptr = fopen(argv[1], "r"); // "r" - read
	assert(fptr != NULL && "Could not open file");

	FILE* fptr_2 = fopen(argv[2], "w"); // "w" - write
	assert(fptr != NULL && "Could not open file");

	fclose(fptr_2);
	fptr_2 = fopen(argv[2], "ab"); // "ab" - append binary
	
	//two_pass_assembly(fptr, fptr_2);		

	while (fgets(line, MAX_LINE_SIZE, fptr)) {
		if (*line == '\n') {
			break;
		}
		remove_newline(line);
		//printf("<<%s>>\n", line);
		label_present = tokenizer(line, label, mnemonic, operands, &num_operands);
		if (label_present) {
			strcpy(labels[i], label);
			memory_addresses[i] = mem;
			//printf("label after function check: %s is referring to line: %d\n", label, memory_addresses[i]);
			i++;
		} else {
			printf("mnemonic after function check: %s\n", mnemonic);
			printf("num_operands: %d\n", num_operands);
			for (int i = 0; i < num_operands; i++) {
				printf("operands[%d]: %s\n", i, operands[i]);
				operands[i] = remove_special_chars(operands[i]);
				printf("[no special chars] operands[%d]: %s\n", i, operands[i]);
				if (is_register(operands[i])) {
					//printf(" is a register\n");
				} else {
					//printf(" isn't a register\n");
				}
				//printf(" with value %u", get_operand_value(operands[i]));
				//printf("\n");
			}
			mem++;
		}	
		if (!label_present && (strcmp(mnemonic, "mul") == 0 || strcmp(mnemonic, "mla") == 0)) {
			//printf("binary instr.: 0x%8x\n", swap(multiply(mnemonic, operands)));
		}
	}

	printf("number of lines read: %d, (non-label: %d, labels: %d)\n", mem + i, mem, i);
	for (int j = 0; j < i; j++) {
		printf("Label: %s, has memory address %d\n", labels[j], memory_addresses[j]);
	}

	num_lines = mem;

	consts_size = 0;

	fclose(fptr);
	
	FILE* second_pass_fptr = fopen(argv[1], "r"); // "r" - read
	assert(second_pass_fptr != NULL && "Could not open file");

	mem = 0;
	
	uint32_t instruction;
	while (fgets(line, MAX_LINE_SIZE, second_pass_fptr)) {
		int target_address;
		if (*line == '\n') {
			break;
		}
		remove_newline(line);
		label_present = tokenizer(line, label, mnemonic, operands, &num_operands);
		if (label_present) {
			
		} else {
			switch (string_to_opcode(mnemonic) / 10) {
				case 1:
					instruction = data_processing(mnemonic, operands, &(constants[0]), &consts_size, num_lines, mem, num_operands);
					break;
				case 2:
					instruction = multiply(mnemonic, operands);
					break;
				case 3:
					instruction = single_data_transfer(mnemonic, operands, 0, &(constants[0]), &consts_size, num_lines, mem, num_operands);
					break;
				case 4:
					for (int j = 0; j < i; j++) {
						if (strcmp(labels[j], *operands) == 0) {
							target_address = memory_addresses[j] * 4;
						}
					}
					instruction = branch(mnemonic, mem * 4, target_address);
					break;
				case 5:
					instruction = special(mnemonic, operands, &(constants[0]), &consts_size, num_lines, mem, num_operands);
					break;
				default:
					printf("mnemonic not recognised\n");
					break;
			}
			mem++;
			fwrite(&instruction, sizeof(instruction), 1, fptr_2);
		}
		printf("loop success\n");
	}

	printf("consts_size: %d\n", consts_size);
	for (int j = 0; j < consts_size; j++) {
		printf("storing constant: 0x%8x at address %d\n", constants[j], (mem + j) * 4);
		fwrite(&(constants[j]), sizeof(uint32_t), 1, fptr_2);
	}

	printf("check\n");
	free(label);
	printf("check\n");
	free(mnemonic);
	printf("check\n");
	//free(operands[0]); // causes segfault - uncoment later and add a better fix
	printf("check\n");
	free(operands);
	printf("check\n");
	fclose(fptr_2);
	printf("check\n");
	fclose(second_pass_fptr);
	printf("check\n");
	return EXIT_SUCCESS;
}

