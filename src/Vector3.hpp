#ifndef AYA_VECTOR3_HPP
#define AYA_VECTOR3_HPP

#include "MathUtility.hpp"

#if defined(AYA_USE_SIMD)
/* expands to the following value */
#define _MM_SHUFFLE(x, y, z, w) (((w) << 6 | (z) << 4 | (y) << 2 | (x)) & 0xff)

#define _mm_pshufd_ps(_a, _mask) _mm_shuffle_ps((_a), (_a), (_mask))
#define _mm_splat_ps(_a, _i) _mm_pshufd_ps((_a), _MM_SHUFFLE(_i, _i, _i, _i))
#define _mm_splat3_ps(_a, _i) _mm_pshufd_ps((_a), _MM_SHUFFLE(_i, _i, _i, 3))

#define vFFF0Mask (_mm_set_epi32(0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF))
#define vFFF0fMask _mm_castsi128_ps(vFFF0Mask)

#define v3AbsiMask (_mm_set_epi32(0x00000000, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF))
#define vAbsMask (_mm_set_epi32(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF))
#define vFFF0Mask (_mm_set_epi32(0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF))

#define vxyzMaskf vFFF0fMask
#define vAbsfMask _mm_castsi128_ps(vAbsMask)

#endif

namespace Aya {
	__declspec(align(16)) class BaseVector3 {
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
			assert(!isnan(m_val[0]) && !isnan(m_val[1]) && !isnan(m_val[2]));
		}
#else
#define numericValid
#endif

	public:
		BaseVector3() {}
		__forceinline BaseVector3(const float &x, const float &y, const float &z) {
			m_val[0] = x;
			m_val[1] = y;
			m_val[2] = z;
			m_val[3] = .0f;
			numericValid(1);
		}

#if defined(AYA_USE_SIMD)
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
		__forceinline void setValue(const float &x, const float &y, const float &z) {
			m_val[0] = x;
			m_val[1] = y;
			m_val[2] = z;
			m_val[3] = .0f;
			numericValid(1);
		}
		__forceinline void setX(const float &x) { m_val[0] = x; numericValid(1); }
		__forceinline void setY(const float &y) { m_val[1] = y; numericValid(1); }
		__forceinline void setZ(const float &z) { m_val[2] = z; numericValid(1); }
		__forceinline void setW(const float &w) { m_val[3] = w; numericValid(1); }
		__forceinline const float& getX() const { return m_val[0]; }
		__forceinline const float& getY() const { return m_val[1]; }
		__forceinline const float& getZ() const { return m_val[2]; }
		__forceinline const float& x() const { return m_val[0]; }
		__forceinline const float& y() const { return m_val[1]; }
		__forceinline const float& z() const { return m_val[2]; }
		__forceinline const float& w() const { return m_val[3]; }

		__forceinline bool operator == (const BaseVector3 &v) const {
#if defined(AYA_USE_SIMD)
			return (0xf == _mm_movemask_ps((__m128)_mm_cmpeq_ps(m_val128, v.m_val128)));
#else
			return ((m_val[0] == v.m_val[0]) &&
				(m_val[1] == v.m_val[1]) &&
				(m_val[2] == v.m_val[2]) &&
				(m_val[3] == v.m_val[3]));
#endif
		}
		__forceinline bool operator != (const BaseVector3 &v) const {
			return !((*this) == v);
		}

