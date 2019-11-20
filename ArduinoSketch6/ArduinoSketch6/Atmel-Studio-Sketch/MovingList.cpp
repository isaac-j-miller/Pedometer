#include "MovingList.h"

template class MovingList<int>;
template class MovingList<float>;

template <typename T>
MovingList<T>::MovingList(int length) {
	_len = length;
	_hlen = _len / 2;
	_actualSize = 0;
	_values = new T[_len];
	for (int i = 0; i < _len; i++) {
		_values[i] = 0;
	}
}
template <typename T>
MovingList<T>::~MovingList<T>() {
	delete[] _values;
}
template <typename T>
void MovingList<T>::push(T value) {
	if (_actualSize < _len) {
		_values[_actualSize] = value;
		_actualSize++;
	}
	else {
		for (int i = 0; i < _len - 2; i++) {
			_values[i] = _values[i + 1];
		}
		_values[_len - 1] = value;
	}
}
template <typename T>
T MovingList<T>::front() {
	return _values[_actualSize - 1];
}
template <typename T>
T MovingList<T>::left_sum() {
	T sum = 0;
	if(full()){
		for (int i = 0; i < _hlen; i++) {
			sum += _values[i];
		}
	}
	else{
		for (int i = 0; i < _actualSize/2; i++) {
			sum += _values[i];
		}
	}
	
	return sum;
}
template <typename T>
T MovingList<T>::right_sum() {
	T sum = 0;
	if(full()){
		for (int i = _hlen; i < _len; i++) {
			sum += _values[i];
		}
	}
	else{
		for (int i = _actualSize/2; i < _actualSize; i++) {
			sum += _values[i];
		}
	}
	return sum;
}
template <typename T>
T MovingList<T>::average() {
	T sum = 0;
	for (int i = 0; i < _actualSize; i++) {
		sum += _values[i];
	}
	return sum / _len;
}
template <typename T>
bool MovingList<T>::full() {
	return _actualSize == _len;
}
template <typename T>
T MovingList<T>::operator[](int index) {
	if (index < 0) {
		index -= _actualSize;
	}
	return _values[index];
}