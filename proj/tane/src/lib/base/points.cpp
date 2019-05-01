#include "base/points.h"
NAMESPACE_BASE_BEGIN
namespace internal
{
template<typename T>
inline void removeByMask(const int type, const Array1<bool>& maskArray, Array1<T>* pOutArray, bool parallel = false /*TODO::do parallel*/)
{
	/* type = 0 -> using mask as visible mask */
	/* type = 1 -> using mask as remove  mask */
	
	const size_t sz = pOutArray->size();
	Array1<T>    buffer;
	buffer.reserve(sz);
	
	for (size_t i = 0; i < sz; ++i)
	{
		if(type == 0)
		{
			if(maskArray[i])
			{
				buffer.push_back((*pOutArray)[i]);
			}
		}
		if(type == 1)
		{
			if(!maskArray[i])
			{
				buffer.push_back((*pOutArray)[i]);
			}
		}
		
	}
	pOutArray->swap(buffer);
}
template<typename T>
inline void removeByIndex(const Array1<size_t>& removeIDs, Array1<T>* pOutArray, bool parallel = false /*TODO::do parallel*/)
{
	Array1<bool> maskArray(pOutArray->size(), false);
	Array1<T>    buffer(pOutArray->size() - removeIDs.size());
	
	for (size_t i = 0; i < removeIDs.size(); ++i)
	{
		maskArray[removeIDs[i]] = true;
	}
	
	const size_t sz  = pOutArray->size();
	size_t       idx = 0;
	for (size_t  i   = 0; i < sz; ++i)
	{
		if(!maskArray[i])
			buffer[idx++] = (*pOutArray)[i];
	}
	pOutArray->swap(buffer);
}
template<typename T>
inline void removeByMask(const Points::MaskType type, const IntArray& mask, Array1<T>* pOutArray, bool gpuParallel = false /*TODO::do parallel*/)
{
	//Array1<T>::iterator endIter;
	//thrust::remove_if(thrust::system::omp::par,
	//                  pOutArray->begin(),
	//                  pOutArray->end(),
	//                  mask.begin()
	//);
	
	///* type = 0 -> using mask as visible mask */
	///* type = 1 -> using mask as remove  mask */
	//
	//const size_t sz = pOutArray->size();
	//Array1<T>    buffer;
	//buffer.reserve(sz);
	//
	//for (size_t i = 0; i < sz; ++i)
	//{
	//	if(type == 0)
	//	{
	//		if(maskArray[i])
	//		{
	//			buffer.push_back((*pOutArray)[i]);
	//		}
	//	}
	//	if(type == 1)
	//	{
	//		if(!maskArray[i])
	//		{
	//			buffer.push_back((*pOutArray)[i]);
	//		}
	//	}
	//
	//}
	//pOutArray->swap(buffer);
}
struct RemoveMaskPred
{
public:
private:;
	Points::MaskType type_;
public:
	RemoveMaskPred(const Points::MaskType& type) : type_(type)
	{
	}
	~RemoveMaskPred()
	{
	}
public:
	__host__ __device__ bool operator()(const bool &x) const
	{
		switch (type_)
		{
			case Points::kVISIBLE_MASK:
				return !x;
			case Points::kDELETE_MASK:
				return x;
		}
	}
};
/*------------------------------------------------------ Attributes Visitors */
/*--------------------------------- AppendVisitor */
struct AppendVisitor : public boost::static_visitor<>
{
private:
	Attributes::variant& var_;
public:
	AppendVisitor(Attributes::variant& var) : var_(var)
	{
	}
public:
	template<typename AttrT>
	void operator()(Array1<AttrT>& other) const
	{
		Array1<AttrT>& attr = boost::get<Array1<AttrT>>(var_);
		
		Array1<AttrT> buffer(attr.size() + other.size());
		
		typename Array1<AttrT>::iterator iter;
		/* copy this attributes data */
		iter = std::copy(attr.begin(), attr.end(), buffer.begin());
		
		/* append this attributes data */
		std::copy(other.begin(), other.end(), iter);
		
		/* swap */
		attr.swap(buffer);
	}
};
/*--------------------------------- AttributesSizeVisitor */
struct SizeVisitor : public boost::static_visitor<size_t>
{
public:
	template<typename AttrT>
	size_t operator()(const Array1<AttrT>& attr) const
	{
		return attr.size();
	}
};
/*--------------------------------- AttributesRemoveByIndexVisitor */
struct RemoveByIndexVisitor : public boost::static_visitor<>
{
private:
	const Array1<size_t>& removeIDs_;
public:
	RemoveByIndexVisitor(const Array1<size_t>& removeIDs) : removeIDs_(removeIDs)
	{
	}
public:
	template<typename AttrT> void operator()(Array1<AttrT>& attr) const
	{
		removeByIndex(removeIDs_, &attr);
	}
	
};
/*--------------------------------- AttributesRemoveByVisibleMaskVisitor */
struct RemoveByVisibleMaskVisitor : public boost::static_visitor<>
{
private:
	const Array1<bool>& maskArray_;
public:
	RemoveByVisibleMaskVisitor(const Array1<bool>& maskArray) : maskArray_(maskArray)
	{
	}
public:
	template<typename AttrT> void operator()(Array1<AttrT>& attr) const
	{
		removeByMask(0, maskArray_, &attr);
	}
};
/*--------------------------------- AttributesRemoveByDeleteMaskVisitor */
struct RemoveByDeleteMaskVisitor : public boost::static_visitor<>
{
private:
	const Array1<bool>& maskArray_;
public:
	RemoveByDeleteMaskVisitor(const Array1<bool>& mask) : maskArray_(mask)
	{
	}
public:
	template<typename AttrT> void operator()(Array1<AttrT>& attr) const
	{
		removeByMask(1, maskArray_, &attr);
	}
};
///*--------------------------------- AttributesResizeVisitor */
//struct ResizeVisitor : public boost::static_visitor<void>
//{
//private:
//	size_t resize_;
//public:
//	ResizeVisitor(const size_t resize) : resize_(resize)
//	{
//	}
//public:
//	template<typename AttrT>
//	void operator()(const Array1<AttrT>& attr) const
//	{
//		attr.resize(resize_);
//	}
//};
/*--------------------------------- AttributesOutputVisitor */
struct OutputVisitor : public boost::static_visitor<std::string>
{
private:
	size_t          idx_;
	std::streamsize precision_;
public:
	OutputVisitor(size_t idx) : idx_(idx), precision_(2)
	{
	}
public:
	template<typename AttrT> std::string operator()(Array1<AttrT>& attr) const
	{
		std::stringstream ss;
		ss << attr[idx_];
		return ss.str();
	}
	std::string operator()(const Array1<u_char>& attr) const
	{
		std::stringstream ss;
		ss << (attr[idx_] ? "true" : "false");
		return ss.str();
	}
	std::string operator()(const Array1<float>& attr) const
	{
		std::stringstream ss;
		ss.precision(precision_);
		ss.setf(std::ios::fixed);
		ss << attr[idx_];
		return ss.str();
	}
	std::string operator()(const Array1<Vec3f>& attr) const
	{
		std::stringstream ss;
		ss.precision(precision_);
		ss.setf(std::ios::fixed);
		ss << attr[idx_].transpose();
		return ss.str();
	}
	std::string operator()(const Array1<Vec4f>& attr) const
	{
		std::stringstream ss;
		ss.precision(precision_);
		ss.setf(std::ios::fixed);
		ss << attr[idx_].transpose();
		return ss.str();
	}
	std::string operator()(const Array1<double>& attr) const
	{
		std::stringstream ss;
		ss.precision(precision_);
		ss.setf(std::ios::fixed);
		ss << attr[idx_];
		return ss.str();
	}
	std::string operator()(const Array1<Vec3d>& attr) const
	{
		std::stringstream ss;
		ss.precision(precision_);
		ss.setf(std::ios::fixed);
		ss << attr[idx_].transpose();
		return ss.str();
	}
	std::string operator()(const Array1<Vec4d>& attr) const
	{
		std::stringstream ss;
		ss.precision(precision_);
		ss.setf(std::ios::fixed);
		ss << attr[idx_].transpose();
		return ss.str();
	}
	
	
};
}
/*------------------------------------------------------ Creator */
const std::shared_ptr<Points> Points::Create(const size_t npoints)
{
	return std::make_shared<Points>(Points(
			{
					npoints,
					/* Uniforms */
					{
							{sementic::uniform::gid, 0u},
					},
					/* Attributes */
					{
							{sementic::attribute::pid,        Int32Array     (npoints, 0                           )},
							{sementic::attribute::sid,        Int32Array     (npoints, 0                           )},
							{sementic::attribute::renderable, Int32Array     (npoints, true                        )},
							{sementic::attribute::eulerAngle, Vec3fArray     (npoints, Vec3f(0.0f, 0.0f, 0.0f)     )},
							{sementic::attribute::scale,      Vec3fArray     (npoints, Vec3f(1.0f, 1.0f, 1.0f)     )},
							{sementic::attribute::localAxis,  Mat3x3fArray   (npoints, Mat3x3f::Identity()         )},
							{sementic::attribute::uvCoord,    TexCoord3fArray(npoints, TexCoord3f(0.0f, 0.0f, 0.0f))},
							{sementic::attribute::radius,     FloatArray     (npoints, 0.0f                        )},
					}
			}
	));
}
const std::shared_ptr<Points> Points::Create(const size_t& sz,
                                             const std::initializer_list<Uniforms::value_type>& ulst,
                                             const std::initializer_list<Attributes::value_type>& alst)
{
	return std::make_shared<Points>(Points(sz, ulst, alst));
}
/*------------------------------------------------------ constructor */
Points::Points(const size_t& sz,
               const std::initializer_list<Uniforms::value_type>& ulst,
               const std::initializer_list<Attributes::value_type>& alst)
		: points_(sz, Point3f(0.0f, 0.0f, 0.0f)), npoints_(sz)
{
	set_(ulst);
	set_(alst);
}
Points::Points(const Point3fArray& array,
               const std::initializer_list<Uniforms::value_type>& ulst,
               const std::initializer_list<Attributes::value_type>& alst)
		: points_(array), npoints_(array.size())
{
	set_(ulst);
	set_(alst);
}
/*------------------------------------------------------ destructor */
Points::~Points()
{
}
/*------------------------------------------------------ operator overloading */
Points::reference Points::operator[](size_type idx)
{
	return points_[idx];
}
Points::const_reference Points::operator[](size_type idx) const
{
	return points_[idx];
}
/*------------------------------------------------------ public functions */
Points::size_type Points::size() const
{
	return npoints_;
}
Point3fArray& Points::get()
{
	return points_;
}
const Point3fArray& Points::get() const
{
	return points_;
}
Point3fArray::iterator Points::begin()
{
	return points_.begin();
}
Point3fArray::iterator Points::end()
{
	return points_.end();
}
Point3fArray::const_iterator Points::cbegin() const
{
	return points_.cbegin();
}
Point3fArray::const_iterator Points::cend() const
{
	return points_.cend();
}
Points::pointer Points::data()
{
	return points_.data();
}
Points::const_pointer Points::data() const
{
	return points_.data();
}
bool Points::exists(const Attributes::key_type& key) const
{
	return (attributes_.find(key) == attributes_.end()) ? false : true;
}
template<typename AttrT>
Array1<AttrT>& Points::get(const Attributes::key_type& key)
{
	return attributes_.get<AttrT>(key);
}
/* explicit template instantitation */
template Array1<int>&               Points::get(const Attributes::key_type& key);
template Array1<unsigned int>&      Points::get(const Attributes::key_type& key);
template Array1<long int>&          Points::get(const Attributes::key_type& key);
template Array1<unsigned long int>& Points::get(const Attributes::key_type& key);
template Array1<float>&             Points::get(const Attributes::key_type& key);
template Array1<double>&            Points::get(const Attributes::key_type& key);
template Array1<std::string>&       Points::get(const Attributes::key_type& key);
template Array1<Vec2i>&             Points::get(const Attributes::key_type& key);
template Array1<Vec3i>&             Points::get(const Attributes::key_type& key);
template Array1<Vec4i>&             Points::get(const Attributes::key_type& key);
template Array1<Vec2f>&             Points::get(const Attributes::key_type& key);
template Array1<Vec3f>&             Points::get(const Attributes::key_type& key);
template Array1<Vec4f>&             Points::get(const Attributes::key_type& key);
template Array1<Mat2x2f>&           Points::get(const Attributes::key_type& key);
template Array1<Mat3x3f>&           Points::get(const Attributes::key_type& key);
template Array1<Mat4x4f>&           Points::get(const Attributes::key_type& key);
template Array1<Quatf>&             Points::get(const Attributes::key_type& key);
template Array1<Vec2d>&             Points::get(const Attributes::key_type& key);
template Array1<Vec3d>&             Points::get(const Attributes::key_type& key);
template Array1<Vec4d>&             Points::get(const Attributes::key_type& key);
template Array1<Mat2x2d>&           Points::get(const Attributes::key_type& key);
template Array1<Mat3x3d>&           Points::get(const Attributes::key_type& key);
template Array1<Mat4x4d>&           Points::get(const Attributes::key_type& key);
template Array1<Quatd>&             Points::get(const Attributes::key_type& key);
template<typename AttrT>
const Array1<AttrT>& Points::get(const Attributes::key_type& key) const
{
	return attributes_.get<AttrT>(key);
}
///* explicit template instantitation */
template const Array1<int>&               Points::get(const Attributes::key_type& key) const;
template const Array1<unsigned int>&      Points::get(const Attributes::key_type& key) const;
template const Array1<long int>&          Points::get(const Attributes::key_type& key) const;
template const Array1<unsigned long int>& Points::get(const Attributes::key_type& key) const;
template const Array1<float>&             Points::get(const Attributes::key_type& key) const;
template const Array1<double>&            Points::get(const Attributes::key_type& key) const;
template const Array1<std::string>&       Points::get(const Attributes::key_type& key) const;
template const Array1<Vec2i>&             Points::get(const Attributes::key_type& key) const;
template const Array1<Vec3i>&             Points::get(const Attributes::key_type& key) const;
template const Array1<Vec4i>&             Points::get(const Attributes::key_type& key) const;
template const Array1<Vec2f>&             Points::get(const Attributes::key_type& key) const;
template const Array1<Vec3f>&             Points::get(const Attributes::key_type& key) const;
template const Array1<Vec4f>&             Points::get(const Attributes::key_type& key) const;
template const Array1<Mat2x2f>&           Points::get(const Attributes::key_type& key) const;
template const Array1<Mat3x3f>&           Points::get(const Attributes::key_type& key) const;
template const Array1<Mat4x4f>&           Points::get(const Attributes::key_type& key) const;
template const Array1<Quatf>&             Points::get(const Attributes::key_type& key) const;
template const Array1<Vec2d>&             Points::get(const Attributes::key_type& key) const;
template const Array1<Vec3d>&             Points::get(const Attributes::key_type& key) const;
template const Array1<Vec4d>&             Points::get(const Attributes::key_type& key) const;
template const Array1<Mat2x2d>&           Points::get(const Attributes::key_type& key) const;
template const Array1<Mat3x3d>&           Points::get(const Attributes::key_type& key) const;
template const Array1<Mat4x4d>&           Points::get(const Attributes::key_type& key) const;
template const Array1<Quatd>&             Points::get(const Attributes::key_type& key) const;
template<typename AttrT>
typename Array1<AttrT>::iterator Points::begin(const Attributes::key_type& key)
{
	return attributes_.get<AttrT>(key).begin();
}
/* explicit template instantitation */
template Array1<int>::iterator               Points::begin<int>              (const Attributes::key_type& key);
template Array1<unsigned int>::iterator      Points::begin<unsigned int>     (const Attributes::key_type& key);
template Array1<long int>::iterator          Points::begin<long int>         (const Attributes::key_type& key);
template Array1<unsigned long int>::iterator Points::begin<unsigned long int>(const Attributes::key_type& key);
template Array1<float>::iterator             Points::begin<float>            (const Attributes::key_type& key);
template Array1<double>::iterator            Points::begin<double>           (const Attributes::key_type& key);
template Array1<std::string>::iterator       Points::begin<std::string>      (const Attributes::key_type& key);
template Array1<Vec2i>::iterator             Points::begin<Vec2i>            (const Attributes::key_type& key);
template Array1<Vec3i>::iterator             Points::begin<Vec3i>            (const Attributes::key_type& key);
template Array1<Vec4i>::iterator             Points::begin<Vec4i>            (const Attributes::key_type& key);
template Array1<Vec2f>::iterator             Points::begin<Vec2f>            (const Attributes::key_type& key);
template Array1<Vec3f>::iterator             Points::begin<Vec3f>            (const Attributes::key_type& key);
template Array1<Vec4f>::iterator             Points::begin<Vec4f>            (const Attributes::key_type& key);
template Array1<Mat2x2f>::iterator           Points::begin<Mat2x2f>          (const Attributes::key_type& key);
template Array1<Mat3x3f>::iterator           Points::begin<Mat3x3f>          (const Attributes::key_type& key);
template Array1<Mat4x4f>::iterator           Points::begin<Mat4x4f>          (const Attributes::key_type& key);
template Array1<Quatf>::iterator             Points::begin<Quatf>            (const Attributes::key_type& key);
template Array1<Vec2d>::iterator             Points::begin<Vec2d>            (const Attributes::key_type& key);
template Array1<Vec3d>::iterator             Points::begin<Vec3d>            (const Attributes::key_type& key);
template Array1<Vec4d>::iterator             Points::begin<Vec4d>            (const Attributes::key_type& key);
template Array1<Mat2x2d>::iterator           Points::begin<Mat2x2d>          (const Attributes::key_type& key);
template Array1<Mat3x3d>::iterator           Points::begin<Mat3x3d>          (const Attributes::key_type& key);
template Array1<Mat4x4d>::iterator           Points::begin<Mat4x4d>          (const Attributes::key_type& key);
template Array1<Quatd>::iterator             Points::begin<Quatd>            (const Attributes::key_type& key);
template<typename AttrT>
typename Array1<AttrT>::iterator Points::end(const Attributes::key_type& key)
{
	return attributes_.get<AttrT>(key).end();
}
/* explicit template instantitation */
template Array1<int>::iterator               Points::end<int>              (const Attributes::key_type& key);
template Array1<unsigned int>::iterator      Points::end<unsigned int>     (const Attributes::key_type& key);
template Array1<long int>::iterator          Points::end<long int>         (const Attributes::key_type& key);
template Array1<unsigned long int>::iterator Points::end<unsigned long int>(const Attributes::key_type& key);
template Array1<float>::iterator             Points::end<float>            (const Attributes::key_type& key);
template Array1<double>::iterator            Points::end<double>           (const Attributes::key_type& key);
template Array1<std::string>::iterator       Points::end<std::string>      (const Attributes::key_type& key);
template Array1<Vec2i>::iterator             Points::end<Vec2i>            (const Attributes::key_type& key);
template Array1<Vec3i>::iterator             Points::end<Vec3i>            (const Attributes::key_type& key);
template Array1<Vec4i>::iterator             Points::end<Vec4i>            (const Attributes::key_type& key);
template Array1<Vec2f>::iterator             Points::end<Vec2f>            (const Attributes::key_type& key);
template Array1<Vec3f>::iterator             Points::end<Vec3f>            (const Attributes::key_type& key);
template Array1<Vec4f>::iterator             Points::end<Vec4f>            (const Attributes::key_type& key);
template Array1<Mat2x2f>::iterator           Points::end<Mat2x2f>          (const Attributes::key_type& key);
template Array1<Mat3x3f>::iterator           Points::end<Mat3x3f>          (const Attributes::key_type& key);
template Array1<Mat4x4f>::iterator           Points::end<Mat4x4f>          (const Attributes::key_type& key);
template Array1<Quatf>::iterator             Points::end<Quatf>            (const Attributes::key_type& key);
template Array1<Vec2d>::iterator             Points::end<Vec2d>            (const Attributes::key_type& key);
template Array1<Vec3d>::iterator             Points::end<Vec3d>            (const Attributes::key_type& key);
template Array1<Vec4d>::iterator             Points::end<Vec4d>            (const Attributes::key_type& key);
template Array1<Mat2x2d>::iterator           Points::end<Mat2x2d>          (const Attributes::key_type& key);
template Array1<Mat3x3d>::iterator           Points::end<Mat3x3d>          (const Attributes::key_type& key);
template Array1<Mat4x4d>::iterator           Points::end<Mat4x4d>          (const Attributes::key_type& key);
template Array1<Quatd>::iterator             Points::end<Quatd>            (const Attributes::key_type& key);
template<typename AttrT>
typename Array1<AttrT>::const_iterator Points::cbegin(const Attributes::key_type& key) const
{
	return attributes_.get<AttrT>(key).cbegin();
}
/* explicit template instantitation */
template Array1<int>::const_iterator               Points::cbegin<int>              (const Attributes::key_type& key) const ;
template Array1<unsigned int>::const_iterator      Points::cbegin<unsigned int>     (const Attributes::key_type& key) const ;
template Array1<long int>::const_iterator          Points::cbegin<long int>         (const Attributes::key_type& key) const ;
template Array1<unsigned long int>::const_iterator Points::cbegin<unsigned long int>(const Attributes::key_type& key) const ;
template Array1<float>::const_iterator             Points::cbegin<float>            (const Attributes::key_type& key) const ;
template Array1<double>::const_iterator            Points::cbegin<double>           (const Attributes::key_type& key) const ;
template Array1<std::string>::const_iterator       Points::cbegin<std::string>      (const Attributes::key_type& key) const ;
template Array1<Vec2i>::const_iterator             Points::cbegin<Vec2i>            (const Attributes::key_type& key) const ;
template Array1<Vec3i>::const_iterator             Points::cbegin<Vec3i>            (const Attributes::key_type& key) const ;
template Array1<Vec4i>::const_iterator             Points::cbegin<Vec4i>            (const Attributes::key_type& key) const ;
template Array1<Vec2f>::const_iterator             Points::cbegin<Vec2f>            (const Attributes::key_type& key) const ;
template Array1<Vec3f>::const_iterator             Points::cbegin<Vec3f>            (const Attributes::key_type& key) const ;
template Array1<Vec4f>::const_iterator             Points::cbegin<Vec4f>            (const Attributes::key_type& key) const ;
template Array1<Mat2x2f>::const_iterator           Points::cbegin<Mat2x2f>          (const Attributes::key_type& key) const ;
template Array1<Mat3x3f>::const_iterator           Points::cbegin<Mat3x3f>          (const Attributes::key_type& key) const ;
template Array1<Mat4x4f>::const_iterator           Points::cbegin<Mat4x4f>          (const Attributes::key_type& key) const ;
template Array1<Quatf>::const_iterator             Points::cbegin<Quatf>            (const Attributes::key_type& key) const ;
template Array1<Vec2d>::const_iterator             Points::cbegin<Vec2d>            (const Attributes::key_type& key) const ;
template Array1<Vec3d>::const_iterator             Points::cbegin<Vec3d>            (const Attributes::key_type& key) const ;
template Array1<Vec4d>::const_iterator             Points::cbegin<Vec4d>            (const Attributes::key_type& key) const ;
template Array1<Mat2x2d>::const_iterator           Points::cbegin<Mat2x2d>          (const Attributes::key_type& key) const ;
template Array1<Mat3x3d>::const_iterator           Points::cbegin<Mat3x3d>          (const Attributes::key_type& key) const ;
template Array1<Mat4x4d>::const_iterator           Points::cbegin<Mat4x4d>          (const Attributes::key_type& key) const ;
template Array1<Quatd>::const_iterator             Points::cbegin<Quatd>            (const Attributes::key_type& key) const ;
template<typename AttrT>
typename Array1<AttrT>::const_iterator Points::cend(const Attributes::key_type& key) const
{
	return attributes_.get<AttrT>(key).cend();
}
/* explicit template instantitation */
template Array1<int>::const_iterator               Points::cend<int>              (const Attributes::key_type& key) const;
template Array1<unsigned int>::const_iterator      Points::cend<unsigned int>     (const Attributes::key_type& key) const;
template Array1<long int>::const_iterator          Points::cend<long int>         (const Attributes::key_type& key) const;
template Array1<unsigned long int>::const_iterator Points::cend<unsigned long int>(const Attributes::key_type& key) const;
template Array1<float>::const_iterator             Points::cend<float>            (const Attributes::key_type& key) const;
template Array1<double>::const_iterator            Points::cend<double>           (const Attributes::key_type& key) const;
template Array1<std::string>::const_iterator       Points::cend<std::string>      (const Attributes::key_type& key) const;
template Array1<Vec2i>::const_iterator             Points::cend<Vec2i>            (const Attributes::key_type& key) const;
template Array1<Vec3i>::const_iterator             Points::cend<Vec3i>            (const Attributes::key_type& key) const;
template Array1<Vec4i>::const_iterator             Points::cend<Vec4i>            (const Attributes::key_type& key) const;
template Array1<Vec2f>::const_iterator             Points::cend<Vec2f>            (const Attributes::key_type& key) const;
template Array1<Vec3f>::const_iterator             Points::cend<Vec3f>            (const Attributes::key_type& key) const;
template Array1<Vec4f>::const_iterator             Points::cend<Vec4f>            (const Attributes::key_type& key) const;
template Array1<Mat2x2f>::const_iterator           Points::cend<Mat2x2f>          (const Attributes::key_type& key) const;
template Array1<Mat3x3f>::const_iterator           Points::cend<Mat3x3f>          (const Attributes::key_type& key) const;
template Array1<Mat4x4f>::const_iterator           Points::cend<Mat4x4f>          (const Attributes::key_type& key) const;
template Array1<Quatf>::const_iterator             Points::cend<Quatf>            (const Attributes::key_type& key) const;
template Array1<Vec2d>::const_iterator             Points::cend<Vec2d>            (const Attributes::key_type& key) const;
template Array1<Vec3d>::const_iterator             Points::cend<Vec3d>            (const Attributes::key_type& key) const;
template Array1<Vec4d>::const_iterator             Points::cend<Vec4d>            (const Attributes::key_type& key) const;
template Array1<Mat2x2d>::const_iterator           Points::cend<Mat2x2d>          (const Attributes::key_type& key) const;
template Array1<Mat3x3d>::const_iterator           Points::cend<Mat3x3d>          (const Attributes::key_type& key) const;
template Array1<Mat4x4d>::const_iterator           Points::cend<Mat4x4d>          (const Attributes::key_type& key) const;
template Array1<Quatd>::const_iterator             Points::cend<Quatd>            (const Attributes::key_type& key) const;
template<typename AttrT> typename Array1<AttrT>::pointer Points::data(const Attributes::key_type& key)
{
	return attributes_.get<AttrT>(key).data();
}
/* explicit template instantitation */
template Array1<int>::pointer               Points::data<int>              (const Attributes::key_type& key);
template Array1<unsigned int>::pointer      Points::data<unsigned int>     (const Attributes::key_type& key);
template Array1<long int>::pointer          Points::data<long int>         (const Attributes::key_type& key);
template Array1<unsigned long int>::pointer Points::data<unsigned long int>(const Attributes::key_type& key);
template Array1<float>::pointer             Points::data<float>            (const Attributes::key_type& key);
template Array1<double>::pointer            Points::data<double>           (const Attributes::key_type& key);
template Array1<std::string>::pointer       Points::data<std::string>      (const Attributes::key_type& key);
template Array1<Vec2i>::pointer             Points::data<Vec2i>            (const Attributes::key_type& key);
template Array1<Vec3i>::pointer             Points::data<Vec3i>            (const Attributes::key_type& key);
template Array1<Vec4i>::pointer             Points::data<Vec4i>            (const Attributes::key_type& key);
template Array1<Vec2f>::pointer             Points::data<Vec2f>            (const Attributes::key_type& key);
template Array1<Vec3f>::pointer             Points::data<Vec3f>            (const Attributes::key_type& key);
template Array1<Vec4f>::pointer             Points::data<Vec4f>            (const Attributes::key_type& key);
template Array1<Mat2x2f>::pointer           Points::data<Mat2x2f>          (const Attributes::key_type& key);
template Array1<Mat3x3f>::pointer           Points::data<Mat3x3f>          (const Attributes::key_type& key);
template Array1<Mat4x4f>::pointer           Points::data<Mat4x4f>          (const Attributes::key_type& key);
template Array1<Quatf>::pointer             Points::data<Quatf>            (const Attributes::key_type& key);
template Array1<Vec2d>::pointer             Points::data<Vec2d>            (const Attributes::key_type& key);
template Array1<Vec3d>::pointer             Points::data<Vec3d>            (const Attributes::key_type& key);
template Array1<Vec4d>::pointer             Points::data<Vec4d>            (const Attributes::key_type& key);
template Array1<Mat2x2d>::pointer           Points::data<Mat2x2d>          (const Attributes::key_type& key);
template Array1<Mat3x3d>::pointer           Points::data<Mat3x3d>          (const Attributes::key_type& key);
template Array1<Mat4x4d>::pointer           Points::data<Mat4x4d>          (const Attributes::key_type& key);
template Array1<Quatd>::pointer             Points::data<Quatd>            (const Attributes::key_type& key);
template<typename AttrT> typename Array1<AttrT>::const_pointer Points::data(const Attributes::key_type& key) const
{
	return attributes_.get<AttrT>(key).data();
}
/* explicit template instantitation */
template Array1<int>::const_pointer               Points::data<int>              (const Attributes::key_type& key) const;
template Array1<unsigned int>::const_pointer      Points::data<unsigned int>     (const Attributes::key_type& key) const;
template Array1<long int>::const_pointer          Points::data<long int>         (const Attributes::key_type& key) const;
template Array1<unsigned long int>::const_pointer Points::data<unsigned long int>(const Attributes::key_type& key) const;
template Array1<float>::const_pointer             Points::data<float>            (const Attributes::key_type& key) const;
template Array1<double>::const_pointer            Points::data<double>           (const Attributes::key_type& key) const;
template Array1<std::string>::const_pointer       Points::data<std::string>      (const Attributes::key_type& key) const;
template Array1<Vec2i>::const_pointer             Points::data<Vec2i>            (const Attributes::key_type& key) const;
template Array1<Vec3i>::const_pointer             Points::data<Vec3i>            (const Attributes::key_type& key) const;
template Array1<Vec4i>::const_pointer             Points::data<Vec4i>            (const Attributes::key_type& key) const;
template Array1<Vec2f>::const_pointer             Points::data<Vec2f>            (const Attributes::key_type& key) const;
template Array1<Vec3f>::const_pointer             Points::data<Vec3f>            (const Attributes::key_type& key) const;
template Array1<Vec4f>::const_pointer             Points::data<Vec4f>            (const Attributes::key_type& key) const;
template Array1<Mat2x2f>::const_pointer           Points::data<Mat2x2f>          (const Attributes::key_type& key) const;
template Array1<Mat3x3f>::const_pointer           Points::data<Mat3x3f>          (const Attributes::key_type& key) const;
template Array1<Mat4x4f>::const_pointer           Points::data<Mat4x4f>          (const Attributes::key_type& key) const;
template Array1<Quatf>::const_pointer             Points::data<Quatf>            (const Attributes::key_type& key) const;
template Array1<Vec2d>::const_pointer             Points::data<Vec2d>            (const Attributes::key_type& key) const;
template Array1<Vec3d>::const_pointer             Points::data<Vec3d>            (const Attributes::key_type& key) const;
template Array1<Vec4d>::const_pointer             Points::data<Vec4d>            (const Attributes::key_type& key) const;
template Array1<Mat2x2d>::const_pointer           Points::data<Mat2x2d>          (const Attributes::key_type& key) const;
template Array1<Mat3x3d>::const_pointer           Points::data<Mat3x3d>          (const Attributes::key_type& key) const;
template Array1<Mat4x4d>::const_pointer           Points::data<Mat4x4d>          (const Attributes::key_type& key) const;
template Array1<Quatd>::const_pointer             Points::data<Quatd>            (const Attributes::key_type& key) const;
void Points::append(Points& other)
{
	if(attributes_ != other.attributes_)
	{
		STDOUT_ERROR("attributes is not equal state");
		return;
	}
	
	/* points */
	{
		Point3fArray buffer(size() + other.size());
		Point3fArray::iterator iter;
		/* copy this position data */
		iter = std::copy(points_.begin(), points_.end(), buffer.begin());
		/* append ohter position data */
		std::copy(other.points_.begin(), other.points_.end(), iter);
		
		/* swap */
		points_.swap(buffer);
	}
	
	/* uniform (not appended)*/
	{
	}
	
	/* attribute */
	{
		for (Attributes::iterator aIter = attributes_.begin(); aIter != attributes_.end(); ++aIter)
		{
			boost::apply_visitor(internal::AppendVisitor(aIter->second), other.attributes_[aIter->first]);
		}
	}
	npoints_ = points_.size();
}
void Points::remove(const Array1<size_t>& deleteIDs)
{
	/* points (TOOD::parallel impl)*/
	{
		internal::removeByIndex(deleteIDs, &points_);
	}
	/* uniform (do not anything)*/
	{
	}
	/* attribute */
	{
		for (Attributes::iterator aIter = attributes_.begin(); aIter != attributes_.end(); ++aIter)
		{
			boost::apply_visitor(internal::RemoveByIndexVisitor(deleteIDs), aIter->second);
		}
	}
	/* refresh npoints */
	npoints_ = points_.size();
	
}
void Points::remove(const Array1<bool>& maskArray, const MaskType& maskType)
{
	if(size() != maskArray.size())
	{
		STDOUT_ERROR("Failed to remove()");
		exit(1);
	}
	/* points (TOOD::parallel impl)*/
	{
		if(maskType == kVISIBLE_MASK)
			internal::removeByMask(0, maskArray, &points_);
		if(maskType == kDELETE_MASK)
			internal::removeByMask(1, maskArray, &points_);
	}
	
	/* uniform (do not anything)*/
	{
	}
	
	/* attribute */
	{
		for (Attributes::iterator aIter = attributes_.begin(); aIter != attributes_.end(); ++aIter)
		{
			if(maskType == MaskType::kVISIBLE_MASK)
			{
				boost::apply_visitor(internal::RemoveByVisibleMaskVisitor(maskArray), aIter->second);
			}
			if(maskType == MaskType::kDELETE_MASK)
			{
				boost::apply_visitor(internal::RemoveByDeleteMaskVisitor(maskArray), aIter->second);
			}
		}
	}
	/* refresh npoints */
	npoints_ = points_.size();
}
void Points::clear()
{
	npoints_ = 0;
	points_.clear();
	attributes_.clear();
	uniforms_.clear();
}
void Points::swap(Points& other)
{
	std::swap<size_type>(npoints_, other.npoints_);
	points_.swap(other.points_);
	attributes_.swap(other.attributes_);
	uniforms_.swap(other.uniforms_);
	
}
/*------------------------------------------------------ private functions */
void Points::set_(const std::initializer_list<Uniforms::value_type>& ulst)
{
	std::initializer_list<Uniforms::value_type>::iterator uIter;
	for (uIter = ulst.begin(); uIter != ulst.end(); ++uIter)
	{
		if(!uniforms_.insert(Uniforms::value_type(uIter->first, uIter->second)).second)
		{
			STDOUT_ERROR("Failed to append uniform");
		}
	}
	
}
void Points::set_(const std::initializer_list<Attributes::value_type>& alst)
{
	std::initializer_list<Attributes::value_type>::iterator aIter;
	for (aIter = alst.begin(); aIter != alst.end(); ++aIter)
	{
		if(!attributes_.insert(Attributes::value_type(aIter->first, aIter->second)).second)
		{
			STDOUT_ERROR("Failed to append attribute");
		}
		
	}
	
}
void Points::valid_() const
{
	if(npoints_ != points_.size())
	{
		STDOUT_ERROR("npoints is not equal to points.size()");
	}
	for (Attributes::const_iterator aIter = attributes_.cbegin(); aIter != attributes_.cend(); ++aIter)
	{
		const size_t& sz = boost::apply_visitor(internal::SizeVisitor(), aIter->second);
		if(npoints_ != sz)
		{
			STDOUT_ERROR("npoints is not equal to attributes's size");
		}
	}
}
/*------------------------------------------------------ archive */
template<typename ARCHIVE>
void Points::serialize(ARCHIVE& ar, const unsigned int ver)
{
	if(ver == 0)
	{
		ar & npoints_;
		ar & points_;
		ar & uniforms_;
		ar & attributes_;
	}
}
NAMESPACE_BASE_END