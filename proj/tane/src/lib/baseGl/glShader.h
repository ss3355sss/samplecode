#pragma once
#include "baseGl/pch.h"

#include <boost/any.hpp>

#include "base/bbox.h"
#include "baseGl/glutil.h"
NAMESPACE_BASE_BEGIN

///------------------------------------------------------------- GlShader
class GlShader
{
protected:
	GLuint program_;

protected:
	static const std::string GetRootDirectory()
	{
		std::string path = std::string(PUBLISH_ROOT);
		path += "/resource/baseGl/shader/";
	
		//cout << "GlShader::path = " << path << endl;
		return path;
	}
	template <typename T>
	static const std::shared_ptr<T> Create(const std::string rootDirectory)
	{
		std::shared_ptr<T> pShader = std::make_shared<T>();
		if(!pShader->initialize(rootDirectory))
			return nullptr;
		return pShader;
	}
public:
	///------------------------------------------------------------ constructor / destructor
	GlShader() : program_(0xffffffff)
	{
	}
	virtual ~GlShader()
	{
	}
public:
	virtual bool initialize(const std::string rootDirectory)            = 0;
	virtual bool setUniform(const int param, const boost::any& uniform) = 0;

public:
	const GLuint program() const
	{
		return program_;
	}
	const GLuint uniformLocation(const std::string& unif) const
	{
		if(!program_)
		{
			STDOUT_ERROR("Failed to get valid program");
			return 0;
		}
		return glGetUniformLocation(program_, unif.c_str());
	}
	const GLuint uniformBlockLocation(const std::string& unif) const
	{
		if(!program_)
		{
			STDOUT_ERROR("Failed to get valid program");
			return 0;
		}
		return glGetUniformBlockIndex(program_, unif.c_str());
	}
	const GLuint attributeLocation(const std::string& attr) const
	{
		if(!program_)
		{
			STDOUT_ERROR("Failed to get valid program");
			return 0;
		}
		return glGetAttribLocation(program_, attr.c_str());
	}

public:
	///------------------------------------------------------------ public helper functions
	void use()
	{
		glUseProgram(program_);
		GLCHECKERRORNORET("Failed to use shader, program = %u", program_);
	}
	void unuse()
	{
		glUseProgram(0);
		GLCHECKERRORNORET("Failed to unuse shader, program = %u", program_);
	}
	void clear()
	{
		if(program_)
		{
			glDeleteProgram(program_);
			GLCHECKERRORNORET("Failed to delete shader program, %u", program_);
		}
		program_ = 0xffffffff;
	}
protected:
	///------------------------------------------------------------ private helper functions
	GLuint loadFromString(GLenum type, const std::string &source)
	{
		GLuint shader = glCreateShader(type);
		
		const char *ptmp = source.c_str();
		glShaderSource(shader, 1, &ptmp, NULL);
		
		//check whether the shader loads fine
		GLint status;
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
			GLchar *infoLog = new GLchar[infoLogLength];
			glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
			STDOUT_ERROR("Failed to compile shader, %s", infoLog);
			delete[] infoLog;
			
			return 0xffffffff;
		}
		return shader;
	}
	GLuint loadFromFile(GLenum whichShader, const std::string &filename)
	{
		GLuint shader;
		std::ifstream fp;
		fp.open(filename.c_str(), std::ios_base::in);
		if(!fp)
		{
			STDOUT_ERROR("Failed to open shader file, %s", filename.c_str());
			return 0xffffffff;
		}
		
		std::string line, buffer;
		while (getline(fp, line))
		{
			buffer.append(line);
			buffer.append("\r\n");
		}
		shader = loadFromString(whichShader, buffer);
		return shader;
	}

protected:
	bool checkLinkStatus_()
	{
		GLint status;
		glGetProgramiv(program_, GL_LINK_STATUS, &status);
		GLCHECKERROR("Failed to get shader program link status");
		
		if (status == GL_FALSE)
		{
			GLint infoLogLength;
			
			glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &infoLogLength);
			GLchar *infoLog = new GLchar[infoLogLength];
			glGetProgramInfoLog(program_, infoLogLength, NULL, infoLog);
			STDOUT_ERROR("Failed to link glShader, %s", infoLog);
			delete[] infoLog;
			return false;
		}
		return true;
	}
};

