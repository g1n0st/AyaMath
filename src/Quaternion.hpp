#ifndef AYA_QUATERNION_HPP
#define AYA_QUATERNION_HPP

#include "Vector3.hpp"

namespace Aya {
	__declspec(align(16)) class Quaternion {
#if defined(AYA_USE_SIMD)
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
#else
	public:

		float m_val[4];

		__forceinline const __m128& get128() const {
			return *((const __m128*)&m_val[0]);
		}
#endif

	private:
#if defined(AYA_DEBUG)
		__forceinline void numericValid(int x) {
			assert(!std::isnan(m_val[0]) && !std::isnan(m_val[1]) && !std::isnan(m_val[2]) && !std::isnan(m_val[3]));
		}
#else
#define numericValid
#endif

	public:
		Quaternion() {}
		__forceinline Quaternion(const float &x, const float &y, const float &z, const float &w) {
			m_val[0] = x;
			m_val[1] = y;
			m_val[2] = z;
			m_val[3] = w;
			numericValid(1);
		}

#if defined(AYA_USE_SIMD)
		__forceinline Quaternion(const __m128 &v128) {
			m_val128 = v128;
		}
		__forceinline Quaternion(const Quaternion &rhs) {
			m_val128 = rhs.m_val128;
		}
		__forceinline Quaternion& operator = (const Quaternion &rhs) {
			m_val128 = rhs.m_val128;
			return *this;
		}
#endif
		__forceinline void setValue(const float &x, const float &y, const float &z, const float &w) {
			m_val[0] = x;
			m_val[1] = y;
			m_val[2] = z;
			m_val[3] = w;
			numericValid(1);
		}
		__forceinline void setX(const float &x) { m_val[0] = x; numericValid(1); }
		__forceinline void setY(const float &y) { m_val[1] = y; numericValid(1); }
		__forceinline void setZ(const float &z) { m_val[2] = z; numericValid(1); }
		__forceinline void setW(const float &w) { m_val[3] = w; numericValid(1); }
		__forceinline const float& getX() const { return m_val[0]; }
		__forceinline const float& getY() const { return m_val[1]; }
		__forceinline const float& getZ() const { return m_val[2]; }
		__forceinline const float& getW() const { return m_val[3]; }
		__forceinline const float& x() const { return m_val[0]; }
		__forceinline const float& y() const { return m_val[1]; }
		__forceinline const float& z() const { return m_val[2]; }
		__forceinline const float& w() const { return m_val[3]; }

		__forceinline bool operator == (const Quaternion &v) const {
#if defined(AYA_USE_SIMD)
			return (0xf == _mm_movemask_ps((__m128)_mm_cmpeq_ps(m_val128, v.m_val128)));
#else
			return ((m_val[0] == v.m_val[0]) &&
				(m_val[1] == v.m_val[1]) &&
				(m_val[2] == v.m_val[2]) &&
				(m_val[3] == v.m_val[3]));
#endif
		}
		__forceinline bool operator != (const Quaternion &v) const {
			return !((*this) == v);
		}

		__forceinline void setMax(const Quaternion &v) {
#if defined(AYA_USE_SIMD)
			m_val128 = _mm_max_ps(m_val128, v.m_val128);
#else
			SetMax(m_val[0], v.m_val[0]);
			SetMax(m_val[1], v.m_val[1]);
			SetMax(m_val[2], v.m_val[2]);
			SetMax(m_val[3], v.m_val[3]);
#endif
		}
		__forceinline void setMin(const Quaternion &v) {
#if defined(AYA_USE_SIMD)
			m_val128 = _mm_min_ps(m_val128, v.m_val128);
#else
			SetMin(m_val[0], v.m_val[0]);
			SetMin(m_val[1], v.m_val[1]);
			SetMin(m_val[2], v.m_val[2]);
			SetMin(m_val[3], v.m_val[3]);
#endif
		}
		__forceinline void setZero() {
#if defined(AYA_USE_SIMD)
			m_val128 = _mm_xor_ps(m_val128, m_val128);
#else
			m_val[0] = 0.f;
			m_val[1] = 0.f;
			m_val[2] = 0.f;
			m_val[3] = 0.f;
#endif
		}

		__forceinline bool isZero() const {
			return (m_val[0] == 0.f && m_val[1] == 0.f && m_val[2] == 0.f);
		}
		__forceinline bool fuzzyZero() const {
			return length2() < SIMD_EPSILON * SIMD_EPSILON;
		}

		__forceinline Quaternion operator + (const Quaternion &v) const {
#if defined(AYA_USE_SIMD)
			return Quaternion(_mm_add_ps(m_val128, v.m_val128));
#else
			return Quaternion(m_val[0] + v.m_val[0],
				m_val[1] + v.m_val[1],
				m_val[2] + v.m_val[2],
				m_val[3] + v.m_val[3]);
#endif
		}
		__forceinline Quaternion & operator += (const Quaternion &v) {
#if defined(AYA_USE_SIMD)
			m_val128 = _mm_add_ps(m_val128, v.m_val128);
#else
			m_val[0] += v.m_val[0];
			m_val[1] += v.m_val[1];
			m_val[2] += v.m_val[2];
			m_val[3] += v.m_val[3];
#endif
			numericValid(1);
			return *this;
		}
		__forceinline Quaternion operator - (const Quaternion &v) const {
#if defined(AYA_USE_SIMD)
			return Quaternion(_mm_sub_ps(m_val128, v.m_val128));
#else
			return Quaternion(m_val[0] - v.m_val[0],
				m_val[1] - v.m_val[1],
				m_val[2] - v.m_val[2],
				m_val[3] - v.m_val[3]);
#endif
		}
		__forceinline Quaternion & operator -= (const Quaternion &v) {
#if defined(AYA_USE_SIMD)
			m_val128 = _mm_sub_ps(m_val128, v.m_val128);
#else
			m_val[0] -= v.m_val[0];
			m_val[1] -= v.m_val[1];
			m_val[2] -= v.m_val[2];
			m_val[3] -= v.m_val[3];
#endif
			numericValid(1);
			return *this;
		}
		__forceinline Quaternion operator- () const {
#if defined(AYA_USE_SIMD)
			return Quaternion(_mm_xor_ps(m_val128, vMzeroMask));
#else
			return Quaternion(-m_val[0], -m_val[1], -m_val[2], -m_val[3]);
#endif
		}
		__forceinline Quaternion operator * (const float &s) const {
#if defined(AYA_USE_SIMD)
			__m128 vs = _mm_load_ss(&s);
			vs = _mm_pshufd_ps(vs, 0x00);
			return Quaternion(_mm_mul_ps(m_val128, vs));
#else
			return Quaternion(m_val[0] * s,
				m_val[1] * s,
				m_val[2] * s,
				m_val[3] * s);
#endif

		}
		__forceinline friend Quaternion operator * (const float &s, const Quaternion &v) {
			return v * s;
		}
		__forceinline Quaternion & operator *= (const float &s) {
#if defined(AYA_USE_SIMD)
			__m128 vs = _mm_load_ss(&s);
			vs = _mm_pshufd_ps(vs, 0x00);
			m_val128 = _mm_mul_ps(m_val128, vs);
#else
			m_val[0] *= s;
			m_val[1] *= s;
			m_val[2] *= s;
			m_val[3] *= s;
#endif
			numericValid(1);
			return *this;
		}
		__forceinline Quaternion operator / (const float &s) const {
			assert(s != 0.f);
			Quaternion ret;
			ret = (*this) * (1.f / s);
			return ret;
		}
		__forceinline Quaternion & operator /= (const float &s) {
			assert(s != 0.f);
			return *this *= (1.f / s);
		}

		float operator [](const int &p) const {
			assert(p >= 0 && p <= 2);
			return m_val[p];
		}
		float &operator [](const int &p) {
			assert(p >= 0 && p <= 3);
			return m_val[p];
		}
		/*
		__forceinline float dot(const Quaternion &v) const {
#if defined(AYA_USE_SIMD)
			__m128 vd = _mm_mul_ps(m_val128, v.m_val128);
			__m128 z = _mm_movehl_ps(vd, vd);
			__m128 y = _mm_pshufd_ps(vd, 0x55);
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
		__forceinline float length() const {
			return Sqrt(length2());
		}
		__forceinline float safeLength() const {
			float d = length2();
			if (d > SIMD_EPSILON) return Sqrt(d);
			return 0.f;
		}
		__forceinline float distance2(const Quaternion &p) const {
			return (p - (*this)).length2();
		}
		__forceinline float distance(const Quaternion &p) const {
			return (p - (*this)).length();
		}

		__forceinline Quaternion& normalize() {
#if defined(AYA_USE_SIMD)
			__m128 vd = _mm_mul_ps(m_val128, m_val128);
			__m128 z = _mm_movehl_ps(vd, vd);
			__m128 y = _mm_pshufd_ps(vd, 0x55);
			vd = _mm_add_ss(vd, y);
			vd = _mm_add_ss(vd, z);

			y = _mm_rsqrt_ss(vd);
			z = v1_5;
			vd = _mm_mul_ss(vd, v0_5);
			vd = _mm_mul_ss(vd, y);
			vd = _mm_mul_ss(vd, y);
			z = _mm_sub_ss(z, vd);
			y = _mm_mul_ss(y, z);

			y = _mm_splat_ps(y, 0x80);
			m_val128 = _mm_mul_ps(m_val128, y);

			return *this;
#else
			return *this /= length();
#endif
		}
		__forceinline Quaternion& safeNormalize() {
			float l2 = safeLength();
			if (l2 >= SIMD_EPSILON) {
				return *this /= l2;
			}
			else {
				setValue(1, 0, 0);
			}
			return *this;
		}
		*/
	};
}
#endif