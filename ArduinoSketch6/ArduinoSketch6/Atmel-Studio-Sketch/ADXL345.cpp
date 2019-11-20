/* 
* ADXL345.cpp
*
* Created: 10/15/2019
* Author: Isaac Miller
*/


#include "ADXL345.h"

// default constructor
ADXL345::ADXL345()
{
	//DO NOTHING HERE
}

// default destructor
ADXL345::~ADXL345()
{
	//DO NOTHING HERE
}

void ADXL345::setup(int _CS_PIN,int _WAKEUP_PIN, ADXL_RANGE _RANGE/*=ADXL_RANGE::TWO_G*/, ADXL_RESOLUTION _RESOLUTION/*=NORMAL_RESOLUTION*/, ADXL_RATE _RATE)
{
	
	//***Step #0***
	//record where the CS is attached, the current range, and resolution
	//set the CS pin to be an OUTPUT HIGH
	csPin=_CS_PIN;
	wakeUpPin = _WAKEUP_PIN;
	currentRange=_RANGE;
	currentResolution=_RESOLUTION;
	currentRate = _RATE;
	
	pinMode(csPin, OUTPUT);
	digitalWrite(csPin, HIGH);
	//***Step #1***
	//call SPI.begin() to initialize the SPI interface
	SPI.begin();
	
	//***Step #2***
	//use SPI.setDataMode() to select the appropriate SPI mode for operations. 
	//See: https://www.arduino.cc/en/Reference/SPISetDataMode and the lecture notes
	SPI.setDataMode(SPI_MODE3);
	//reset data format
	writeRegister(DATA_FORMAT,0x00);
	//reset power control
	writeRegister(POWER_CTRL,0x00);
	//configure interrupts
	writeRegister(INT_ENABLE,INT_ENABLE_FORMAT);
	writeRegister(INT_MAP,INT_MAP_FORMAT);
	
	//configure tap
	writeRegister(THRESH_TAP, 200);
	writeRegister(WINDOW, 3);
	writeRegister(TAP_DUR, 1);
	writeRegister(TAP_LATENCY,0);
	writeRegister(TAP_AXES, 0x01);
	//configure active/inactive
	writeRegister(THRESH_ACT,15);
	writeRegister(ACT_CRTL,0xFF);
	writeRegister(TIME_INACT, 5); //sleep after 5 seconds of inactivity
	writeRegister(THRESH_INACT,15);
	//***Step #3 (Optional) ***
	//write to the DATA_FORMAT register to adjust the sampling resolution
	setResolution(_RESOLUTION);
	//***Step #4 (Optional) ***
	//write to the DATA_FORMAT register to adjust the sampling range
	setRange(_RANGE);
	//set data sample rate
	setRate(_RATE);
	//***Step #5***
	//write to the MEASURE bit in the POWER_CLT register
	//to begin sampling. Ensure the write does not modify other bits by
	//reading the current value, modifying the data locally, and then writing
	//back the new register value.
	wakeUp();
	
	
	
}

void ADXL345::writeRegister(uint8_t address, uint8_t value)
{
	//bring CS pin LOW to begin communication
	digitalWrite(csPin, LOW);
	
	//send address of register to be written
	SPI.transfer(address);
	
	//send new value to be written to the register
	SPI.transfer(value);
	
	//bring CS pin HIGH to end communication
	digitalWrite(csPin, HIGH);

	return;
}

byte ADXL345::readRegister(uint8_t address)
{
	//modify the address to set bit 7 HIGH to indicate a read
	address |= 0x80; //set MSB high for READ
	
	//bring the CS pin low to begin communication
	digitalWrite(csPin, LOW);
	
	//transfer the register address
	SPI.transfer(address);
	
	//transfer bogus data so the accelerometer can respond
	uint8_t rx = SPI.transfer(0x0);
	
	//bring CS pin HIGH to end communication
	digitalWrite(csPin, HIGH);
	
	//return the data read from the register
	return rx;
}

