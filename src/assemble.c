#include <stdlib.h>

// maps labels to memory addresses and expected operand count
// could use function pointers with the symbol_table
Typedef struct symbol_table {
    char* [] labels;
    uint32_t[] addresses;
    Uint32_t[] operand_count;
    // char*[] opcodes;
    // function*[] functions;
}

// an attempt at function pointers - probably should look different
(function*)func_from_opcode(uint32_t opcode) {
    return (function*)ptr;
}

// splits line into label, opcode, operands and operand count
void tokenizer(char* line, char* label, int* opcode, uint32_t** operands, uint32_t* operand_count) {
    … = strtok_r();
    … = strtol();
}

// returns relevant binary instruction
uint32_t dataProcessing(char* label, int* opcode, uint32_t** operands, uint32_t operand_count) {
}

uint32_t multiply(char* label, int* opcode, uint32_t** operands, uint32_t operand_count) {
}

uint32_t singleDataTransfer(char* label, int* opcode, uint32_t** operands, uint32_t operand_count) {
}

uint32_t branch(char* label, int* opcode, uint32_t** operands, uint32_t operand_count) {
}

symbol_table* first_pass(FILE* fp) {
    //fill in symbol table
    return (symbol_table*)table;
}

void second_pass(FILE* fp, symbol_table* table) {
    // write binary to file
}

void two_pass_assembly(FILE* fp, FILE* fp_2) {

    symbol_table* table = first_pass(fp);
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

