#include "MovingList.h"

MovingList::MovingList(int length) {
	_len = length;
	_hlen = _len / 2;
	_actualSize = 0;
	_values = new float[_len];
	for (int i = 0; i < _len; i++) {
		_values[i] = 0;
	}
}
MovingList::~MovingList() {
	delete[] _values;
}
void MovingList::push(float value) {
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
float MovingList::front() {
	return _values[_actualSize - 1];
}
float MovingList::left_sum() {
	float sum = 0;
	for (int i = 0; i < _hlen; i++) {
		sum += _values[i];
	}
	return sum;
}
float MovingList::right_sum() {
	float sum = 0;
	for (int i = _hlen; i < _len; i++) {
		sum += _values[i];
	}
	return sum;
}
float MovingList::average() {
	float sum = 0;
	for (int i = 0; i < _len; i++) {
		sum += _values[i];
	}
	return sum / _len;
}
bool MovingList::full() {
	return _actualSize == _len;
}
float MovingList::operator[](int index) {
	if (index < 0) {
		index -= _actualSize;
	}
	return _values[index];
}