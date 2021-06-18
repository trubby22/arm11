#include <stdbool.h>
#include <stdint.h>
#include "text_handling.h"
#include "sdt_helpers.h"

bool has_expression(char* operand) {
	operand = remove_whitespace(operand);
	return has_equals(operand);
}

bool is_post(char* operand) { 
	//Looks like: [Rn],<#expression>
	for (int i = 0; operand[i] != '\0'; i++) {
		if (operand[i] == ']') {
			return true;
		}
		if (operand[i] == ',') {
			return false;
		}
	}
	return false;
}
