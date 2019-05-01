#pragma once
#include "baseGl/pch.h"

#include "base/array.h"
#include "base/query.h"

#include "baseGl/query.h"

/* ------------------------------------------------- Vertex Array Object */
NAMESPACE_BASE_BEGIN
class GlVertexArrayObject final
{
private:
	GLuint id_;

public:
	GlVertexArrayObject() : id_(0xffffffff)
	{
	}
	~GlVertexArrayObject()
	{
	}
public:
	const GLuint id() const             { return id_;  }
public:
	void generate()
	{
		glGenVertexArrays(1, &id_);
		GLCHECKERRORNORET("Failed to generate vao");
	}
	void bind()
	{
		glBindVertexArray(id_);
		GLCHECKERRORNORET("Failed to bind vao");
	}
	void unbind()
	{
		glBindVertexArray(0);
		GLCHECKERRORNORET("Failed to unbind vao");
	}
	void clear()
	{
		glDeleteVertexArrays(1, &id_);
		GLCHECKERRORNORET("Failed to delete vao");
		id_ = 0xffffffff;
	}
};
NAMESPACE_BASE_END
/* ------------------------------------------------- GlBuffer Base */
NAMESPACE_BASE_BEGIN
template <class TYPE>
class GlBuffer
{
protected:
	const GLenum target_;
	const size_t stride_;
	
	GLuint  id_;
	GLuint  loc_;
	size_t  count_;
protected:
	GlBuffer(const GLenum target, const size_t stride)
		: target_(target),
		  stride_(stride),
		  id_(0xffffffff),
		  loc_(0xffffffff),
		  count_(0u)
	{
	}
	virtual ~GlBuffer()
	{
	}
public:
	const GLuint id() const             { return id_;                   }
	const GLuint location() const       { return loc_;                  }
	
	const size_t stride() const         { return stride_;               }
	const size_t count() const          { return count_;                }
	const size_t bytes() const          { return stride() * count();    }

protected:
	void generate_()
	{
		glGenBuffers(1, &id_);
		GLCHECKERRORNORET("Failed to generate glBuffer");
	}
	void bind_()
	{
		glBindBuffer(target_, id_);
		GLCHECKERRORNORET("Failed to bind glBuffer");
	}
	void bindBufferBase_(const GLuint location)
	{
		loc_ = location;
		glBindBufferBase(target_, loc_, id_);
		GLCHECKERRORNORET("Failed to bind glBuffer Base");
	}
	void bindBufferBase_(const GLenum target, const GLuint location)
	{
		loc_ = location;
		glBindBufferBase(target, loc_, id_);
		GLCHECKERRORNORET("Failed to bind glBuffer Base");
	}
	void bufferData_(const size_t cnt, const TYPE* pData, const GLenum usage)
	{
		count_ = cnt;
		if(pData)
		{
			glBufferData(target_, bytes(), pData, usage);
		}
		else
		{
			glBufferData(target_, bytes(), nullptr, usage);
		}
		GLCHECKERRORNORET("Failed to set glBuffer data");
	}
	void bufferSubData_(const int startIndex, const int count, const TYPE* pData)
	{
		//assert(stride() * count <= bytes());
		glBufferSubData(target_, stride() * startIndex, stride() * count, pData);
		GLCHECKERRORNORET("Failed to set glBuffer sub data");
	}
	TYPE* mapBuffer_(const GLenum& access)
	{
		return reinterpret_cast<TYPE*>(glMapBuffer(target_, access));
	}
	TYPE* mapBufferRange_(const int startIndex, const int count, GLbitfield access)
	{
		//assert(stride() * count <= bytes());
		return reinterpret_cast<TYPE*>(glMapBufferRange(target_, stride() * startIndex, stride() * count, access));
	}
	void unmapBuffer_()
	{
		glUnmapBuffer(target_);
		GLCHECKERRORNORET("Failed to unmap glBuffer data");
	}
	void unbind_()
	{
		glBindBuffer(target_, 0);
		GLCHECKERRORNORET("Failed to unbind glBuffer data");
	}
	void clear_()
	{
		glDeleteBuffers(1, &id_);
		GLCHECKERRORNORET("Failed to delete glBuffer");
		
		id_    = 0xffffffff;
		loc_   = 0xffffffff;
		count_ = 0u;
	}
	
};
NAMESPACE_BASE_END
/* ------------------------------------------------- GlArrayBuffer */
NAMESPACE_BASE_BEGIN
template <typename T>
class GlArrayBuffer : public GlBuffer<T>
{
public:
	typedef GlBuffer<T>                 Base;
	typedef Array1<T>                   ArrayT;
	typedef typename ArrayT::value_type value_type;
public:
	GlArrayBuffer() : Base(GL_ARRAY_BUFFER, sizeof(value_type))
	{
	}
	virtual ~GlArrayBuffer()
	{
	}
public:
	const size_t dim() const
	{
		return TypeDimension<value_type>();
	}
	const size_t componentSize() const
	{
		return Base::stride() / dim();
	}
	const GLenum glDataType() const
	{
		return GlDataType<value_type>();
	}
public:
	virtual void generate()
	{
		Base::generate_();
	}
	virtual void bind()
	{
		Base::bind_();
	}
	virtual void bindBufferBase(const GLuint location)
	{
		Base::bindBufferBase_(location);
	}
	virtual void bindBufferBase(const GLenum target, const GLuint location)
	{
		Base::bindBufferBase_(target, location);
	}

