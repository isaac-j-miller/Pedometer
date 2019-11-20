#ifndef NULL
#define NULL 0
#endif
#ifndef MOVINGLIST_H
#define MOVINGLIST_H
template <typename T>
class MovingList
{
public:
	MovingList<T>(int length);
	~MovingList();
	void push(T value);
	T front();
	T left_sum();
	T right_sum();
	T average();
	bool full();
	T operator[](int index);
private:
	int _len = 0;
	int _hlen = 0;
	int _actualSize = 0;
	T* _values = NULL;
};
#endif