///------------------------------------------------------------- GlIcrShader
class GlIcrShader : public GlShader
{
public:
	typedef GlShader Base;
public:
	enum Attribute
	{
		InRenderable     = 0,
		InInstancingMtx  = 1,
		InColor          = 5,
		InWireColor      = 6,
	};
	
	enum Uniform
	{
		kXformMtx,
		
		kEnableIcr,
		kMinCorner,
		kMaxCorner,
		
		kEnableLOD,
		kMinDistance,
		kMaxDistance,
	};
	
public:
	static std::shared_ptr<GlIcrShader>& Get(const std::string rootDirectory = GlShader::GetRootDirectory())
	{
		static std::shared_ptr<GlIcrShader> pShader = nullptr;
		if(!pShader)
			pShader = GlShader::Create<GlIcrShader>(rootDirectory);
		return pShader;
	}
public:
	GlIcrShader() : Base()
	{
	}
	virtual ~GlIcrShader()
	{
	}
public:
	virtual bool initialize(const std::string rootDirectory)
	{
		/* create shader program  */
		program_ = glCreateProgram();
		
		/* attach shader to program  */
		GLuint vs = loadFromFile(GL_VERTEX_SHADER,   rootDirectory + "icr.vs");
		GLuint gs = loadFromFile(GL_GEOMETRY_SHADER, rootDirectory + "icr.gs");
		glAttachShader(program_, vs);
		GLCHECKERROR("Failed to attach icr.vs to program");
		glAttachShader(program_, gs);
		GLCHECKERROR("Failed to attach icr.gs to program");
		
		/* binding attribute */
		glBindAttribLocation(program_, InRenderable,    ENUM_TO_STRING(InRenderable).c_str());
		GLCHECKERROR("Failed to bind InRenderable to icr.vs");
		glBindAttribLocation(program_, InInstancingMtx, ENUM_TO_STRING(InInstancingMtx).c_str());
		GLCHECKERROR("Failed to bind InInstancingMtx to icr.vs");
		glBindAttribLocation(program_, InColor,         ENUM_TO_STRING(InColor).c_str());
		GLCHECKERROR("Failed to bind InColor to icr.vs");
		glBindAttribLocation(program_, InWireColor,     ENUM_TO_STRING(InWireColor).c_str());
		GLCHECKERROR("Failed to bind InWireColor to icr.vs");
		
		const char* varyings[] = {
			"OutInstancingMtxLOD00",
			"OutInstancingMtxLOD01",
			"OutInstancingMtxLOD02",
			"OutInstancingMtxLOD03", /// mat4
			"OutColorLOD0",          /// vec4
			"OutWireColorLOD0",      /// vec4
			"gl_NextBuffer",
		
			"OutInstancingMtxLOD10",
			"OutInstancingMtxLOD11",
			"OutInstancingMtxLOD12",
			"OutInstancingMtxLOD13", /// mat4
			"OutColorLOD1",          /// vec4
			"OutWireColorLOD1",      /// vec4
			"gl_NextBuffer",
		
			"OutInstancingMtxLOD20",
			"OutInstancingMtxLOD21",
			"OutInstancingMtxLOD22",
			"OutInstancingMtxLOD23", /// mat4
			"OutColorLOD2",          /// vec4
			"OutWireColorLOD2",      /// vec4
		};
		
		
		glTransformFeedbackVaryings(program_, sizeof(varyings) / sizeof(varyings[0]), varyings, GL_INTERLEAVED_ATTRIBS);
		
		/* link shader program */
		glLinkProgram(program_);
		CHECKERROR(checkLinkStatus_(), "Failed to link icr shader");
		
		glDeleteShader(vs);
		GLCHECKERROR("Failed to delete icr.vs");
		
		glDeleteShader(gs);
		GLCHECKERROR("Failed to delete icr.gs");
		
		return true;
	}
	virtual bool setUniform(const int param, const boost::any& uniform)
	{
		try
		{
			switch (param)
			{
				case kXformMtx:
				{
					const Mat4x4f value = boost::any_cast<Mat4x4f>(uniform);
					glUniformMatrix4fv(uniformLocation(ENUM_TO_STRING(kXformMtx)), 1, GL_FALSE, value.data());
				}
					break;
				case kEnableIcr:
				{
					const int value = static_cast<int>(boost::any_cast<bool>(uniform));
					glUniform1i(uniformLocation(ENUM_TO_STRING(kEnableIcr)), value);
				}
					break;
				case kMinCorner:
				{
					const Point3f value = boost::any_cast<Point3f>(uniform);
					glUniform3f(uniformLocation(ENUM_TO_STRING(kMinCorner)), value[0], value[1], value[2]);
				}
					break;
				case kMaxCorner:
				{
					const Point3f value = boost::any_cast<Point3f>(uniform);
					glUniform3f(uniformLocation(ENUM_TO_STRING(kMaxCorner)), value[0], value[1], value[2]);
				}
					break;
				case kEnableLOD:
				{
					const int value = static_cast<int>(boost::any_cast<bool>(uniform));
					glUniform1i(uniformLocation(ENUM_TO_STRING(kEnableLOD)), value);
				}
					break;
				case kMinDistance:
				{
					const float value = static_cast<float>(boost::any_cast<double>(uniform));
					glUniform1f(uniformLocation(ENUM_TO_STRING(kMinDistance)), value);
				}
					break;
				case kMaxDistance:
				{
					const float value = static_cast<float>(boost::any_cast<double>(uniform));
					glUniform1f(uniformLocation(ENUM_TO_STRING(kMaxDistance)), value);
				}
					break;
				
			}
		}
		catch(const boost::bad_any_cast&)
		{
			STDOUT_ERROR("GlIcrShader::Failed to set uniform, %d", param);
			return false;
		}
		return true;
	}
	
};

