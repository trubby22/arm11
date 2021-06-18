#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

uint32_t arithmetic_right(uint32_t register_value, uint8_t shift_value);
uint32_t rotate_right(uint32_t register_value, uint8_t shift_value);
uint32_t shift(uint32_t offset, bool set_condition);

#endif
