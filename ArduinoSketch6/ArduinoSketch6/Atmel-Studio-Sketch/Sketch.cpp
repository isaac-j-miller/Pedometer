/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
/*End of auto generated code by Atmel studio */


//including all standard libraries as listed: https://www.arduino.cc/en/Reference/Libraries
//comment out the ones you do not need once in ATMEL Studio

//#include <EEPROM.h>
//#include <Ethernet.h>
//#include <Firmata.h>
//#include <GSM.h> //removed due to compile conflict with SoftwareSerial
//#include <LiquidCrystal.h>
//#include <SD.h>
//#include <Servo.h>
#include <SPI.h>
//#include <SoftwareSerial.h>
//#include <Stepper.h>
//#include <TFT.h>
//#include <WiFi.h>
//#include <Wire.h>
//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio
#include "ADXL345.h"
#include "Helper.h"
#include "Pedometer.h"


const int RADIO_SLEEP = A4;
const int CS_PIN = 7;
const int WAKEUP_PIN = 3;
const int SLEEP_PIN = 2;
const int LEDS[] = {4, 6, 5, 9, 10, 11, 12, A5};  // LEDs used for binary counter
const int UP[] = {1,3,7,15,31,63,127,255};
const int DOWN[] = {128,192,224,240,248,252,254,255};
bool asleep = false;

void displayLED(uint8_t number){
	for(int i = 0; i < 8; i++){
		digitalWrite(LEDS[i], number&(1<<i));
	}
}

void displayUpSequence(){
	for(auto i:UP){
		displayLED(i);
		delay(50);
	}
}

void displayDownSequence(){
	for(auto i:DOWN){
		displayLED(i);
		delay(50);
	}
}

void flashNtimes(uint8_t value, int n){
	for(int i =0; i<n;i++){
		displayLED(value);
		delay(50);
		displayLED(0);
		delay(50);
	}
}

bool handshake(uint16_t trials, unsigned long int numSteps){
	uint8_t buffer = 0;
	digitalWrite(RADIO_SLEEP,LOW);
	for(int i=0; i<trials; i++){
		//delay(delayPeriod/4);
		while (Serial1.available()){
			buffer = Serial1.read();
			displayLED(128);
			//call from python script
			displayLED(buffer);
			if(buffer=='A'){
				
				Serial1.println('Y');
				Serial1.flush();
				return false;
			}
			else if (buffer=='H'){
				
				Serial1.print("steps: ");
				Serial1.println(numSteps);
				Serial1.flush();
				return true;
			}
			else{

			}

		}
	}
	return false;
}

bool getReceipt(String header, String data){
	digitalWrite(RADIO_SLEEP, LOW);
	delay(100);
	while(Serial1.available()){
		Serial1.read();
	}
	Serial1.print("!::ID=ISAAC_PEDOMETER::"+header+"="+data+"::!");
	
	Serial1.setTimeout(3000);
	
	//Serial1.flush();
	delay(1000);
	String r;
	if(Serial1.available()){
		delay(1000);
	}
	char temp;
	while (Serial1.available()){
		temp=(char)Serial1.read();
		r+=temp;
	}
	int a = r.indexOf("RD");
	//Serial1.print("!::ID=ISAAC_PEDOMETER::ECHO="+r+"::RECEIPT="+String(a!=-1)+"::!");
	//delay(1000);
	Serial1.flush();
	digitalWrite(RADIO_SLEEP, HIGH);
	return a!=-1;
}


ADXL345 accel;
Pedometer pedometer;
MovingList<float> magnitudes(5);  //list of magnitudes used for keeping track of average magnitude for range and sleep purposes
bool tapped = false;
bool transmitted = true;
int trials =0;
int maxTrials = 1;
//
void wakeup(){
	if(asleep){
		flashNtimes(0xA5, 5);
	}
	tapped |= accel.getTap();
	transmitted &= !tapped;
	asleep = false;
}

void goToSleep(){
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	cli();
	if(pedometer.ready() && !tapped && transmitted){
		asleep=true;
		sleep_enable();
		sei();
		sleep_cpu();
		sleep_disable();
	}
	sei();
}



void setup() {

	//setup SPI interface for CPOL=1, CPHA=1 or SPI_MODE_3 in Arduino
	pinMode(RADIO_SLEEP,OUTPUT);
	pinMode(WAKEUP_PIN, INPUT_PULLUP);
	//pinMode(TAP_PIN,INPUT);
	for (auto i: LEDS){  //set up LEDs for binary counter
		pinMode(i, OUTPUT);
	}
	digitalWrite(RADIO_SLEEP,LOW);
	delay(1000);
	Serial1.begin(9600);
	delay(2000);
	digitalWrite(RADIO_SLEEP,HIGH);
	SPI.begin();
	SPI.setDataMode(SPI_MODE3); //this works best. New SPI_TRANSACTION is not well documented
	
	accel.setup(CS_PIN, WAKEUP_PIN,ADXL345::EIGHT_G, ADXL345::NORMAL_RESOLUTION, ADXL345::O25);
	//accel.setSleepTime(10);
	attachInterrupt(digitalPinToInterrupt(WAKEUP_PIN),wakeup,RISING);
	attachInterrupt(digitalPinToInterrupt(SLEEP_PIN),goToSleep,RISING);
	
	while(!accel.selfTest());
	
}

void loop() {
	static int16_t x, y, z;
	static int16_t numSteps;
	static float magnitude;
	static float xf, yf, zf;
	
	static bool pedometerReady = false;
	static bool prevPedometerReady = false;
	pedometerReady=pedometer.ready();
	if(pedometerReady && !prevPedometerReady){
		flashNtimes(0xFF, 3);
	}
	//tapped |= accel.getTap();
	//transmitted &= !tapped;
	if(tapped && !transmitted){
		tapped = false;
		if(getReceipt("STEPS", String(numSteps))){
			displayUpSequence();
			transmitted=true;
		}
		else if (trials<maxTrials){
			displayDownSequence();
			trials++;
		}
		else{
			transmitted=true;
			trials=0;
		}
	}
	displayLED(numSteps);
	//displayLED(0xF0*accel.isActive()+0x0F*accel.isInactive());
	
	x = accel.getXAcceleration();
	y = accel.getYAcceleration();
	z = accel.getZAcceleration();
	
	xf = accel.convertRawToFloat(x);
	yf = accel.convertRawToFloat(y);
	zf = accel.convertRawToFloat(z);
	//tapped |= accel.getTap();
	
	if(accel.dataReady()){
		digitalWrite(LED_BUILTIN, HIGH);
		magnitude = sqrt(pow(xf,2)+pow(yf,2)+pow(zf,2));
		magnitudes.push(magnitude);
		pedometer.add_point(magnitude);  
		unsigned long int oldSteps = numSteps; 
		numSteps = pedometer.getSteps();
		uint8_t rawRange = accel.getRange();
		float range = pow(2, rawRange+1);
		float averageMagnitude = magnitudes.average();
		if (averageMagnitude > range*.90 & rawRange < ADXL345::ADXL_RANGE::SIXTEEN_G){  //if close to maximum range
			accel.setRange(rawRange+1);
		}
		else if (averageMagnitude < range*0.25 & rawRange > ADXL345::ADXL_RANGE::TWO_G){  //if close to minimum range
			accel.setRange(rawRange-1);
		}
	}
	else{
		
		digitalWrite(LED_BUILTIN, LOW);
	}
	prevPedometerReady=pedometerReady;
}