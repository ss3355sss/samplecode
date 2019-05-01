#include "maya/taneMPxTransform.h"


#include "baseGl/glutil.h"
#include "baseGl/taskManager.h"

#include "lib/baseMaya/util.h"

#include "maya/drawOverride.h"

#include "maya/taneMPxTransform.h"
#include "maya/taneMPxShape.h"

NAMESPACE_BASE_BEGIN
///--------------------------------------------------------------------------- TaneMPxTransformationMatrix
MTypeId TaneMPxTransformationMatrix::id(RegisteredNodes->TaneMPxTransformMtx.id);
///------------------------------------------------------------ static functions
void* TaneMPxTransformationMatrix::Creator()
{
	return new TaneMPxTransformationMatrix;
}

///------------------------------------------------------------ constructor / destructor
TaneMPxTransformationMatrix::TaneMPxTransformationMatrix()
{
}
///------------------------------------------------------------ virtual functions
MMatrix TaneMPxTransformationMatrix::asMatrix() const
{
	return ParentClass::asMatrix();
}
MMatrix TaneMPxTransformationMatrix::asMatrix(double percent) const
{
	return ParentClass::asMatrix();
}
MMatrix TaneMPxTransformationMatrix::asRotateMatrix() const
{
	return ParentClass::asRotateMatrix();
}
///------------------------------------------------------------ public helper functions
///------------------------------------------------------------ private helper functions
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///--------------------------------------------------------------------------- TaneMPxTransform
MTypeId TaneMPxTransform::id(RegisteredNodes->TaneMPxTransform.id);
MString TaneMPxTransform::nodeName(RegisteredNodes->TaneMPxTransform.name);
MString TaneMPxTransform::drawDbClassification("drawdb/geometry/" + RegisteredNodes->TaneMPxTransform.name);
MString TaneMPxTransform::drawOverrideRegistrantId(RegisteredNodes->TaneMPxTransform.name + "DrawOverride");
///------------------------------------------------------------ node attributes
///------------------------------------------------------------ input attributes
///------------------------------------------------------------ output attributes
///------------------------------------------------------------ internal attributes
IMPL_MISCELLANEOUS_ATTRIBUTE(TaneMPxTransform);
///------------------------------------------------------------ static functions
void* TaneMPxTransform::Creator()
{
	return new TaneMPxTransform();
}
MStatus TaneMPxTransform::Initialize()
{
	MStatus              stat;
	MFnTypedAttribute    tAttr;
	MFnNumericAttribute  nAttr;
	MFnCompoundAttribute cAttr;
	MFnMatrixAttribute   mAttr;
	MFnEnumAttribute     eAttr;
	MFnUnitAttribute     uAttr;

	INITIALIZE_MISCELLANEOUS_ATTRIBUTE(false);

	return stat;
}

///------------------------------------------------------------ constructor / destructor
TaneMPxTransform::TaneMPxTransform() : ParentClass()
{
}
TaneMPxTransform::TaneMPxTransform(MPxTransformationMatrix* tm) : ParentClass(tm)
{
}
TaneMPxTransform::~TaneMPxTransform()
{

}

///------------------------------------------------------------ virtual functions
MPxTransformationMatrix* TaneMPxTransform::createTransformationMatrix()
{
	return new TaneMPxTransformationMatrix();
}

void TaneMPxTransform::postConstructor()
{
	ParentClass::postConstructor();
	if(NULL == baseTransformationMatrix)
	{
		MGlobal::displayWarning("NULL baseTransformationMatrix found!");
		baseTransformationMatrix = new MPxTransformationMatrix();
	}

	MFnDependencyNode nodeFn;
	nodeFn.setObject(thisMObject());
	nodeFn.setName(RegisteredNodes->TaneMPxShape.name);
}
MStatus TaneMPxTransform::compute(const MPlug &plug, MDataBlock &data)
{
	return MPxTransform::compute(plug, data);
}

MStatus TaneMPxTransform::validateAndSetValue(const MPlug &plug, const MDataHandle &handle, const MDGContext &context)
{
	if(plug.isNull())
		return MS::kFailure;
	return ParentClass::validateAndSetValue(plug, handle, context);
}

void TaneMPxTransform::resetTransformation(MPxTransformationMatrix* resetMatrix)
{
	ParentClass::resetTransformation(resetMatrix);
}
void TaneMPxTransform::resetTransformation(const MMatrix &matrix)
{
	ParentClass::resetTransformation(matrix);
}
///------------------------------------------------------------ public helper functions
///------------------------------------------------------------ private helper functions
NAMESPACE_BASE_END