	virtual void enableVertexAttribIPointer(const GLint location, const GLvoid* offset = (void*)0)
	{
		glEnableVertexAttribArray(location);
		GLCHECKERRORNORET("Failed to enable vertex attrib array, loc = %u", location);
		
		glVertexAttribIPointer(location,
		                       dim(),
		                       glDataType(),
		                       componentSize() * dim(),
		                       offset
		);
		GLCHECKERRORNORET("Failed to vertex attrib IPointer, loc = %u", location);
		
	}
	virtual void enableVertexAttribIPointer(const GLint location, GLint divisor, const GLboolean normalized = GL_FALSE, const GLvoid* offset = (void*)0)
	{
		enableVertexAttribIPointer(location, offset);
		glVertexAttribDivisor(location, divisor);
		GLCHECKERRORNORET("Failed to vertex attrib divisor, loc = %u", location);
	}

	virtual void enableVertexAttribLPointer(const GLint location, const GLvoid* offset = (void*)0)
	{
		glEnableVertexAttribArray(location);
		GLCHECKERRORNORET("Failed to enable vertex attrib array, loc = %u", location);
		
		glVertexAttribLPointer(location,
		                       dim(),
		                       glDataType(),
		                       componentSize() * dim(),
		                       offset
		);
		GLCHECKERRORNORET("Failed to vertex attrib LPointer, loc = %u", location);
	}
	virtual void enableVertexAttribLPointer(const GLint location, GLint divisor, const GLboolean normalized = GL_FALSE, const GLvoid* offset = (void*)0)
	{
		enableVertexAttribLPointer(location, offset);
		glVertexAttribDivisor(location, divisor);
		GLCHECKERRORNORET("Failed to vertex attrib divisor, loc = %u", location);
	}

	virtual void enableVertexAttribPointer(const GLint location, const GLboolean normalized = GL_FALSE, const GLvoid* offset = (void*)0)
	{
		glEnableVertexAttribArray(location);
		GLCHECKERRORNORET("Failed to enable vertex attrib array, loc = %u", location);

		glVertexAttribPointer(location,
		                      dim(),
		                      glDataType(),
		                      normalized,
		                      componentSize() * dim(),
		                      offset
		);
		GLCHECKERRORNORET("Failed to vertex attrib Pointer, loc = %u", location);

	}
	virtual void enableVertexAttribPointer(const GLint location, GLint divisor, const GLboolean normalized = GL_FALSE, const GLvoid* offset = (void*)0)
	{
		enableVertexAttribPointer(location, normalized, offset);
		glVertexAttribDivisor(location, divisor);
		GLCHECKERRORNORET("Failed to vertex attrib divisor, loc = %u", location);
	}
	
