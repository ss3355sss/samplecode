#pragma once
#include "base/pch.h"

#ifdef True
#undef True
#endif

#ifdef False
#undef False
#endif

#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>
#include <OpenImageIO/imagebufalgo_util.h>

#include "base/logger.h"

NAMESPACE_BASE_BEGIN
template<typename T>
class Texture
{
public:
	typedef T value_type;
	
public:
	std::shared_ptr<OIIO::ImageBuf> pBuffer_;
	
public:
	Texture()
	{
	}
	virtual ~Texture()
	{
	}
public:
	const T* operator()(const int x, const int y, const int z = 0) const
	{
		return reinterpret_cast<T*>(pBuffer_->pixeladdr(x, y, z));
	}
	T* operator()(const int x, const int y, const int z = 0)
	{
		return reinterpret_cast<T*>(pBuffer_->pixeladdr(x, y, z));
	}

public:
	const T* operator()(const float s, const float t, const float w = 0) const
	{
		return reinterpret_cast<T*>(pBuffer_->pixeladdr(rowIndex_(s), colIndex_(t), depthIndex_(w)));
	}
	T* operator()(const float s, const float t, const float w = 0)
	{
		return reinterpret_cast<T*>(pBuffer_->pixeladdr(rowIndex_(s), colIndex_(t), depthIndex_(w)));
	}
	
public:
	bool open(const std::string fileName)
	{
		DECL_LOGGER(base::Texture);
		
		if(pBuffer_)
		{
			pBuffer_->clear();
		}
		pBuffer_ = std::make_shared<OIIO::ImageBuf>(fileName);
		if(!pBuffer_->read(0, 0, true, type_()))
		{
			LOGGER_ERROR("Failed to open file, %s", fileName.c_str());
			return false;
		}
		return valid();
	}
public:
	bool resize(int w, int h, int nthreads = 0)
	{
		DECL_LOGGER(base::Texture);
		if(!valid())
			return false;
		OIIO::ROI roi(0, w, 0, h, 0, depth(), 0, nchannel());
		std::shared_ptr<OIIO::ImageBuf> pDst = std::make_shared<OIIO::ImageBuf>();
		if(!OIIO::ImageBufAlgo::resize(*pDst, *pBuffer_, "", 0.0f, roi, nthreads))
		{
			LOGGER_ERROR("Failed to resize texture");
			return false;
		}
		pBuffer_->clear();
		pBuffer_ = pDst;
		return true;
	}
	bool resize(int w, int h, int d, int nthreads = 0)
	{
		DECL_LOGGER(base::Texture);
		if(!valid())
			return false;
		OIIO::ROI roi(0, w, 0, h, 0, d, 0, nchannel());
		std::shared_ptr<OIIO::ImageBuf> pDst = std::make_shared<OIIO::ImageBuf>();
		if(!OIIO::ImageBufAlgo::resize(*pDst, *pBuffer_, "", 0.0f, roi, nthreads))
		{
			LOGGER_ERROR("Failed to resize texture");
			return false;
		}
		pBuffer_->clear();
		pBuffer_ = pDst;
		return true;
	}
	bool resize(int w, int h, int d, int nch, int nthreads = 0)
	{
		DECL_LOGGER(base::Texture);
		if(!valid())
			return false;
		OIIO::ROI roi(0, w, 0, h, 0, d, 0, nch);
		std::shared_ptr<OIIO::ImageBuf> pDst = std::make_shared<OIIO::ImageBuf>();
		if(!OIIO::ImageBufAlgo::resize(*pDst, *pBuffer_, "", 0.0f, roi, nthreads))
		{
			LOGGER_ERROR("Failed to resize texture");
			return false;
		}
		pBuffer_->clear();
		pBuffer_ = pDst;
		return true;
	}
public:
	const T* data() const
	{
		if(!valid())
			return nullptr;
		return reinterpret_cast<T*>(pBuffer_->localpixels());
	}
	T* data()
	{
		if(!valid())
			return nullptr;
		return reinterpret_cast<T*>(pBuffer_->localpixels());
	}
public:
	const int width() const
	{
		DECL_LOGGER(base::Texture);
		if(!valid())
		{
			LOGGER_ERROR("Failed to get valid buffer");
			return 0;
		}
		return pBuffer_->spec().width;
	}
	const int height() const
	{
		DECL_LOGGER(base::Texture);
		if(!valid())
		{
			LOGGER_ERROR("Failed to get valid buffer");
			return 0;
		}
		return pBuffer_->spec().height;
	}
	const int depth() const
	{
		DECL_LOGGER(base::Texture);
		if(!valid())
		{
			LOGGER_ERROR("Failed to get valid buffer");
			return 0;
		}
		return pBuffer_->spec().depth;
	}
	const int nchannel() const
	{
		DECL_LOGGER(base::Texture);
		if(!valid())
		{
			LOGGER_ERROR("Failed to get valid buffer");
			return 0;
		}
		return pBuffer_->spec().nchannels;
	}
	const std::string format() const
	{
		DECL_LOGGER(base::Texture);
		if(!valid())
		{
			LOGGER_ERROR("Failed to get valid buffer");
			return "";
		}
		return pBuffer_->spec().format.c_str();
	}
	const bool valid() const
	{
		DECL_LOGGER(base::Texture);
		if(!pBuffer_)
		{
			LOGGER_ERROR("Failed to get valid pBuffer");
			return false;
		}
		if(!pBuffer_->initialized())
		{
			LOGGER_ERROR("Failed to get valid pixels");
			return false;
		}
		if(!pBuffer_->pixels_valid())
		{
			LOGGER_ERROR("Failed to get valid pixels");
			return false;
		}
		if(!pBuffer_->localpixels())
		{
			LOGGER_ERROR("Failed to get valid pixels");
			return false;
		}
		return true;
	}
public:
	void clear()
	{
		if(pBuffer_)
		{
			pBuffer_->clear();
		}
		pBuffer_.reset();
		pBuffer_ = nullptr;
	}
private:
	bool resize_(int xbegin, int xend,
	             int ybegin, int yend,
	             int zbegin, int zend,
	             int chbegin, int chend, int nthreads)
	{
		DECL_LOGGER(base::Texture);
		
		OIIO::ROI roi(xbegin, xend, ybegin, yend, zbegin, zend, chbegin, chend);
		
		std::shared_ptr<OIIO::ImageBuf> pDst = std::make_shared<OIIO::ImageBuf>();
		if(!OIIO::ImageBufAlgo::resize(*pDst, *pBuffer_, "", 0.0f, roi, nthreads))
		{
			LOGGER_ERROR("Failed to resize texture");
			return false;
		}
		pBuffer_->clear();
		pBuffer_ = pDst;
		return true;
	}
	const OIIO::TypeDesc type_()
	{
		DECL_LOGGER(base::Texture);
		const std::type_info& typeInfo = typeid(T);
		
		if(typeInfo == UCharType())
		{
			return OIIO::TypeDesc::UCHAR;
		}
		else if(typeInfo == UShortType())
		{
			return OIIO::TypeDesc::USHORT;
		}
		else if(typeInfo == UIntType())
		{
			return OIIO::TypeDesc::UINT;
		}
		else if(typeInfo == CharType())
		{
			return OIIO::TypeDesc::CHAR;
		}
		else if(typeInfo == ShortType())
		{
			return OIIO::TypeDesc::SHORT;
		}
		else if(typeInfo == IntType())
		{
			return OIIO::TypeDesc::INT;
		}
		else if(typeInfo == FloatType())
		{
			return OIIO::TypeDesc::FLOAT;
		}
		else if(typeInfo == DoubleType())
		{
			return OIIO::TypeDesc::DOUBLE;
		}
		else
		{
			LOGGER_ERROR("Failed to get valid type");
			return OIIO::TypeDesc::NONE;
		}
	}
	const int rowIndex_(const float s) const
	{
		float x = s * static_cast<float>(width());
		x = boost::algorithm::clamp(x, 0.0f, static_cast<float>(width() - 1.0));
		return x;
	}
	const int colIndex_(const float t) const
	{
		float y = (1.0f - t) * static_cast<float>(height());
		y = boost::algorithm::clamp(y, 0.0f, static_cast<float>(height() - 1.0));
		return y;
	}
	const int depthIndex_(const float w) const
	{
		float z = w * static_cast<float>(depth());
		z = boost::algorithm::clamp(z, 0.0f, static_cast<float>(depth() - 1.0));
		return z;
	}
	
};
typedef Texture<char>           Textureu;
typedef Texture<short>          Textures;
typedef Texture<int>            Texturei;
typedef Texture<unsigned char>  Textureuc;
typedef Texture<unsigned short> Textureus;
typedef Texture<unsigned int>   Textureui;
typedef Texture<float>          Texturef;
typedef Texture<double>         Textured;

