#define MAX_LINE_SIZE 511
#define NUM_OPCODES 24
#define MAX_LABELS_COUNT 100
#define MAX_CONSTANTS_COUNT 100

char* line;
char* label;
char* mnemonic;
char** operands;
uint32_t num_operands;
uint32_t constants[MAX_CONSTANTS_COUNT] = { 0 };
uint32_t consts_size;
uint32_t num_lines;
uint32_t curr_line;
uint32_t last_line;

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
	{"orr", 15},
	{"mov", 16},
	{"tst", 17},
	{"teq", 18},
	{"cmp", 19},
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


// represents the true opcode of data processing mnemonics
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
