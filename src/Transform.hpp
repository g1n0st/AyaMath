#ifndef AYA_TRANSFORM
#define AYA_TRANSFORM

#include "Matrix3x3.hpp"

namespace Aya {
	__declspec(align(16)) class Transform {
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
				-m_trans.mul(m_inv));
		}

		__forceinline Transform operator * (const Transform &t) const {
			return Transform(m_mat * t.m_mat,
				t.m_inv * m_inv,
				t.m_trans.mul(m_mat) + m_trans);
		}
		__forceinline Transform& operator *= (const Transform &t) {
			m_mat *= t.m_mat;
			m_inv = t.m_inv * m_inv;
			m_trans = t.m_trans.mul(m_mat) + m_trans;
		}
	};
}
#endif