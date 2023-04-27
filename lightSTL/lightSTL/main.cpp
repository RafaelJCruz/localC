#include<iostream>
#include "myDeque.h"
#include "myVector.h"
#include "myString.h"


using namespace std;

int main() {
	size_t value = 256 / sizeof(int);
	lightstl::deque<int> d1(144, 114514);
	d1.push_back(1);
	/*lightstl::deque<int> dq(d1.begin(),d1.end());
	lightstl::deque<int> d2(d1);
	lightstl::deque<int>::iterator it = d2.end();
	int arr[300];
	for (int i = 0; i < 144; i++) {
		arr[i] = d2[i];
	}
	int a = 10;
	lightstl::vector<int> v1(10,5);
	lightstl::vector<int> v2(v1)*/;

	return 0;

}
