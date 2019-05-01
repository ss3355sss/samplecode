#pragma once

#include "base/pch.h"

#include "base/frame.h"
#include "base/logger.h"

#include "base/matrixStack.h"

#include "base/bbox.h"
#include "base/renderItem.h"

#include <pxr/pxr.h>
#include <pxr/usd/ar/resolver.h>
#include <pxr/usd/sdf/layer.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/stageCache.h>
#include <pxr/usd/usd/stageCacheContext.h>
#include <pxr/usd/usd/variantSets.h>
#include <pxr/usd/usdGeom/points.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/xform.h>
#include <pxr/usd/usdUtils/pipeline.h>
#include <pxr/usd/usdUtils/stageCache.h>

NAMESPACE_BASE_BEGIN

template<typename T>
inline pxr::SdfValueTypeName GetUsdTypeName()
{
	//TODO
	//SdfValueTypeName Bool;
	//pxr::SdfValueTypeName UChar, Int, UInt, Int64, UInt64;
	//SdfValueTypeName Half, Float, Double;
	//SdfValueTypeName String, Token, Asset;
	//SdfValueTypeName Int2,     Int3,     Int4;
	//SdfValueTypeName Half2,    Half3,    Half4;
	//SdfValueTypeName Float2,   Float3,   Float4;
	//SdfValueTypeName Double2,  Double3,  Double4;
	//SdfValueTypeName Point3h,  Point3f,  Point3d;
	//SdfValueTypeName Vector3h, Vector3f, Vector3d;
	//SdfValueTypeName Normal3h, Normal3f, Normal3d;
	//SdfValueTypeName Color3h,  Color3f,  Color3d;
	//SdfValueTypeName Color4h,  Color4f,  Color4d;
	//SdfValueTypeName Quath,    Quatf,    Quatd;
	//SdfValueTypeName Matrix2d, Matrix3d, Matrix4d;
	//SdfValueTypeName Frame4d;
	//SdfValueTypeName TexCoord2h, TexCoord2f, TexCoord2d;
	//SdfValueTypeName TexCoord3h, TexCoord3f, TexCoord3d;
	//SdfValueTypeName BoolArray;
	//SdfValueTypeName UCharArray, IntArray, UIntArray, Int64Array, UInt64Array;
	//SdfValueTypeName HalfArray, FloatArray, DoubleArray;
	//SdfValueTypeName StringArray, TokenArray, AssetArray;
	//SdfValueTypeName Int2Array,     Int3Array,     Int4Array;
	//SdfValueTypeName Half2Array,    Half3Array,    Half4Array;
	//SdfValueTypeName Float2Array,   Float3Array,   Float4Array;
	//SdfValueTypeName Double2Array,  Double3Array,  Double4Array;
	//SdfValueTypeName Point3hArray,  Point3fArray,  Point3dArray;
	//SdfValueTypeName Vector3hArray, Vector3fArray, Vector3dArray;
	//SdfValueTypeName Normal3hArray, Normal3fArray, Normal3dArray;
	//SdfValueTypeName Color3hArray,  Color3fArray,  Color3dArray;
	//SdfValueTypeName Color4hArray,  Color4fArray,  Color4dArray;
	//SdfValueTypeName QuathArray,    QuatfArray,    QuatdArray;
	//SdfValueTypeName Matrix2dArray, Matrix3dArray, Matrix4dArray;
	//SdfValueTypeName Frame4dArray;
	//SdfValueTypeName TexCoord2hArray, TexCoord2fArray, TexCoord2dArray;
	//SdfValueTypeName TexCoord3hArray, TexCoord3fArray, TexCoord3dArray;

	const std::type_info& typeInfo = typeid(T);
	pxr::SdfValueTypeName valueType;
	
	if     (typeInfo == typeid(bool)) 	                        { valueType = pxr::SdfValueTypeNames->Bool;           }
	
	else if(typeInfo == typeid(unsigned char))                  { valueType = pxr::SdfValueTypeNames->UChar;          }
	
	else if(typeInfo == typeid(int))                            { valueType = pxr::SdfValueTypeNames->Int;            }
	else if(typeInfo == typeid(unsigned int))                   { valueType = pxr::SdfValueTypeNames->UInt;           }
	
	else if(typeInfo == typeid(long int))                       { valueType = pxr::SdfValueTypeNames->Int64;          }
	else if(typeInfo == typeid(unsigned long int))              { valueType = pxr::SdfValueTypeNames->UInt64;         }
	
	else if(typeInfo == typeid(float))                          { valueType = pxr::SdfValueTypeNames->Float;          }
	else if(typeInfo == typeid(double))                         { valueType = pxr::SdfValueTypeNames->Double;         }
	
	else if(typeInfo == typeid(std::string))                    { valueType = pxr::SdfValueTypeNames->String;         }
	
	else if(typeInfo == typeid(Vec2i))                          { valueType = pxr::SdfValueTypeNames->Int2;           }
	else if(typeInfo == typeid(Vec3i))                          { valueType = pxr::SdfValueTypeNames->Int3;           }
	else if(typeInfo == typeid(Vec4i))                          { valueType = pxr::SdfValueTypeNames->Int4;           }
	
	else if(typeInfo == typeid(Vec3f))                          { valueType = pxr::SdfValueTypeNames->Vector3f;       }
	else if(typeInfo == typeid(Vec3d))                          { valueType = pxr::SdfValueTypeNames->Vector3d;       }
	
	else if(typeInfo == typeid(Point3f))                        { valueType = pxr::SdfValueTypeNames->Point3f;        }
	else if(typeInfo == typeid(Point3d))                        { valueType = pxr::SdfValueTypeNames->Point3d;        }
	
	else if(typeInfo == typeid(Normal3f))                       { valueType = pxr::SdfValueTypeNames->Normal3f;       }
	else if(typeInfo == typeid(Normal3d))                       { valueType = pxr::SdfValueTypeNames->Normal3d;       }
	
	else if(typeInfo == typeid(TexCoord2f))                     { valueType = pxr::SdfValueTypeNames->TexCoord2f;     }
	else if(typeInfo == typeid(TexCoord2d))                     { valueType = pxr::SdfValueTypeNames->TexCoord2d;     }
	
	else if(typeInfo == typeid(TexCoord3f))                     { valueType = pxr::SdfValueTypeNames->TexCoord3f;     }
	else if(typeInfo == typeid(TexCoord3d))                     { valueType = pxr::SdfValueTypeNames->TexCoord3d;     }
	
	else if(typeInfo == typeid(RGBColor))                       { valueType = pxr::SdfValueTypeNames->Color3f;        }
	else if(typeInfo == typeid(RGBAColor))                      { valueType = pxr::SdfValueTypeNames->Color4f;        }
	
	else if(typeInfo == typeid(Quatf))                          { valueType = pxr::SdfValueTypeNames->Quatf;          }
	else if(typeInfo == typeid(Quatd))                          { valueType = pxr::SdfValueTypeNames->Quatd;          }
	
	else if(typeInfo == typeid(Mat2x2d))                        { valueType = pxr::SdfValueTypeNames->Matrix2d;       }
	else if(typeInfo == typeid(Mat3x3d))                        { valueType = pxr::SdfValueTypeNames->Matrix3d;       }
	else if(typeInfo == typeid(Mat4x4d))                        { valueType = pxr::SdfValueTypeNames->Matrix4d;       }
	
	else if(typeInfo == typeid(BoolArray))                      { valueType = pxr::SdfValueTypeNames->BoolArray;      }
	else if(typeInfo == typeid(UCharArray))                     { valueType = pxr::SdfValueTypeNames->UCharArray;     }
	
	else if(typeInfo == typeid(Int32Array))                     { valueType = pxr::SdfValueTypeNames->IntArray;       }
	else if(typeInfo == typeid(UInt32Array))                    { valueType = pxr::SdfValueTypeNames->UIntArray;      }
	else if(typeInfo == typeid(Int64Array))                     { valueType = pxr::SdfValueTypeNames->Int64Array;     }
	else if(typeInfo == typeid(UInt64Array))                    { valueType = pxr::SdfValueTypeNames->UInt64Array;    }
	
	else if(typeInfo == typeid(FloatArray))                     { valueType = pxr::SdfValueTypeNames->FloatArray;     }
	else if(typeInfo == typeid(DoubleArray))                    { valueType = pxr::SdfValueTypeNames->DoubleArray;    }
	
	else if(typeInfo == typeid(StringArray))                    { valueType = pxr::SdfValueTypeNames->StringArray;    }
	
	else if(typeInfo == typeid(Vec2iArray))                     { valueType = pxr::SdfValueTypeNames->Int2Array;      }
	else if(typeInfo == typeid(Vec3iArray))                     { valueType = pxr::SdfValueTypeNames->Int3Array;      }
	else if(typeInfo == typeid(Vec4iArray))                     { valueType = pxr::SdfValueTypeNames->Int4Array;      }
	
	else if(typeInfo == typeid(Vec3fArray))                     { valueType = pxr::SdfValueTypeNames->Vector3fArray;  }
	else if(typeInfo == typeid(Vec3dArray))                     { valueType = pxr::SdfValueTypeNames->Vector3dArray;  }
	
	else if(typeInfo == typeid(Point3fArray))                   { valueType = pxr::SdfValueTypeNames->Point3fArray;   }
	else if(typeInfo == typeid(Point3dArray))                   { valueType = pxr::SdfValueTypeNames->Point3dArray;   }
	
	else if(typeInfo == typeid(Normal3fArray))                  { valueType = pxr::SdfValueTypeNames->Normal3fArray;  }
	else if(typeInfo == typeid(Normal3dArray))                  { valueType = pxr::SdfValueTypeNames->Normal3dArray;  }
	
	else if(typeInfo == typeid(TexCoord2fArray))                { valueType = pxr::SdfValueTypeNames->TexCoord2fArray;}
	else if(typeInfo == typeid(TexCoord2dArray))                { valueType = pxr::SdfValueTypeNames->TexCoord2dArray;}
	
	else if(typeInfo == typeid(TexCoord3fArray))                { valueType = pxr::SdfValueTypeNames->TexCoord3fArray;}
	else if(typeInfo == typeid(TexCoord3dArray))                { valueType = pxr::SdfValueTypeNames->TexCoord3dArray;}
	
	else if(typeInfo == typeid(RGBColorArray))                  { valueType = pxr::SdfValueTypeNames->Color3fArray;   }
	else if(typeInfo == typeid(RGBAColorArray))                 { valueType = pxr::SdfValueTypeNames->Color4fArray;   }
	
	else if(typeInfo == typeid(QuatfArray))                     { valueType = pxr::SdfValueTypeNames->QuatfArray;     }
	else if(typeInfo == typeid(QuatdArray))                     { valueType = pxr::SdfValueTypeNames->QuatdArray;     }
	
	else if(typeInfo == typeid(Mat2x2dArray))                   { valueType = pxr::SdfValueTypeNames->Matrix2dArray;  }
	else if(typeInfo == typeid(Mat3x3dArray))                   { valueType = pxr::SdfValueTypeNames->Matrix3dArray;  }
	else if(typeInfo == typeid(Mat4x4dArray))                   { valueType = pxr::SdfValueTypeNames->Matrix4dArray;  }

	else                                                        { STDOUT_ERROR("Invalid value type, %s") typeInfo.name();}
	return valueType;
};