//typedef Texture<char>           Texture2u;
//typedef Texture<short>          Texture2s;
//typedef Texture<int>            Texture2i;
//typedef Texture<unsigned char>  Texture2uc;
//typedef Texture<unsigned short> Texture2us;
//typedef Texture<unsigned int>   Texture2ui;
//typedef Texture<float>          Texture2f;
//typedef Texture<double>         Texture2d;
//typedef Texture<char>           Texture3u;
//typedef Texture<short>          Texture3s;
//typedef Texture<int>            Texture3i;
//typedef Texture<unsigned char>  Texture3uc;
//typedef Texture<unsigned short> Texture3us;
//typedef Texture<unsigned int>   Texture3ui;
//typedef Texture<float>          Texture3f;
//typedef Texture<double>         Texture3d;


NAMESPACE_BASE_END



















//NAMESPACE_BASE_BEGIN
//
//template<typename T>
//class TextureBase
//{
//public:
//	static bool Open()
//	{
//	}
//
//protected:
//	std::shared_ptr<OIIO::ImageBuf> pBuffer_;
//
//protected:
//	TextureBase()
//	{
//	}
//	virtual ~TextureBase()
//	{
//	}
//
//public:
//	bool open(const std::string& fileName)
//	{
//		pBuffer_->spec()
//
//		if(pBuffer_)
//			pBuffer_->clear();
//
//		pBuffer_ = std::make_shared<OIIO::ImageBuf>(fileName);
//
//		if(!pBuffer_->read(0, 0, true, type_()))
//		{
//			STDOUT_ERROR("Failed to open file, %s", fileName.c_str());
//			return false;
//		}
//
//		return true;
//	}
//public:
//	//const T* data() const
//	//{
//	//
//	//}
//	//T* data()
//	//{
//	//}
//
//private:
//	const OIIO::TypeDesc type_()
//	{
//		const std::type_info& typeInfo = typeid(T);
//
//		if(typeInfo == UCharType())
//		{
//			return OIIO::TypeDesc::UCHAR;
//		}
//		else if(typeInfo == UShortType())
//		{
//			return OIIO::TypeDesc::USHORT;
//		}
//		else if(typeInfo == UIntType())
//		{
//			return OIIO::TypeDesc::UINT;
//		}
//		else if(typeInfo == CharType())
//		{
//			return OIIO::TypeDesc::CHAR;
//		}
//		else if(typeInfo == ShortType())
//		{
//			return OIIO::TypeDesc::SHORT;
//		}
//		else if(typeInfo == IntType())
//		{
//			return OIIO::TypeDesc::INT;
//		}
//		else if(typeInfo == FloatType())
//		{
//			return OIIO::TypeDesc::FLOAT;
//		}
//		else if(typeInfo == DoubleType())
//		{
//			return OIIO::TypeDesc::DOUBLE;
//		}
//		else
//		{
//			STDOUT_ERROR("Failed to get valid type");
//			return OIIO::TypeDesc::NONE;
//		}
//	}
//};
//
//
//template<typename T>
//class Texture2 : public TextureBase<T>
//{
//public:
//	typedef TextureBase<T> Base;
//
//private:
//
//public:
//	Texture2() : Base()
//	{
//	}
//	virtual ~Texture2()
//	{
//	}
//public:
//	const T* operator()(const int x, const int y) const
//	{
//		return reinterpret_cast<T*>(Base::pBuffer_->pixeladdr(x, y));
//	}
//	T* operator()(const int x, const int y)
//	{
//		return reinterpret_cast<T*>(Base::pBuffer_->pixeladdr(x, y));
//	}
//};
//
//typedef Texture2<char>           Texture2u;
//typedef Texture2<short>          Texture2s;
//typedef Texture2<int>            Texture2i;
//
//typedef Texture2<unsigned char>  Texture2uc;
//typedef Texture2<unsigned short> Texture2us;
//typedef Texture2<unsigned int>   Texture2ui;
//
//typedef Texture2<float>          Texture2f;
//typedef Texture2<double>         Texture2d;
//
//NAMESPACE_BASE_END
