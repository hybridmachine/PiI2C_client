#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void SCL_Interrupt (void) { 
	// If idle, check SDA status and if matching start condtion, transition to start
	// If running, shift bit into data register
	// If running and data register full (on bit 9), send ACK
}

void SDA_Interrupt (void) {
	// If idle and bit low, set starting
	// If running, set SDA bit val (will be shifted in on next clock)
}

uint32_t hexStringToUint(const char* hexString) {
    char* endptr;
    unsigned long result = strtoul(hexString, &endptr, 16);
    if (endptr == hexString || *endptr != '\0') {
	printf("Invalid hex string %s!\n", hexString); // Invalid hex string
        return 0;
    }
    return (uint32_t)result;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <address in hex>\n", argv[0]);
		return -1;
	}

	uint32_t address = hexStringToUint(argv[1]);
	printf("Hello I2C %d!\n", address);
	return 0;
}
