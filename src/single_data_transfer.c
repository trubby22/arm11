void single_data_transfer(uint32_t instruction) {
	bool immediate_offset = (instruction >> 25) & 0x1;
	bool pre_indexing = (instruction >> 24) & 0x1;
	bool up = (instruction >> 23) & 0x1;
	bool load = (instruction >> 20) & 0x1;
	uint8_t register_n = (instruction >> 16) & 0xf; // holds memory address
	uint8_t register_m = (instruction >> 12) & 0xf;  // holds value
	uint32_t offset = instruction & 0xfff;
	uint32_t memory = Registers[register_n];

	if (immediate_offset) {
		offset = shift(offset, false);
	}

	if (up) {
		offset = Registers[register_n] + offset;
	} else {
		offset = Registers[register_n] - offset;
	}

	if (pre_indexing) {
		memory = offset;
	}

	if (memory < 0 || memory >= RAM_SIZE) {
		printf("Error: Out of bounds memory access at address 0x%08x\n", memory);
		return;
	}

	if (load) {
		Registers[register_m] = read_ram(start + memory);
	} else {
		write_ram(start + memory, Registers[register_m]);
	}

	if (!pre_indexing) {
		Registers[register_n] = offset;
	}
}
