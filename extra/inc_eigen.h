#pragma once

#ifdef Success
    #undef Success
#endif

/* define array major order for matrix */
//#define EIGEN_MATRIX_STORAGE_COL_MAJOR_ORDER    Eigen::ColMajor /* ColMajor = Opengl */
#define EIGEN_MATRIX_STORAGE_ROW_MAJOR_ORDER    Eigen::RowMajor /* RowMajor = Maya Api, PBRT, DirectX, RenderMan (!!Performance Optimized!!)*/

/* ----------------------------------------------------- Eigen Extension Plugin */
#define EIGEN_ARRAYBASE_PLUGIN          "eigen_array_base_plugin.h"
#define EIGEN_ARRAY_PLUGIN              "eigen_array_plugin.h"

#define EIGEN_MATRIXBASE_PLUGIN         "eigen_matrix_base_plugin.h"
#define EIGEN_MATRIX_PLUGIN             "eigen_matrix_plugin.h"

#define EIGEN_QUATERNIONBASE_PLUGIN     "eigen_quaternion_base_plugin.h"
#define EIGEN_QUATERNION_PLUGIN         "eigen_quaternion_plugin.h"

/* ----------------------------------------------------- Include Eigen */
#include <Eigen/Sparse>
#include <Eigen/Dense>

/* ----------------------------------------------------- Boost Serialize Interface */
#ifndef BOOST_SERIALIZATION_IO_SAVELOAD_EIGEN_H
#define BOOST_SERIALIZATION_IO_SAVELOAD_EIGEN_H
#include <boost/serialization/array.hpp>
#include <boost/serialization/split_free.hpp>
namespace boost
{
namespace serialization
{
template<class Archive, typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols> inline void save(Archive &ar, const Eigen::Matrix<
    _Scalar,
    _Rows,
    _Cols,
    _Options,
    _MaxRows,
    _MaxCols> &M, const unsigned int /* file_version */)
{
    typename Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>::Index rows = M.rows();
    typename Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>::Index cols = M.cols();
    ar << rows;
    ar << cols;
    ar << make_array(M.data(), M.size());
}
template<class Archive, typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols> inline void load(Archive &ar, Eigen::Matrix<
    _Scalar,
    _Rows,
    _Cols,
    _Options,
    _MaxRows,
    _MaxCols> &M, const unsigned int /* file_version */)
{
    typename Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>::Index rows, cols;
    ar >> rows;
    ar >> cols;
    //if (rows=!_Rows) throw std::exception(/*"Unexpected number of rows"*/);
    //if (cols=!_Cols) throw std::exception(/*"Unexpected number of cols"*/);
    ar >> make_array(M.data(), M.size());
}
template<class Archive, typename _Scalar, int _Cols, int _Options, int _MaxRows, int _MaxCols> inline void load(Archive &ar, Eigen::Matrix<
    _Scalar,
    Eigen::Dynamic,
    _Cols,
    _Options,
    _MaxRows,
    _MaxCols> &M, const unsigned int /* file_version */)
{
    typename Eigen::Matrix<_Scalar, Eigen::Dynamic, _Cols, _Options, _MaxRows, _MaxCols>::Index rows, cols;
    ar >> rows;
    ar >> cols;
    //if (cols=!_Cols) throw std::exception(/*"Unexpected number of cols"*/);
    M.resize(rows, Eigen::NoChange);
    ar >> make_array(M.data(), M.size());
}
template<class Archive, typename _Scalar, int _Rows, int _Options, int _MaxRows, int _MaxCols> inline void load(Archive &ar, Eigen::Matrix<
    _Scalar,
    _Rows,
    Eigen::Dynamic,
    _Options,
    _MaxRows,
    _MaxCols> &M, const unsigned int /* file_version */)
{
    typename Eigen::Matrix<_Scalar, _Rows, Eigen::Dynamic, _Options, _MaxRows, _MaxCols>::Index rows, cols;
    ar >> rows;
    ar >> cols;
    //if (rows=!_Rows) throw std::exception(/*"Unexpected number of rows"*/);
    M.resize(Eigen::NoChange, cols);
    ar >> make_array(M.data(), M.size());
}
template<class Archive, typename _Scalar, int _Options, int _MaxRows, int _MaxCols> inline void load(Archive &ar, Eigen::Matrix<
    _Scalar,
    Eigen::Dynamic,
    Eigen::Dynamic,
    _Options,
    _MaxRows,
    _MaxCols> &M, const unsigned int /* file_version */)
{
    typename Eigen::Matrix<_Scalar, Eigen::Dynamic, Eigen::Dynamic, _Options, _MaxRows, _MaxCols>::Index rows, cols;
    ar >> rows;
    ar >> cols;
    M.resize(rows, cols);
    ar >> make_array(M.data(), M.size());
}
template<class Archive, typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols> inline void serialize(Archive &ar, Eigen::Matrix<
    _Scalar,
    _Rows,
    _Cols,
    _Options,
    _MaxRows,
    _MaxCols> &M, const unsigned int file_version)
{
    split_free(ar, M, file_version);
}
template<class Archive, typename _Scalar, int _Dim, int _Mode, int _Options> inline void serialize(Archive &ar, Eigen::Transform<
    _Scalar,
    _Dim,
    _Mode,
    _Options> &t, const unsigned int version)
{
    serialize(ar, t.matrix(), version);
}
template<class Archive, typename _Scalar> void save(Archive &ar, const Eigen::Triplet<_Scalar> &m, const unsigned int /*version*/)
{
    ar << m.row();
    ar << m.col();
    ar << m.value();
}
template<class Archive, typename _Scalar> void load(Archive &ar, Eigen::Triplet<_Scalar> &m, const unsigned int /*version*/)
{
    typename Eigen::Triplet<_Scalar>::Index row, col;
    _Scalar                                 value;
    ar >> row;
    ar >> col;
    ar >> value;
    m = Eigen::Triplet<_Scalar>(row, col, value);
}
template<class Archive, class _Scalar> void serialize(Archive &ar, Eigen::Triplet<_Scalar> &m, const unsigned int version)
{
    split_free(ar, m, version);
}
template<class Archive, typename _Scalar, int _Options, typename _Index> void save(Archive &ar, const Eigen::SparseMatrix<
    _Scalar,
    _Options,
    _Index> &m, const unsigned int /*version*/)
{
    _Index                                   innerSize = m.innerSize();
    _Index                                   outerSize = m.outerSize();
    typedef typename Eigen::Triplet<_Scalar> Triplet;
    std::vector<Triplet>                     triplets;
    for (_Index                              i         = 0; i < outerSize; ++i)
        for (typename Eigen::SparseMatrix<_Scalar, _Options, _Index>::InnerIterator it(m, i); it; ++it)
            triplets.push_back(Triplet(it.row(), it.col(), it.value()));
    ar << innerSize;
    ar << outerSize;
    ar << triplets;
}
template<class Archive, typename _Scalar, int _Options, typename _Index> void load(Archive &ar, Eigen::SparseMatrix<
    _Scalar,
    _Options,
    _Index> &m, const unsigned int /*version*/)
{
    _Index innerSize;
    _Index outerSize;
    ar >> innerSize;
    ar >> outerSize;
    _Index rows = (m.IsRowMajor) ? outerSize : innerSize;
    _Index cols = (m.IsRowMajor) ? innerSize : outerSize;
    m.resize(rows, cols);
    typedef typename Eigen::Triplet<_Scalar> Triplet;
    std::vector<Triplet>                     triplets;
    ar >> triplets;
    m.setFromTriplets(triplets.begin(), triplets.end());
}
template<class Archive, typename _Scalar, int _Options, typename _Index> void serialize(Archive &ar, Eigen::SparseMatrix<
    _Scalar,
    _Options,
    _Index> &m, const unsigned int version)
{
    split_free(ar, m, version);
}
template<class Archive, typename _Scalar> void serialize(Archive &ar, Eigen::Quaternion<_Scalar> &q, const unsigned int /*version*/)
{
    ar & q.w();
    ar & q.x();
    ar & q.y();
    ar & q.z();
}
} // namespace serialization
}
#endif