	virtual void disableVertexAttribPointer(const GLint location)
	{
		glDisableVertexAttribArray(location);
		GLCHECKERRORNORET("Failed to disable vertex attrib array, loc = %u", location);
	}
	
	virtual void bufferData(const size_t cnt, const value_type* pData, const GLenum usage)
	{
		Base::bufferData_(cnt, pData, usage);
	}
	virtual void bufferSubData(const int startIndex, const int count, const value_type* pData)
	{
		Base::bufferSubData_(startIndex, count, pData);
	}
	virtual value_type* mapBuffer(const GLenum& access)
	{
		return Base::mapBuffer_(access);
	}
	virtual value_type* mapBufferRange(const int startIndex, const int count, GLbitfield access)
	{
		return Base::mapBufferRange_(startIndex, count, access);
	}
	virtual void* unmapBuffer()
	{
		Base::unmapBuffer_();
	}
	virtual void unbind()
	{
		Base::unbind_();
	}
	virtual void clear()
	{
		Base::clear_();
	}
};
typedef GlArrayBuffer<int>         GlIntArrayBuffer;

typedef GlArrayBuffer<float>       GlFloatArrayBuffer;       typedef GlArrayBuffer<double>      GlDoubleArrayBuffer;

typedef GlArrayBuffer<Vec2f>       GlVec2fArrayBuffer;        typedef GlArrayBuffer<Vec2d>       GlVec2dArrayBuffer;
typedef GlArrayBuffer<Vec3f>       GlVec3fArrayBuffer;        typedef GlArrayBuffer<Vec3d>       GlVec3dArrayBuffer;
typedef GlArrayBuffer<Vec4f>       GlVec4fArrayBuffer;        typedef GlArrayBuffer<Vec4d>       GlVec4dArrayBuffer;
typedef GlArrayBuffer<Point3f>     GlPoint3fArrayBuffer;      typedef GlArrayBuffer<Point3d>     GlPoint3dArrayBuffer;
typedef GlArrayBuffer<Normal3f>    GlNormal3fArrayBuffer;     typedef GlArrayBuffer<Normal3d>    GlNormal3dArrayBuffer;
typedef GlArrayBuffer<TexCoord1f>  GlTexCoord1fArrayBuffer;   typedef GlArrayBuffer<TexCoord1d>  GlTexCoord1dArrayBuffer;
typedef GlArrayBuffer<TexCoord2f>  GlTexCoord2fArrayBuffer;   typedef GlArrayBuffer<TexCoord2d>  GlTexCoord2dArrayBuffer;
typedef GlArrayBuffer<TexCoord3f>  GlTexCoord3fArrayBuffer;   typedef GlArrayBuffer<TexCoord3d>  GlTexCoord3dArrayBuffer;
typedef GlArrayBuffer<RGBColor>    GlRGBColorArrayBuffer;     typedef GlArrayBuffer<RGBAColor>   GlRGBAColorArrayBuffer;

