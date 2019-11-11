#ifndef AYA_TRANSFORM_HPP
#define AYA_TRANSFORM_HPP

#include "Matrix3x3.hpp"

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
			Vector3 left = u.normalize.cross(dir).normalize();
			Vector3 up = dir.cross(left);

			Matrix3x3 cam_to_world(left, up, dir);
			m_mat = cam_to_world.inverse();
			m_inv = cam_to_world;

			return *this;
		}

		__forceinline Vector3 operator() (Vector3 v) const {
			return m_mat * v;
		}
		__forceinline Point3 operator() (Point3 v) const {
			return m_mat * v + m_trans;
		}
		__forceinline Normal3 operator() (Normal3 v) const {
			return m_inv.transpose() * v;
		}
	};
}
#endif