inline pxr::SdfValueTypeName GetUsdTypeName(const size_t type)
{
	pxr::SdfValueTypeName valueType;
	if(type == cppType::kInvalid)
	{
		STDOUT_ERROR("Invalid value type, %u", type);
		return valueType;
	}

	switch (type)
	{
		case cppType::kBool: 	                valueType = pxr::SdfValueTypeNames->Bool;                   break;
		
		//case cppType::kChar:
		//case cppType::kShort:
		case cppType::kInt:                     valueType = pxr::SdfValueTypeNames->Int;                    break;
		case cppType::kLongInt:                 valueType = pxr::SdfValueTypeNames->Int64;                  break;
		
		//case cppType::kUnsignedChar:
		//case cppType::kUnsignedShort:
		case cppType::kUnsignedInt:             valueType = pxr::SdfValueTypeNames->UInt;                   break;
		case cppType::kUnsignedLongInt:         valueType = pxr::SdfValueTypeNames->UInt64;                 break;
		
		case cppType::kFloat:                   valueType = pxr::SdfValueTypeNames->Float;                  break;
		case cppType::kDouble:                  valueType = pxr::SdfValueTypeNames->Double;                 break;
		
		case cppType::kString:                  valueType = pxr::SdfValueTypeNames->String;                 break;
		
		case cppType::kVec2i:                   valueType = pxr::SdfValueTypeNames->Int2;                   break;
		case cppType::kVec3i:                   valueType = pxr::SdfValueTypeNames->Int3;                   break;
		case cppType::kVec4i:                   valueType = pxr::SdfValueTypeNames->Int4;                   break;
		
		//case cppType::kVec2f:
		case cppType::kVec3f:                   valueType = pxr::SdfValueTypeNames->Vector3f;               break;
		//case cppType::kVec4f:
		
		//case cppType::kVec2d:
		case cppType::kVec3d:                   valueType = pxr::SdfValueTypeNames->Vector3d;               break;
		//case cppType::kVec4d:
		
		//case cppType::kPoint2f:
		case cppType::kPoint3f:                 valueType = pxr::SdfValueTypeNames->Point3f;                break;
		//case cppType::kNormal2f:
		case cppType::kNormal3f:                valueType = pxr::SdfValueTypeNames->Normal3f;               break;
		case cppType::kTexCoord2f:              valueType = pxr::SdfValueTypeNames->TexCoord2f;             break;
		case cppType::kTexCoord3f:              valueType = pxr::SdfValueTypeNames->TexCoord3f;             break;
		//case cppType::kPoint2d:
		case cppType::kPoint3d:                 valueType = pxr::SdfValueTypeNames->Point3d;                break;
		//case cppType::kNormal2d:
		case cppType::kNormal3d:                valueType = pxr::SdfValueTypeNames->Normal3d;               break;
		case cppType::kTexCoord2d:              valueType = pxr::SdfValueTypeNames->TexCoord2d;             break;
		case cppType::kTexCoord3d:              valueType = pxr::SdfValueTypeNames->TexCoord3d;             break;
		
		case cppType::kRGBColor:                valueType = pxr::SdfValueTypeNames->Color3f;                break;
		case cppType::kRGBAColor:               valueType = pxr::SdfValueTypeNames->Color4f;                break;
		
		case cppType::kQuatf:                   valueType = pxr::SdfValueTypeNames->Quatf;                  break;
		case cppType::kQuatd:                   valueType = pxr::SdfValueTypeNames->Quatd;                  break;
		
		//case cppType::kMat2x2f:
		//case cppType::kMat3x3f:
		//case cppType::kMat4x4f:
		case cppType::kMat2x2d:                 valueType = pxr::SdfValueTypeNames->Matrix2d;               break;
		case cppType::kMat3x3d:                 valueType = pxr::SdfValueTypeNames->Matrix3d;               break;
		case cppType::kMat4x4d:                 valueType = pxr::SdfValueTypeNames->Matrix4d;               break;
		
		case cppType::kBoolArray:               valueType = pxr::SdfValueTypeNames->BoolArray;              break;
		
		//case cppType::kCharArray:
		//case cppType::kShortArray:
		case cppType::kInt32Array:              valueType = pxr::SdfValueTypeNames->IntArray;               break;
		case cppType::kInt64Array:              valueType = pxr::SdfValueTypeNames->Int64Array;             break;
		
		//case cppType::kUCharArray:
		//case cppType::kUShortArray:
		case cppType::kUInt32Array:             valueType = pxr::SdfValueTypeNames->UIntArray;              break;
		case cppType::kUInt64Array:             valueType = pxr::SdfValueTypeNames->UInt64Array;            break;
		
		case cppType::kFloatArray:              valueType = pxr::SdfValueTypeNames->FloatArray;             break;
		case cppType::kDoubleArray:             valueType = pxr::SdfValueTypeNames->DoubleArray;            break;
		
		case cppType::kStringArray:             valueType = pxr::SdfValueTypeNames->StringArray;            break;
		
		case cppType::kVec2iArray:              valueType = pxr::SdfValueTypeNames->Int2Array;              break;
		case cppType::kVec3iArray:              valueType = pxr::SdfValueTypeNames->Int3Array;              break;
		case cppType::kVec4iArray:              valueType = pxr::SdfValueTypeNames->Int4Array;              break;
		
		//case cppType::kVec2fArray:
		case cppType::kVec3fArray:              valueType = pxr::SdfValueTypeNames->Vector3fArray;          break;
		//case cppType::kVec4fArray:
		
		//case cppType::kVec2dArray:
		case cppType::kVec3dArray:              valueType = pxr::SdfValueTypeNames->Vector3dArray;          break;
		//case cppType::kVec4dArray:
		
		//case cppType::kPoint2fArray:
		case cppType::kPoint3fArray:            valueType = pxr::SdfValueTypeNames->Point3fArray;           break;
		//case cppType::kNormal2fArray:
		case cppType::kNormal3fArray:           valueType = pxr::SdfValueTypeNames->Normal3fArray;          break;
		case cppType::kTexCoord2fArray:         valueType = pxr::SdfValueTypeNames->TexCoord2fArray;        break;
		case cppType::kTexCoord3fArray:         valueType = pxr::SdfValueTypeNames->TexCoord3fArray;        break;
		
		//case cppType::kPoint2dArray:
		case cppType::kPoint3dArray:            valueType = pxr::SdfValueTypeNames->Point3dArray;           break;
		//case cppType::kNormal2dArray:
		case cppType::kNormal3dArray:           valueType = pxr::SdfValueTypeNames->Normal3dArray;          break;
		//case cppType::kTexCoord2dArray:
		case cppType::kTexCoord3dArray:         valueType = pxr::SdfValueTypeNames->TexCoord3dArray;        break;
		
		case cppType::kRGBColorArray:           valueType = pxr::SdfValueTypeNames->Color3fArray;           break;
		case cppType::kRGBAColorArray:          valueType = pxr::SdfValueTypeNames->Color4fArray;           break;
		
		case cppType::kQuatfArray:              valueType = pxr::SdfValueTypeNames->QuatfArray;             break;
		case cppType::kQuatdArray:              valueType = pxr::SdfValueTypeNames->QuatdArray;             break;
		
		//case cppType::kMat2x2fArray:
		//case cppType::kMat2x2dArray:
		//case cppType::kMat3x3fArray:
		
		case cppType::kMat2x2dArray:            valueType = pxr::SdfValueTypeNames->Matrix2dArray;          break;
		case cppType::kMat3x3fArray:            valueType = pxr::SdfValueTypeNames->Matrix3dArray;          break;
		case cppType::kMat4x4dArray:            valueType = pxr::SdfValueTypeNames->Matrix4dArray;          break;
		default :                               STDOUT_ERROR("Failed to get supporting usdType, %u", type); break;
	}
	return valueType;
};