///------------------------------------------------------------- GlIcrPhongShader
class GlIcrPhongShader : public GlShader
{
public:
	typedef GlShader Base;
public:
	enum Attribute
	{
		InPoint         = 0,
		InNormal        = 1,
		InTexCoord      = 2,
		
		InInstancingMtx = 3,
		InColor         = 7,
		InWireColor     = 8,
		
	};
	
	enum Uniform
	{
		kXformMtx,
		
		kDrawMode,
		kOverrideMask,
		kMaskColor,
		
		kOverrideTexture,
		
		kNoLighting,
	};

public:
	static std::shared_ptr<GlIcrPhongShader>& Get(const std::string rootDirectory = GlShader::GetRootDirectory())
	{
		static std::shared_ptr<GlIcrPhongShader> pShader = nullptr;
		if(!pShader)
			pShader = GlShader::Create<GlIcrPhongShader>(rootDirectory);
		return pShader;
	}
	
public:
	GlIcrPhongShader() : Base()
	{
	}
	virtual ~GlIcrPhongShader ()
	{
	}
public:
	virtual bool initialize(const std::string rootDirectory)
	{
		/* create shader program  */
		program_ = glCreateProgram();
		
		/* attach shader to program  */
		GLuint vs = loadFromFile(GL_VERTEX_SHADER, rootDirectory   + "icrPhong.vs");
		GLuint fs = loadFromFile(GL_FRAGMENT_SHADER, rootDirectory + "icrPhong.fs");
		
		glAttachShader(program_, vs);
		GLCHECKERROR("Failed to attach icrPhong.vs to program");
		glAttachShader(program_, fs);
		GLCHECKERROR("Failed to attach icrPhong.fs to program");
		
		/* binding attribute */
		glBindAttribLocation(program_, InPoint, ENUM_TO_STRING(InPoint).c_str());
		GLCHECKERROR("Failed to bind InPoint to icrPhong.vs");
		glBindAttribLocation(program_, InNormal, ENUM_TO_STRING(InNormal).c_str());
		GLCHECKERROR("Failed to bind InNormal to icrPhong.vs");
		glBindAttribLocation(program_, InTexCoord, ENUM_TO_STRING(InTexCoord).c_str());
		GLCHECKERROR("Failed to bind InTexCoord to icrPhong.vs");
		glBindAttribLocation(program_, InInstancingMtx, ENUM_TO_STRING(InInstancingMtx).c_str());
		GLCHECKERROR("Failed to bind InInstancingMtx to icrPhong.vs");
		glBindAttribLocation(program_, InColor, ENUM_TO_STRING(InColor).c_str());
		GLCHECKERROR("Failed to bind InColor to icrPhong.vs");
		glBindAttribLocation(program_, InWireColor, ENUM_TO_STRING(InWireColor).c_str());
		GLCHECKERROR("Failed to bind InWireColor to icrPhong.vs");
		
		/* link shader program */
		glLinkProgram(program_);
		CHECKERROR(checkLinkStatus_(), "Failed to link phong shader");
		
		glDeleteShader(vs);
		GLCHECKERROR("Failed to delete icrPhong.vs");
		glDeleteShader(fs);
		GLCHECKERROR("Failed to delete icrPhong.fs");
		
		return true;
	}
	virtual bool setUniform(const int param, const boost::any& uniform)
	{
		try
		{
			switch (param)
			{
				case kXformMtx:
				{
					const Mat4x4f value = boost::any_cast<Mat4x4f>(uniform);
					glUniformMatrix4fv(uniformLocation(ENUM_TO_STRING(kXformMtx)), 1, GL_FALSE, value.data());
					GLCHECKERROR("Failed to set kXformMtx uniform");
				}
					break;
				case kDrawMode:
				{
					const int value = boost::any_cast<int>(uniform);
					glUniform1i(uniformLocation(ENUM_TO_STRING(kDrawMode)), value);
					GLCHECKERROR("Failed to set kDrawMode uniform");
				}
					break;
				case kOverrideMask:
				{
					const int value = static_cast<int>(boost::any_cast<bool>(uniform));
					glUniform1i(uniformLocation(ENUM_TO_STRING(kOverrideMask)), value);
					GLCHECKERROR("Failed to set kOverrideMask uniform");
				}
					break;
				case kMaskColor:
				{
					const RGBAColor value = boost::any_cast<RGBAColor>(uniform);
					glUniform4f(uniformLocation(ENUM_TO_STRING(kMaskColor)), value[0], value[1], value[2], value[3]);
					GLCHECKERROR("Failed to set kMaskColor uniform");
					
				}
					break;
				
				case kOverrideTexture:
				{
					const int value = static_cast<int>(boost::any_cast<bool>(uniform));
					glUniform1i(uniformLocation(ENUM_TO_STRING(kOverrideTexture)), value);
					GLCHECKERROR("Failed to set kOverrideTexture uniform");
					
				}
					break;
				
				case kNoLighting:
				{
					const int value = static_cast<int>(boost::any_cast<bool>(uniform));
					glUniform1i(uniformLocation(ENUM_TO_STRING(kNoLighting)), value);
					GLCHECKERROR("Failed to set kNoLighting uniform");
					
				}
					break;
			}
		}
		catch(const boost::bad_any_cast&)
		{
			STDOUT_ERROR("GlIcrPhongShader::Failed to set uniform, %d", param);
			return false;
		}
		return true;
	}
};

