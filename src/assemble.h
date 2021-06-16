#ifndef ASSEMBLE_H_
#define ASSEMBLE_H_

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
uint8_t string_to_opcode(char* key);
int string_to_opcode_dp(char* key);
uint32_t rotate_left(uint32_t value, uint8_t shift);
uint32_t data_processing();
uint32_t multiply();
bool has_expression(char* operand);
bool is_post(char* operand);
uint32_t single_data_transfer();
uint32_t branch(const int target_address);
uint32_t andeq(void);
uint32_t special_lsl();
uint32_t special();
void remove_newline(char* str);
char* alloc_label (void);
char* alloc_mnemonic (void);
char** alloc_operands (void);
char* alloc_line (void);
int main(int argc, char** argv);

#endif