/* ------------------------------------------------------------------------- attribute */
inline UsdAttribute CreateOrGetAttribute(const pxr::UsdPrim&            prim,
                                           const std::string            name,
                                           const pxr::SdfValueTypeName  valueType,
                                           const bool                   custom      = true,
                                           const pxr::SdfVariability    variability = pxr::SdfVariabilityVarying)
{
	if (!prim)
	{
		STDOUT_ERROR("Failed to get prim");
		return UsdAttribute();
	}
	
	pxr::TfToken nameToken(name);
	if (nameToken.IsEmpty())
	{
		STDOUT_ERROR("Failed to get valid UsdAttribute name");
		return UsdAttribute();
	}
	if(prim.HasAttribute(nameToken))
	{
		return prim.GetAttribute(nameToken);
	}

	UsdAttribute usdAttribute = prim.CreateAttribute(nameToken,
			                                         valueType,
			                                         custom,
			                                         variability
	);
	if(!usdAttribute)
	{
		STDOUT_ERROR("Failed to create UsdAttribute, \"%s\"", name.c_str());
		return UsdAttribute();
	}
	return usdAttribute;
}

/* ------------------------------------------------------------------------- primvar */
inline UsdGeomPrimvar CreateOrGetGeomPrimvar(const pxr::UsdPrim&          prim,
                                             const std::string            name,
                                             const pxr::SdfValueTypeName  valueType,
                                             const pxr::TfToken&          interpolation = pxr::TfToken(),
                                             const int                    elementSize = -1)
{
	pxr::UsdGeomImageable imageable(prim);
	if (!imageable)
	{
		STDOUT_ERROR("Failed to get imageable");
		return UsdGeomPrimvar();
	}
	pxr::TfToken nameToken(name);
	if (nameToken.IsEmpty())
	{
		STDOUT_ERROR("Failed to get valid GeomPrimVar name");
		return UsdGeomPrimvar();
	}
	if(imageable.HasPrimvar(nameToken))
	{
		return imageable.GetPrimvar(nameToken);
	}
	pxr::UsdGeomPrimvar primvar = imageable.CreatePrimvar(nameToken,
	                                                      valueType,
	                                                      interpolation,
	                                                      elementSize
	);
	
	if(!primvar)
	{
		STDOUT_ERROR("Failed to create GeomPrimvar, \"%s\"", name.c_str());
		return UsdGeomPrimvar();
	}
	return primvar;
}

