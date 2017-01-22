//CREATED BY HADI NOURALLAH

#include <stdlib.h>
#include <stdio.h>

volatile int * HEX = (int *) 0x10000020; // 7 Segment display address
volatile int * red_LED_ptr = (int *) 0x10000000; // red LED address
volatile int * green_LED_ptr = (int *) 0x10000010; // green LED address
volatile int * SW_switch_ptr = (int *) 0x10000040; // SW slider switch address

int delay_count,KEY_value;
volatile int * KEY_ptr = (int *) 0x10000050; // pushbutton KEY 
volatile short didStart = 0;
volatile short findWinner = 0;

int rand1;
int rand2;
int rand3;

signed int getRandom()
{
    int r;
    const signed int range = 1 + 7 - 1;
    const signed int buckets = RAND_MAX / range;
    const signed int limit = buckets * range;
    
    do
    {
        r = rand();
    } while (r >= limit);
    
    return 1 + (r / buckets);
}

long int binToHex(long int binaryval) {
    long int hexadecimalval = 0, i = 1, remainder;

    while (binaryval != 0)
    {
        remainder = binaryval % 10;
        hexadecimalval = hexadecimalval + remainder * i;
        i = i * 2;
        binaryval = binaryval / 10;
    }
    return hexadecimalval;
}

long prepareBinary(long bin) {
    long b = bin % 100000000;
    return b;
}

long long combine(long r1, long r2, long r3) {
    long first = binToHex(prepareBinary(r1));
    long second = binToHex(prepareBinary(r2));
    long third = binToHex(prepareBinary(r3));
    
    long long comb = first;
    comb = comb << 8;
    comb += second;
    comb = comb << 8;
    comb += third;
    return comb;
}

void start(const long numbers[]) {
	int sw_value = *(SW_switch_ptr);
	didStart = 1;
	rand1 = (findWinner > 30) ? ((sw_value == 0) ? rand1 : numbers[7]) : numbers[getRandom()];
    rand2 = (findWinner > 15) ? ((sw_value == 0) ? rand2 : numbers[7]) : numbers[getRandom()];
    rand3 = (findWinner > 1)  ? ((sw_value == 0) ? rand3 : numbers[7]) : numbers[getRandom()];
    
    long long r1 = combine(rand1, rand2, rand3);
	*HEX = r1;
	if (findWinner > 0) findWinner++;
	for (delay_count = 2000000; delay_count != 0; --delay_count); // delay loop
	if (findWinner > 30) {
		if ((rand1 & rand2 & rand3) == rand1 || sw_value != 0) {
			*green_LED_ptr = 0xFF;
		} else {
			*red_LED_ptr = 0xFFFFFFFF;
		}
	}
}

void getWinner() {
	if (findWinner == 0) findWinner = 1;
}

void restart(const long numbers[]) {
	didStart = 0;
	int zero = numbers[0];
	findWinner = 0;
    long long r1 = combine(zero, zero, zero);
	*HEX = r1;	
	*green_LED_ptr = 0x00;
	*red_LED_ptr = 0x00;
		
}

int main()
{
    const long numbers[] = { 500111111, 500000110, 501011011, 501001111, 501100110, 501101101, 501111101, 500000111, 501111111, 501100111 };
    while(1){
		KEY_value = *(KEY_ptr); // read the pushbutton KEY values
	        if (KEY_value != 0) // check if any KEY was pressed
			{
				if (KEY_value == 2) {
					start(numbers);
				} else if (KEY_value == 4 ) {
					restart(numbers);
				} else if (KEY_value == 8) {
					getWinner();
				}
				while (*KEY_ptr); // wait for pushbutton KEY release
			}
			if (didStart == 1) start(numbers);
			else restart(numbers);
	}			
    
    return 0;
}
