#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <wiringPi.h>

// Assume pins in pulled up state on startup.
static int SCL = 1; 
static int SDA = 1;
// This is where SDA is left shifted into 
static int REG_DATA = 0;
// The address received on the I2C line
static int REG_ADDRESS = 0;
// The bit index (remember high order bits are shifted in first)
static int BIT_INDEX = 7; // Bits 7 downto 0 for 8 bit registers

const int sclPin = 0;
const int sdaPin = 1;
const int ACK_VAL = 0;

enum FSM_I2C {
    IDLE,
    STARTING,
    ADDRESSING,
    RECEIVING,
    STOPPING
};

void interruptHandler_SCL (void) { 
    SCL = digitalRead(sclPin);
}

void interruptHandlerSDA (void) {
    SDA = digitalRead(sdaPin);
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

void setupInterruptHandlers(void) {
	wiringPiISR (0, INT_EDGE_BOTH, &interruptHandler_SCL) ;
	wiringPiISR (1, INT_EDGE_BOTH, &interruptHandlerSDA) ;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <address in hex>\n", argv[0]);
		return -1;
	}

    int currentSCL = SCL;
    int currentSDA = SDA;
	
	wiringPiSetup () ;
    setupInterruptHandlers ();

    enum FSM_I2C fsmCurrentState = IDLE;

    // Loop, processing the I2C state machine
    for (;;) 
    {
        if (currentSCL == SCL && currentSDA == SDA)
        {
            continue;
        }

        switch(fsmCurrentState) {

            case IDLE:
                break;
             
            case STARTING:
                break;

            case ADDRESSING:
                break;

            case RECEIVING:
                break;

            case STOPPING:
                break;

            default:
                continue;
        }
    }

	uint32_t address = hexStringToUint(argv[1]);
	printf("Hello I2C %d!\n", address);
	return 0;
}