template <typename T>
inline const T GetPrimvar(const pxr::UsdPrim& prim, const std::string name, const pxr::UsdTimeCode timeCode = pxr::UsdTimeCode::Default())
{
	pxr::UsdGeomImageable imageable(prim);
	if (!imageable)
	{
		STDOUT_ERROR("Failed to get imageable, %s", name.c_str());
		return T();
	}
	bool hasPrimvar = imageable.HasPrimvar(pxr::TfToken(name));
	if(!hasPrimvar)
	{
		STDOUT_ERROR("Failed to get primvar, %s", name.c_str());
		return T();
	}
	T t;
	imageable.GetPrimvar(pxr::TfToken(name)).Get(&t, timeCode);
	return t;
}
template <typename T>
inline void SetPrimvar(const pxr::UsdPrim& prim, const std::string name, const T& in, const pxr::UsdTimeCode timeCode = pxr::UsdTimeCode::Default())
{
	pxr::UsdGeomImageable imageable(prim);
	if (!imageable)
	{
		STDOUT_ERROR("Failed to get imageable");
		return;
	}
	bool hasPrimvar = imageable.HasPrimvar(pxr::TfToken(name));
	if(!hasPrimvar)
	{
		STDOUT_ERROR("Failed to get primvar, %s", name.c_str());
		return;
	}
	imageable.GetPrimvar(pxr::TfToken(name)).Set(in, timeCode);
}


NAMESPACE_BASE_END