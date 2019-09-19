#include "PointPair.h"

PointPair::PointPair() {

}
void PointPair::push(int val) {
	_back = _front;
	_front = val;
}
int PointPair::front() {
	return _front;
}
int PointPair::back() {
	return _back;
}