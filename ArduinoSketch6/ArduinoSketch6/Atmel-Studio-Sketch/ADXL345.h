/* 
* ADXL345.h
*
* Created: 10/15/2019
* Author: Isaac Miller
*/


#ifndef __ADXL345_H__
#define __ADXL345_H__

/*
   DEFINE REGISTER ADDRESSES
   Useful when calling registers such as readRegister(DATA_FORMAT)
*/
#define DEVICE_ID_REG_ADDRESS 0x0
#define DEVICE_ID 0xE5
#define POWER_CTRL 0x2D
#define DATA_FORMAT 0x31
#define INT_ENABLE 0x2E
#define INT_ENABLE_FORMAT 0x18 //allows inactivity and activity to trigger interrupts
#define INT_MAP 0x2F
#define INT_MAP_FORMAT 0x30
#define TAP_AXES 0x2A 
#define TAP_AXES_FORMAT 0x01
#define TAP_LATENCY 0x22
#define WINDOW 0x23
#define THRESH_TAP 0x1D
#define THRESH_ACT 0x24
#define ACT_CRTL 0x27
#define TIME_INACT 0x26
#define THRESH_INACT 0x25
#define INT_SOURCE 0x30
#define TAP_DUR 0x21
#define RATE_ADDRESS 0x2C
#define X_ADDRESS 0x32
#define Y_ADDRESS 0x34
#define Z_ADDRESS 0x36

//include SPI header from Arduino
#include <SPI.h>
#include "Helper.h"
	
class ADXL345
{
//variables
public:

	//enumeration to specify what acceleration range to measure
	enum ADXL_RANGE {TWO_G, FOUR_G, EIGHT_G, SIXTEEN_G};
		
	//enumeration to specify whether 10-bit or full 13-bit resolution should be applied.
	enum ADXL_RESOLUTION {NORMAL_RESOLUTION, FULL_RESOLUTION};
	//enum to specify data rate. P is ., O is just to let the name start with a "number". I'll clean this up later. number in Hz
	enum ADXL_RATE {P10, P20, P39, P78, O1P56, O3P13, O6P25, O12P5, O25, O50, O100, O200, O400, O800, O1600, O3200};
	const float ADXL_RATES[16] = {0.1, 0.2, 0.39, 0.78, 1.56, 3.13, 6.25, 12.5, 25, 50, 100, 200, 400, 800, 1600, 3200};	
protected:
private:

	//local field to keep track of which pin is used for the CS
	int csPin=-1;
	int wakeUpPin = -1;
	//local fields to track the current measurement range and resolution
	ADXL_RANGE	currentRange;
	ADXL_RESOLUTION currentResolution;
	ADXL_RATE currentRate;
	//float maxValue;
	//float range;
//functions
public:
	ADXL345();
	~ADXL345();
	

	/**
	 * \brief function to initialize the ADXL345. Must specify which pin on the Arduino is connected to the CS line.
	 *  Optional argument provided to specify a new sampling range
	 * 
	 * \param _CS_PIN Digital I/O pin which will be used for CS line
	 * \param _RANGE Optional parameter to specify a desired sampling range.
	 * \param _RESOLUTION Optional parameter to specify a desired sampling resolution.
	 * 
	 * \return void
	 */
	void setup(int _CS_PIN, int _WAKEUP_PIN,ADXL_RANGE _RANGE=ADXL_RANGE::TWO_G, ADXL_RESOLUTION _RESOLUTION=NORMAL_RESOLUTION, ADXL_RATE _RATE = O12P5);
	
	/**
	 * \brief Perform a self-test on the ADXL345 by attempting to read the DEVICE_ID register.
	 * 
	 * 
	 * \return bool TRUE is self-test was successful, FALSE otherwise.
	 */
	bool selfTest();
	
	/**
	 * \brief Read acceleration from X-axis. Result is raw reading in local units.
	 * 
	 * 
	 * \return int16_t X-axis acceleration in local units.
	 */
	int16_t getXAcceleration();
	
	/**
	 * \brief Read acceleration from Y-axis. Result is raw reading in local units.
	 * 
	 * 
	 * \return int16_t Y-axis acceleration in local units.
	 */
	int16_t getYAcceleration();
	
	/**
	 * \brief Read acceleration from Z-axis. Result is raw reading in local units.
	 * 
	 * 
	 * \return int16_t Z-axis acceleration in local units.
	 */
	int16_t getZAcceleration();
	
	/**
	 * \brief Converts an ADXL345 acceleration reading into G's
	 * 
	 * \param  Raw ADXL345 acceleration reading
	 * 
	 * \return float Acceleration in terms of Gs
	 */
	float convertRawToFloat(int16_t);
	
	void wakeUp();
	
	void sleep();
	
	void setRange(ADXL_RANGE);
	void setRange(uint8_t);
	
	void setResolution(ADXL_RESOLUTION);
	
	void setRate(ADXL_RATE);

	bool getTap();
	uint8_t getRange();
	float getRate();
	bool isActive();
	bool isInactive();
	bool dataReady();
	
protected:

	/**
	 * \brief Read 8-bits from the register specified by address
	 * 
	 * \param address 8-bit register located in the ADXL345
	 * 
	 * \return uint8_t Data read from that register
	 */
	uint8_t readRegister(uint8_t address);
	

	/**
	 * \brief Write 8-bits to a register in the ADXL345
	 * 
	 * \param address 8-bit register located in the ADXL345 
	 * \param value Data to be written to that register
	 * 
	 * \return void
	 */
	void writeRegister(uint8_t address, uint8_t value);
	
	/**
	 * \brief Read multiple bytes beginning at the base address. 
	 * 
	 * \param baseAddress 8-bit register located in the ADXL345. Read will consume additional (increment) registers.
	 * \param num Number of registers to read including the based address
	 * \param data Pointer to array to hold read data. Must be of sufficient length to hold nums.
	 * 
	 * \return void
	 */
	void readMultiBytes(uint8_t baseAddress, int num, uint8_t data[]);
	
	
	
private:

}; //ADXL345

#endif //__ADXL345_H__
	