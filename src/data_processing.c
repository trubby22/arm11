void dataProcessing(uint32_t instruction) {
	uint32_t operand2 = instruction & 0xfff;
	uint8_t register_d = (instruction >> 12) & 0xf;
	uint8_t register_n = (instruction >> 16) & 0xf;
	bool set_condition = (instruction >> 20) & 0x1;
	uint8_t opcode = (instruction >> 21) & 0xf;
	bool immediate_operand = (instruction >> 25) & 0x1;

	uint64_t result = 0;

	bool is_arithmetic = false;
	bool is_addition = false;
	bool write_result = false;

	if (immediate_operand) {
		// operand2 is an immediate constant
		uint8_t rotate = operand2 >> 8;
		uint8_t imm = operand2 & 0xff;
		operand2 = rotate_right(imm, rotate << 1); // Shifted 1 to the left to multiply by 2
	} else {
		operand2 = shift(operand2, set_condition); //operand2 is a shifted register
	}

	switch (opcode) {
	case 0x0: //and
		result = Registers[register_n] & operand2;
		write_result = true;
		break;
	case 0x1: //eor
		result = Registers[register_n] ^ operand2;
		write_result = true;
		break;
	case 0x2: //sub
		result = Registers[register_n] - operand2;
		is_arithmetic = true;
		write_result = true;
		break;
	case 0x3: //rsb
		result = operand2 - Registers[register_n];
		is_arithmetic = true;
		write_result = true;
		break;
	case 0x4: //add
		result = (uint64_t)operand2 + Registers[register_n];
		is_addition = true;
		is_arithmetic = true;
		write_result = true;
		break;
	case 0x8: //tst
		result = Registers[register_n] & operand2;
		break;
	case 0x9: //teq
		result = Registers[register_n] ^ operand2;
		break;
	case 0xa: //cmp
		result = Registers[register_n] - operand2;
		is_arithmetic = true;
		break;
	case 0xc: //orr
		result = Registers[register_n] | operand2;
		write_result = true;
		break;
	case 0xd: //mov
		result = operand2;
		write_result = true;
		break;
	}

	if (write_result) {
		Registers[register_d] = (uint32_t)result;
	}

	if (set_condition) {
		// For C bit: if logic operation -> set to carry out from any shift operation (result from barrel shifter)
		//            if arithmetic      -> set the the carry out from the ALU (1 for addition if there was a carry,
		//																		0 for subtraction if there was a borrow)
		if (is_arithmetic) {
			if (is_addition) {
				set_bit(Registers + CPSR_REGISTER, CPSR_C, result > 0xffffffff);
			} else {
				// result >= 0 --> no borrow; result < 0 --> borrow
				set_bit(Registers + CPSR_REGISTER, CPSR_C, result <= 0x80000000);
			}
		}

		// Z = 1 if the result is all 0s 
		if (result == 0) {
			set_bit(Registers + CPSR_REGISTER, CPSR_Z, 1);
		} else {
			set_bit(Registers + CPSR_REGISTER, CPSR_Z, 0);
		}
		// N = logical value of 31st bit of result
		set_bit(Registers + CPSR_REGISTER, CPSR_N, (result >> 31) & 1);
	}
}
