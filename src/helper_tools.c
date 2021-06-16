uint32_t read_ram(uint16_t address) {
	return *(uint32_t*)(Ram + address);
}

void write_ram(uint16_t address, uint32_t value) {
	*(uint32_t*)(Ram + address) = value;
}

void set_bit(uint32_t* number, uint8_t bit, bool set) {
	if (set) {
		*number |= (1 << bit);
		return;
	}
	*number &= ~(1 << bit);
}

// checks if flags stored in CPSR register satisfy a condition code

bool cond(uint8_t code) {
	bool n = (Registers[CPSR_REGISTER] >> CPSR_N) & 0x1;
	bool z = (Registers[CPSR_REGISTER] >> CPSR_Z) & 0x1;
	bool v = (Registers[CPSR_REGISTER] >> CPSR_V) & 0x1;

	switch (code) {
	case 0x0:
		return z;
	case 0x1:
		return !z;
	case 0xa:
		return n == v;
	case 0xb:
		return n != v;
	case 0xc:
		return !z && (n == v);
	case 0xd:
		return z | (n != v);
	case 0xe:
		return true;
	}
	return false;
}

uint32_t fetch(void) {
	if (Registers[PC_REGISTER] >= RAM_SIZE) { 
		return 0;
	}

	uint32_t fetched = read_ram(start + Registers[PC_REGISTER]);
	Registers[PC_REGISTER] += 4;
	return fetched;
}

// works just as fetch but increments PC BEFORE reading memory

uint32_t fetch_pre(void) {
	if (Registers[PC_REGISTER] >= RAM_SIZE) {
		return 0;
	}

	Registers[PC_REGISTER] += 4;
	uint32_t fetched = read_ram(start + Registers[PC_REGISTER]);
	return fetched;
}

// swap used to convert from big endian to little endian

uint32_t swap(uint32_t num) {
	uint32_t swapped = ((num >> 24) & 0xff) | // move byte 3 to byte 0
		((num << 8) & 0xff0000) | // move byte 1 to byte 2
		((num >> 8) & 0xff00) | // move byte 2 to byte 1
		((num << 24) & 0xff000000); // byte 0 to byte 3
	return swapped;
}

void print_state(uint16_t program_size) {
	printf("Registers:\n");
	uint16_t i;
	for (i = 0; i <= 9; i++) {
		printf("$%d  : %10d (0x%08x)\n", i, Registers[i], Registers[i]);
	}
	for (i = 10; i <= 12; i++) {
		printf("$%d : %10d (0x%08x)\n", i, Registers[i], Registers[i]);
	}

	printf("PC  : %10d (0x%08x)\n", Registers[PC_REGISTER], Registers[PC_REGISTER]);
	printf("CPSR: %10d (0x%08x)\n", Registers[CPSR_REGISTER], Registers[CPSR_REGISTER]);
	printf("Non-zero memory:\n");
	for (i = 0; i <= RAM_SIZE-4; i += 4) {
		if (read_ram(i) != 0) {
			printf("0x%08x: 0x%08x\n", i, swap(read_ram(i))); // if you want to view numbers in regular big endian format, delete swap here
		}
		if (i == RAM_SIZE-4) {
			break;
		}
	}
}
