/* -------------------------------------------------------- macro */
#define EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE_2_3_4(TYPE) \
	EIGEN_STATIC_ASSERT_VECTOR_ONLY(TYPE);\
	static_assert(TYPE::SizeAtCompileTime == 2 || TYPE::SizeAtCompileTime == 3 || TYPE::SizeAtCompileTime == 4, "THIS_METHOD_IS_ONLY_FOR_VECTORS_OF_A_SPECIFIC_SIZE_2_3_4");
/* -------------------------------------------------------- variables */
static const size_t size_ 	= _Rows * _Cols; /* Supporting OpenMesh Decimator*/

/* -------------------------------------------------------- constructor */
/* ------------------------------------------ Vector2 */
/* ------------------------------------------ Vector3 */
/* ------------------------------------------ Vector4 */
/* ------------------------------------------ Matrix2x2 (not supported) */
//EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
//Matrix(Scalar m00, Scalar m01,
//	   Scalar m10, Scalar m11)
//{
//	Base::_check_template_params();
//	EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(Matrix, 2, 2);
//	this->coeffRef(0, 0) = m00; this->coeffRef(0, 1) = m01;
//	this->coeffRef(1, 0) = m10; this->coeffRef(1, 1) = m11;
//}
//
//template <typename OtherDerived>
//EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
//Matrix(const DenseBase<OtherDerived>& v0,
//	   const DenseBase<OtherDerived>& v1)
//{
//	Base::_check_template_params();
//	EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(Matrix, 2, 2);
//	EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(DenseBase<OtherDerived>, 2);
//	if(DenseBase<OtherDerived>::RowsAtCompileTime == 1 && DenseBase<OtherDerived>::ColsAtCompileTime == 2 )
//	{
//		/* row vector */
//		this->row(0) = v0;
//		this->row(1) = v1;
//	}
//	if(DenseBase<OtherDerived>::RowsAtCompileTime == 2 && DenseBase<OtherDerived>::ColsAtCompileTime == 1 )
//	{
//		/* col vector */
//		this->col(0) = v0;
//		this->col(1) = v1;
//	}
//
//
//}

/* ------------------------------------------ Matrix3x3 */
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Matrix(Scalar m00, Scalar m01, Scalar m02,
	   Scalar m10, Scalar m11, Scalar m12,
	   Scalar m20, Scalar m21, Scalar m22)
{
	Base::_check_template_params();
	EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(Matrix, 3, 3);

	this->coeffRef(0, 0) = m00; this->coeffRef(0, 1) = m01; this->coeffRef(0, 2) = m02;
	this->coeffRef(1, 0) = m10; this->coeffRef(1, 1) = m11; this->coeffRef(1, 2) = m12;
	this->coeffRef(2, 0) = m20; this->coeffRef(2, 1) = m21; this->coeffRef(2, 2) = m22;
}

template <typename OtherDerived>
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Matrix(const DenseBase<OtherDerived>& v0,
	   const DenseBase<OtherDerived>& v1,
	   const DenseBase<OtherDerived>& v2)
{
	Base::_check_template_params();
	EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(Matrix, 3, 3);
	EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(DenseBase<OtherDerived>, 3);

	if(DenseBase<OtherDerived>::RowsAtCompileTime == 1 && DenseBase<OtherDerived>::ColsAtCompileTime == 3 )
	{
		/* row vector */
		this->row(0) = v0;
		this->row(1) = v1;
		this->row(2) = v2;
	}
	if(DenseBase<OtherDerived>::RowsAtCompileTime == 3 && DenseBase<OtherDerived>::ColsAtCompileTime == 1 )
	{
		/* col vector */
		this->col(0) = v0;
		this->col(1) = v1;
		this->col(2) = v2;
	}
}

