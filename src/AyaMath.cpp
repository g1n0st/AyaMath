#include <intrin.h> 
#include <iostream>
#include <ctime>

#include "Matrix3x3.hpp"

using namespace Aya;
using namespace std;

Matrix3x3 x(1, 2, 3,
	4, 5, 6,
	7, 8, 9);
Matrix3x3 y(1, 1, 4,
	5, 1, 4,
	2, 3, 3);
//#define TEST_TIME_SPEED

#if defined(TEST_TIME_SPEED)

Matrix3x3 a[10000000], b[10000000], c[10000000];
#endif
int main()
{
#ifdef TEST_TIME_SPEED
	for (int i = 0; i < 10000000; i++) a[i] = x, b[i] = y;
	int st = clock();
	for (int i = 0; i < 10000000; i++) {
		a[i] = a[i] * b[i];
		b[i] = b[i] * a[i];
	}
	int ed = clock();
	std::cout << ed - st << std::endl;
#else

	y *= x;
	cout << y << endl;
#endif
	return 0;
}