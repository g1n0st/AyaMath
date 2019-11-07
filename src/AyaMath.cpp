#include <intrin.h> 
#include <iostream>
#include <ctime>

#include "Vector3.hpp"

using namespace Aya;
using namespace std;


Vector3 x(9, 53, 13), y(7, 23, 19);
//#define TEST_TIME_SPEED

#if defined(TEST_TIME_SPEED)
Vector3 a[30000000], b[30000000], c[30000000];
#endif
int main()
{
#ifdef TEST_TIME_SPEED
	for (int i = 0; i < 30000000; i++) a[i] = x, b[i] = y;
	int st = clock();
	for (int i = 0; i < 30000000; i++) {
		c[i] = a[i].cross(b[i]);
		b[i] = c[i].cross(a[i]);
		a[i] = b[i].cross(a[i]);
		c[i] = a[i].cross(b[i]);
		b[i] = c[i].cross(a[i]);
		a[i] = b[i].cross(a[i]);
		c[i] = a[i].cross(b[i]);
		b[i] = c[i].cross(a[i]);
		a[i] = b[i].cross(a[i]);
	}
	int ed = clock();
	std::cout << ed - st << std::endl;
#else
	Vector3 a(1, 0, 0);
	Normal3 b(0, 0, 1);
	cout << a.rotate(b, 3.14159265357f / 1) << endl;
#endif
	return 0;
}
