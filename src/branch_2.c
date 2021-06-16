// the target address passed in to this function is either a specific 32-bit 
// address or a label.
uint32_t branch(const int target_address) {
	uint32_t  result, cond = string_to_opcode(mnemonic) % 10;
	int32_t offset;
	uint32_t current_address = curr_line * 4;
	const uint32_t unchanged_bits = 0xa << 24;

	if (strcmp("b", mnemonic) == 0) {
		cond--;
	}
	if (cond > 1) {
		// cond = bge, blt, bgt, ble, OR bal
		cond |= 0x8;
	}

	offset = target_address - current_address - 8;
	offset &= 0x3ffffff; // make offset 26 bits in length
	offset >>= 2;
	offset &= 0xffffff; // store the lower 24 bits
	
	result = (cond << 28) | unchanged_bits | offset;
	return result;
}
