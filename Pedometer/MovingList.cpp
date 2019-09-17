#include "MovingList.h"

MovingList::MovingList(int length) {
	_len = length;
	_hlen = _len / 2;
	_actualSize = 0;
	_values = new int[_len];
	for (int i = 0; i < _len; i++) {
		_values[i] = 0;
	}
}
MovingList::~MovingList() {
	delete[] _values;
}
void MovingList::push(int value) {
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
int MovingList::front() {
	return _values[_actualSize - 1];
}
int MovingList::left_sum() {
	int sum = 0;
	for (int i = 0; i < _hlen; i++) {
		sum += _values[i];
	}
	return sum;
}
int MovingList::right_sum() {
	int sum = 0;
	for (int i = _hlen; i < _len; i++) {
		sum += _values[i];
	}
	return sum;
}
int MovingList::average() {
	int sum = 0;
	for (int i = 0; i < _len; i++) {
		sum += _values[i];
	}
	return sum / _len;
}
bool MovingList::full() {
	return _actualSize == _len;
}
int MovingList::operator[](int index) {
	if (index < 0) {
		index -= _actualSize;
	}
	return _values[index];
}