/* -------------------------------------------------------- constructor */
/* -------------------------------------------------------- extensions */
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Derived strictLowerTri()
{
	return Derived(triangularView<StrictlyLower>());
}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Derived strictUpperTri()
{
	return Derived(triangularView<StrictlyUpper>());
}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Derived lowerTri()
{
	return Derived(triangularView<Lower>());
}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Derived upperTri()
{
	return Derived(triangularView<Upper>());
}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Derived selfAdjoint()
{
	return Derived(triangularView<SelfAdjoint>());
}
EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Derived symmetric()
{
	return Derived(triangularView<Symmetric>());
}

EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
//Derived diagonal()
Derived diag()
{
	return ((*this) - (strictLowerTri() + strictUpperTri()));
}

EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE
Derived offDiagonal()
{
	return (*this) - ((*this) - (strictLowerTri() + strictUpperTri()));
}
