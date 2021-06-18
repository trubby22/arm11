#ifndef HELPER_TOOLS_H
#define HELPER_TOOLS_H

uint32_t read_ram(uint16_t address);
void write_ram(uint16_t address, uint32_t value);
void set_bit(uint32_t* number, uint8_t bit, bool set);
bool cond(uint8_t code);
uint32_t fetch(void);
uint32_t fetch_pre(void);
void print_state(uint16_t program_size);

#endif