bool ADXL345::selfTest()
{
	
	//implement code to check if you can
	//read the DEVICE_ID register. Return TRUE if
	//the correct result is found, otherwise FALSE.
	
	return readRegister(DEVICE_ID_REG_ADDRESS) == DEVICE_ID;
}

void ADXL345::readMultiBytes(uint8_t baseAddress, int num, uint8_t data[])
{
	for (int i = 0; i < num; i++){
		data[i] = readRegister(baseAddress + i);
	}
	return;
}

int16_t ADXL345::getXAcceleration()
{
	uint8_t xData[2] = {0,0};
	readMultiBytes(X_ADDRESS, 2, xData);
	return (xData[1] << 8) | xData[0];
}

int16_t ADXL345::getYAcceleration()
{
	uint8_t yData[2] = {0,0};
	readMultiBytes(Y_ADDRESS, 2, yData);
	return (yData[1] << 8) | yData[0];
}

int16_t ADXL345::getZAcceleration()
{
	uint8_t zData[2] = {0,0};
	readMultiBytes(Z_ADDRESS, 2, zData);
	return (zData[1] << 8) | zData[0];
}

float ADXL345::convertRawToFloat(int16_t value)
{
	float castValue = value;
	int range = currentResolution? 16: intPow(2,currentRange+1);
	int maxValue = ((currentResolution? intPow(2,13): intPow(2,10))-1)/2;
	return castValue*(float)range/(float)maxValue;
}

void ADXL345::wakeUp(){
	uint8_t currentPowerCtrl = readRegister(POWER_CTRL);
	uint8_t newValue = modifyBit(currentPowerCtrl,2,0);  //set sleep bit to 0
	newValue = modifyBit(newValue,3,1);  //set measure bit to 1
	writeRegister(POWER_CTRL, newValue);
}

void ADXL345::sleep(){
	uint8_t currentPowerCtrl = readRegister(POWER_CTRL);
	uint8_t newValue = modifyBit(currentPowerCtrl,2,1);  //set sleep bit to 1
	newValue = modifyBit(newValue,3,0); //set measure bit to 0
	writeRegister(POWER_CTRL, newValue);
}

void ADXL345::setRange(ADXL_RANGE range){
	setRange((uint8_t)range);
}

void ADXL345::setRange(uint8_t range){
	uint8_t dataFormat = readRegister(DATA_FORMAT);
	currentRange = (ADXL_RANGE)range;
	//range = currentResolution? 16: intPow(2,currentRange+1);
	dataFormat&=0xFC;  //clear the range bits
	dataFormat|=range;  //write the range bits
	writeRegister(DATA_FORMAT, dataFormat);
}

void ADXL345::setResolution(ADXL_RESOLUTION resolution){
	currentResolution = resolution;
	//maxValue = ((currentResolution? intPow(2,13): intPow(2,10))-1)/2;
	uint8_t dataFormat = readRegister(DATA_FORMAT);
	writeRegister(DATA_FORMAT, modifyBit(dataFormat,3,resolution));
}

void ADXL345::setRate(ADXL_RATE rate){
	currentRate = rate;
	writeRegister(RATE_ADDRESS, rate);
}

uint8_t ADXL345::getRange(){
	return currentRange;
}

float ADXL345::getRate(){
	return ADXL_RATES[currentRate];
}
bool ADXL345::getTap(){
	//return true; //for test
	//return true;
	uint8_t src =readRegister(INT_SOURCE);
	return src&(1<<5);
}
bool ADXL345::dataReady(){
	uint8_t src =readRegister(INT_SOURCE);
	return (src&(1<<7));
}
bool ADXL345::isActive(){
	uint8_t src =readRegister(INT_SOURCE);
	return (src&(1<<4));
}
bool ADXL345::isInactive(){
	uint8_t src =readRegister(INT_SOURCE);
	return (src&(1<<3));
}