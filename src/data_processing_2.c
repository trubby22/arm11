uint32_t data_processing(void) {
	uint8_t opcode = string_to_opcode(mnemonic) - 10;
	bool immediate = has_hashtag(remove_whitespace(operands[1])) || has_equals(operands[1]);
	if (opcode <= 5) {
		immediate = has_hashtag(remove_whitespace(operands[2]));
	}

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
	
	// calculate the imm value and rotate value
	while (operand > 0xff) {
		operand = rotate_left(operand, 2);
		rotate++;
		if (rotate > 15) {
			printf("Error!\n");
			break;
		}
	}
	
	operand &= 0xff; // Imm value must be unsigned 8-bit
	operand = (rotate << 8) | operand;
	
	if (immediate) {
		result = result | 0x02000000; // operand2 is an immediate value
	}

	result |= string_to_opcode_dp(mnemonic) << 21; // gets the actual opcode
	result |= register_n << 16;
	result |= register_d << 12;
	result |= operand;

	result = result | 0xe0000000; // cond field = 0111

	return result;
}