/* ------------------------------------------ Matrix4x4 */
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Matrix(Scalar m00, Scalar m01, Scalar m02, Scalar m03,
	   Scalar m10, Scalar m11, Scalar m12, Scalar m13,
	   Scalar m20, Scalar m21, Scalar m22, Scalar m23,
	   Scalar m30, Scalar m31, Scalar m32, Scalar m33)
{
	Base::_check_template_params();
	EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(Matrix, 4, 4);

	this->coeffRef(0, 0) = m00; this->coeffRef(0, 1) = m01; this->coeffRef(0, 2) = m02; this->coeffRef(0, 3) = m03;
	this->coeffRef(1, 0) = m10; this->coeffRef(1, 1) = m11; this->coeffRef(1, 2) = m12; this->coeffRef(1, 3) = m13;
	this->coeffRef(2, 0) = m20; this->coeffRef(2, 1) = m21; this->coeffRef(2, 2) = m22; this->coeffRef(2, 3) = m23;
	this->coeffRef(3, 0) = m30; this->coeffRef(3, 1) = m31; this->coeffRef(3, 2) = m32; this->coeffRef(3, 3) = m33;
}

template <typename OtherDerived>
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Matrix(const EigenBase<OtherDerived>& v0, const EigenBase<OtherDerived>& v1, const EigenBase<OtherDerived>& v2, const EigenBase<OtherDerived>& v3)
{
	Base::_check_template_params();
	EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(Matrix, 4, 4);
	EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(DenseBase<OtherDerived>, 4);

	if(DenseBase<OtherDerived>::RowsAtCompileTime == 1 && DenseBase<OtherDerived>::ColsAtCompileTime == 4 )
	{
		/* row vector */
		this->row(0) = v0;
		this->row(1) = v1;
		this->row(2) = v2;
		this->row(3) = v3;

	}
	if(DenseBase<OtherDerived>::RowsAtCompileTime == 4 && DenseBase<OtherDerived>::ColsAtCompileTime == 1 )
	{
		/* col vector */
		this->col(0) = v0;
		this->col(1) = v1;
		this->col(2) = v2;
		this->col(3) = v3;
	}
}

/* -------------------------------------------------------- operator overloading */



/* -------------------------------------------------------- extension */
/* ------------------------------------------ extra */
/* component-wise operation */
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Scalar cwiseMin	() const { return this->minCoeff(); }
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Scalar cwiseMax	() const { return this->maxCoeff(); }
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Scalar cwiseSum	() const { return this->sum(); }
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Scalar cwiseAbsMin() const { return 0; }
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Scalar cwiseAbsMax() const { return 0; }

EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix add(const Scalar& s) const { return this->array() + s; 		}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix sub(const Scalar& s) const { return this->array() - s; 		}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix mul(const Scalar& s) const { return this->array() * s; 		}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix div(const Scalar& s) const { return this->array() / s; 		}

EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix add(const Matrix& m) const { return this->array() + m.array(); }
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix sub(const Matrix& m) const { return this->array() - m.array(); }
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix mul(const Matrix& m) const { return this->array() * m.array(); }
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix div(const Matrix& m) const { return this->array() / m.array(); }

EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix radd(const Scalar& s) const { return s + this->array(); 		}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix rsub(const Scalar& s) const { return s - this->array(); 		}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix rmul(const Scalar& s) const { return s * this->array(); 		}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix rdiv(const Scalar& s) const { return s / this->array(); 		}

EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix radd(const Matrix& m) const { return m.array() + this->array(); }
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix rsub(const Matrix& m) const { return m.array() - this->array(); }
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix rmul(const Matrix& m) const { return m.array() * this->array(); }
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Matrix rdiv(const Matrix& m) const { return m.array() / this->array(); }

EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE void iadd(const Scalar& s) { this->array() += s; 		}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE void isub(const Scalar& s) { this->array() -= s; 		}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE void imul(const Scalar& s) { this->array() *= s; 		}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE void idiv(const Scalar& s) { this->array() /= s; 		}

EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE void iadd(const Matrix& m) { this->array() += m.array(); }
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE void isub(const Matrix& m) { this->array() -= m.array(); }
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE void imul(const Matrix& m) { this->array() *= m.array(); }
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE void idiv(const Matrix& m) { this->array() /= m.array(); }

EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Scalar sum() const 			{ return this->array().sum();							}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Scalar average() const 		{ return this->array().sum() / this->array().size();	}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Scalar min() const 			{ return this->array().minCoeff(); 						}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Scalar max() const 			{ return this->array().maxCoeff(); 						}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Scalar absMin() const
{
	Scalar m = this->coeff(0);
	for(typename Matrix::Index i = 1; i < this->size(); ++i)
		m = ((m * m < this->coeff(i) * this->coeff(i)) ? m : this->coeff(i));
	return m;
}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Scalar absMax() const
{
	Scalar m = this->coeff(0);
	for(typename Matrix::Index i = 1; i < this->size(); ++i)
		m = ((m * m > this->coeff(i) * this->coeff(i)) ? m : this->coeff(i));
	return m;
}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Matrix& normalize()
{
	*this /= this->norm();
	return *this;
}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Matrix& vectorize(Scalar v)
{
	this->fill(v);
	return *this;
}
/* ------------------------------------------ Vector2/3/4 */
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
RealScalar length() const
{
	EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE_2_3_4(Matrix);
	return this->norm();
}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
RealScalar lengthSquared() const
{
	EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE_2_3_4(Matrix);
	return this->squaredNorm();
}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
RealScalar distanceTo(const Matrix& other) const
{
	EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE_2_3_4(Matrix);
	return sub(other).length();
}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
RealScalar distanceSquaredTo(const Matrix& other) const
{
	EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE_2_3_4(Matrix);
	return sub(other).lengthSquared();
}

/* ------------------------------------------ Vector2 */
/* ------------------------------------------ Vector3 */
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
typename Matrix::Index dominantAxis() const
{
	EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Matrix, 3);
	return (std::fabs(this->m_storage.data()[0]) > std::fabs(this->m_storage.data()[1])) ?
		   ((std::fabs(this->m_storage.data()[0]) > std::fabs(this->m_storage.data()[2])) ? 0 : 2) :
		   ((std::fabs(this->m_storage.data()[1]) > std::fabs(this->m_storage.data()[2])) ? 1 : 2);
}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
typename Matrix::Index subminantAxis() const
{
	EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Matrix, 3);
	return (std::fabs(this->m_storage.data()[0]) < std::fabs(this->m_storage.data()[1])) ?
		   ((std::fabs(this->m_storage.data()[1]) < std::fabs(this->m_storage.data()[2])) ? 0 : 2) :
		   ((std::fabs(this->m_storage.data()[1]) < std::fabs(this->m_storage.data()[2])) ? 1 : 2);
}

EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Matrix reflected(const Matrix& normal) const
{
	EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Matrix, 3);
	return sub(normal.mul(2 * this->dot(normal)));
}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Matrix projected(const Matrix& normal) const
{
	EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Matrix, 3);
	return sub(normal.mul(this->dot(normal)));
}

EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
std::tuple<Matrix, Matrix> tangential() const
{
	EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Matrix, 3);

	Matrix a = ((std::fabs(this->m_storage.data()[1]) > 0 || std::fabs(this->m_storage.data()[2]) > 0) ?
				Matrix(1, 0, 0) :
				Matrix(0, 1, 0)).cross(*this).normalized();
	Matrix b = this->cross(a);
	return std::make_tuple(a, b);
}

/* ------------------------------------------ Vector4 specific */

/* ------------------------------------------ Matrix2x2/3x3/4x4 */
//EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
//Matrix diagonal() const
//{
//	return Matrix();
//}
//EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
//Matrix offDiagonal() const
//{
//	return (*this) - this->diagonal();
//	//return Matrix();
//}
//EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
//typename ConstTriangularViewReturnType<StrictlyLower>::Type strictLowerTri() const
//{
//	//Base::triangularView<StrictlyLower>();
//	//this->triangularView<StrictlyLower>();
//}




//EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
//Matrix strictUpperTri() const
//{
//	return this->triangularView<Eigen::StrictlyUpper>();
//}
//EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
//Matrix lowerTri() const
//{
//	return this->triangularView<Eigen::Lower>();
//}
//EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
//Matrix upperTri() const
//{
//	return this->triangularView<Eigen::Upper>();
//}




