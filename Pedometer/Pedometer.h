#pragma once
#include "MovingList.h"
#include "PointPair.h"
#ifndef PEDOMETER_H
class Pedometer
{
public:
	Pedometer();
	bool detect_step();
	bool add_point(int mag);
	int read_accelerometer(int x, int y, int z);
	void output_data();
private:
	const int SCALE = 100000;
	const int TIMING_THRESHOLD = 12;
	const int FINAL_WINDOW = 6;
	const int SMA_WINDOW = 30;
	const int EMA_WINDOW = 8;
	const int STARTUP_TIME = 0;
	const int DIFF_THRESHOLD = 0.12*SCALE;
	const int K =  (SCALE*2) / (EMA_WINDOW + 1);
	PointPair* dataPoints = new PointPair;
	MovingList* derivative = new MovingList(SMA_WINDOW * 2 +1);
	MovingList* sma = new MovingList(SMA_WINDOW * 2 +1);
	PointPair* ema = new PointPair;
	MovingList* detectDeriv = new MovingList(FINAL_WINDOW * 2 +1);
	unsigned long int numPoints = 0;
	unsigned long int steps = 0;
	unsigned long int mostRecent = 0;
	unsigned long int index = 0;
};
#endif