/* ----------------------------------------------------- Vector extension */
/* ------------------------------------ Dot */
//template<typename Derived>
//typename Derived::Scalar Dot(Eigen::MatrixBase<Derived> const& v1, Eigen::MatrixBase<Derived> const& v2)
//{
//    return v1.dot(v2);
//}
/* ------------------------------------ Cross */
//template<typename Derived>
//typename Derived::template cross_product_return_type<Derived>::type Cross(Eigen::MatrixBase<Derived> const& v1, Eigen::MatrixBase<Derived> const& v2)
//{
//    return v1.cross(v2);
//}
/* ------------------------------------ Vector2 */
/* ------------------------------------ Vector3 */
/* ------------------------------------ Vector4 */
/* ----------------------------------------------------- Matrix extension */
#define IMPL_EIGEN_MATRIX_RESHAPE(TYPE, IN_SIZE, OUT_SIZE)\
        Eigen::Matrix<TYPE, OUT_SIZE, OUT_SIZE> ret(Eigen::Matrix<TYPE, OUT_SIZE, OUT_SIZE>::Identity());\
        if(OUT_SIZE < IN_SIZE)\
        {\
            const size_t SIZE = OUT_SIZE;\
            ret.block<SIZE, SIZE>(0,0,SIZE,SIZE) = m.block<SIZE, SIZE>(0, 0, SIZE, SIZE);\
        }\
        else /*OUT_SIZE > IN_SIZE*/\
        {\
            const size_t SIZE = IN_SIZE;\
            ret.block<SIZE, SIZE>(0,0,SIZE,SIZE) = m.block<SIZE, SIZE>(0, 0, SIZE, SIZE);\
        }\
        return ret;\

