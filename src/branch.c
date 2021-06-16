void branch(uint32_t instruction) {

	uint32_t offset = (instruction & 0xffffff) << 2;
	bool negative = (offset >> 25) & 0x1;
	if (negative) {
		offset = (~offset) + 1;
		offset &= 0xffffff;
		Registers[PC_REGISTER] -= offset;
	} else {
		Registers[PC_REGISTER] += offset;
	}
}
