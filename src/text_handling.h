#include <stdbool.h>

#ifndef TEXT_HANDLING_H
#define TEXT_HANDLING_H

bool tokenizer(char* line, char* label, char* mnemonic, char** operands, uint32_t* num_operands);
bool is_register(char* operand);
bool has_equals(char* operand);
bool has_sq_brackets(char* operand);
bool has_hashtag(char* operand);
bool has_whitespace(char* operand);
bool begins_with_sq_bracket(char* operand);
bool ends_with_sq_bracket(char* str);
bool minus(char* str);
char* remove_equals(char* operand);
char* remove_sq_brackets(char* operand);
char* remove_hashtag(char* operand);
char* remove_whitespace(char* operand);
char* remove_l_bracket(char* operand);
char* remove_r_bracket(char* str);
char* remove_minus(char* str);
char* remove_special_chars(char* operand);
uint32_t get_operand_value(char* operand);
void remove_newline(char* str);

#endif
