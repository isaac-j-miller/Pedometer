#include "Pedometer.h"
#include <math.h>
#include <iostream>

Pedometer::Pedometer() {

}
bool Pedometer::detect_step() {
	if (numPoints - mostRecent > TIMING_THRESHOLD) {
		float left = detectDeriv->left_sum();
		float right = detectDeriv->right_sum();
		if ((right - left) > DIFF_THRESHOLD) {
			mostRecent = numPoints;
			steps++;
			return true;
		}
	}
	return false;
}
bool Pedometer::add_point(float mag) {
	index++;
	if (index > STARTUP_TIME) {
		numPoints++;
		dataPoints->push(mag);
		if (numPoints > 1) {
			derivative->push(dataPoints->front() - dataPoints->back());
			sma->push(derivative->average());
			ema->push(sma->front() * K + ema->front() * (1 - K));
			if (numPoints > 2) {
				detectDeriv->push(ema->front() - ema->back());
				return detect_step();
			}
		}
	}
	return false;
}
float Pedometer::read_accelerometer(int x, int y, int z) {
	


	return (float)sqrt(pow((float)x , 2) + pow((float)y , 2) + pow((float)z , 2));
}
void Pedometer::output_data() {
	std::cout << "Total Steps: " << steps << std::endl;
}