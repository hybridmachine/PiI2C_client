#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <wiringPi.h>
#include <unistd.h>


// Assume pins in pulled up state on startup.
static int SCL = 1; 
static int SDA = 1;
// This is where SDA is left shifted into 
static int REG_DATA = 0;
// The address received on the I2C line
static int REG_ADDRESS = 0;
// The bit index (remember high order bits are shifted in first)
static int BIT_INDEX = 7; // Bits 7 downto 0 for 8 bit registers

const int sclPin = 1;
const int sdaPin = 0;
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
    //printf("SCL %d\n", SCL);
}

void interruptHandlerSDA (void) {
    SDA = digitalRead(sdaPin);
    //printf("SDA %d\n", SDA);
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

int risingEdge(int current, int incoming) {
    if (current == LOW && incoming == HIGH)
        return 1;

    return 0;
}

int fallingEdge(int current, int incoming) {
    if (current == HIGH && incoming == LOW)
        return 1;

    return 0;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <address in hex>\n", argv[0]);
		return -1;
	}

    int currentSCL = SCL;
    int currentSDA = SDA;

    printf ("Setting up wiringPi\n");	
    wiringPiSetup () ;

    pinMode(sclPin, INPUT);
    pinMode(sdaPin, INPUT);
    pullUpDnControl(sclPin, PUD_UP);
    pullUpDnControl(sdaPin, PUD_UP);

    printf ("Setting up interrupt handlers\n");
    setupInterruptHandlers ();

    enum FSM_I2C fsmCurrentState = IDLE;

    printf ("Interrupt handlers setup, starting I2C client state machine\n");
    // Loop, processing the I2C state machine
    for (;;) 
    {
        if (currentSCL == SCL && currentSDA == SDA)
        {
            continue;
        }

        switch(fsmCurrentState) {

            case IDLE:
                if (SCL == HIGH && fallingEdge(currentSDA, SDA))
                {
                    REG_DATA = 0;
                    REG_ADDRESS = 0;
                    printf ("Starting \n");
                    fsmCurrentState = STARTING;
                }
                break;
             
            case STARTING:
                if (SCL == LOW && SDA == LOW)
                {
		    printf ("Addressing \n");
                    fsmCurrentState = ADDRESSING;
                    BIT_INDEX = 7; 
                    REG_DATA = 0;
                }
                break;

            case ADDRESSING:
                if (risingEdge(currentSCL, SCL))
                {
                    // Shift in the current bit, remember I2C goes from MSB downto LSB 
                    // Bitwise and SDA with 0x01, it should always just be 1 or 0, but just in case      
                    REG_DATA = (REG_DATA << 1) | (SDA & 0x01);
                    BIT_INDEX--;

                    if (BIT_INDEX <= 0)
                    {
                        REG_ADDRESS = REG_DATA;
                        REG_DATA = 0;
                        BIT_INDEX = 7;
                        fsmCurrentState = IDLE;
                        // For debug, print address then wait for next start
                        printf("Address: 0x%02x\n", REG_ADDRESS);
                    }
                }
                break;

            case RECEIVING:
                break;

            case STOPPING:
                break;

            default:
                continue;
        }

        currentSCL = SCL;
        currentSDA = SDA;

        usleep(10);
    }

	uint32_t address = hexStringToUint(argv[1]);
	printf("Hello I2C %d!\n", address);
	return 0;
}
