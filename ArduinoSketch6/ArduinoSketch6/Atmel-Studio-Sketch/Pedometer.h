#pragma once

#ifndef PEDOMETER_H
#include "MovingList.h"
#include "PointPair.h"
#include "Helper.h"
class Pedometer
{
public:
	Pedometer();
	bool detect_step();
	bool add_point(float mag);
	int read_accelerometer(int x, int y, int z);
	void output_data();
	int getSteps();
	bool ready();
	const int TIMING_THRESHOLD = 6;
private:
	const float SCALE = 1;
	const int FINAL_WINDOW = 12;
	const int SMA_WINDOW = 30;
	const int EMA_WINDOW = 8;
	const int STARTUP_TIME = 0;
	const float DIFF_THRESHOLD = 0.0007;//0.12*SCALE;
	const float K =  (SCALE*2.f) / ((float)(EMA_WINDOW + 1));
	PointPair<float>* dataPoints = new PointPair<float>;
	MovingList<float>* derivative = new MovingList<float>(SMA_WINDOW * 2 +1);
	MovingList<float>* sma = new MovingList<float>(SMA_WINDOW * 2 +1);
	PointPair<float>* ema = new PointPair<float>;
	MovingList<float>* detectDeriv = new MovingList<float>(FINAL_WINDOW * 2 +1);
	unsigned long int numPoints = 0;
	unsigned long int steps = 0;
	unsigned long int mostRecent = 0;
	unsigned long int index = 0;
};
#endif