NAMESPACE_BASE_END
/* ------------------------------------------------- Uniform Buffer Blocks Base */
NAMESPACE_BASE_BEGIN
template<class T>
class GlUniformBlock : public GlBuffer<T>
{
public:
	typedef GlBuffer<T> Base;
	typedef T           value_type;

public:
	GlUniformBlock() : Base(GL_UNIFORM_BUFFER, sizeof(value_type))
	{
	}
	virtual ~GlUniformBlock()
	{
	}

public:
	virtual void generate()
	{
		Base::generate_();
	}
	virtual void bind()
	{
		Base::bind_();
	}
	virtual void bindBufferBase(const GLuint location)
	{
		Base::bindBufferBase_(location);
	}
	virtual void bufferData(const value_type* pData, const GLenum usage)
	{
		Base::bufferData_(1, pData, usage);
	}
	virtual void bufferSubData(const value_type* pData)
	{
		Base::bufferSubData_(0, 1, pData);
	}
	virtual value_type* mapBuffer(const GLenum& access)
	{
		return Base::mapBuffer_(access);
	}
	virtual value_type* mapBufferRange(GLbitfield access)
	{
		return Base::mapBufferRange_(0, 1, access);
	}
	virtual void unmapBuffer()
	{
		Base::unmapBuffer_();
	}
	virtual void unbind()
	{
		Base::unbind_();
	}
	virtual void clear()
	{
		Base::clear_();
	}
};
template<class T>
class GlUniformArrayBlock : public GlBuffer<T>
{
public:
	typedef GlBuffer<T>                 Base;
	typedef Array1 <T>                  ArrayT;
	typedef typename ArrayT::value_type value_type;
public:
	GlUniformArrayBlock() : Base(GL_UNIFORM_BUFFER, sizeof(value_type))
	{
	}
	virtual ~GlUniformArrayBlock()
	{
	}
public:
	virtual void generate()
	{
		Base::generate_();
	}
	virtual void bind()
	{
		Base::bind_();
	}
	virtual void bindBufferBase(const GLuint location)
	{
		Base::bindBufferBase_(location);
	}
	virtual void bufferData(const size_t cnt, const value_type* pData, const GLenum usage)
	{
		Base::bufferData_(cnt, pData, usage);
	}
	virtual void bufferSubData(const int32_t startIndex, const int32_t count, const value_type* pData)
	{
		Base::bufferSubData_(startIndex, count, pData);
	}
	virtual value_type* mapBuffer(const GLenum& access)
	{
		return Base::mapBuffer_(access);
	}
	virtual value_type* mapBufferRange(const size_t startIndex, const size_t endIndex, GLbitfield access)
	{
		return Base::mapBufferRange_(startIndex, endIndex, access);
	}
	virtual void* unmapBuffer()
	{
		Base::unmapBuffer_();
	}
	virtual void unbind()
	{
		Base::unbind_();
	}
	virtual void clear()
	{
		Base::clear_();
	}
};
NAMESPACE_BASE_END
/* ------------------------------------------------- Transform Feedback Buffer */
NAMESPACE_BASE_BEGIN
class GlTransformFeedBackBuffer final
{
//private:
//	GLuint id_;
//
//public:
//	GlTransformFeedBackBuffer() : id_(0xffff)
//	{
//	}
//	~GlTransformFeedBackBuffer()
//	{
//	}
//public:
//	const GLuint id() const             { return id_;  }
//public:
//	void generate()
//	{
//		glGenTransformFeedbacks(1, &id_);
//		GLCHECKERRORNORET();
//	}
//	void bind()
//	{
//		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, id_);
//		GLCHECKERROR;
//	}
//	void unbind()
//	{
//		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
//		GLCHECKERROR;
//	}
//	void clear()
//	{
//		glDeleteTransformFeedbacks(1, &id_);
//		GLCHECKERROR;
//
//		id_ = 0xffff;
//	}
};
NAMESPACE_BASE_END
/* ------------------------------------------------- Texture Buffer */
NAMESPACE_BASE_BEGIN
template<typename T>
class GlTextureBuffer
{
public:
	typedef T                               value_type;
	typedef std::pair<GLenum, value_type*>  Buffer;
private:
	const GLenum    target_;
	const GLenum    type_;
	GLuint          id_;
	
protected:
	GlTextureBuffer(const GLenum bufferTarget) : target_(bufferTarget), type_(glDataType_())
	{
	}
	virtual ~GlTextureBuffer()
	{
	}
public:
	void generate()
	{
		glGenTextures(1, &id_);
		GLCHECKERRORNORET("Failed to generate texture");
	}
	void generateMipmap(const GLint maxLevel = 4)
	{
		glGenerateMipmap(target_);
		GLCHECKERRORNORET("Failed to glGenerateMipmap");
		glTexParameteri(target_, GL_TEXTURE_MAX_LEVEL, maxLevel);
		GLCHECKERRORNORET("Failed to set GL_TEXTURE_MAX_LEVEL");
		
	}
	void bind()
	{
		glBindTexture(target_, id_);
		GLCHECKERRORNORET("Failed to bind GL_TEXTURE_2D_ARRAY");
	}
	void texParameteri(const GLenum pname, const GLint param)
	{
		glTexParameteri(target_, pname, param);
		GLCHECKERRORNORET("Failed to set texture parameter");
	}
	void unbind()
	{
		glBindTexture(target_, 0);
		GLCHECKERRORNORET("Failed to unbind GL_TEXTURE_2D_ARRAY");
	}
	void clear()
	{
		glDeleteTextures(1, &id_);
		GLCHECKERRORNORET("Failed to clear texture");
		
		id_ = 0xffffffff;
	}
public:
	const GLenum target() const
	{
		return target_;
	}
	const GLenum type() const
	{
		return type_;
		
	}
	const GLuint id() const
	{
		return id_;
	}

private:
	const GLenum glDataType_()
	{
		const std::type_info& typeInfo = typeid(T);
		
		GLenum type;
		if(typeInfo == CharType() || typeInfo == Int8Type())
		{
			type = GL_BYTE;
		}
		else if(typeInfo == ShortType() || typeInfo == Int16Type())
		{
			type = GL_SHORT;
		}
		else if(typeInfo == IntType() || typeInfo == Int32Type())
		{
			type = GL_INT;
		}
		else if(typeInfo == UCharType() || typeInfo == UInt8Type())
		{
			type = GL_UNSIGNED_BYTE;
		}
		else if(typeInfo == UShortType() || typeInfo == UInt16Type())
		{
			type = GL_UNSIGNED_SHORT;
		}
		else if(typeInfo == UIntType() || typeInfo == UInt32Type())
		{
			type = GL_UNSIGNED_INT;
		}
		else if(typeInfo == FloatType())
		{
			type = GL_FLOAT;
		}
		else
		{
			STDOUT_ERROR("Failed to get supporting type")
			type = GL_INVALID_ENUM;
		}
		return type;
	}
	
};

