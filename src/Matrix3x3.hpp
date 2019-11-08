#ifndef AYA_MATRIX4X4_HPP
#define AYA_MATRIX4X4_HPP

#include "Vector3.hpp"
#include <stdio.h>

#if defined(AYA_USE_SIMD)
#define vMPPP (_mm_set_ps(+0.0f, +0.0f, +0.0f, -0.0f))
#define v1000 (_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f))
#define v0100 (_mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f))
#define v0010 (_mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f))
#endif

namespace Aya {
	__declspec(align(16)) class Matrix3x3 {
		BaseVector3 m_el[3];

	public:
		Matrix3x3() {}
		Matrix3x3(const float &xx, const float &xy, const float &xz,
			const float &yx, const float &yy, const float &yz,
			const float &zx, const float &zy, const float &zz) {
			setValue(xx, xy, xz,
				yz, yy, yz,
				xz, zy, zz);
		}

#if defined(AYA_USE_SIMD)
		__forceinline Matrix3x3(const __m128  &v0, const __m128  &v1, const __m128  &v2) {
			m_el[0].m_val128 = v0;
			m_el[1].m_val128 = v1;
			m_el[2].m_val128 = v2;
		}
		__forceinline Matrix3x3(const BaseVector3 &v0, const BaseVector3 &v1, const BaseVector3 &v2) {
			m_el[0] = v0;
			m_el[1] = v1;
			m_el[2] = v2;
		}
		__forceinline Matrix3x3(const Matrix3x3 &rhs) {
			m_el[0].m_val128 = rhs.m_el[0].m_val128;
			m_el[1].m_val128 = rhs.m_el[1].m_val128;
			m_el[2].m_val128 = rhs.m_el[2].m_val128;
		}
		__forceinline Matrix3x3& operator = (const Matrix3x3 &rhs) {
			m_el[0].m_val128 = rhs.m_el[0].m_val128;
			m_el[1].m_val128 = rhs.m_el[1].m_val128;
			m_el[2].m_val128 = rhs.m_el[2].m_val128;
			
			return *this;
		}
#else
		__forceinline Matrix3x3(const BaseVector3 &v0, const BaseVector3 &v1, const BaseVector3 &v2) {
			m_el[0] = v0;
			m_el[1] = v1;
			m_el[2] = v2;
	}
		__forceinline Matrix3x3(const Matrix3x3 &rhs) {
			m_el[0] = rhs.m_el[0];
			m_el[1] = rhs.m_el[1];
			m_el[2] = rhs.m_el[2];
		}
		__forceinline Matrix3x3& operator = (const Matrix3x3 &rhs) {
			m_el[0] = rhs.m_el[0];
			m_el[1] = rhs.m_el[1];
			m_el[2] = rhs.m_el[2];

			return *this;
		}
#endif
		__forceinline BaseVector3 getColumn(int x) const {
			assert(x >= 0 && x < 3);
			return BaseVector3(m_el[0][x], m_el[1][x], m_el[2][x]);
		}
		__forceinline BaseVector3 getRow(int x) const {
			assert(x >= 0 && x < 3);
			return m_el[x];
		}
		__forceinline BaseVector3& operator [](int x) {
			assert(x >= 0 && x < 3);
			return m_el[x];
		}
		__forceinline const BaseVector3& operator [](int x) const {
			assert(x >= 0 && x < 3);
			return m_el[x];
		}

		void setValue(const float &xx, const float &xy, const float &xz,
			const float &yx, const float &yy, const float &yz,
			const float &zx, const float &zy, const float &zz) {
			m_el[0].setValue(xx, xy, xz);
			m_el[1].setValue(yx, yy, yz);
			m_el[2].setValue(zx, zy, zz);
		}
		/*
		void setFromOpenGL(const float *m) {
			m_el[0].setValue(m[0], m[4], m[8]);
			m_el[1].setValue(m[1], m[5], m[9]);
			m_el[2].setValue(m[2], m[6], m[10]);
		}
		*/
	};
}
#endif