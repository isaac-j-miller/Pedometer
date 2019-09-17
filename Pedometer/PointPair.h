#ifndef POINTPAIR_H
#define POINTPAIR_H

class PointPair
{
public:
	PointPair();
	void push(int val);
	int front();
	int back();
private:
	int _front = 0;
	int _back = 0;
};
#endif