//class GlTexture1DBuffer final : public GlTextureBuffer<T>
//class GlTexture2DBuffer final : public GlTextureBuffer<T>
//class GlTexture3DBuffer final : public GlTextureBuffer<T>
//class GlTexture1DArrayBuffer final : public GlTextureBuffer<T>
//class GlTexture3DArrayBuffer final : public GlTextureBuffer<T>

template<typename T, GLenum INTERNAL_FORMAT>
class GlTexture2DArrayBuffer final : public GlTextureBuffer<T>
{
public:
	typedef GlTextureBuffer<T>        Base;
	typedef typename Base::value_type value_type;
	typedef typename Base::Buffer     Buffer;
public:
	GlTexture2DArrayBuffer() : Base(GL_TEXTURE_2D_ARRAY)
	{
	}
	~GlTexture2DArrayBuffer()
	{
	}

public:
	void texImage(const GLint   level,
	              const GLsizei width,
	              const GLsizei height,
	              const GLsizei depth,
	              const Buffer  buffer = Buffer(GL_RGBA, nullptr)
	)
	{
		if(buffer.second)
		{
			glTexImage3D(Base::target(),   /* target */
			             level,            /* level  */
			             INTERNAL_FORMAT,  /* internal format */
			             width,            /* width */
			             height,           /* height */
			             depth,            /* depth */
			             0,                /* border */
			             buffer.first,     /* format */
			             Base::type(),     /* type */
			             nullptr           /* data */
			);
		}
		else
		{
			glTexImage3D(Base::target(),   /* target */
			             level,            /* level  */
			             INTERNAL_FORMAT,  /* internal format */
			             width,            /* width */
			             height,           /* height */
			             depth,            /* depth */
			             0,                /* border */
			             buffer.first,     /* format */
			             Base::type(),     /* type */
			             buffer.second     /* data */
			);
		}
		GLCHECKERRORNORET("Failed to set texImage3D");
	}
	void texSubImage(const GLint   level,
	                 const GLint   xoffset,
	                 const GLint   yoffset,
	                 const GLint   zoffset,
	                 const GLsizei width,
	                 const GLsizei height,
	                 const GLsizei depth,
	                 const Buffer  buffer
	)
	{
		glTexSubImage3D(Base::target(),       /* target */
		                level,                /* level  */
		                xoffset,              /* x offset*/
		                yoffset,              /* y offset */
		                zoffset,              /* z offset */
		                width,                /* width */
		                height,               /* height */
		                depth,                /* depth */
		                buffer.first,         /* format */
		                Base::type(),         /* type */
		                buffer.second         /* data */
		);
		GLCHECKERRORNORET("Failed to set texSubImage3D");
	}
};
typedef GlTexture2DArrayBuffer<unsigned char,  GL_RGBA>     GlRGBA8uiTexture2DArray;
typedef GlTexture2DArrayBuffer<unsigned short, GL_RGBA>     GlRGBA16uiTexture2DArray;
typedef GlTexture2DArrayBuffer<unsigned int,   GL_RGBA>     GlRGBA32uiTexture2DArray;

