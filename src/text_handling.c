// splits line into label, opcode, operands and operand count
// returns true when it is a label, false otherwise
bool tokenizer(char* line, char* label, char* mnemonic, char** operands, uint32_t* num_operands) {
	char* token = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
	if (!token) {
		printf("Error!\n");
	}
	char str[MAX_LINE_SIZE];
	for (int i = 0; i < MAX_LINE_SIZE; i++) {
		str[i] = *line;
		line += sizeof(char);
	}
	char str_org[MAX_LINE_SIZE];
	memcpy(str_org, str, MAX_LINE_SIZE);
	token = strtok(str, ":");

	// strings are different
	if (strcmp(str_org, token) != 0) {
		// we get a label
		memcpy(label, token, MAX_LINE_SIZE);
		return true;
	}

	// we get a mnemonic
	token = strtok(str, " ");
	memcpy(mnemonic, token, MAX_LINE_SIZE);
	memcpy(str, token, MAX_LINE_SIZE);
	int n = 0;
	token = strtok(NULL, ",");

	while (token != NULL) {
		strcpy(operands[n], token);
		token = strtok(NULL, ",");
		n++;
	}
	*num_operands = n;

	free(token);
	return false;
}

// the functions below are useful in the 4 instruction functions: DP, SDT, multiply, branch

bool is_register(char* operand) {
	if (operand[0] == 'r') {
		return true;
	}
	return false;
}

bool has_equals(char* operand) {
	return (*operand == '=');
}

bool has_sq_brackets(char* operand) {
	return (*operand == '[');
}

bool has_hashtag(char* operand) {
	return (*operand == '#');
}

bool has_whitespace(char* operand) {
	return (*operand == ' ');
}

bool begins_with_sq_bracket(char* operand) {
	return (*operand == '[');
}

bool ends_with_sq_bracket(char* str) {
	int i;
	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] == ']') {
			return true;
		}
	}
	return false;
}

bool minus(char* str) {
	return (*str == '-');
}

char* remove_equals(char* operand) {
	if (has_equals(operand)) {
		operand += sizeof(char);
	}
	return operand;
}

char* remove_sq_brackets(char* operand) {
	if (has_sq_brackets(operand)) {
		operand += sizeof(char);
		int i;
		for (i = 0; operand[i] != ']'; i++) {
			
		}
		operand[i] = '\0';
	}
	return operand;
}

char* remove_hashtag(char* operand) {
	if (has_hashtag(operand)) {
		operand += sizeof(char);
	}
	return operand;
}

char* remove_whitespace(char* operand) {
	if (has_whitespace(operand)) {
		operand += sizeof(char);
	}
	return operand;
}

char* remove_l_bracket(char* operand) {
	if (begins_with_sq_bracket(operand)) {
		operand += sizeof(char);
	}
	return operand;
}

char* remove_r_bracket(char* str) {
	int i;
	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] == ']') {
			str[i] = '\0';
		}
	}
	return str;
}

char* remove_minus(char* str) {
	if (minus(str)) {
		str += sizeof(char);
	}
	return str;
}

char* remove_special_chars(char* operand) {
	operand = remove_whitespace(operand);
	operand = remove_equals(operand);
	operand = remove_sq_brackets(operand);
	operand = remove_hashtag(operand);
	return operand;
}

// takes operand string and returns either decimal register number or decimal immediate value
uint32_t get_operand_value(char* operand) {
	// register
	operand = remove_special_chars(operand);
	if (is_register(operand)) {
		operand += sizeof(char);
		return strtol(operand, NULL, 10);
	}
	operand += sizeof(char);
	// hex immediate
	if (*operand == 'x') {
		operand += sizeof(char);
		return strtol(operand, NULL, 16);
	}
	// decimal immediate
	operand -= sizeof(char);
	return strtol(operand, NULL, 10);
}

// we assert the string ends with a \n
void remove_newline(char* str) {
	int i;
	for (i = 0; str[i] != '\n'; i++) {		

	}
	str[i] = '\0';
}


