char* alloc_label (void) {
	char* label = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
	if (!label) {
		printf("Error!\n");
	}
	return label;
}

char* alloc_mnemonic (void) {
	char* mnemonic = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
	if (!mnemonic) {
		printf("Error!\n");
	}
	return mnemonic;
}

char** alloc_operands (void) {
	char** operands = (char**) malloc(MAX_LINE_SIZE * sizeof(char*));
	if (!operands) {
		printf("Error!\n");
	}
	operands[0] = (char*) malloc(MAX_LINE_SIZE * MAX_LINE_SIZE * sizeof(char));
	if (!operands[0]) {
		free(operands);
		printf("Error!\n");
	}
	for (int i = 1; i < MAX_LINE_SIZE; i++) {
		operands[i] = operands[i - 1] + MAX_LINE_SIZE;
	}
	return operands;
}

char* alloc_line (void) {
	char* line = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
	if (!line) {
		printf("Error!\n");
	}
	return line;
}