typedef GlTexture2DArrayBuffer<char,           GL_RGBA>     GlRGBA8iTexture2DArray;
typedef GlTexture2DArrayBuffer<short,          GL_RGBA>     GlRGBA16iTexture2DArray;
typedef GlTexture2DArrayBuffer<int,            GL_RGBA>     GlRGBA32iTexture2DArray;

typedef GlTexture2DArrayBuffer<float,          GL_RGBA16F>  GlRGBA16fTexture2DArray;
typedef GlTexture2DArrayBuffer<float,          GL_RGBA32F>  GlRGBA32fTexture2DArray;

typedef GlTexture2DArrayBuffer<unsigned char,  GL_RGB>     GlRGB8uiTexture2DArray;
typedef GlTexture2DArrayBuffer<unsigned short, GL_RGB>     GlRGB16uiTexture2DArray;
typedef GlTexture2DArrayBuffer<unsigned int,   GL_RGB>     GlRGB32uiTexture2DArray;

typedef GlTexture2DArrayBuffer<char,           GL_RGB>     GlRGB8iTexture2DArray;
typedef GlTexture2DArrayBuffer<short,          GL_RGB>     GlRGB16iTexture2DArray;
typedef GlTexture2DArrayBuffer<int,            GL_RGB>     GlRGB32iTexture2DArray;

typedef GlTexture2DArrayBuffer<float,          GL_RGB16F>  GlRGB16fTexture2DArray;
typedef GlTexture2DArrayBuffer<float,          GL_RGB32F>  GlRGB32fTexture2DArray;

NAMESPACE_BASE_END