		__forceinline void setMax(const BaseVector3 &v) {
#if defined(AYA_USE_SIMD)
			m_val128 = _mm_max_ps(m_val128, v.m_val128);
#else
			SetMax(m_val[0], v.m_val[0]);
			SetMax(m_val[1], v.m_val[1]);
			SetMax(m_val[2], v.m_val[2]);
#endif
		}
		__forceinline void setMin(const BaseVector3 &v) {
#if defined(AYA_USE_SIMD)
			m_val128 = _mm_min_ps(m_val128, v.m_val128);
#else
			SetMin(m_val[0], v.m_val[0]);
			SetMin(m_val[1], v.m_val[1]);
			SetMin(m_val[2], v.m_val[2]);
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

		__forceinline BaseVector3 operator + (const BaseVector3 &v) const {
#if defined(AYA_USE_SIMD)
			return BaseVector3(_mm_add_ps(m_val128, v.m_val128));
#else
			return BaseVector3(m_val[0] + v.m_val[0],
				m_val[1] + v.m_val[1],
				m_val[2] + v.m_val[2]);
#endif
		}
		__forceinline BaseVector3 & operator += (const BaseVector3 &v) {
#if defined(AYA_USE_SIMD)
			m_val128 = _mm_add_ps(m_val128, v.m_val128);
#else
			m_val[0] += v.m_val[0];
			m_val[1] += v.m_val[1];
			m_val[2] += v.m_val[2];
#endif
			numericValid(1);
			return *this;
		}
		__forceinline BaseVector3 operator - (const BaseVector3 &v) const {
#if defined(AYA_USE_SIMD)
			return BaseVector3(_mm_sub_ps(m_val128, v.m_val128));
#else
			return BaseVector3(m_val[0] - v.m_val[0],
								m_val[1] - v.m_val[1],
								m_val[2] - v.m_val[2]);
#endif
		}
		__forceinline BaseVector3 & operator -= (const BaseVector3 &v) {
#if defined(AYA_USE_SIMD)
			m_val128 = _mm_sub_ps(m_val128, v.m_val128);
#else
			m_val[0] -= v.m_val[0];
			m_val[1] -= v.m_val[1];
			m_val[2] -= v.m_val[2];
#endif
			numericValid(1);
			return *this;
		}
		__forceinline BaseVector3 operator- () const {
			return BaseVector3(-m_val[0], -m_val[1], -m_val[2]);
		}
		__forceinline BaseVector3 operator * (const float &s) const {
#if defined(AYA_USE_SIMD)
			__m128 vs = _mm_load_ss(&s);
			vs = _mm_pshufd_ps(vs, 0x80);
			return BaseVector3(_mm_mul_ps(m_val128, vs));
#else
			return BaseVector3(m_val[0] * s,
								m_val[1] * s,
								m_val[2] * s);
#endif
			
		}
		__forceinline friend BaseVector3 operator * (const float &s, const BaseVector3 &v) {
			return v * s;
		}
		__forceinline BaseVector3 & operator *= (const float &s) {
#if defined(AYA_USE_SIMD)
			__m128 vs = _mm_load_ss(&s);
			vs = _mm_pshufd_ps(vs, 0x80);
			m_val128 = _mm_mul_ps(m_val128, vs);
#else
			m_val[0] *= s;
			m_val[1] *= s;
			m_val[2] *= s;
#endif
			numericValid(1);
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
		__forceinline BaseVector3& normalize() {
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
		__forceinline BaseVector3& safeNormalize() {
			float l2 = safeLength();
			if (l2 >= SIMD_EPSILON) {
				return *this /= l2;
			}
			else {
				setValue(1, 0, 0);
			}
			return *this;
		}

		__forceinline BaseVector3 cross(const BaseVector3 &v) const {
#if defined(AYA_USE_SIMD)
			__m128 T, V;

			T = _mm_pshufd_ps(m_val128, _MM_SHUFFLE(1, 2, 0, 3)); //			(Y Z X 0)
			V = _mm_pshufd_ps(v.m_val128, _MM_SHUFFLE(1, 2, 0, 3)); //		(Y Z X 0)

			V = _mm_mul_ps(V, m_val128);
			T = _mm_mul_ps(T, v.m_val128);
			V = _mm_sub_ps(V, T);

			V = _mm_pshufd_ps(V, _MM_SHUFFLE(1, 2, 0, 3));
			return BaseVector3(V);
#else
			return BaseVector3(
				m_val[1] * v.m_val[2] - m_val[2] * v.m_val[1],
				m_val[2] * v.m_val[0] - m_val[0] * v.m_val[2],
				m_val[0] * v.m_val[1] - m_val[1] * v.m_val[0]);
#endif
		}
		
		__forceinline float angle(const BaseVector3 &v) const {
			float s = Sqrt(length2() * v.length2());
			assert(s != 0);
			return std::acos(dot(v) / s);
		}
		__forceinline BaseVector3 rotate(const BaseVector3 &axis, const float &angle) const {
#if defined(AYA_USE_SIMD)
			__m128 O = _mm_mul_ps(axis.m_val128, m_val128);
			float ssin = std::sin(angle);
			__m128 C = axis.cross(m_val128).m_val128;
			O = _mm_and_ps(O, vFFF0fMask);
			float scos = std::cos(angle);

			__m128 vsin = _mm_load_ss(&ssin); // (S 0 0 0)
			__m128 vcos = _mm_load_ss(&scos); // (S 0 0 0)

			__m128 Y = _mm_pshufd_ps(O, 0xC9);    // (Y Z X 0)
			__m128 Z = _mm_pshufd_ps(O, 0xD2);    // (Z X Y 0)
			O = _mm_add_ps(O, Y);
			vsin = _mm_pshufd_ps(vsin, 0x80);     // (S S S 0)
			O = _mm_add_ps(O, Z);
			vcos = _mm_pshufd_ps(vcos, 0x80);     // (S S S 0)

			vsin = _mm_mul_ps(vsin, C);
			O = _mm_mul_ps(O, axis.m_val128);
			__m128 X = _mm_sub_ps(m_val128, O);

			O = _mm_add_ps(O, vsin);
			vcos = _mm_mul_ps(vcos, X);
			O = _mm_add_ps(O, vcos);

			return BaseVector3(O);
#else
			BaseVector3 o = axis * axis.dot(*this);
			BaseVector3 X = *this - o;
			BaseVector3 Y = axis.cross(*this);

			return (o + X * std::cos(angle) + Y * std::sin(angle));
#endif
		}

		friend inline std::ostream &operator<<(std::ostream &os, const BaseVector3 &v) {
			os << "[ "  << AYA_SCALAR_OUTPUT(v.m_val[0])
				<< ", " << AYA_SCALAR_OUTPUT(v.m_val[1])
				<< ", " << AYA_SCALAR_OUTPUT(v.m_val[2])
				<< " ]";
			return os;
		}
	};

	__declspec(align(16)) class Vector3 : public BaseVector3 {
	public:
		Vector3() {}
		__forceinline Vector3(const float &x, const float &y, const float &z) {
			m_val[0] = x;
			m_val[1] = y;
			m_val[2] = z;
			m_val[3] = .0f;
		}

#if defined(AYA_USE_SIMD)
		__forceinline Vector3(const __m128 &v128) {
			m_val128 = v128;
		}
		__forceinline Vector3(const BaseVector3 &rhs) {
			m_val128 = rhs.m_val128;
		}
		__forceinline Vector3& operator =(const BaseVector3 &rhs) {
			m_val128 = rhs.m_val128;
			return *this;
		}
#else
		__forceinline Vector3(const BaseVector3 &rhs) {
			m_val[0] = rhs.m_val[0];
			m_val[1] = rhs.m_val[1];
			m_val[2] = rhs.m_val[2];
		}
		__forceinline Vector3& operator =(const BaseVector3 &rhs) {
			m_val[0] = rhs.m_val[0];
			m_val[1] = rhs.m_val[1];
			m_val[2] = rhs.m_val[2];
			return *this;
		}
#endif
	};

	__declspec(align(16)) class Point3 : public BaseVector3 {
	public:
		Point3() {}
		__forceinline Point3(const float &x, const float &y, const float &z) {
			m_val[0] = x;
			m_val[1] = y;
			m_val[2] = z;
			m_val[3] = .0f;
		}

#if defined(AYA_USE_SIMD)
		__forceinline Point3(const __m128 &v128) {
			m_val128 = v128;
		}
		__forceinline Point3(const BaseVector3 &rhs) {
			m_val128 = rhs.m_val128;
		}
		__forceinline Point3& operator =(const BaseVector3 &rhs) {
			m_val128 = rhs.m_val128;
			return *this;
		}
#else
		__forceinline Point3(const BaseVector3 &rhs) {
			m_val[0] = rhs.m_val[0];
			m_val[1] = rhs.m_val[1];
			m_val[2] = rhs.m_val[2];
		}
		__forceinline Point3& operator =(const BaseVector3 &rhs) {
			m_val[0] = rhs.m_val[0];
			m_val[1] = rhs.m_val[1];
			m_val[2] = rhs.m_val[2];
			return *this;
		}
#endif
	};

	__declspec(align(16)) class Normal3 : public BaseVector3 {
	public:
		Normal3() {}
		__forceinline Normal3(const float &x, const float &y, const float &z) {
			m_val[0] = x;
			m_val[1] = y;
			m_val[2] = z;
			m_val[3] = .0f;
		}

#if defined(AYA_USE_SIMD)
		__forceinline Normal3(const __m128 &v128) {
			m_val128 = v128;
		}
		__forceinline Normal3(const BaseVector3 &rhs) {
			m_val128 = rhs.m_val128;
		}
		__forceinline Normal3& operator =(const BaseVector3 &rhs) {
			m_val128 = rhs.m_val128;
			return *this;
		}
#else
		__forceinline Normal3(const BaseVector3 &rhs) {
			m_val[0] = rhs.m_val[0];
			m_val[1] = rhs.m_val[1];
			m_val[2] = rhs.m_val[2];
		}
		__forceinline Normal3& operator =(const BaseVector3 &rhs) {
			m_val[0] = rhs.m_val[0];
			m_val[1] = rhs.m_val[1];
			m_val[2] = rhs.m_val[2];
			return *this;
		}
#endif
	};
}
#endif