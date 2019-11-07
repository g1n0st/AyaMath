#include <intrin.h> 
#include <iostream>
#include <ctime>
#include "vector3.hpp"
using namespace AyaMath;

BaseVector3 a[30000000], b[30000000], c[30000000];
BaseVector3 x(1, 2, 3), y(7, 7, 7);
//#define TEST_TIME_SPEED
int main()
{
#ifdef TEST_TIME_SPEED
	for (int i = 0; i < 30000000; i++) a[i] = x, b[i] = y;
	int st = clock();
	for (int i = 0; i < 30000000; i++) {
		a[i] += b[i];
		a[i] *= 3.333f;
		b[i] *= 7.7777f;
		a[i] /= 66;
		b[i] = a[i] / 3;
		a[i] = 3 * a[i];
		b[i] -= a[i];
		c[i] = a[i] + b[i];
		b[i] = c[i] + a[i];
	}
	int ed = clock();
	std::cout << ed - st << std::endl;
#else
	std::cout << x.dot(y) << std::endl;
#endif
	return 0;
}
