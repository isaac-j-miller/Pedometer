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
	void push(int value);
	int front();
	int left_sum();
	int right_sum();
	int average();
	bool full();
	int operator[](int index);
private:
	int _len = 0;
	int _hlen = 0;
	int _actualSize = 0;
	int* _values = NULL;
};
#endif