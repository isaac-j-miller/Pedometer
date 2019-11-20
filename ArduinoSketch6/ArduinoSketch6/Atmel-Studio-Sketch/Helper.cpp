/*
 * Helper.cpp
 *
 * Created: 10/14/2019 7:20:43 PM
 *  Author: Isaac Miller
 */ 
#include "Helper.h"

uint8_t modifyBit(uint8_t inputByte, uint8_t bitPosition, bool newValue){
	uint8_t mask = 1 << bitPosition;
	return (inputByte & ~ mask) | ((newValue << bitPosition) & mask);
}

uint16_t intPow(uint16_t base, uint16_t exponent){
	uint16_t answer = 1;
	for(int i = 0; i<exponent; i++){
		answer*=base;
	}
	return answer;
}