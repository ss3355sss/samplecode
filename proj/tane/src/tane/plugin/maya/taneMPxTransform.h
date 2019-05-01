// Created by dohyeon.yang on 11/24/17.
#pragma once

#include "maya/pch.h"
NAMESPACE_BASE_BEGIN
///---------------------------------------------- TaneMPxTransformationMatrix
class TaneMPxTransformationMatrix : public MPxTransformationMatrix
{
protected:
	typedef MPxTransformationMatrix ParentClass;
public:
	static MTypeId id;
public:
	static void* Creator();

public:
	TaneMPxTransformationMatrix();

	virtual MMatrix asMatrix() const;
	virtual MMatrix asMatrix(double percent) const;
	virtual MMatrix asRotateMatrix() const;
};
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///---------------------------------------------- TaneMPxTransform
class TaneMPxTransform : public MPxTransform
{
protected:
	typedef MPxTransform ParentClass;

public:
	static MTypeId id;
	static MString nodeName;
	static MString drawDbClassification;
	static MString drawOverrideRegistrantId;

public:
	DECL_MISCELLANEOUS_ATTRIBUTE;
public:
	///------------------------------------------------------------ static functions
	static void* Creator();
	static MStatus Initialize();


public:
	///------------------------------------------------------------ constructor / destructor
	TaneMPxTransform();
	TaneMPxTransform(MPxTransformationMatrix* tm);
	virtual ~TaneMPxTransform();
	///------------------------------------------------------------ virtual functions
	virtual MPxTransformationMatrix* createTransformationMatrix();
	virtual void postConstructor();
	virtual MStatus compute(const MPlug &plug, MDataBlock &data);
	virtual MStatus validateAndSetValue(const MPlug &plug, const MDataHandle &handle, const MDGContext &context);
	virtual void resetTransformation(MPxTransformationMatrix* resetMatrix);
	virtual void resetTransformation(const MMatrix &matrix);
public:
	///------------------------------------------------------------ public helper functions
	const char* className();
	///------------------------------------------------------------ private helper functions
};
NAMESPACE_BASE_END


