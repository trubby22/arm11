#ifndef EMULATE_H_
#define EMULATE_H_

/*
uint32_t read_ram(uint16_t address);
void write_ram(uint16_t address, uint32_t value);
void set_bit(uint32_t* number, uint8_t bit, bool set);
bool cond(uint8_t code);
uint32_t arithmetic_right(uint32_t register_value, uint8_t shift_value);
uint32_t rotate_right(uint32_t register_value, uint8_t shift_value);
uint32_t shift(uint32_t offset, bool set_condition);
void dataProcessing(uint32_t instruction);
void multiply(uint32_t instruction);
void singleDataTransfer(uint32_t instruction);
void branch(uint32_t instruction);
uint32_t fetch(void);
uint32_t fetch_pre(void);
uint32_t swap(uint32_t num);
void print_state(uint16_t program_size);
*/
int main(int argc, char* argv[]);

#endif
