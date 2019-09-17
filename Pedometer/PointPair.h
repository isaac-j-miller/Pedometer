#ifndef POINTPAIR_H
#define POINTPAIR_H

class PointPair
{
public:
	PointPair();
	void push(float val);
	float front();
	float back();
private:
	float _front = 0.f;
	float _back = 0.f;
};
#endif