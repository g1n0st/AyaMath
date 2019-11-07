#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <assert.h>

#ifdef _WIN32
#include <intrin.h>
#endif

#define AYA_USE_SIMD

#define pshufs_ps(_a, _mask) _mm_shuffle_ps((_a), (_a), (_mask))

namespace AyaMath {
	__declspec(align(16)) class BaseVector3 {
#ifndef AYA_USE_SIMD
	 public:

		float m_val[4];

		__forceinline const __m128& get128() const {
			return *((const __m128*)&m_val[0]);
		}
#else
	 public:

		union {
			float m_val[4];
			__m128 m_val128;
		};

		__forceinline __m128 get128() const {
			return m_val128;
		}
		__forceinline void set128(const __m128 &v128) {
			m_val128 = v128;
		}
#endif
	public:
		BaseVector3() {}
		__forceinline BaseVector3(const float &x, const float &y, const float &z) {
			m_val[0] = x;
			m_val[1] = y;
			m_val[2] = z;
			m_val[3] = .0f;
		}

#ifdef AYA_USE_SIMD
		__forceinline BaseVector3(const __m128 &v128) {
			m_val128 = v128;
		}
		__forceinline BaseVector3(const BaseVector3 &rhs) {
			m_val128 = rhs.m_val128;
		}
		__forceinline BaseVector3& operator =(const BaseVector3 &rhs) {
			m_val128 = rhs.m_val128;
			return *this;
		}
#endif
		__forceinline BaseVector3 operator + (const BaseVector3 &v) const {
			BaseVector3 ret;
#ifdef AYA_USE_SIMD
			ret.m_val128 = _mm_add_ps(m_val128, v.m_val128);
#else
			ret.m_val[0] += v.m_val[0];
			ret.m_val[1] += v.m_val[1];
			ret.m_val[2] += v.m_val[2];
#endif
			return ret;
		}
		__forceinline BaseVector3 & operator += (const BaseVector3 &v) {
#ifdef AYA_USE_SIMD
			m_val128 = _mm_add_ps(m_val128, v.m_val128);
#else
			m_val[0] += v.m_val[0];
			m_val[1] += v.m_val[1];
			m_val[2] += v.m_val[2];
#endif
			return *this;
		}
		__forceinline BaseVector3 operator - (const BaseVector3 &v) const {
			BaseVector3 ret;
#ifdef AYA_USE_SIMD
			ret.m_val128 = _mm_sub_ps(m_val128, v.m_val128);
#else
			ret.m_val[0] -= v.m_val[0];
			ret.m_val[1] -= v.m_val[1];
			ret.m_val[2] -= v.m_val[2];
#endif
			return ret;
		}
		__forceinline BaseVector3 & operator -= (const BaseVector3 &v) {
#ifdef AYA_USE_SIMD
			m_val128 = _mm_sub_ps(m_val128, v.m_val128);
#else
			m_val[0] -= v.m_val[0];
			m_val[1] -= v.m_val[1];
			m_val[2] -= v.m_val[2];
#endif
			return *this;
		}
		__forceinline BaseVector3 operator- () const {
			return BaseVector3(-m_val[0], -m_val[1], -m_val[2]);
		}
		__forceinline BaseVector3 operator * (const float &s) const {
			BaseVector3 ret;
#ifdef AYA_USE_SIMD
			__m128 vs = _mm_load_ss(&s);
			vs = pshufs_ps(vs, 0x80);
			ret.m_val128 = _mm_mul_ps(m_val128, vs);
#else
			ret.m_val[0] *= s;
			ret.m_val[1] *= s;
			ret.m_val[2] *= s;
#endif
			return ret;
		}
		__forceinline friend BaseVector3 operator * (float s, const BaseVector3 &v) {
			return v * s;
		}
		__forceinline BaseVector3 & operator *= (const float &s) {
#ifdef AYA_USE_SIMD
			__m128 vs = _mm_load_ss(&s);
			vs = pshufs_ps(vs, 0x80);
			m_val128 = _mm_mul_ps(m_val128, vs);
#else
			m_val[0] *= s;
			m_val[1] *= s;
			m_val[2] *= s;
#endif
			return *this;
		}
		__forceinline BaseVector3 operator / (const float &s) const {
			assert(s != 0.f);
			BaseVector3 ret;
			ret = (*this) * (1.f / s);
			return ret;
		}
		__forceinline BaseVector3 & operator /= (const float &s) {
			assert(s != 0.f);
			return *this *= (1.f / s);
		}

		float operator [](const int &p) const {
			assert(p >= 0 && p <= 2);
			return m_val[p];
		}
		float &operator [](const int &p) {
			assert(p >= 0 && p <= 2);
			return m_val[p];
		}

		__forceinline float dot(const BaseVector3 &v) const {
#ifdef AYA_USE_SIMD
			__m128 vd = _mm_mul_ps(m_val128, v.m_val128);
			__m128 z = _mm_movehl_ps(vd, vd);
			__m128 y = pshufs_ps(vd, 0x55);
			vd = _mm_add_ss(vd, y);
			vd = _mm_add_ss(vd, z);
			return _mm_cvtss_f32(vd);

#else
			return	m_val[0] * v.m_val[0] +
					m_val[1] * v.m_val[1] +
					m_val[2] * v.m_val[2];
#endif
		}

		__forceinline float length2() const {
			return dot(*this);
		}


	};
}

#endif