///------------------------------------------------------------- GlTestShader
class GlTestShader : public GlShader
{
public:
	typedef GlShader Base;
public:
	enum Attribute
	{
		InPoint         = 0,
		InNormal        = 1,
		InTexCoord      = 2,
		
		InInstancingMtx = 3,
		InColor         = 7,
		InWireColor     = 8,
		
	};
	
	enum Uniform
	{
		kXformMtx,
		
		kDrawMode,
		kOverrideMask,
		kMaskColor,
		
		kOverrideTexture,
		
		kNoLighting,
	};

public:
	static const std::shared_ptr<GlTestShader>& Get(const std::string rootDirectory = GlShader::GetRootDirectory())
	{
		static std::shared_ptr<GlTestShader> pShader = nullptr;
		if(!pShader)
			pShader = GlShader::Create<GlTestShader>(rootDirectory);
		return pShader;
	}

public:
	GlTestShader() : Base()
	{
	}
	virtual ~GlTestShader ()
	{
	}
public:
	virtual bool initialize(const std::string rootDirectory)
	{
		/* create shader program  */
		program_ = glCreateProgram();
		
		/* attach shader to program  */
		GLuint vs = loadFromFile(GL_VERTEX_SHADER,   rootDirectory + "test.vs");
		GLuint fs = loadFromFile(GL_FRAGMENT_SHADER, rootDirectory + "test.fs");
		
		glAttachShader(program_, vs);
		GLCHECKERROR("Failed to attach test.vs to program");
		glAttachShader(program_, fs);
		GLCHECKERROR("Failed to attach test.fs to program");
		
		/* binding attribute */
		glBindAttribLocation(program_, InPoint, ENUM_TO_STRING(InPoint).c_str());
		GLCHECKERROR("Failed to bind InPoint to icrPhong.vs");
		glBindAttribLocation(program_, InNormal, ENUM_TO_STRING(InNormal).c_str());
		GLCHECKERROR("Failed to bind InNormal to icrPhong.vs");
		glBindAttribLocation(program_, InTexCoord, ENUM_TO_STRING(InTexCoord).c_str());
		GLCHECKERROR("Failed to bind InTexCoord to icrPhong.vs");
		glBindAttribLocation(program_, InInstancingMtx, ENUM_TO_STRING(InInstancingMtx).c_str());
		GLCHECKERROR("Failed to bind InInstancingMtx to icrPhong.vs");
		glBindAttribLocation(program_, InColor, ENUM_TO_STRING(InColor).c_str());
		GLCHECKERROR("Failed to bind InColor to icrPhong.vs");
		glBindAttribLocation(program_, InWireColor, ENUM_TO_STRING(InWireColor).c_str());
		GLCHECKERROR("Failed to bind InWireColor to icrPhong.vs");
		
		/* link shader program */
		glLinkProgram(program_);
		CHECKERROR(checkLinkStatus_(), "Failed to link test shader");
		
		glDeleteShader(vs);
		GLCHECKERROR("Failed to delete test.vs");
		glDeleteShader(fs);
		GLCHECKERROR("Failed to delete test.fs");
		
		return true;
	}
	virtual bool setUniform(const int param, const boost::any& uniform)
	{
		try
		{
			switch (param)
			{
				case kXformMtx:
				{
					const Mat4x4f value = boost::any_cast<Mat4x4f>(uniform);
					glUniformMatrix4fv(uniformLocation(ENUM_TO_STRING(kXformMtx)), 1, GL_FALSE, value.data());
					GLCHECKERROR("Failed to set kXformMtx uniform");
				}
					break;
				case kDrawMode:
				{
					const int value = boost::any_cast<int>(uniform);
					glUniform1i(uniformLocation(ENUM_TO_STRING(kDrawMode)), value);
					GLCHECKERROR("Failed to set kDrawMode uniform");
				}
					break;
				case kOverrideMask:
				{
					const int value = static_cast<int>(boost::any_cast<bool>(uniform));
					glUniform1i(uniformLocation(ENUM_TO_STRING(kOverrideMask)), value);
					GLCHECKERROR("Failed to set kOverrideMask uniform");
				}
					break;
				case kMaskColor:
				{
					const RGBAColor value = boost::any_cast<RGBAColor>(uniform);
					glUniform4f(uniformLocation(ENUM_TO_STRING(kMaskColor)), value[0], value[1], value[2], value[3]);
					GLCHECKERROR("Failed to set kMaskColor uniform");
					
				}
					break;
				
				case kOverrideTexture:
				{
					const int value = static_cast<int>(boost::any_cast<bool>(uniform));
					glUniform1i(uniformLocation(ENUM_TO_STRING(kOverrideTexture)), value);
					GLCHECKERROR("Failed to set kOverrideTexture uniform");
					
				}
					break;
				
				case kNoLighting:
				{
					const int value = static_cast<int>(boost::any_cast<bool>(uniform));
					glUniform1i(uniformLocation(ENUM_TO_STRING(kNoLighting)), value);
					GLCHECKERROR("Failed to set kNoLighting uniform");
					
				}
					break;
			}
		}
		catch(const boost::bad_any_cast&)
		{
			STDOUT_ERROR("GlIcrPhongShader::Failed to set uniform, %d", param);
			return false;
		}
		return true;
	}
};


NAMESPACE_BASE_END
