#define RAM_SIZE 65536
#define PC_REGISTER 15
#define CPSR_REGISTER 16
#define CPSR_N 31
#define CPSR_Z 30
#define CPSR_C 29
#define CPSR_V 28
#define REGISTER_COUNT 17

uint8_t* Ram;
uint16_t start;
uint32_t Registers[REGISTER_COUNT] = { 0 };
