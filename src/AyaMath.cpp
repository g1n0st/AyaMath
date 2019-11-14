#include <intrin.h> 
#include <iostream>
#include <ctime>
#include <windows.h>

#include "Transform.hpp"
#include "BBox.hpp"
#include "Quaternion.hpp"

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
BBox a[10000000];
#endif
int rd() {
	return rand() % 1000 - 500;
}
int main()
{
#ifdef TEST_TIME_SPEED
	Transform xx = Transform().setScale(1, 2, 3) * Transform().setTranslate(7, 4, -1) * Transform().setRotateZ(55) * Transform().setEulerZYX(2.3, 1.1, 4.5);
	for (int i = 0; i < 10000000; i++) a[i] = BBox(Point3(rd(), rd(), rd()), Point3(rd(), rd(), rd()));
	cout << "start" << endl;
	int st = clock();
	for (int i = 0; i < 10000000; i++) {
		a[i] = xx(a[i]);
	}
	int ed = clock();
	cout << "end" << endl;
	std::cout << ed - st << std::endl;
#else
	Quaternion a;
	a.setEulerZYX(1, 2, 3);
	cout << a.getAxis() << endl;
	cout << a.getAngle() << endl;
	Transform m1(a);
	cout << m1.m_mat * m1.m_inv << endl;
	Transform m2;
	m2.setRotation(Vector3(3, 4, 0), 1.2);
	cout << m2.m_mat * m2.m_inv << endl;
#endif
	return 0;
}