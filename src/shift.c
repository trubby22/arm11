uint32_t arithmetic_right(uint32_t register_value, uint8_t shift_value) {
	bool negative = register_value >> 31 & 0x1;
	uint32_t result = register_value >> shift_value;

	if (negative) {
		for (uint32_t i = 0; i < shift_value; i++) {
			result |= 1 << (31 - i);
		}
	}

	return result;
}

uint32_t rotate_right(uint32_t register_value, uint8_t shift_value) {
	uint32_t shifted_value = register_value >> shift_value;
	uint32_t rotated_value = register_value << (32 - shift_value);
	return shifted_value | rotated_value;
}

uint32_t shift(uint32_t offset, bool set_condition) {
	uint8_t register_m = offset & 0xf;
	bool variable = (offset >> 4) & 0x1;
	uint8_t shift_type = (offset >> 5) & 0x3;
	uint8_t shift_value = (offset >> 7) & 0x1f;

	uint32_t register_value = Registers[register_m];

	if (variable) {
		uint8_t register_s = (shift_value >> 1) & 0xf;
		assert(register_s != PC_REGISTER && "shift register cannot be PC_REGISTER");
		shift_value = Registers[register_s] & 0xff;
	}

	if (set_condition && shift_value != 0) {
		if (shift_type == 0) {
			set_bit(Registers + CPSR_REGISTER, CPSR_Z, (register_value >> (31 - shift_value)) & 0x1);
		} else {
			set_bit(Registers + CPSR_REGISTER, CPSR_Z, (register_value >> shift_value) & 0x1);
		}
	}

	switch (shift_type) {
	case 0x0:
		return register_value << shift_value;
	case 0x1:
		return register_value >> shift_value;
	case 0x2:
		return arithmetic_right(register_value, shift_value);
	case 0x3:
		return rotate_right(register_value, shift_value);
	}

	return 0;
}
