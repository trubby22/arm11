#ifndef __ALLOCATE_H__
#define __ALLOCATE_H__

#include <stdint.h>

char* alloc_label(void);

char* alloc_mnemonic(void);

char** alloc_operands(void);

char* alloc_line(void);

#endif //__ALLOCATE_H__