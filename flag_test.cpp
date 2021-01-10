#include<iostream>

#define SET_FLAG(bitfield, flag) bitfield = bitfield | flag
#define UNSET_FLAG(bitfield, flag) bitfield = bitfield & ~(flag)
#define FLAG_IS_SET(bitfield, flag) (bitfield & (flag))
#define VISITED ((unsigned)(1<<8))
int main(){
	uint8_t x = 0;
	std::cout << sizeof(uint8_t) << std::endl;
	std::cout << sizeof(1<<8) << std::endl;
	std::cout << sizeof(unsigned int) << std::endl;
	SET_FLAG(x, VISITED);
	x |= 1<<7;
	std::cout << unsigned(x) << std::endl;
}
