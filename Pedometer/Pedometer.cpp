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
bool Pedometer::add_point(int mag) {
	index++;
	if (index > STARTUP_TIME) {
		numPoints++;
		dataPoints->push(mag);
		if (numPoints > 1) {
			derivative->push(dataPoints->front() - dataPoints->back());
			
			
			
			sma->push(derivative->average());
			if (sma->full()) {
				ema->push((sma->front() * K + ema->front() * (SCALE-K))/SCALE);
				if (numPoints > EMA_WINDOW) {
					detectDeriv->push(ema->front() - ema->back());
					return detect_step();

				}
			}
			
		}
	}
	return false;
}
int Pedometer::read_accelerometer(int x, int y, int z) {
	


	return SCALE*sqrt(pow(x , 2) + pow(y , 2) + pow(z , 2));
}
void Pedometer::output_data() {
	std::cout << "Total Steps: " << steps << "; datapoint: " << dataPoints->front() << "; sma: " << sma->front() << "; ema: " << ema->front() << "; detect: " << detectDeriv->front() << std::endl;
}