NAMESPACE_BASE_BEGIN
/* ------------------------------------------------- GlMatArrayBuffer */
template <typename T, int N>
class GlMatArrayBuffer : public GlArrayBuffer<T>
{
public:
	typedef GlArrayBuffer<T>          Base;
	typedef typename Base::ArrayT     ArrayT;
	typedef typename Base::value_type value_type;
public:
	GlMatArrayBuffer() : Base()
	{
	}
	virtual ~GlMatArrayBuffer()
	{
	}
public:
	virtual void enableVertexAttribPointer(const GLint location, const GLboolean normalized = GL_FALSE, const GLvoid* offset = (void*)0)
	{
		for(int i = 0; i < N; ++i)
		{
			glEnableVertexAttribArray(location + i);
			GLCHECKERRORNORET("Failed to enable vertex attrib array, loc = %u", location + i);

			glVertexAttribPointer(location + i,
			                      N,
			                      Base::glDataType(),
			                      normalized,
			                      Base::componentSize() * N * N,
			                      (void*)(Base::componentSize() * i * N)
			);
			GLCHECKERRORNORET("Failed to vertex attrib Pointer, loc = %u", location + i);
		}
	}
	virtual void enableVertexAttribPointer(const GLint location, GLint divisor, const GLboolean normalized = GL_FALSE, const GLvoid* offset = (void*)0)
	{
		enableVertexAttribPointer(location, normalized, offset);
		for(int i = 0; i < N; ++i)
		{
			glVertexAttribDivisor(location + i, divisor);
			GLCHECKERRORNORET("Failed to vertex attrib divisor, loc = %u", location + i);
		}
	}
	virtual void disableVertexAttribPointer(const GLint location)
	{
		for(int i = 0; i < N; ++i)
		{
			glDisableVertexAttribArray(location + i);
			GLCHECKERRORNORET("Failed to disable vertex attrib array, loc = %u", location + i);
		}
	}
};

typedef GlMatArrayBuffer<Mat2x2f, 2> GlMat2x2fArrayBuffer; typedef GlMatArrayBuffer<Mat2x2d, 2> GlMat2x2dArrayBuffer;
typedef GlMatArrayBuffer<Mat3x3f, 3> GlMat3x3fArrayBuffer; typedef GlMatArrayBuffer<Mat3x3d, 3> GlMat3x3dArrayBuffer;
typedef GlMatArrayBuffer<Mat4x4f, 4> GlMat4x4fArrayBuffer; typedef GlMatArrayBuffer<Mat4x4d, 4> GlMat4x4dArrayBuffer;
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
/* ------------------------------------------------- GlIcrArrayBuffer */
struct IcrData
{
	Mat4x4f     xform;
	RGBAColor   color;
	RGBAColor   wireColor;
};
class GlIcrArrayBuffer : public GlBuffer<IcrData>
{
public:
	typedef GlBuffer<IcrData>           Base;
	typedef Array1<IcrData>             ArrayT;
	typedef typename ArrayT::value_type value_type;

public:
	GlIcrArrayBuffer() : Base(GL_ARRAY_BUFFER, sizeof(value_type))
	{
	}
	virtual ~GlIcrArrayBuffer()
	{
	}
public:
	virtual void generate()
	{
		Base::generate_();
	}
	virtual void bind()
	{
		Base::bind_();
	}
	virtual void bindBufferBase(const GLuint location)
	{
		Base::bindBufferBase_(location);
	}
	virtual void bindBufferBase(const GLenum target, const GLuint location)
	{
		Base::bindBufferBase_(target, location);
	}
	