inline Eigen::Matrix<float, 2, 2> Reshape2(const Eigen::Matrix<float, 2, 2>& m) { IMPL_EIGEN_MATRIX_RESHAPE(float, 2, 2); };
inline Eigen::Matrix<float, 2, 2> Reshape2(const Eigen::Matrix<float, 3, 3>& m) { IMPL_EIGEN_MATRIX_RESHAPE(float, 3, 2); };
inline Eigen::Matrix<float, 2, 2> Reshape2(const Eigen::Matrix<float, 4, 4>& m) { IMPL_EIGEN_MATRIX_RESHAPE(float, 4, 2); };
inline Eigen::Matrix<float, 3, 3> Reshape3(const Eigen::Matrix<float, 2, 2>& m) { IMPL_EIGEN_MATRIX_RESHAPE(float, 2, 3); };
inline Eigen::Matrix<float, 3, 3> Reshape3(const Eigen::Matrix<float, 3, 3>& m) { IMPL_EIGEN_MATRIX_RESHAPE(float, 3, 3); };
inline Eigen::Matrix<float, 3, 3> Reshape3(const Eigen::Matrix<float, 4, 4>& m) { IMPL_EIGEN_MATRIX_RESHAPE(float, 4, 3); };
inline Eigen::Matrix<float, 4, 4> Reshape4(const Eigen::Matrix<float, 2, 2>& m) { IMPL_EIGEN_MATRIX_RESHAPE(float, 2, 4); };
inline Eigen::Matrix<float, 4, 4> Reshape4(const Eigen::Matrix<float, 3, 3>& m) { IMPL_EIGEN_MATRIX_RESHAPE(float, 3, 4); };
inline Eigen::Matrix<float, 4, 4> Reshape4(const Eigen::Matrix<float, 4, 4>& m) { IMPL_EIGEN_MATRIX_RESHAPE(float, 4, 4); };
inline Eigen::Matrix<double, 2, 2> Reshape2(const Eigen::Matrix<double, 2, 2>& m) { IMPL_EIGEN_MATRIX_RESHAPE(double, 2, 2); };
inline Eigen::Matrix<double, 2, 2> Reshape2(const Eigen::Matrix<double, 3, 3>& m) { IMPL_EIGEN_MATRIX_RESHAPE(double, 3, 2); };
inline Eigen::Matrix<double, 2, 2> Reshape2(const Eigen::Matrix<double, 4, 4>& m) { IMPL_EIGEN_MATRIX_RESHAPE(double, 4, 2); };
inline Eigen::Matrix<double, 3, 3> Reshape3(const Eigen::Matrix<double, 2, 2>& m) { IMPL_EIGEN_MATRIX_RESHAPE(double, 2, 3); };
inline Eigen::Matrix<double, 3, 3> Reshape3(const Eigen::Matrix<double, 3, 3>& m) { IMPL_EIGEN_MATRIX_RESHAPE(double, 3, 3); };
inline Eigen::Matrix<double, 3, 3> Reshape3(const Eigen::Matrix<double, 4, 4>& m) { IMPL_EIGEN_MATRIX_RESHAPE(double, 4, 3); };
inline Eigen::Matrix<double, 4, 4> Reshape4(const Eigen::Matrix<double, 2, 2>& m) { IMPL_EIGEN_MATRIX_RESHAPE(double, 2, 4); };
inline Eigen::Matrix<double, 4, 4> Reshape4(const Eigen::Matrix<double, 3, 3>& m) { IMPL_EIGEN_MATRIX_RESHAPE(double, 3, 4); };
inline Eigen::Matrix<double, 4, 4> Reshape4(const Eigen::Matrix<double, 4, 4>& m) { IMPL_EIGEN_MATRIX_RESHAPE(double, 4, 4); };
/* ------------------------------------ Matrix2x2 */
/* ------------------------------------ Matrix3x3 */
/* ------------------------------------ Matrix4x4 */
/* ----------------------------------------------------- Quaternion extension  */
template<typename Derived>
typename Derived::Scalar dot(const Eigen::QuaternionBase<Derived>& q1, const Eigen::QuaternionBase<Derived>& q2)
{
	return q1.dot(q2);
}
template<typename Derived>
bool operator==(const Eigen::QuaternionBase<Derived>& q1, const Eigen::QuaternionBase<Derived>& q2)
{
    return q1.w() == q2.w() && q1.x() == q2.x() && q1.y() == q2.y() && q1.z() == q2.z();
}
template<typename Derived>
bool operator!=(const Eigen::QuaternionBase<Derived>& q1, const Eigen::QuaternionBase<Derived>& q2)
{
    return q1.w() != q2.w() || q1.x() != q2.x() || q1.y() != q2.y() || q1.z() != q2.z();
}
template<typename Derived>
inline std::ostream& operator<<(std::ostream& os, const Eigen::QuaternionBase<Derived>& q)
{
    os << q.w() << " " << q.x() << " " << q.y() << " " << q.z();
    return os;
}
template <typename _Scalar>
inline std::ostream& operator<<(std::ostream& os, const Eigen::AngleAxis<_Scalar>& aa)
{
    //TODO::operator<< overloading for AngleAxis
    return os;
}




