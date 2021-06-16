uint8_t string_to_opcode(char* key) {
	for (uint8_t i = 0; i < NUM_OPCODES; i++) {
		if (strcmp(opcode[i].str, key) == 0) {
			return opcode[i].n;
		}
	}
	return 0;
}

// works just as string_to_opcode but using the opcode_dp struct
int string_to_opcode_dp(char* key) {
	int i = 0;
	char* name = opcode_dp[i].str;
	while (name) {
		if (strcmp(name, key) == 0)
			return opcode_dp[i].n;
		name = opcode_dp[++i].str;
	}
	return 0;
}
