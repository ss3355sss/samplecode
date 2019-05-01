/* -------------------------------------------------------- constructor */
EIGEN_DEVICE_FUNC
EIGEN_STRONG_INLINE Quaternion(const typename Base::Vector3& u,
							   const typename Base::Vector3& v,
							   const typename Base::Vector3& w)
{
	typename Base::Matrix3 m;
	m.col(0) = u;
	m.col(1) = v;
	m.col(2) = w;

	*this = m;
}
EIGEN_DEVICE_FUNC
EIGEN_STRONG_INLINE Quaternion(const typename Base::Vector3& axis, const typename Base::Scalar& angle)
{
	*this = typename Base::AngleAxisType(angle, axis);
}

/* -------------------------------------------------------- extensions */
EIGEN_DEVICE_FUNC
EIGEN_STRONG_INLINE typename Base::Vector3 axis()
{
	EIGEN_USING_STD_MATH(acos);

	typename Base::Vector3 result(this->x(), this->y(), this->z());
	result.normalize();

	if (static_cast<typename Base::Scalar>(2) * acos(this->w()) < static_cast<typename Base::Scalar>(EIGEN_PI))
		return result;
	else
		return -result;


}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
typename Base::Scalar angle()
{
	EIGEN_USING_STD_MATH(acos);

	typename Base::Scalar result = static_cast<typename Base::Scalar>(2) * acos(this->w());

	if (result < static_cast<typename Base::Scalar>(EIGEN_PI))
		return result;
	else
		return static_cast<typename Base::Scalar>(2) * static_cast<typename Base::Scalar>(EIGEN_PI) - result;		// Wrap around


}

//EIGEN_DEVICE_FUNC
//EIGEN_STRONG_INLINE void set(const typename Base::Scalar& w, const typename Base::Scalar& x, const typename Base::Scalar& y, const typename Base::Scalar& z)
//{
//	this->w() = w;
//	this->x() = x;
//	this->y() = y;
//	this->z() = z;
//}

EIGEN_DEVICE_FUNC
EIGEN_STRONG_INLINE void getAxisAngle(typename Base::Vector3* axis, typename Base::Scalar* angle) const
{
	//axis->set(x, y, z);
	(*axis)[0] = this->x();
	(*axis)[1] = this->y();
	(*axis)[2] = this->z();

	axis->normalize();
	*angle = 2 * std::acos(this->w());

	if (*angle > EIGEN_PI)
	{
		// Wrap around
		(*axis) = -(*axis);
		*angle = 2 * EIGEN_PI - (*angle);
	}
}


EIGEN_DEVICE_FUNC
EIGEN_STRONG_INLINE void rotate(typename Base::Scalar rad)
{
	typename Base::Vector3 	axis;
	typename Base::Scalar	currentAngle;

	this->getAxisAngle(&axis, &currentAngle);

	currentAngle += rad;

	typename Base::AngleAxisType aa(currentAngle, axis);

	*this = aa;
}

EIGEN_DEVICE_FUNC
EIGEN_STRONG_INLINE typename Base::Scalar* data()
{
	return this->coeffs().data();
}
EIGEN_DEVICE_FUNC
EIGEN_STRONG_INLINE const typename Base::Scalar* data() const
{
	return this->coeffs().data();
}


