/* -------------------------------------------------------- macro */
/* -------------------------------------------------------- typedef */
typedef Matrix<Scalar,4,4> Matrix4;

/* -------------------------------------------------------- constructor */
/* -------------------------------------------------------- extensions */
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Matrix3 matrix3() const
{
	return this->toRotationMatrix();
}
//EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
//Matrix4 matrix4() const
//{
//	Matrix4 ret(Matrix4::Identity());
//
//
//	//Matrix3 rot(this->toRotationMatrix());
// 	ret.block<3,3>(0, 0) = Matrix3();
//	return ret;
//}




