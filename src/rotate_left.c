#include <stdint.h>
#include "rotate_left.h"

// bitwise circular left rotation for calculating rotate and operand fields of operand2 
uint32_t rotate_left(uint32_t value, uint8_t shift) {
	 if ((shift &= (sizeof(value) << 3) - 1) == 0)
      return value;
    return (value << shift) | (value >> ((sizeof(value) << 3) - shift));
}
