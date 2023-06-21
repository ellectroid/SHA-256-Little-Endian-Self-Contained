#include "sha256.hpp"
int main() {
	uint8_t Data[64] = { 0 };
	Data[0] = 'H';
	Data[1] = 'e';
	Data[2] = 'l';
	Data[3] = 'l';
	Data[4] = 'o';
	Data[5] = ',';
	Data[6] = ' ';
	Data[7] = 'w';
	Data[8] = 'o';
	Data[9] = 'r';
	Data[10] = 'l';
	Data[11] = 'd';
	Data[12] = '!';
	uint8_t Data_SHA256[32];

	/* The first 32-bit word of the real hash of string 'Hello, world!' 13 characters long (no null termination) is: 0x315F5BDB */

	SHA256(Data_SHA256, Data, 13);
	return 0;
}
