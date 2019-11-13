#ifndef AYA_TRANSFORM_HPP
#define AYA_TRANSFORM_HPP

#include "Matrix3x3.hpp"
#include "BBox.hpp"
#include "Quaternion.hpp"

namespace Aya {
	__declspec(align(16)) class Transform {
#if defined(AYA_DEBUG)
	public:
#endif
		Matrix3x3 m_mat, m_inv;
		Vector3 m_trans;
	
	public:
		Transform() {
			m_mat = m_inv = Matrix3x3().getIdentity();
		}
		explicit __forceinline Transform(const Matrix3x3& m, const Vector3 &t) :
			m_mat(m), m_inv(m.inverse()), m_trans(t) {}
		explicit __forceinline Transform(const Matrix3x3& m) :
			m_mat(m), m_inv(m.inverse()), m_trans(Vector3()) {}
		explicit __forceinline Transform(const Matrix3x3& m, const Matrix3x3& inv, const Vector3 &t) :
			m_mat(m), m_inv(inv), m_trans(t) {}
		explicit __forceinline Transform(const Matrix3x3& m, const Matrix3x3& inv) :
			m_mat(m), m_inv(inv), m_trans(Vector3()) {}
		explicit __forceinline Transform(const Quaternion& q) { setRotation(q); }
		explicit __forceinline Transform(const Quaternion& q, const BaseVector3 &v) { setRotation(q); m_trans = v; }
		explicit __forceinline Transform(const Vector3& t) :
			m_mat(Matrix3x3().getIdentity()), m_inv(Matrix3x3().getIdentity()), m_trans(t) {}
		__forceinline Transform(const Transform &rhs) :
			m_mat(rhs.m_mat), m_inv(rhs.m_mat), m_trans(rhs.m_trans) {}
		__forceinline Transform& operator = (const Transform &rhs) {
			m_mat = rhs.m_mat;
			m_inv = rhs.m_inv;
			m_trans = rhs.m_trans;
			return *this;
		}

		__forceinline Transform inverse() const {
			return Transform(m_inv,
				m_mat,
				-(m_inv * m_trans));
		}

		__forceinline Transform operator * (const Transform &t) const {
			return Transform(m_mat * t.m_mat,
				t.m_inv * m_inv,
				(m_mat * t.m_trans) + m_trans);
		}
		__forceinline Transform& operator *= (const Transform &t) {
			m_mat *= t.m_mat;
			m_inv = t.m_inv * m_inv;
			m_trans = (m_mat * t.m_trans) + m_trans;
		}

		__forceinline Transform& setTranslate(const Vector3 &delta) {
			m_trans = delta;
			m_mat.setIdentity();
			m_inv.setIdentity();

			return *this;
		}
		__forceinline Transform& setTranslate(const float &x, const float &y, const float &z) {
			m_trans.setValue(x, y, z);
			m_mat.setIdentity();
			m_inv.setIdentity();

			return *this;
		}
		__forceinline Transform& setScale(const float &x, const float &y, const float &z) {
			assert(x != 0 && y != 0 && z != 0);
			m_mat.setValue(x, 0, 0,
				0, y, 0,
				0, 0, z);
			m_inv.setValue(1.f / x, 0, 0,
				0, 1.f / y, 0,
				0, 0, 1.f / z);
			m_trans.setZero();

			return *this;
		}
		__forceinline Transform& setRotateX(const float &angle) {
			float sin_t = std::sin(Radian(angle));
			float cos_t = std::cos(Radian(angle));
			m_mat.setValue(1, 0, 0,
				0, cos_t, sin_t,
				0, sin_t, -cos_t);
			m_inv = m_mat.transpose();
			m_trans.setZero();

			return *this;
		}
		__forceinline Transform& setRotateY(const float &angle) {
			float sin_t = std::sin(Radian(angle));
			float cos_t = std::cos(Radian(angle));
			m_mat.setValue(cos_t, -sin_t, 0,
				sin_t, cos_t, 0,
				0, 0, 1);
			m_inv = m_mat.transpose();
			m_trans.setZero();

			return *this;
		}
		__forceinline Transform& setRotateZ(const float &angle) {
			float sin_t = std::sin(Radian(angle));
			float cos_t = std::cos(Radian(angle));
			m_mat.setValue(cos_t, -sin_t, 0,
				sin_t, cos_t, 0,
				0, 0, 1);
			m_inv = m_mat.transpose();
			m_trans.setZero();

			return *this;
		}
		__forceinline Transform& setRotation(const float &angle, const Vector3 &axis) {
			Vector3 a = axis;
			a.normalize();
			float x = a.x();
			float y = a.y();
			float z = a.z();
			float s = std::sin(Radian(angle));
			float c = std::cos(Radian(angle));

			m_mat.setValue(x * x + (1.f - x * x) * c, x * y * (1.f - c) - z * s, x * z * (1.f - c) + y * s,
						   x * y * (1.f - c) + z * s, y * y + (1.f - y * y) * c, y * z * (1.f - c) - x * s,
				           x * z * (1.f - c) - y * s, y * z * (1.f - c) + x * s, z * z + (1.f - z * z) * c);
			m_inv = m_mat.transpose();
			m_trans.setZero();

			return *this;
		}
		void setRotation(const Quaternion& q)
		{
			float d = q.length2();
			assert(d != 0.f);
			float s = 2.f / d;

#if defined(AYA_USE_SIMD)
			__m128 vs, Q = q.get128();
			__m128i Qi = _mm_castps_si128(Q);
			__m128 Y, Z;
			__m128 V1, V2, V3;
			__m128 V11, V21, V31;
			__m128 NQ = _mm_xor_ps(Q, vMzeroMask);
			__m128i NQi = _mm_castps_si128(NQ);

			V1 = _mm_castsi128_ps(_mm_shuffle_epi32(Qi, _MM_SHUFFLE(1, 0, 2, 3)));  // Y X Z W
			V2 = _mm_shuffle_ps(NQ, Q, _MM_SHUFFLE(0, 0, 1, 3));                 // -X -X  Y  W
			V3 = _mm_castsi128_ps(_mm_shuffle_epi32(Qi, _MM_SHUFFLE(2, 1, 0, 3)));  // Z Y X W
			V1 = _mm_xor_ps(V1, vMPPP);                                         //	change the sign of the first element

			V11 = _mm_castsi128_ps(_mm_shuffle_epi32(Qi, _MM_SHUFFLE(1, 1, 0, 3)));  // Y Y X W
			V21 = _mm_unpackhi_ps(Q, Q);                                         //  Z  Z  W  W
			V31 = _mm_shuffle_ps(Q, NQ, _MM_SHUFFLE(0, 2, 0, 3));                 //  X  Z -X -W

			V2 = _mm_mul_ps(V2, V1);   //
			V1 = _mm_mul_ps(V1, V11);  //
			V3 = _mm_mul_ps(V3, V31);  //

			V11 = _mm_shuffle_ps(NQ, Q, _MM_SHUFFLE(2, 3, 1, 3));                //	-Z -W  Y  W
			V11 = _mm_mul_ps(V11, V21);                                                    //
			V21 = _mm_xor_ps(V21, vMPPP);                                       //	change the sign of the first element
			V31 = _mm_shuffle_ps(Q, NQ, _MM_SHUFFLE(3, 3, 1, 3));                //	 W  W -Y -W
			V31 = _mm_xor_ps(V31, vMPPP);                                       //	change the sign of the first element
			Y = _mm_castsi128_ps(_mm_shuffle_epi32(NQi, _MM_SHUFFLE(3, 2, 0, 3)));  // -W -Z -X -W
			Z = _mm_castsi128_ps(_mm_shuffle_epi32(Qi, _MM_SHUFFLE(1, 0, 1, 3)));   //  Y  X  Y  W

			vs = _mm_load_ss(&s);
			V21 = _mm_mul_ps(V21, Y);
			V31 = _mm_mul_ps(V31, Z);

			V1 = _mm_add_ps(V1, V11);
			V2 = _mm_add_ps(V2, V21);
			V3 = _mm_add_ps(V3, V31);

			vs = _mm_splat3_ps(vs, 0);
			//	s ready
			V1 = _mm_mul_ps(V1, vs);
			V2 = _mm_mul_ps(V2, vs);
			V3 = _mm_mul_ps(V3, vs);

			V1 = _mm_add_ps(V1, v1000);
			V2 = _mm_add_ps(V2, v0100);
			V3 = _mm_add_ps(V3, v0010);

			m_mat[0] = V1;
			m_mat[1] = V2;
			m_mat[2] = V3;
#else
			float xs = q.x() * s, ys = q.y() * s, zs = q.z() * s;
			float wx = q.w() * xs, wy = q.w() * ys, wz = q.w() * zs;
			float xx = q.x() * xs, xy = q.x() * ys, xz = q.x() * zs;
			float yy = q.y() * ys, yz = q.y() * zs, zz = q.z() * zs;

			m_mat.setValue(
				1.f - (yy + zz), xy - wz, xz + wy,
				xy + wz, 1.f - (xx + zz), yz - wx,
				xz - wy, yz + wx, 1.f - (xx + yy));
#endif
			m_inv = m_mat.transpose();
			m_trans.setZero();
		}
		__forceinline Transform& setEulerZYX(const float &e_x, const float &e_y, const float &e_z) {
			float ci(std::cosf(e_x));
			float cj(std::cosf(e_y));
			float ch(std::cosf(e_z));
			float si(std::sinf(e_x));
			float sj(std::sinf(e_y));
			float sh(std::sinf(e_z));
			float cc = ci * ch;
			float cs = ci * sh;
			float sc = si * ch;
			float ss = si * sh;

			m_mat.setValue(cj * ch, sj * sc - cs, sj * cc + ss,
				cj * sh, sj * ss + cc, sj * cs - sc,
				-sj, cj * si, cj * ci);
			m_inv = m_mat.transpose();
			m_trans.setZero();

			return *this;
		}
		__forceinline Transform& setEulerYPR(const float &yaw, const float &pitch, const float &roll) {
			return setEulerZYX(roll, pitch, yaw);
		}
		__forceinline Transform& setLookAt(const Point3 &pos, const Point3 &look, const Vector3 &u) {
			m_trans = pos;

			Vector3 dir = (look - pos).normalize();
			Vector3 left = u;
			left.normalize().cross(dir).normalize();
			Vector3 up = dir.cross(left);

			Matrix3x3 cam_to_world(left, up, dir);
			m_mat = cam_to_world.inverse();
			m_inv = cam_to_world;

			return *this;
		}

		__forceinline Vector3 operator() (Vector3 v) const {
			return m_mat * v;
		}
		__forceinline Point3 operator() (Point3 p) const {
			return m_mat * p + m_trans;
		}
		__forceinline Normal3 operator() (Normal3 n) const {
			return m_inv.transpose() * n;
		}
		__forceinline BBox operator() (BBox b) const {

			// const Transform &M = *this;
			// BBox ret(M(Point3(b.m_pmin.x(), b.m_pmin.y(), b.m_pmin.z())));
			// ret.unity(M(Point3(b.m_pmax.x(), b.m_pmin.y(), b.m_pmin.z())));
			// ret.unity(M(Point3(b.m_pmin.x(), b.m_pmax.y(), b.m_pmin.z())));
			// ret.unity(M(Point3(b.m_pmin.x(), b.m_pmin.y(), b.m_pmax.z())));
			// ret.unity(M(Point3(b.m_pmin.x(), b.m_pmax.y(), b.m_pmax.z())));
			// ret.unity(M(Point3(b.m_pmax.x(), b.m_pmax.y(), b.m_pmin.z())));
			// ret.unity(M(Point3(b.m_pmax.x(), b.m_pmin.y(), b.m_pmax.z())));
			// ret.unity(M(Point3(b.m_pmax.x(), b.m_pmax.y(), b.m_pmax.z())));
			// return ret;

			BBox ret;

			Point3 mid = (b.m_pmax + b.m_pmin) * .5f;
			Vector3 c = (b.m_pmax - b.m_pmin) * .5f;

			Vector3 x = m_mat.getColumn(0) * c[0];
			Vector3 y = m_mat.getColumn(1) * c[1];
			Vector3 z = m_mat.getColumn(2) * c[2];
			mid = (*this)(mid);

#if defined(AYA_USE_SIMD)
			x.m_val128 = _mm_and_ps(x.m_val128, vAbsfMask);
			y.m_val128 = _mm_and_ps(y.m_val128, vAbsfMask);
			z.m_val128 = _mm_and_ps(z.m_val128, vAbsfMask);
			x.m_val128 = _mm_add_ps(x.m_val128, y.m_val128);
			x.m_val128 = _mm_add_ps(x.m_val128, z.m_val128);

			ret.m_pmax = mid + x;
			ret.m_pmin = mid - x;
#else
			Vector3 cro(std::abs(x.x()) + std::abs(y.x()) + std::abs(z.x()),
				std::abs(x.y()) + std::abs(y.y()) + std::abs(z.y()),
				std::abs(x.z()) + std::abs(y.z()) + std::abs(z.z()));
			ret.m_pmax = mid + cro;
			ret.m_pmin = mid - cro;
#endif
			return ret;
		}
	};
}
#endif