	virtual void enableVertexAttribPointer(const GLint location, const GLboolean normalized = GL_FALSE, const GLvoid* offset = (void*)0)
	{
		static const int N = 4;
		static const int componentSize = sizeof(float);
		
		/* connect xform to shader*/
		GLuint baseLocation = location;
		{
			for(int i = 0; i < N; ++i)
			{
				glEnableVertexAttribArray(baseLocation + i);
				GLCHECKERRORNORET("Failed to enable vertex attrib array, loc = %u", baseLocation + i);
				
				glVertexAttribPointer(baseLocation + i,
				                      N,
				                      GL_FLOAT,
				                      normalized,
				                      stride(),
				                      (void*) (offsetof(IcrData, xform) + componentSize * i * N)
				);
				GLCHECKERRORNORET("Failed to vertex attrib Pointer, loc = %u", location + i);
			}
			baseLocation += N;
		}
		/* connect color to shader*/
		{
			glEnableVertexAttribArray(baseLocation);
			GLCHECKERRORNORET("Failed to enable vertex attrib array, loc = %u", baseLocation);
			glVertexAttribPointer(baseLocation,
			                      N,
			                      GL_FLOAT,
			                      normalized,
			                      stride(),
			                      (void*) (offsetof(IcrData, color))
			);
			GLCHECKERRORNORET("Failed to vertex attrib Pointer, loc = %u", baseLocation);
			baseLocation += 1;
		}
		/* connect wireColor to shader*/
		{
			glEnableVertexAttribArray(baseLocation);
			GLCHECKERRORNORET("Failed to enable vertex attrib array, loc = %u", baseLocation);
			glVertexAttribPointer(baseLocation,
			                      N,
			                      GL_FLOAT,
			                      normalized,
			                      stride(),
			                      (void*) (offsetof(IcrData, wireColor))
			);
			GLCHECKERRORNORET("Failed to vertex attrib Pointer, loc = %u", baseLocation);
			baseLocation += 1;
		}
	}
	virtual void enableVertexAttribPointer(const GLint location, GLint divisor, const GLboolean normalized = GL_FALSE, const GLvoid* offset = (void*)0)
	{
		static const int N = 4;
		static const int componentSize = sizeof(float);
		
		enableVertexAttribPointer(location, normalized, offset);
		
		/* connect xform to shader*/
		GLuint baseLocation = location;
		{
			for(int i = 0; i < N; ++i)
			{
				glVertexAttribDivisor(baseLocation + i, divisor);
				GLCHECKERRORNORET("Failed to vertex attrib divisor, loc = %u", baseLocation + i);
			}
			baseLocation += N;
		}
		/* connect color to shader*/
		{
			glVertexAttribDivisor(baseLocation, divisor);
			GLCHECKERRORNORET("Failed to vertex attrib divisor, loc = %u", baseLocation);
			baseLocation += 1;
		}
		/* connect wireColor to shader*/
		{
			glVertexAttribDivisor(baseLocation, divisor);
			GLCHECKERRORNORET("Failed to vertex attrib divisor, loc = %u", baseLocation);
			baseLocation += 1;
		}
	}
	virtual void disableVertexAttribPointer(const GLint location)
	{
		/* disconnect xform to shader*/
		GLuint baseLocation = location;
		{
			const int N = 4;
			for(int i = 0; i < N; ++i)
			{
				glDisableVertexAttribArray(baseLocation + i);
				GLCHECKERRORNORET("Failed to disable vertex attrib array, loc = %u", baseLocation + i);
			}
			baseLocation += N;
		}
		/* disconnect color to shader*/
		{
			glDisableVertexAttribArray(baseLocation);
			GLCHECKERRORNORET("Failed to disable vertex attrib array, loc = %u", baseLocation);
			
			baseLocation += 1;
		}
		/* disconnect wireColor to shader*/
		{
			glDisableVertexAttribArray(baseLocation);
			GLCHECKERRORNORET("Failed to disable vertex attrib array, loc = %u", baseLocation);
			baseLocation += 1;
		}
	}
	
	
	virtual void bufferData(const size_t cnt, const value_type* pData, const GLenum usage)
	{
		Base::bufferData_(cnt, pData, usage);
	}
	virtual void bufferSubData(const int startIndex, const int count, const value_type* pData)
	{
		Base::bufferSubData_(startIndex, count, pData);
	}
	virtual value_type* mapBuffer(const GLenum& access)
	{
		return Base::mapBuffer_(access);
	}
	virtual value_type* mapBufferRange(const int startIndex, const int count, GLbitfield access)
	{
		return Base::mapBufferRange_(startIndex, count, access);
	}
	
	virtual void* unmapBuffer()
	{
		Base::unmapBuffer_();
	}
	virtual void unbind()
	{
		Base::unbind_();
	}
	virtual void clear()
	{
		Base::clear_();
	}
private:

};
NAMESPACE_BASE_END


