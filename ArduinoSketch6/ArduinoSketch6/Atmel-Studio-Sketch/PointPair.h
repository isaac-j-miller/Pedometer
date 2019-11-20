#ifndef POINTPAIR_H
#define POINTPAIR_H

template <typename T>
class PointPair
{
public:
	PointPair<T>();
	void push(T val);
	T front();
	T back();
private:
	T _front = 0;
	T _back = 0;
};
#endif