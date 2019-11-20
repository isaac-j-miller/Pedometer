#include "PointPair.h"

template class PointPair<int>;
template class PointPair<float>;

template <typename T>
PointPair<T>::PointPair() {

}
template <typename T>
void PointPair<T>::push(T val) {
	_back = _front;
	_front = val;
}
template <typename T>
T PointPair<T>::front() {
	return _front;
}
template <typename T>
T PointPair<T>::back() {
	return _back;
}