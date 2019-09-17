
#include "PointPair.h"
PointPair::PointPair() {

}
void PointPair::push(float val) {
	_back = _front;
	_front = val;
}
float PointPair::front() {
	return _front;
}
float PointPair::back() {
	return _back;
}