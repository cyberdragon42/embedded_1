#include "spi.c"

uint8_t status1; 
uint8_t _status2; 


int main(void)
{
	uint8_t arr[] = {0x01, 0x02, 0x05, 0x66, 0x17, 
										 0xA8, 0x09, 0xAA, 0x03, 0x04};
	InitializeClock();									 
	Initialize();
	status1 = ReadRegister();
	WriteToRegister(0);
	_status2 = ReadRegister();
	Clear();
										 
	WriteDataArray(0x000000, arr);								 
	while(1)
	{
		
	}
}