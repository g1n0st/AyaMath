#ifndef AYA_QUATERNION_HPP
#define AYA_QUATERNION_HPP

#include "MathUtility.hpp"
#include "Vector3.hpp"

#define AYA_EULER_DEFAULT_ZYX
#if defined (AYA_USE_SIMD)
#define vOnes (_mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f))
#define vQInv (_mm_set_ps(+0.0f, -0.0f, -0.0f, -0.0f))
#define vPPPM (_mm_set_ps(-0.0f, +0.0f, +0.0f, +0.0f))
#endif

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
		__forceinline Quaternion(const BaseVector3 &axis, const float &angle) {
			setRotation(axis, angle);
		}
		__forceinline Quaternion(const float &yaw, const float &pitch, const float &roll) {
#ifndef AYA_EULER_DEFAULT_ZYX
			setEuler(yaw, pitch, roll);
#else
			setEulerZYX(yaw, pitch, roll);
#endif
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

		__forceinline bool operator == (const Quaternion &q) const {
#if defined(AYA_USE_SIMD)
			return (0xf == _mm_movemask_ps((__m128)_mm_cmpeq_ps(m_val128, q.m_val128)));
#else
			return ((m_val[0] == q.m_val[0]) &&
				(m_val[1] == q.m_val[1]) &&
				(m_val[2] == q.m_val[2]) &&
				(m_val[3] == q.m_val[3]));
#endif
		}
		__forceinline bool operator != (const Quaternion &q) const {
			return !((*this) == q);
		}

		__forceinline void setMax(const Quaternion &q) {
#if defined(AYA_USE_SIMD)
			m_val128 = _mm_max_ps(m_val128, q.m_val128);
#else
			SetMax(m_val[0], q.m_val[0]);
			SetMax(m_val[1], q.m_val[1]);
			SetMax(m_val[2], q.m_val[2]);
			SetMax(m_val[3], q.m_val[3]);
#endif
		}
		__forceinline void setMin(const Quaternion &q) {
#if defined(AYA_USE_SIMD)
			m_val128 = _mm_min_ps(m_val128, q.m_val128);
#else
			SetMin(m_val[0], q.m_val[0]);
			SetMin(m_val[1], q.m_val[1]);
			SetMin(m_val[2], q.m_val[2]);
			SetMin(m_val[3], q.m_val[3]);
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

		void setRotation(const BaseVector3 &axis, const float &angle)
		{
			float d = axis.length();
			assert(d != 0.0f);
			float s = std::sin(angle * 0.5f) / d;
			setValue(axis.x() * s, axis.y() * s, axis.z() * s, std::cos(angle * 0.5f));
		}
		void setEuler(const float& yaw, const float& pitch, const float& roll)
		{
			float half_yaw = 0.5f * yaw;
			float half_pitch = 0.5f * pitch;
			float half_roll = 0.5f * roll;
			float cos_yaw = std::cos(half_yaw);
			float sin_yaw = std::sin(half_yaw);
			float cos_pitch = std::cos(half_pitch);
			float sin_pitch = std::sin(half_pitch);
			float cos_roll = std::cos(half_roll);
			float sin_roll = std::sin(half_roll);
			setValue(cos_roll * sin_pitch * cos_yaw + sin_roll * cos_pitch * sin_yaw,
				cos_roll * cos_pitch * sin_yaw - sin_roll * sin_pitch * cos_yaw,
				sin_roll * cos_pitch * cos_yaw - cos_roll * sin_pitch * sin_yaw,
				cos_roll * cos_pitch * cos_yaw + sin_roll * sin_pitch * sin_yaw);
		}
		void setEulerZYX(const float& yawZ, const float& pitch_y, const float& roll_x)
		{
			float half_yaw = 0.5f * yawZ;
			float half_pitch = 0.5f * pitch_y;
			float half_roll = 0.5f * roll_x;
			float cos_yaw = std::cos(half_yaw);
			float sin_yaw = std::sin(half_yaw);
			float cos_pitch = std::cos(half_pitch);
			float sin_pitch = std::sin(half_pitch);
			float cos_roll = std::cos(half_roll);
			float sin_roll = std::sin(half_roll);
			setValue(sin_roll * cos_pitch * cos_yaw - cos_roll * sin_pitch * sin_yaw,   //x
				cos_roll * sin_pitch * cos_yaw + sin_roll * cos_pitch * sin_yaw,   //y
				cos_roll * cos_pitch * sin_yaw - sin_roll * sin_pitch * cos_yaw,   //z
				cos_roll * cos_pitch * cos_yaw + sin_roll * sin_pitch * sin_yaw);  //formerly yzx
		}
		void getEulerZYX(float &yawZ, float &pitchY, float &rollX) const
		{
			float squ = m_val[0] * m_val[0];
			float sqx = m_val[1] * m_val[1];
			float sqy = m_val[2] * m_val[2];
			float sqz = m_val[3] * m_val[3];
			float sarg = -2.f * (m_val[0] * m_val[2] - m_val[3] * m_val[1]);

			// If the pitch angle is PI/2 or -PI/2, we can only compute
			// the sum roll + yaw.  However, any combination that gives
			// the right sum will produce the correct orientation, so we
			// set rollX = 0 and compute yawZ.
			if (sarg <= -0.99999f)
			{
				pitchY = -0.5f * float(M_PI);
				rollX = 0;
				yawZ = 2.f * std::atan2(m_val[0], -m_val[1]);
			}
			else if (sarg >= 0.99999f)
			{
				pitchY = 0.5f * float(M_PI);
				rollX = 0;
				yawZ = 2.f * std::atan2(-m_val[0], m_val[1]);
			}
			else
			{
				pitchY = std::asin(sarg);
				rollX = std::atan2(2.f * (m_val[1] * m_val[2] + m_val[3] * m_val[0]), squ - sqx - sqy + sqz);
				yawZ = std::atan2(2.f * (m_val[0] * m_val[1] + m_val[3] * m_val[2]), squ + sqx - sqy - sqz);
			}
		}

		__forceinline Quaternion operator + (const Quaternion &q) const {
#if defined(AYA_USE_SIMD)
			return Quaternion(_mm_add_ps(m_val128, q.m_val128));
#else
			return Quaternion(m_val[0] + q.m_val[0],
				m_val[1] + q.m_val[1],
				m_val[2] + q.m_val[2],
				m_val[3] + q.m_val[3]);
#endif
		}
		__forceinline Quaternion & operator += (const Quaternion &q) {
#if defined(AYA_USE_SIMD)
			m_val128 = _mm_add_ps(m_val128, q.m_val128);
#else
			m_val[0] += q.m_val[0];
			m_val[1] += q.m_val[1];
			m_val[2] += q.m_val[2];
			m_val[3] += q.m_val[3];
#endif
			numericValid(1);
			return *this;
		}
		__forceinline Quaternion operator - (const Quaternion &q) const {
#if defined(AYA_USE_SIMD)
			return Quaternion(_mm_sub_ps(m_val128, q.m_val128));
#else
			return Quaternion(m_val[0] - q.m_val[0],
				m_val[1] - q.m_val[1],
				m_val[2] - q.m_val[2],
				m_val[3] - q.m_val[3]);
#endif
		}
		__forceinline Quaternion & operator -= (const Quaternion &q) {
#if defined(AYA_USE_SIMD)
			m_val128 = _mm_sub_ps(m_val128, q.m_val128);
#else
			m_val[0] -= q.m_val[0];
			m_val[1] -= q.m_val[1];
			m_val[2] -= q.m_val[2];
			m_val[3] -= q.m_val[3];
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
		__forceinline Quaternion operator * (const Quaternion & q) const {
#if defined(AYA_USE_SIMD)
			__m128 vQ1 = m_val128;
			__m128 vQ2 = q.m_val128;
			__m128 A0, A1, B1, A2, B2;

			A1 = _mm_pshufd_ps(vQ1, _MM_SHUFFLE(0, 1, 2, 0));  // X Y  z x     //      vtrn
			B1 = _mm_pshufd_ps(vQ2, _MM_SHUFFLE(3, 3, 3, 0));  // W W  W X     // vdup vext

			A1 = _mm_mul_ps(A1, B1);

			A2 = _mm_pshufd_ps(vQ1, _MM_SHUFFLE(1, 2, 0, 1));  // Y Z  X Y     // vext
			B2 = _mm_pshufd_ps(vQ2, _MM_SHUFFLE(2, 0, 1, 1));  // z x  Y Y     // vtrn vdup

			A2 = _mm_mul_ps(A2, B2);

			B1 = _mm_pshufd_ps(vQ1, _MM_SHUFFLE(2, 0, 1, 2));  // z x Y Z      // vtrn vext
			B2 = _mm_pshufd_ps(vQ2, _MM_SHUFFLE(1, 2, 0, 2));  // Y Z x z      // vext vtrn

			B1 = _mm_mul_ps(B1, B2);  //	A3 *= B3

			A0 = _mm_splat_ps(vQ1, 3);  //	A0
			A0 = _mm_mul_ps(A0, vQ2);             //	A0 * B0

			A1 = _mm_add_ps(A1, A2);  //	AB12
			A0 = _mm_sub_ps(A0, B1);  //	AB03 = AB0 - AB3

			A1 = _mm_xor_ps(A1, vPPPM);  //	change sign of the last element
			A0 = _mm_add_ps(A0, A1);                //	AB03 + AB12

			return Quaternion(A0);
#else
			return Quaternion(
				w() * q.x() + x() * q.w() + y() * q.z() - z() * q.y(),
				w() * q.y() + y() * q.w() + z() * q.x() - x() * q.z(),
				w() * q.z() + z() * q.w() + x() * q.y() - y() * q.x(),
				w() * q.w() - x() * q.x() - y() * q.y() - z() * q.z());
#endif
		}
		__forceinline Quaternion& operator *= (const Quaternion & q) {
#if defined(AYA_USE_SIMD)
			__m128 vQ2 = q.get128();

			__m128 A1 = _mm_pshufd_ps(m_val128, _MM_SHUFFLE(0, 1, 2, 0));
			__m128 B1 = _mm_pshufd_ps(vQ2, _MM_SHUFFLE(3, 3, 3, 0));

			A1 = _mm_mul_ps(A1, B1);

			__m128 A2 = _mm_pshufd_ps(m_val128, _MM_SHUFFLE(1, 2, 0, 1));
			__m128 B2 = _mm_pshufd_ps(vQ2, _MM_SHUFFLE(2, 0, 1, 1));

			A2 = _mm_mul_ps(A2, B2);

			B1 = _mm_pshufd_ps(m_val128, _MM_SHUFFLE(2, 0, 1, 2));
			B2 = _mm_pshufd_ps(vQ2, _MM_SHUFFLE(1, 2, 0, 2));

			B1 = _mm_mul_ps(B1, B2);  //	A3 *= B3

			m_val128 = _mm_splat_ps(m_val128, 3);  //	A0
			m_val128 = _mm_mul_ps(m_val128, vQ2);            //	A0 * B0

			A1 = _mm_add_ps(A1, A2);                //	AB12
			m_val128 = _mm_sub_ps(m_val128, B1);      //	AB03 = AB0 - AB3
			A1 = _mm_xor_ps(A1, vPPPM);  //	change sign of the last element
			m_val128 = _mm_add_ps(m_val128, A1);      //	AB03 + AB12
#else
			setValue(
				m_val[3] * q.x() + m_val[0] * q.w() + m_val[1] * q.z() - m_val[2] * q.y(),
				m_val[3] * q.y() + m_val[1] * q.w() + m_val[2] * q.x() - m_val[0] * q.z(),
				m_val[3] * q.z() + m_val[2] * q.w() + m_val[0] * q.y() - m_val[1] * q.x(),
				m_val[3] * q.w() - m_val[0] * q.x() - m_val[1] * q.y() - m_val[2] * q.z());
#endif
			return *this;
		}
		__forceinline Quaternion operator * (const BaseVector3 & v) const {
#if defined(AYA_USE_SIMD)
			__m128 vQ1 = get128();
			__m128 vQ2 = v.get128();
			__m128 A1, B1, A2, B2, A3, B3;

			A1 = _mm_pshufd_ps(vQ1, _MM_SHUFFLE(3, 3, 3, 0));
			B1 = _mm_pshufd_ps(vQ2, _MM_SHUFFLE(0, 1, 2, 0));

			A1 = _mm_mul_ps(A1, B1);

			A2 = _mm_pshufd_ps(vQ1, _MM_SHUFFLE(1, 2, 0, 1));
			B2 = _mm_pshufd_ps(vQ2, _MM_SHUFFLE(2, 0, 1, 1));

			A2 = _mm_mul_ps(A2, B2);

			A3 = _mm_pshufd_ps(vQ1, _MM_SHUFFLE(2, 0, 1, 2));
			B3 = _mm_pshufd_ps(vQ2, _MM_SHUFFLE(1, 2, 0, 2));

			A3 = _mm_mul_ps(A3, B3);  //	A3 *= B3

			A1 = _mm_add_ps(A1, A2);                //	AB12
			A1 = _mm_xor_ps(A1, vPPPM);  //	change sign of the last element
			A1 = _mm_sub_ps(A1, A3);                //	AB123 = AB12 - AB3

			return Quaternion(A1);
#else
			return Quaternion(
				w() * v.x() + y() * v.z() - z() * v.y(),
				w() * v.y() + z() * v.x() - x() * v.z(),
				w() * v.z() + x() * v.y() - y() * v.x(),
				-x() * v.x() - y() * v.y() - z() * v.z());
#endif
		}
		friend __forceinline Quaternion operator * (const BaseVector3 &w, const Quaternion& q) {
#if defined(AYA_USE_SIMD)
			__m128 vQ1 = w.get128();
			__m128 vQ2 = q.get128();
			__m128 A1, B1, A2, B2, A3, B3;

			A1 = _mm_pshufd_ps(vQ1, _MM_SHUFFLE(0, 1, 2, 0));  // X Y  z x
			B1 = _mm_pshufd_ps(vQ2, _MM_SHUFFLE(3, 3, 3, 0));  // W W  W X

			A1 = _mm_mul_ps(A1, B1);

			A2 = _mm_pshufd_ps(vQ1, _MM_SHUFFLE(1, 2, 0, 1));
			B2 = _mm_pshufd_ps(vQ2, _MM_SHUFFLE(2, 0, 1, 1));

			A2 = _mm_mul_ps(A2, B2);

			A3 = _mm_pshufd_ps(vQ1, _MM_SHUFFLE(2, 0, 1, 2));
			B3 = _mm_pshufd_ps(vQ2, _MM_SHUFFLE(1, 2, 0, 2));

			A3 = _mm_mul_ps(A3, B3);  //	A3 *= B3

			A1 = _mm_add_ps(A1, A2);                //	AB12
			A1 = _mm_xor_ps(A1, vPPPM);  //	change sign of the last element
			A1 = _mm_sub_ps(A1, A3);                //	AB123 = AB12 - AB3

			return Quaternion(A1);
#else
			return Quaternion(
				+w.x() * q.w() + w.y() * q.z() - w.z() * q.y(),
				+w.y() * q.w() + w.z() * q.x() - w.x() * q.z(),
				+w.z() * q.w() + w.x() * q.y() - w.y() * q.x(),
				-w.x() * q.x() - w.y() * q.y() - w.z() * q.z());
#endif
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
			assert(p >= 0 && p <= 3);
			return m_val[p];
		}
		float &operator [](const int &p) {
			assert(p >= 0 && p <= 3);
			return m_val[p];
		}
		
		__forceinline float dot(const Quaternion &q) const {
#if defined(AYA_USE_SIMD)
			__m128 vd = _mm_mul_ps(m_val128, q.m_val128);

			__m128 t = _mm_movehl_ps(vd, vd);
			vd = _mm_add_ps(vd, t);
			t = _mm_shuffle_ps(vd, vd, 0x55);
			vd = _mm_add_ss(vd, t);

			return _mm_cvtss_f32(vd);
#else
			return	m_val[0] * q.m_val[0] +
				m_val[1] * q.m_val[1] +
				m_val[2] * q.m_val[2] +
				m_val[3] * q.m_val[3];
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

		__forceinline Quaternion& normalize() {
#if defined(AYA_USE_SIMD)
			__m128 vd = _mm_mul_ps(m_val128, m_val128);

			__m128 t = _mm_movehl_ps(vd, vd);
			vd = _mm_add_ps(vd, t);
			t = _mm_pshufd_ps(vd, 0x55);
			vd = _mm_add_ss(vd, t);

			vd = _mm_sqrt_ss(vd);
			vd = _mm_div_ss(vOnes, vd);
			vd = _mm_pshufd_ps(vd, 0);
			m_val128 = _mm_mul_ps(m_val128, vd);

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
				setValue(1, 0, 0, 0);
			}
			return *this;
		}
		
		__forceinline float getAngle() const {
			return 2.f * std::acos(m_val[3]);
		}
		__forceinline BaseVector3 getAxis() const {
			float s_squared = 1.f - m_val[3] * m_val[3];
			if (s_squared < 10.f * SIMD_EPSILON) //Check for divide by zero
				return BaseVector3(1.0, 0.0, 0.0);           // Arbitrary
			float s = 1.f / Sqrt(s_squared);
			return BaseVector3(m_val[0] * s, m_val[1] * s, m_val[2] * s);
		}
		__forceinline Quaternion inverse() const {
#if defined(AYA_USE_SIMD)
			return Quaternion(_mm_xor_ps(m_val128, vQInv));
#else
			return Quaternion(-m_val[0], -m_val[1], -m_val[2], m_val[3]);
#endif
		}
		Quaternion slerp(const Quaternion &q, const float &t) const
		{
			const float magnitude = Sqrt(length2() * q.length2());
			assert(magnitude > 0.f);

			const float product = dot(q) / magnitude;
			const float absproduct = std::fabs(product);

			if (absproduct < 1.0f - SIMD_EPSILON)
			{
				// Take care of long angle case see http://en.wikipedia.org/wiki/Slerp
				const float theta = std::cos(absproduct);
				const float d = std::sin(theta);
				assert(d > 0.f);

				const float sign = (product < 0) ? -1.f : 1.f;
				const float s0 = std::sin((1.0f - t) * theta) / d;
				const float s1 = std::sin(sign * t * theta) / d;

				return Quaternion(
					(m_val[0] * s0 + q.x() * s1),
					(m_val[1] * s0 + q.y() * s1),
					(m_val[2] * s0 + q.z() * s1),
					(m_val[3] * s0 + q.w() * s1));
			}
			else
			{
				return *this;
			}
		}

		friend inline std::ostream &operator<<(std::ostream &os, const Quaternion &v) {
			os << "[ " << AYA_SCALAR_OUTPUT(v.m_val[0])
				<< ", " << AYA_SCALAR_OUTPUT(v.m_val[1])
				<< ", " << AYA_SCALAR_OUTPUT(v.m_val[2])
				<< ", " << AYA_SCALAR_OUTPUT(v.m_val[3])
				<< " ]";
			return os;
		}
	};
}
#endif