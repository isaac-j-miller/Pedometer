#ifndef NULL
#define NULL 0
#endif

#ifndef MOVINGLIST_H
#define MOVINGLIST_H
class MovingList
{
public:
	MovingList(int length);
	~MovingList();
	void push(float value);
	float front();
	float left_sum();
	float right_sum();
	float average();
	bool full();
	float operator[](int index);
private:
	int _len = 0;
	int _hlen = 0;
	int _actualSize = 0;
	float* _values = NULL;
};
#endif

