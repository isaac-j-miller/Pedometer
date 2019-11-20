/*
 * Helper.h
 *
 * Created: 10/14/2019 6:34:13 PM
 *  Author: Isaac Miller
 */ 

#ifndef HELPER_H_
#define HELPER_H_
#include <Arduino.h>

uint8_t modifyBit(uint8_t inputByte, uint8_t bitPosition, bool newValue);

uint16_t intPow(uint16_t base, uint16_t exponent);

#endif /* HELPER_H_ */