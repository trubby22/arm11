uint32_t single_data_transfer(void) {
	bool preindexing = true;

	if (num_operands == 3 && ends_with_sq_bracket(operands[1])) {
		preindexing = false;
	}
		
	bool immediate = true;
	bool is_post;
	operands[1] = remove_whitespace(operands[1]);
	is_post = has_sq_brackets(operands[1]);
	uint32_t offset = 0;

	if (has_sq_brackets(operands[1])) {
		operands[1] = remove_sq_brackets(operands[1]);
		immediate = false;
	}

	if (num_operands == 3 && is_register(operands[2])) {
		immediate = true;
	}
	uint32_t address = get_operand_value(operands[1]);

	if (strcmp(mnemonic, "ldr") == 0 && !is_register(operands[1])) { // If expression is a constant
		if (address <= 0xFF) {
			strcpy(mnemonic, "mov");
			return data_processing();
		} else {	
			constants[consts_size] = get_operand_value(operands[1]);
			offset = ((num_lines + (consts_size)) - curr_line) * 4 - 8;
			consts_size = consts_size + 1;
			strcpy(operands[1], "[r15");
			if (sprintf(operands[2], "#%d]", offset) < 0) {
				printf("Error!\n");
			}
			num_operands++;
			return single_data_transfer();
		}
	}

	// operand[1] is a register
	
	bool up = true;
	
	bool load = strcmp(mnemonic, "ldr") == 0;
	uint32_t register_n = get_operand_value(operands[1]);
	uint32_t register_d = get_operand_value(operands[0]);
	uint32_t result = 0xe4000000;
	operands[2] = remove_hashtag(operands[2]);

	if (num_operands == 3 && minus(operands[2])) {
		up = false;
		operands[2] = remove_minus(operands[2]);
	}

	if (num_operands == 3) {
		offset = get_operand_value(operands[2]);
	}

	result |= immediate << 25;
	result |= preindexing << 24;
	result |= up << 23;
	result |= load << 20;
	result |= register_n << 16;
	result |= register_d << 12;
	result |= offset;

	return result;
}
