#pragma once
#include "pch.h"

#include "boost/format.hpp"
#include "boost/functional/hash.hpp"

#include "lib/base/mathUtil.h"
#include "lib/base/frame.h"

NAMESPACE_BASE_BEGIN
///-------------------------------------------------------------------------- MDagPath Wrapper
struct hashDag
{
	size_t operator()(const MDagPath& dagPath) const
	{
		MStatus stat;
		size_t  seed = 0;
		
		unsigned int hashcode = MObjectHandle(dagPath.node(&stat)).hashCode();
		boost::hash_combine(seed, hashcode);
		
		return seed;
	}
};
struct cmpDag
{
	bool operator()(const MDagPath& lhs, const MDagPath& rhs) const
	{
		//return strcmp(lhs.fullPathName().asChar(), rhs.fullPathName().asChar()) < 0;
		
		MStatus      stat;
		unsigned int hashcode1 = MObjectHandle(lhs.node(&stat)).hashCode();
		//if(stat == MS::kFailure)
		//{
		//	cout << "stat is failed" << endl;
		//	return false;
		//}
		unsigned int hashcode2 = MObjectHandle(rhs.node(&stat)).hashCode();
		//if(stat == MS::kFailure)
		//{
		//	cout << "stat is failed" << endl;
		//	return false;
		//}
		if(hashcode1 == hashcode2)
			return true;
		return false;
		
		
	}
};

typedef std::set<MDagPath, cmpDag> MDagPathSet;

template<typename ItemT> using MDagPathMap     = std::map<MDagPath, ItemT, cmpDag>;
template<typename ItemT> using MDagPathHashMap = std::unordered_map<MDagPath, ItemT, hashDag, cmpDag>;


///-------------------------------------------------------------------------- CreateTypedAttribute
inline MObject CreateTypedAttribute(MFnTypedAttribute&                      tAttr,
                                    const MString                           longName,
                                    const MString                           shortName,
                                    const MFnData::Type                     dataType,
                                    const MObject                           defaultData,
                                    const bool                              readable,
                                    const bool                              writable,
                                    const bool                              connectable,
                                    const bool                              storable,
                                    const bool                              cached,
                                    const bool                              channelBox,
                                    const bool                              array,
                                    const bool                              usesArrayDataBuilder,
                                    const bool                              keyable,
                                    const bool                              hidden,
                                    const MFnAttribute::DisconnectBehavior  disconnectBehavior,
                                    const bool                              affectsAppearance,
                                    MStatus*                                outStat
)
{
	MStatus stat;
	MObject attrObj;
	attrObj = tAttr.create(longName,
	                       shortName,
	                       dataType,
	                       defaultData,
	                       &stat
	);
	if(stat != MS::kSuccess)
	{
		*outStat = MS::kFailure;
		return MObject();
	}
	SET_ATTRIBUTE_STATUS(tAttr,
	                     readable,              /* Readable              (default : true)     */
	                     writable,              /* Writable              (default : true)     */
	                     connectable,           /* Connectable           (default : true)     */
	                     storable,              /* Stroable              (default : true)     */
	                     cached,                /* Cached                (default : true)     */
	                     channelBox,            /* ChannelBox            (default : false)    */
	                     array,                 /* Array                 (default : false)    */
	                     usesArrayDataBuilder,  /* Use Array DataBuilder (default : false)    */
	                     keyable,               /* Keyable               (default : false)    */
	                     hidden,                /* Hidden                (default : false)    */
	                     disconnectBehavior,    /* Disconnect Behavior   (default : kNothing) */
	                     affectsAppearance      /* Affect Appearance     (default : false)    */
	);
	*outStat = MS::kSuccess;
	return attrObj;
}
inline MObject CreateTypedAttribute(MFnTypedAttribute&                      tAttr,
                                    const MString                           longName,
                                    const MString                           shortName,
                                    const MTypeId                           typeId,
                                    const MObject                           defaultData,
                                    const bool                              readable,
                                    const bool                              writable,
                                    const bool                              connectable,
                                    const bool                              storable,
                                    const bool                              cached,
                                    const bool                              channelBox,
                                    const bool                              array,
                                    const bool                              usesArrayDataBuilder,
                                    const bool                              keyable,
                                    const bool                              hidden,
                                    const MFnAttribute::DisconnectBehavior  disconnectBehavior,
                                    const bool                              affectsAppearance,
                                    MStatus*                                outStat
)
{
	MStatus stat;
	MObject attrObj;
	attrObj = tAttr.create(longName,
	                       shortName,
	                       typeId,
	                       defaultData,
	                       &stat
	);
	if(stat != MS::kSuccess)
	{
		*outStat = MS::kFailure;
		return MObject();
	}
	SET_ATTRIBUTE_STATUS(tAttr,
	                     readable,              /* Readable              (default : true)     */
	                     writable,              /* Writable              (default : true)     */
	                     connectable,           /* Connectable           (default : true)     */
	                     storable,              /* Stroable              (default : true)     */
	                     cached,                /* Cached                (default : true)     */
	                     channelBox,            /* ChannelBox            (default : false)    */
	                     array,                 /* Array                 (default : false)    */
	                     usesArrayDataBuilder,  /* Use Array DataBuilder (default : false)    */
	                     keyable,               /* Keyable               (default : false)    */
	                     hidden,                /* Hidden                (default : false)    */
	                     disconnectBehavior,    /* Disconnect Behavior   (default : kNothing) */
	                     affectsAppearance      /* Affect Appearance     (default : false)    */
	);
	*outStat = MS::kSuccess;
	return attrObj;
}

///-------------------------------------------------------------------------- CreateUnitAttribute
inline MObject CreateUnitAttribute(MFnUnitAttribute&                       uAttr,
                                   const MString                           longName,
                                   const MString                           shortName,
                                   const MFnUnitAttribute::Type            unitType,
                                   const double                            defaultValue,
                                   const bool                              readable,
                                   const bool                              writable,
                                   const bool                              connectable,
                                   const bool                              storable,
                                   const bool                              cached,
                                   const bool                              channelBox,
                                   const bool                              array,
                                   const bool                              usesArrayDataBuilder,
                                   const bool                              keyable,
                                   const bool                              hidden,
                                   const MFnAttribute::DisconnectBehavior  disconnectBehavior,
                                   const bool                              affectsAppearance,
                                   MStatus*                                outStat
)
{
	MStatus stat;
	MObject attrObj;
	
	attrObj = uAttr.create(longName,
	                       shortName,
	                       unitType,
	                       defaultValue,
	                       &stat
	);
	if(stat != MS::kSuccess)
	{
		*outStat = MS::kFailure;
		return MObject();
	}
	SET_ATTRIBUTE_STATUS(uAttr,
	                     readable,              /* Readable              (default : true)     */
	                     writable,              /* Writable              (default : true)     */
	                     connectable,           /* Connectable           (default : true)     */
	                     storable,              /* Stroable              (default : true)     */
	                     cached,                /* Cached                (default : true)     */
	                     channelBox,            /* ChannelBox            (default : false)    */
	                     array,                 /* Array                 (default : false)    */
	                     usesArrayDataBuilder,  /* Use Array DataBuilder (default : false)    */
	                     keyable,               /* Keyable               (default : false)    */
	                     hidden,                /* Hidden                (default : false)    */
	                     disconnectBehavior,    /* Disconnect Behavior   (default : kNothing) */
	                     affectsAppearance      /* Affect Appearance     (default : false)    */
	);
	*outStat = MS::kSuccess;
	return attrObj;
}

///-------------------------------------------------------------------------- CreateEnumAttribute
inline MObject CreateEnumAttribute(MFnEnumAttribute&                       eAttr,
                                   const MString                           longName,
                                   const MString                           shortName,
                                   const short                             defaultValue,
                                   const bool                              readable,
                                   const bool                              writable,
                                   const bool                              connectable,
                                   const bool                              storable,
                                   const bool                              cached,
                                   const bool                              channelBox,
                                   const bool                              array,
                                   const bool                              usesArrayDataBuilder,
                                   const bool                              keyable,
                                   const bool                              hidden,
                                   const MFnAttribute::DisconnectBehavior  disconnectBehavior,
                                   const bool                              affectsAppearance,
                                   MStatus*                                outStat
)
{
	MStatus stat;
	MObject attrObj;
	attrObj = eAttr.create(longName,
		                   shortName,
		                   defaultValue,
		                   &stat);
	if(stat != MS::kSuccess)
	{
		*outStat = MS::kFailure;
		return MObject();
	}
	SET_ATTRIBUTE_STATUS(eAttr,
	                     readable,              /* Readable              (default : true)     */
	                     writable,              /* Writable              (default : true)     */
	                     connectable,           /* Connectable           (default : true)     */
	                     storable,              /* Stroable              (default : true)     */
	                     cached,                /* Cached                (default : true)     */
	                     channelBox,            /* ChannelBox            (default : false)    */
	                     array,                 /* Array                 (default : false)    */
	                     usesArrayDataBuilder,  /* Use Array DataBuilder (default : false)    */
	                     keyable,               /* Keyable               (default : false)    */
	                     hidden,                /* Hidden                (default : false)    */
	                     disconnectBehavior,    /* Disconnect Behavior   (default : kNothing) */
	                     affectsAppearance      /* Affect Appearance     (default : false)    */
	);
	*outStat = MS::kSuccess;
	return attrObj;
}


///-------------------------------------------------------------------------- CreateNumericAttribute
inline MObject CreateNumericAttribute(MFnNumericAttribute&                    nAttr,
                                      const MString                           longName,
                                      const MString                           shortName,
                                      const MFnNumericData::Type              dataType,
                                      const double                            defaultValue,
                                      const bool                              readable,
                                      const bool                              writable,
                                      const bool                              connectable,
                                      const bool                              storable,
                                      const bool                              cached,
                                      const bool                              channelBox,
                                      const bool                              array,
                                      const bool                              usesArrayDataBuilder,
                                      const bool                              keyable,
                                      const bool                              hidden,
                                      const MFnAttribute::DisconnectBehavior  disconnectBehavior,
                                      const bool                              affectsAppearance,
                                      MStatus*                                outStat
)
{
	MStatus stat;
	MObject attrObj;
	
	attrObj = nAttr.create(longName,
	                       shortName,
	                       dataType,
	                       defaultValue,
	                       &stat
	);
	if(stat != MS::kSuccess)
	{
		*outStat = MS::kFailure;
		return MObject();
	}
	SET_ATTRIBUTE_STATUS(nAttr,
	                     readable,              /* Readable              (default : true)     */
	                     writable,              /* Writable              (default : true)     */
	                     connectable,           /* Connectable           (default : true)     */
	                     storable,              /* Stroable              (default : true)     */
	                     cached,                /* Cached                (default : true)     */
	                     channelBox,            /* ChannelBox            (default : false)    */
	                     array,                 /* Array                 (default : false)    */
	                     usesArrayDataBuilder,  /* Use Array DataBuilder (default : false)    */
	                     keyable,               /* Keyable               (default : false)    */
	                     hidden,                /* Hidden                (default : false)    */
	                     disconnectBehavior,    /* Disconnect Behavior   (default : kNothing) */
	                     affectsAppearance      /* Affect Appearance     (default : false)    */
	);
	*outStat = MS::kSuccess;
	return attrObj;
}




///-------------------------------------------------------------------------- Query
inline MString GetGeometryDrawDbClassification(const MString& name)
{
	return MString("drawdb/geometry/" + name);
}
inline MString GetDrawOverrideRegistrantId(const MString& name)
{
	return MString(name + "DrawOverride");
}

inline MayaValueType GetPlugValueType(const MPlug& attrPlug)
{
	MObject attrObj(attrPlug.attribute());
	if (attrObj.isNull())
	{
		STDOUT_ERROR("Failed to get valid MPlug");
		return MayaValueType::kInvalid;
	}
	MFnData::Type            typedDataType   = MFnData::kInvalid;
	MFnUnitAttribute::Type   unitDataType    = MFnUnitAttribute::kInvalid;
	MFnNumericData::Type     numericDataType = MFnNumericData::kInvalid;
	
	if(attrObj.hasFn(MFn::kNumericAttribute))
	{
		MFnNumericAttribute numericAttrFn(attrObj);
		numericDataType = numericAttrFn.unitType();
	}
	else if(attrObj.hasFn(MFn::kTypedAttribute))
	{
		MFnTypedAttribute typedAttrFn(attrObj);
		typedDataType = typedAttrFn.attrType();
		
		if(typedDataType == MFnData::kNumeric)
		{
			// Inspect the type of the data itself to find the actual type.
			MObject plugObj = attrPlug.asMObject();
			if(plugObj.hasFn(MFn::kNumericData))
			{
				MFnNumericData numericDataFn(plugObj);
				numericDataType = numericDataFn.numericType();
			}
		}
	}
	else if(attrObj.hasFn(MFn::kUnitAttribute))
	{
		MFnUnitAttribute unitAttrFn(attrObj);
		unitDataType = unitAttrFn.unitType();
	}
	
	if(attrObj.hasFn(MFn::kEnumAttribute))
		return MayaValueType::kShort;
	
	if(attrObj.hasFn(MFn::kMatrixAttribute))
		return MayaValueType::kMatrix;
	
	switch (typedDataType)
	{
		case MFnData::kString:          return MayaValueType::kString;
		case MFnData::kMatrix:          return MayaValueType::kMatrix;
		case MFnData::kStringArray:     return MayaValueType::kStringArray;
		case MFnData::kDoubleArray:     return MayaValueType::kDoubleArray;
		case MFnData::kFloatArray:      return MayaValueType::kFloatArray;
		case MFnData::kIntArray:        return MayaValueType::kIntArray;
		case MFnData::kPointArray:      return MayaValueType::kPointArray;
		case MFnData::kVectorArray:     return MayaValueType::kVectorArray;
	}
	switch (numericDataType)
	{
		case MFnNumericData::kBoolean:  return MayaValueType::kBoolean;
		case MFnNumericData::kByte:     return MayaValueType::kByte;
		case MFnNumericData::kChar:     return MayaValueType::kChar;
		case MFnNumericData::kShort:    return MayaValueType::kShort;
		case MFnNumericData::kInt:      return MayaValueType::kInt;
		case MFnNumericData::k2Short:   return MayaValueType::k2Short;
		case MFnNumericData::k2Int:     return MayaValueType::k2Int;
		case MFnNumericData::k3Short:   return MayaValueType::k3Short;
		case MFnNumericData::k3Int:     return MayaValueType::k3Int;
		case MFnNumericData::kFloat:    return MayaValueType::kFloat;
		case MFnNumericData::k2Float:   return MayaValueType::k2Float;
		case MFnNumericData::k3Float:   return MayaValueType::k3Float;
		case MFnNumericData::kDouble:   return MayaValueType::kDouble;
		case MFnNumericData::k2Double:  return MayaValueType::k2Double;
		case MFnNumericData::k3Double:  return MayaValueType::k3Double;
		case MFnNumericData::k4Double:  return MayaValueType::k4Double;
	}
	
	switch (unitDataType)
	{
		case MFnUnitAttribute::kAngle:      return MayaValueType::kAngle;
		case MFnUnitAttribute::kDistance:   return MayaValueType::kDistance;
	}
	STDOUT_ERROR("Failed to get valid MayaValueType");
	return MayaValueType::kInvalid;
}




///-------------------------------------------------------------------------- Query
//inline bool IsAnimated(const MObject& object, bool checkParent = false)
//{
//	MStatus            stat;
//	MItDependencyGraph iter(object, MFn::kInvalid,
//	                        MItDependencyGraph::kUpstream,
//	                        MItDependencyGraph::kDepthFirst,
//	                        MItDependencyGraph::kNodeLevel,
//	                        &stat);
//
//	if(stat != MS::kSuccess)
//	{
//		MGlobal::displayError("Unable to create DG iterator ");
//	}
//
//	// MAnimUtil::isAnimated(node) will search the history of the node
//	// for any animation curve nodes. It will return true for those nodes
//	// that have animation curve in their history.
//	// The average time complexity is O(n^2) where n is the number of history
//	// nodes. But we can improve the best case by split the loop into two.
//	std::vector<MObject> nodesToCheckAnimCurve;
//
//	for (; !iter.isDone(); iter.next())
//	{
//		MObject node = iter.thisNode();
//
//		if(node.hasFn(MFn::kPluginDependNode) ||
//		   node.hasFn(MFn::kConstraint) ||
//		   node.hasFn(MFn::kPointConstraint) ||
//		   node.hasFn(MFn::kAimConstraint) ||
//		   node.hasFn(MFn::kOrientConstraint) ||
//		   node.hasFn(MFn::kScaleConstraint) ||
//		   node.hasFn(MFn::kGeometryConstraint) ||
//		   node.hasFn(MFn::kNormalConstraint) ||
//		   node.hasFn(MFn::kTangentConstraint) ||
//		   node.hasFn(MFn::kParentConstraint) ||
//		   node.hasFn(MFn::kPoleVectorConstraint) ||
//		   node.hasFn(MFn::kParentConstraint) ||
//		   node.hasFn(MFn::kTime) ||
//		   node.hasFn(MFn::kJoint) ||
//		   node.hasFn(MFn::kGeometryFilt) ||
//		   node.hasFn(MFn::kTweak) ||
//		   node.hasFn(MFn::kPolyTweak) ||
//		   node.hasFn(MFn::kSubdTweak) ||
//		   node.hasFn(MFn::kCluster) ||
//		   node.hasFn(MFn::kFluid) ||
//		   node.hasFn(MFn::kPolyBoolOp))
//		{
//			return true;
//		}
//
//		if(node.hasFn(MFn::kExpression))
//		{
//			MFnExpression fn(node, &stat);
//			if(stat == MS::kSuccess && fn.isAnimated())
//			{
//				return true;
//			}
//		}
//
//		nodesToCheckAnimCurve.push_back(node);
//	}
//
//	for (size_t i = 0; i < nodesToCheckAnimCurve.size(); i++)
//	{
//		if(IsAnimated(nodesToCheckAnimCurve[i], checkParent))
//		{
//			return true;
//		}
//	}
//	return false;
//}
inline bool IsIntermediate(const MObject& object)
{
	MStatus    stat;
	MFnDagNode mFn(object);
	
	MPlug plug = mFn.findPlug("intermediateObject", false, &stat);
	if(stat == MS::kSuccess && plug.asBool())
		return true;
	else
		return false;
}
inline bool IsRenderable(const MObject& object)
{
	MStatus    stat;
	MFnDagNode mFn(object);
	
	// templated turned on?  return false
	MPlug plug = mFn.findPlug("template", false, &stat);
	if(stat == MS::kSuccess && plug.asBool())
		return false;
	
	// visibility or lodVisibility off?  return false
	plug = mFn.findPlug("visibility", false, &stat);
	if(stat == MS::kSuccess && !plug.asBool())
	{
		// the value is off. let's check if it has any in-connection,
		// otherwise, it means it is not animated.
		MPlugArray arrayIn;
		plug.connectedTo(arrayIn, true, false, &stat);
		
		if(stat == MS::kSuccess && arrayIn.length() == 0)
		{
			return false;
		}
	}
	
	plug = mFn.findPlug("lodVisibility", false, &stat);
	if(stat == MS::kSuccess && !plug.asBool())
	{
		MPlugArray arrayIn;
		plug.connectedTo(arrayIn, true, false, &stat);
		
		if(stat == MS::kSuccess && arrayIn.length() == 0)
		{
			return false;
		}
	}
	
	// this shape is renderable
	return true;
}
inline bool IsBatchMode()
{
	if(MGlobal::mayaState() == MGlobal::kBatch)
		return true;
	return false;
}
inline bool IsInteractiveMode()
{
	if(MGlobal::mayaState() == MGlobal::kInteractive || MGlobal::mayaState() == MGlobal::kBaseUIMode)
		return true;
	return false;
}
inline bool IsLibraryAppMode()
{
	if(MGlobal::mayaState() == MGlobal::kLibraryApp)
		return true;
	return false;
}
inline bool IsOutputConnected(const MObject& obj)
{
	MStatus stat;
	
	MFnDependencyNode depFn(obj, &stat);
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get depFn");
		return false;
	};
	
	MPlug outputPlug(obj, depFn.attribute("output", &stat));
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get output attribute");
		return false;
	};
	
	bool    isConnected = outputPlug.isConnected(&stat);
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get outputPlug.isConnected().");
		return false;
	};
	
	return isConnected;
}
inline bool IsNodeVisible(const MObject& obj /* DAG node obj*/)
{
	MStatus    stat;
	MFnDagNode dagNodeFn(obj, &stat);
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get dagNodeFn");
		return false;
	};
	
	MPlug visibilityPlug(obj, dagNodeFn.attribute("visibility", &stat));
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get visibility attribute");
		return false;
	};
	
	bool       visible = visibilityPlug.asBool();
	
	MPlug intermediateObjectPlug(obj, dagNodeFn.attribute("intermediateObject", &stat));
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get intermediateObject attribute");
		return false;
	};
	visible = visible && !(intermediateObjectPlug.asBool());
	
	MPlug overrideVisibilityPlug(obj, dagNodeFn.attribute("overrideVisibility", &stat));
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get overrideVisibility attribute");
		return false;
	};
	visible = visible && overrideVisibilityPlug.asBool();
	
	if(visible)
	{
		if(dagNodeFn.parentCount(&stat) > 0)
		{
			MObject parentObj = dagNodeFn.parent(0, &stat);
			if(stat != MS::kSuccess)
			{
				STDOUT_ERROR("Failed to get parentObj");
				return false;
			};
			visible = visible && IsNodeVisible(parentObj);
		}
	}
	return visible;
}
inline bool IsNodeDrawable(const MObject& obj /*DAG node obj*/)
{
	MStatus    stat;
	MFnDagNode dagNodeFn(obj, &stat);
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get dagNodeFn");
		return false;
	};
	
	MPlug displayModePlug(obj, dagNodeFn.attribute("displayMode", &stat));
	if(displayModePlug.asShort() == mode::display::none)
		return false;
	return true;
}
inline bool IsLastEndNodeVisible(const MObject& obj /*intermediate DG obj*/)
{
	MStatus           stat;
	MFnDependencyNode depFn(obj, &stat);
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get depFn");
		return false;
	};
	
	MPlug outputPlug(obj, depFn.attribute("output", &stat));
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get output attribute");
		return false;
	};
	
	MPlugArray outputPlugArray;
	outputPlug.destinations(outputPlugArray);
	
	bool              isVisible = false;
	for (int   i                = 0; i < outputPlugArray.length(); ++i)
	{
		MObject lastEndObj = outputPlugArray[i].node(&stat);
		isVisible = IsNodeVisible(lastEndObj);
		if(isVisible)
		{
			return isVisible;
		}
	}
	return isVisible;
}
inline bool IsLastEndNodeDrawable(const MObject& obj /*intermediate DG obj*/)
{
	MStatus           stat;
	MFnDependencyNode depFn(obj, &stat);
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get depFn");
		return false;
	};
	
	MPlug outputPlug(obj, depFn.attribute("output", &stat));
	if(stat != MS::kSuccess)
	{
		STDOUT_ERROR("Failed to get output attribute");
		return false;
	};
	
	MPlugArray outputPlugArray;
	outputPlug.destinations(outputPlugArray);
	
	bool              isDrawable = false;
	for (int   i                 = 0; i < outputPlugArray.length(); ++i)
	{
		MObject lastEndObj = outputPlugArray[i].node(&stat);
		isDrawable = IsNodeDrawable(lastEndObj);
		if(isDrawable)
		{
			return isDrawable;
		}
	}
	return isDrawable;
}

///-------------------------------------------------------------------------- Timer
inline Time GetTime(const MTime& inTime)
{
	MTime::Unit unit = inTime.unit();
	
	MTime  tmp(1.0, MTime::kSeconds);
	double fps       = (tmp.as(unit));
	double sec       = inTime.as(MTime::kSeconds);
	
	return Time(sec, fps);
}

///-------------------------------------------------------------------------- MPlug
inline size_t GetCppType(const MPlug& attrPlug)
{
	size_t type = cppType::kInvalid;
	
	MObject attrObj(attrPlug.attribute());
	if (attrObj.isNull())
		return cppType::kInvalid;
	
	
	MFnData::Type            typedDataType   = MFnData::kInvalid;
	MFnUnitAttribute::Type   unitDataType    = MFnUnitAttribute::kInvalid;
	MFnNumericData::Type     numericDataType = MFnNumericData::kInvalid;
	
	if(attrObj.hasFn(MFn::kNumericAttribute))
	{
		MFnNumericAttribute numericAttrFn(attrObj);
		numericDataType = numericAttrFn.unitType();
	}
	else if(attrObj.hasFn(MFn::kTypedAttribute))
	{
		MFnTypedAttribute typedAttrFn(attrObj);
		typedDataType = typedAttrFn.attrType();
		
		if(typedDataType == MFnData::kNumeric)
		{
			// Inspect the type of the data itself to find the actual type.
			MObject plugObj = attrPlug.asMObject();
			if(plugObj.hasFn(MFn::kNumericData))
			{
				MFnNumericData numericDataFn(plugObj);
				numericDataType = numericDataFn.numericType();
			}
		}
	}
	else if(attrObj.hasFn(MFn::kUnitAttribute))
	{
		MFnUnitAttribute unitAttrFn(attrObj);
		unitDataType = unitAttrFn.unitType();
	}
	
	if(attrObj.hasFn(MFn::kEnumAttribute))
		return cppType::kShort;
	
	if(attrObj.hasFn(MFn::kMatrixAttribute))
		return cppType::kMat4x4d;
	
	switch (typedDataType)
	{
		case MFnData::kString:      return cppType::kString;
		case MFnData::kMatrix:      return cppType::kMat4x4d;
		case MFnData::kStringArray: return cppType::kStringArray;
		case MFnData::kDoubleArray: return cppType::kDoubleArray;
		case MFnData::kFloatArray:  return cppType::kFloatArray;
		case MFnData::kIntArray:    return cppType::kInt32Array;
		case MFnData::kPointArray:  return cppType::kPoint3dArray;
		case MFnData::kVectorArray: return cppType::kVec3dArray;
	}
	switch (numericDataType)
	{
		case MFnNumericData::kBoolean:  return cppType::kBool;
		
		case MFnNumericData::kByte:
		case MFnNumericData::kChar:
		case MFnNumericData::kShort:
		case MFnNumericData::kInt:      return cppType::kInt;
		
		case MFnNumericData::k2Short:
		case MFnNumericData::k2Int:     return cppType::kVec2i;
		
		case MFnNumericData::k3Short:
		case MFnNumericData::k3Int:     return cppType::kVec3i;
		
		case MFnNumericData::kFloat:    return cppType::kFloat;
		case MFnNumericData::k2Float:   return cppType::kVec2f;
		case MFnNumericData::k3Float:   return cppType::kVec3f;
		
		case MFnNumericData::kDouble:   return cppType::kDouble;
		case MFnNumericData::k2Double:  return cppType::kVec2d;
		case MFnNumericData::k3Double:  return cppType::kVec3d;
		case MFnNumericData::k4Double:  return cppType::kVec4d;
		default:
			break;
	}
	
	switch (unitDataType)
	{
		case MFnUnitAttribute::kAngle:
		case MFnUnitAttribute::kDistance:   return cppType::kDouble;
		default:
			break;
	}
	return type;
}


inline MPlug GetConnected(const MPlug& plug)
{
	MStatus    status = MS::kFailure;
	MPlugArray conn;
	plug.connectedTo(conn, true, false, &status);
	if(!status || conn.length() != 1)
	{
		return MPlug();
	}
	return conn[0];
}
inline bool IsPlugAnimated(const MPlug& plug)
{
	if(plug.isNull())
	{
		return false;
	}
	if(plug.isKeyable() && MAnimUtil::isAnimated(plug))
	{
		return true;
	}
	if(plug.isDestination())
	{
		const MPlug source(GetConnected(plug));
		if(!source.isNull() && MAnimUtil::isAnimated(source.node()))
		{
			return true;
		}
	}
	return false;
}
template<typename T>
inline const T _GetPlugValue(const MObject obj, const MString attrname, bool* isAnimated, MStatus* pStat)
{
	MStatus           stat;
	MFnDependencyNode depNode(obj, &stat);
	if(!stat)
	{
		STDOUT_ERROR("Failed to create MFnDependencyNode");
		if(pStat) *pStat = MS::kFailure;
		return T();
	}
	
	MPlug plg = depNode.findPlug(attrname, /* findNetworked = */ true);
	if(plg.isNull())
	{
		STDOUT_ERROR("Failed to find %s attribute", attrname.asChar());
		if(pStat) *pStat = MS::kFailure;
		return T();
	}
	
	if(isAnimated)
		*isAnimated = IsPlugAnimated(plg);
	
	T val;
	if(!plg.getValue(val))
	{
		STDOUT_ERROR("Failed to get %s attribute value", attrname.asChar());
		if(pStat) *pStat = MS::kFailure;
		return T();
	}
	
	if(pStat) *pStat = MS::kSuccess;
	return val;
}
template<typename T>
inline const T GetPlugValue(const MObject obj, const MString attrname, MStatus* pStat = nullptr)
{
	return _GetPlugValue<T>(obj, attrname, nullptr, pStat);
}
template<typename T>
inline const T GetPlugValue(const MObject obj, const MString attrname, bool* isAnimated, MStatus* pStat = nullptr)
{
	return _GetPlugValue<T>(obj, attrname, isAnimated, pStat);
}


template<typename T>
inline void SetPlugValue(const MObject& obj, const MString& attrname, const T& val, MStatus* pStat = nullptr)
{
	MStatus           stat;
	MFnDependencyNode depFn(obj, &stat);
	if(!stat)
	{
		if(pStat) *pStat = MS::kFailure;
		return;
	}
	
	//MPlug plg = depFn.findPlug(attrname, /* findNetworked = */ false );
	MPlug plg = depFn.findPlug(attrname);
	if(plg.isNull())
	{
		if(pStat) *pStat = MS::kFailure;
		return;
	}
	
	if(!plg.setValue(val))
	{
		if(pStat) *pStat = MS::kFailure;
		return;
	}
}

inline bool GetLogFlag(const MObject& obj, MStatus* statPtr)
{
	MFnDependencyNode dependencyNode(obj, statPtr);
	if(*statPtr != MS::kSuccess)
	{
		return false;
	}
	MObject attributeObj = dependencyNode.attribute("log", statPtr);
	if(*statPtr != MS::kSuccess)
	{
		return false;
	}
	MPlug logPlug(obj, attributeObj);
	if(logPlug.isNull())
	{
		return false;
	}
	return logPlug.asBool();
}
inline MDagPath GetDagPath(const MObject& obj, MStatus* outStat = nullptr)
{
	MStatus    stat;
	MFnDagNode dagNodeFn(obj, &stat);
	if(!stat)
	{
		if(outStat) *outStat = stat;
		return MDagPath();
	}
	MDagPath dagPath;
	stat = dagNodeFn.getPath(dagPath);
	if(!stat)
	{
		if(outStat) *outStat = stat;
		return MDagPath();
	}
	
	bool       valid     = dagPath.isValid(&stat);
	if(!stat)
	{
		if(outStat) *outStat = stat;
		return MDagPath();
	}
	
	if(!valid)
	{
		if(outStat) *outStat = stat;
		return MDagPath();
	}
	if(outStat) *outStat = stat;
	return dagPath;
}
inline MString GetPartialPathName(const MObject& obj, MStatus* outStat = nullptr)
{
	MStatus    stat;
	MFnDagNode dagNodeFn(obj, &stat);
	if(!stat)
	{
		if(outStat) *outStat = stat;
		return "invalid object";
	}
	MString dagPath      = dagNodeFn.partialPathName(&stat);
	if(!stat)
	{
		if(outStat) *outStat = stat;
		return "invalid object";
	}
	if(outStat) *outStat = stat;
	return dagPath.asChar();
	
}
inline MString GetAbsoluteName(const MObject& obj, MStatus* outStat = nullptr)
{
	MStatus           stat;
	MFnDependencyNode dependencyNode(obj, &stat);
	if(!stat)
	{
		if(outStat) *outStat = stat;
		return "invalid object";
	}
	MString absName      = dependencyNode.absoluteName(&stat);
	if(!stat)
	{
		if(outStat) *outStat = stat;
		return "invalid object";
	}
	if(outStat) *outStat = stat;
	return absName;
}

inline double GetFPS(const MTime::Unit unit)
{
	double fps = 24.0;
	switch (unit)
	{
		case MTime::Unit::k15FPS:
			fps = 15.0;
			break;
		case MTime::Unit::k24FPS:
			fps = 24.0;
			break;
		case MTime::Unit::k25FPS:
			fps = 25.0;
			break;
		case MTime::Unit::k30FPS:
			fps = 30.0;
			break;
		case MTime::Unit::k48FPS:
			fps = 48.0;
			break;
		case MTime::Unit::k50FPS:
			fps = 50.0;
			break;
		case MTime::Unit::k60FPS:
			fps = 60.0;
			break;
		default:
			fps = 24.0;
			break;
	}
	return fps;
}
inline std::string GetFPS(const double unit)
{
	if(IsEqual<double>(unit, GetFPS(MTime::Unit::k15FPS))) return "k15FPS";
	if(IsEqual<double>(unit, GetFPS(MTime::Unit::k24FPS))) return "k24FPS";
	if(IsEqual<double>(unit, GetFPS(MTime::Unit::k25FPS))) return "k25FPS";
	if(IsEqual<double>(unit, GetFPS(MTime::Unit::k30FPS))) return "k30FPS";
	if(IsEqual<double>(unit, GetFPS(MTime::Unit::k48FPS))) return "k48FPS";
	if(IsEqual<double>(unit, GetFPS(MTime::Unit::k50FPS))) return "k50FPS";
	if(IsEqual<double>(unit, GetFPS(MTime::Unit::k60FPS))) return "k60FPS";
	return std::to_string(unit);
}

inline MMatrix Convert(const Mat4x4f& mtx)
{
	MMatrix  ret;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			ret(j, i) = mtx(i, j);
		}
	}
	return ret;
}
inline Mat4x4f Convert(const MMatrix& mtx)
{
	Mat4x4f  ret;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			ret(j, i) = mtx(i, j);
		}
	}
	return ret;
}

inline MStatus GetMObjectByName(const MString& nodeName, MObject* obj)
{
	MStatus        stat;
	MSelectionList selectionList;
	
	stat = selectionList.add(nodeName);
	MCHECKERROR(stat, "Failed to add nodename to selectionList");
	
	stat = selectionList.getDependNode(0, *obj);
	MCHECKERROR(stat, "Failed to get MObject from selectionList");
	return stat;
}

inline MStatus GetDagPathByName(const MString& nodeName, MDagPath* dagPath)
{
	MStatus        stat;
	MSelectionList selectionList;
	
	stat = selectionList.add(nodeName);
	MCHECKERROR(stat, "Failed to add nodename to selectionList");
	
	stat = selectionList.getDagPath(0, *dagPath);
	MCHECKERROR(stat, "Failed to get MDagPath from selectionList");
	return stat;
}
inline MStatus GetMObjectByName(const std::string& nodeName, MObject* obj)
{
	MStatus stat;
	stat = GetMObjectByName(MString(nodeName.c_str()), obj);
	return stat;
}
inline MStatus GetDagPathByName(const std::string& nodeName, MDagPath* dagPath)
{
	MStatus stat;
	stat = GetDagPathByName(MString(nodeName.c_str()), dagPath);
	return stat;
}

inline MStatus GetMtx(const MObject& mtxObj, MMatrix* mtx)
{
	MStatus stat;
	
	MFnMatrixData matrixDataFn(mtxObj);
	*mtx = matrixDataFn.matrix(&stat);
	MCHECKERROR(stat, "Failed to matrixDataFn.matrix()");
	
	return stat;
}
inline MStatus GetXformMtx(const MObject& dagObj /*transform*/, MMatrix* xformMtx)
{
	MStatus stat;
	
	MFnDependencyNode depFn(dagObj);
	
	MPlug xformPlug(dagObj, depFn.attribute("m", &stat));
	MCHECKERROR(stat, "Failed to create xformPlug");
	
	MObject mtxObj;
	stat = xformPlug.getValue(mtxObj);
	MCHECKERROR(stat, "Failed to get mtxObj");
	
	stat = GetMtx(mtxObj, xformMtx);
	MCHECKERROR(stat, "Failed to get xformMtx");
	
	return stat;
}
inline MStatus GetWorldMtx(const MObject& dagObj, MMatrix* worldMtx)
{
	MStatus stat;
	
	MFnDependencyNode depFn(dagObj);
	
	MPlug wmPlug(dagObj, depFn.attribute("wm", &stat));
	MCHECKERROR(stat, "Failed to create wmPlug");
	wmPlug = wmPlug.elementByLogicalIndex(0, &stat);
	MCHECKERROR(stat, "Failed to get wmPlug[0]");
	
	MObject mtxObj;
	stat = wmPlug.getValue(mtxObj);
	MCHECKERROR(stat, "Failed to get mtxObj");
	
	stat = GetMtx(mtxObj, worldMtx);
	MCHECKERROR(stat, "Failed to get worldMtx");
	
	return stat;
}
inline MStatus GetParentMtx(const MObject& dagObj, MMatrix* parentMtx)
{
	MStatus stat;
	
	MFnDependencyNode depFn(dagObj);
	
	MPlug pmPlug(dagObj, depFn.attribute("pm", &stat));
	MCHECKERROR(stat, "Failed to create pmPlug");
	
	MObject mtxObj;
	stat = pmPlug.getValue(mtxObj);
	MCHECKERROR(stat, "Failed to get mtxObj");
	
	stat = GetMtx(mtxObj, parentMtx);
	MCHECKERROR(stat, "Failed to get parentMtx");
	
	return stat;
}

inline MStatus GetXformMtx(const MObject& dagObj /*transform*/, Mat4x4f* xformMtx)
{
	MStatus stat;
	MMatrix mat;
	stat = GetXformMtx(dagObj, &mat);
	MCHECKERROR(stat, "Failed to get xformMtx");
	*xformMtx = Convert(mat);
	return stat;
}
inline MStatus GetWorldMtx(const MObject& dagObj, Mat4x4f* worldMtx)
{
	MStatus stat;
	MMatrix mat;
	stat = GetWorldMtx(dagObj, &mat);
	MCHECKERROR(stat, "Failed to get worldMtx");
	*worldMtx = Convert(mat);
	return stat;
}
inline MStatus GetParentMtx(const MObject& dagObj, Mat4x4f* parentMtx)
{
	MStatus stat;
	MMatrix mat;
	stat = GetParentMtx(dagObj, &mat);
	MCHECKERROR(stat, "Failed to get parentMtx");
	*parentMtx = Convert(mat);
	return stat;
}

inline MStatus GetTransform(const MTransformationMatrix& mtx, double t[3], double r[3], double s[3], MTransformationMatrix::RotationOrder order = MTransformationMatrix::kXYZ, MSpace::Space space = MSpace::kTransform)
{
	MStatus stat;
	
	//get translation
	{
		MVector tmp = mtx.getTranslation(space, &stat);
		MCHECKERROR(stat, "Failed to mtx.getTranslation()");
		t[0] = tmp[0];
		t[1] = tmp[1];
		t[2] = tmp[2];
	}
	
	//get rotation
	{
		double tmp[3];
		stat = mtx.getRotation(tmp, order);
		MCHECKERROR(stat, "Failed to mtx.getRotation()");
		
		r[0] = tmp[0];
		r[1] = tmp[1];
		r[2] = tmp[2];
		
	}
	
	//get scale
	{
		double tmp[3];
		stat = mtx.getScale(tmp, space);
		MCHECKERROR(stat, "Failed to mtx.getScale()");
		s[0] = tmp[0];
		s[1] = tmp[1];
		s[2] = tmp[2];
		
	}
	return stat;
}
inline MStatus GetTranslation(const MTransformationMatrix& mtx, double t[3], MSpace::Space space = MSpace::kTransform)
{
	MStatus stat;
	
	MVector tmp = mtx.getTranslation(space, &stat);
	MCHECKERROR(stat, "Failed to mtx.getTranslation()")
	
	t[0] = tmp[0];
	t[1] = tmp[1];
	t[2] = tmp[2];
	return stat;
}
inline MStatus GetRotation(const MTransformationMatrix& mtx, double r[3], MTransformationMatrix::RotationOrder order = MTransformationMatrix::kXYZ)
{
	MStatus stat;
	
	double tmp[3];
	stat = mtx.getRotation(tmp, order);
	MCHECKERROR(stat, "Failed to mtx.getRotation()")
	
	r[0] = tmp[0];
	r[1] = tmp[1];
	r[2] = tmp[2];
	
	return stat;
}
inline MStatus GetScale(const MTransformationMatrix& mtx, double s[3], MSpace::Space space = MSpace::kTransform)
{
	MStatus stat;
	double  tmp[3];
	stat = mtx.getScale(tmp, space);
	MCHECKERROR(stat, "Failed to mtx.getScale()")
	s[0] = tmp[0];
	s[1] = tmp[1];
	s[2] = tmp[2];
	
	
	return stat;
}

///--------------------------------------------------------------- DAG Node Creator
inline MStatus SetNodeName(const MObject& obj, const MString& name, const bool& createNameSpace = false)
{
	MStatus           stat;
	MFnDependencyNode dgFn(obj, &stat);
	if(!stat)
		return stat;
	
	dgFn.setName(name, createNameSpace, &stat);
	return stat;
}
inline MObject CreateDAGNode(const MString& nodeType,
                             const MObject& parentObj,
                             const MString& name,
                             const bool& createNameSpace,
                             MStatus* stat)
{
	MDagModifier dagMod;
	MObject obj = dagMod.createNode(nodeType, parentObj, stat);
	*stat = dagMod.doIt();
	if(*stat == MS::kSuccess)
	{
		if(name != "")
		{
			*stat = SetNodeName(obj, MString(name) + MString("#"), createNameSpace);
		}
	}
	return obj;
}
inline MObject CreateDAGNode(const MTypeId& typeId,
                             const MObject& parentObj,
                             const MString& name,
                             const bool& createNameSpace,
                             MStatus* stat)
{
	MDagModifier dagMod;
	MObject obj = dagMod.createNode(typeId, parentObj, stat);
	*stat = dagMod.doIt();
	if(*stat == MS::kSuccess)
	{
		if(name != "")
		{
			*stat = SetNodeName(obj, MString(name) + MString("#"), createNameSpace);
		}
	}
	return obj;
}

inline MObject CreateDGNode(const MString& nodeType,
                            const MString& name         = "",
                            const bool& createNameSpace = false)
{
	MStatus stat;
	MDGModifier dgMod;
	MObject obj = dgMod.createNode(nodeType, &stat);
	
	if(!stat)
		return MObject();
	stat = dgMod.doIt();
	
	if(!stat)
		return MObject();
	
	if (name != "")
	{
		stat = SetNodeName(obj, MString(name) + MString("#"), createNameSpace);
		if(!stat)
			return MObject();
		
	}
	return obj;
}
inline MObject CreateDGNode(const MTypeId& typeId,
                            const MString& name         = "",
                            const bool& createNameSpace = false)
{
	MStatus stat;
	MDGModifier dgMod;
	MObject obj = dgMod.createNode(typeId, &stat);
	
	if(!stat)
		return MObject();
	stat = dgMod.doIt();
	
	if(!stat)
		return MObject();

	if (name != "")
	{
		stat = SetNodeName(obj, MString(name) + MString("#"), createNameSpace);
		if(!stat)
			return MObject();
		
	}
	return obj;
}


//inline MStatus CreateDAGNode(const MTypeId& typeId, const MObject& parentObj, const MString& name, const bool& createNameSpace, MObject* outObj)
//{
//	MStatus      stat;
//	MDagModifier dagMod;
//
//	*outObj = dagMod.createNode(typeId, parentObj, &stat);
//	stat = dagMod.doIt();
//	if(stat)
//	{
//		if(name != "")
//			stat = SetNodeName(*outObj, name, createNameSpace);
//	}
//	return stat;
//}
//inline MStatus CreateDAGNode(const MTypeId& typeId, const MObject& parentObj, const MString& name, MObject* outObj)
//{
//	return CreateDAGNode(typeId, parentObj, name, false, outObj);
//}
//inline MStatus CreateDAGNode(const MTypeId& typeId, const MObject& parentObj, MObject* outObj)
//{
//	return CreateDAGNode(typeId, parentObj, "", false, outObj);
//}
//inline MStatus CreateDAGNode(const MTypeId& typeId, const MString& name, const bool& createNameSpace, MObject* outObj)
//{
//	MStatus      stat;
//	MDagModifier dagMod;
//	*outObj = dagMod.createNode(typeId, MObject::kNullObj, &stat);
//	stat = dagMod.doIt();
//	if(stat)
//	{
//		if(name != "")
//			stat = SetNodeName(*outObj, name, createNameSpace);
//	}
//	return stat;
//}
//inline MStatus CreateDAGNode(const MTypeId& typeId, const MString& name, MObject* outObj)
//{
//	return CreateDAGNode(typeId, name, false, outObj);
//}
//inline MStatus CreateDAGNode(const MTypeId& typeId, MObject* outObj)
//{
//	return CreateDAGNode(typeId, "", false, outObj);
//}



/////--------------------------------------------------------------- DG Node Creator
//inline MStatus CreateDGNode(const MTypeId& typeId, const MString& name, const bool& createNameSpace, MObject* outObj)
//{
//	MStatus     stat;
//	MDGModifier dgMod;
//
//	*outObj = dgMod.createNode(typeId, &stat);
//	stat = dgMod.doIt();
//	if(stat)
//	{
//		if(name != "")
//			SetNodeName(*outObj, name, createNameSpace);
//	}
//	return stat;
//}
//inline MStatus CreateDGNode(const MTypeId& typeId, const MString& name, MObject* outObj)
//{
//	return CreateDGNode(typeId, name, false, outObj);
//}
//inline MStatus CreateDGNode(const MTypeId& typeId, MObject* outObj)
//{
//	return CreateDGNode(typeId, "", false, outObj);
//}


inline MStatus ConnectOutTime(const MObject& obj)
{
	MStatus     stat;
	MDGModifier dgMod;
	
	MFnDependencyNode depFn(obj, &stat);
	if(stat)
	{
		const std::string name = depFn.absoluteName().asChar();
		std::string       pyCmd;
		pyCmd = (boost::format("cmds.connectAttr('time1.outTime', '%s.inTime')") % name).str();
		dgMod.pythonCommandToExecute(pyCmd.c_str());
		stat = dgMod.doIt();
	}
	return stat;
}

inline MStatus SetVisibility(const MObject& obj, const bool visibility)
{
	MStatus           stat;
	MFnDependencyNode dgFn(obj, &stat);
	if(stat)
	{
		MPlug visibilityPlug(obj, dgFn.attribute("visibility"));
		stat = visibilityPlug.setValue(visibility);
	}
	return stat;
}

inline MStatus SetInitialShadingGroup(const MObject& obj)
{
	MStatus     stat;
	MDGModifier dgMod;
	
	MFnDependencyNode depFn(obj, &stat);
	if(stat)
	{
		const std::string name = depFn.absoluteName().asChar();
		std::string       pyCmd;
		pyCmd = (boost::format("cmds.sets('%s', e=True, fe='initialShadingGroup')") % name).str();
		dgMod.pythonCommandToExecute(pyCmd.c_str());
		stat = dgMod.doIt();
	}
	return (stat == MS::kSuccess ? MS::kSuccess : MS::kFailure);
}

inline MStatus SetRendermanVisibility(const MObject& obj, const bool visibilityCamera = true, const bool visibilityIndirect = true, const bool visibilityTransmission = true)
{
	MStatus           stat;
	MDGModifier       dgMod;
	MFnDependencyNode depFn(obj, &stat);
	if(stat)
	{
		const std::string name = depFn.absoluteName().asChar();
		std::string       pyCmd;
		pyCmd = (boost::format("cmds.addAttr('%s', ln='rman__riattr__visibility_camera',       dv=%d);") % name % static_cast<int>(visibilityCamera)).str();
		pyCmd += (boost::format("cmds.addAttr('%s', ln='rman__riattr__visibility_indirect',     dv=%d);") % name % static_cast<int>(visibilityIndirect)).str();
		pyCmd += (boost::format("cmds.addAttr('%s', ln='rman__riattr__visibility_transmission', dv=%d);") % name % static_cast<int>(visibilityTransmission)).str();
		dgMod.pythonCommandToExecute(pyCmd.c_str());
		stat = dgMod.doIt();
	}
	return (stat == MS::kSuccess ? MS::kSuccess : MS::kFailure);
}
inline MStatus SetRendermanPreShapeScript(const MObject obj, const MString& cmd)
{
	MStatus           stat;
	MDGModifier       dgMod;
	MFnDependencyNode depFn(obj, &stat);
	if(stat)
	{
		const std::string name = depFn.absoluteName().asChar();
		std::string       pyCmd;
		pyCmd                  = (boost::format("cmds.addAttr('%s', ln='rman__torattr___preShapeScript', dt='string');") % name).str();
		pyCmd += (boost::format("cmds.setAttr('%s.rman__torattr___preShapeScript', 'python(\"%s\")', type='string')") % name % cmd.asChar()).str();
		dgMod.pythonCommandToExecute(pyCmd.c_str());
		stat = dgMod.doIt();
	}
	return (stat == MS::kSuccess ? MS::kSuccess : MS::kFailure);
	
}

inline MStatus ReparentNode(const MObject& obj, const MObject& newParentObj)
{
	MStatus      stat;
	MDagModifier dagMod;
	stat = dagMod.reparentNode(obj, newParentObj);
	stat = dagMod.doIt();
	return stat;
}

inline MStatus Connect(const MPlug& srcPlug, const MPlug& destPlug)
{
	MStatus stat;
	
	MDGModifier dgMod;
	stat = dgMod.connect(srcPlug, destPlug);
	if(!stat)
		return MS::kFailure;
	stat = dgMod.doIt();
	return stat;
}
inline MStatus Connect(const MObject srcObj, const MString srcAttr, const MObject destObj, const MString destAttr)
{
	MStatus           stat;
	MFnDependencyNode depFn;
	stat = depFn.setObject(srcObj);
	if(!stat)
		return stat;
	
	MPlug srcPlug(srcObj, depFn.attribute(srcAttr, &stat));
	if(!stat)
		return stat;
	
	stat = depFn.setObject(destObj);
	if(!stat)
		return stat;
	MPlug destPlug(destObj, depFn.attribute(destAttr, &stat));
	if(!stat)
		return stat;
	
	MDGModifier dgMod;
	stat = dgMod.connect(srcPlug, destPlug);
	stat = dgMod.doIt();
	
	return stat;
}

//inline MStatus CreateTaneMPxSurfaceShape(const MString& name, MObject* outTransformObj, MObject* outShapeObj = nullptr)
//{
//	MStatus stat;
//	/* create TaneTransformMPxNode */
//	MObject transformObj;
//	stat = CreateDAGNode(TN_TANE_MPXTRANSFORM_ID, &transformObj);
//	if(!stat) return MS::kFailure;
//
//	/* create TaneMPxSurfaceShape */
//	MObject shapeObj;
//	stat = CreateDAGNode(TN_TANE_MPXSURFACESHAPE_ID, transformObj, &shapeObj);
//	if(!stat) return MS::kFailure;
//
//	/* rename Tane */
//	stat = SetNodeName(transformObj, name);
//
//	/* connect time1.outTime to tane.inTime */
//	stat = ConnectOutTime(shapeObj);
//	if(!stat) return MS::kFailure;
//
//	/* set tane initialShadingGroup */
//	stat = SetInitialShadingGroup(shapeObj);
//	if(!stat) return MS::kFailure;
//
//	/* set renderman Visibility */
//	stat = SetRendermanVisibility(shapeObj);
//	if(!stat) return MS::kFailure;
//
//	/* add renderman procedural command */
//	stat = SetRendermanPreShapeScript(shapeObj, "TaneUtil.TaneMPxSurfaceShapeRenderCmd()");
//	if(!stat) return MS::kFailure;
//
//	*outTransformObj = transformObj;
//	if(outShapeObj)
//		*outShapeObj = shapeObj;
//	return MS::kSuccess;
//}

inline bool SkipDraw(const MHWRender::MDrawContext& context, const MUserData* pUserData)
{
	///-------------------------------------------------- Get renderer
	MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
	if (theRenderer == nullptr)
	{
		MGlobal::displayInfo("theRenderer is null, drawing skipped");
		return true;
	}
	///--------------------------------------------------  Just return and draw nothing, if drawApi isn't OpenGL
	if (!(theRenderer->drawAPI() == MHWRender::kOpenGL || theRenderer->drawAPI() == MHWRender::kOpenGLCoreProfile))
	{
		MGlobal::displayInfo("drawAPI is not openGL; drawing skipped");
		return true;
	}
	if(MGlobal::mayaState() == MGlobal::kBatch)
	{
		MGlobal::displayInfo("maya state is kBatch; drawing skipped");
		return true;
	}
	if(MGlobal::mayaState() == MGlobal::kLibraryApp)
	{
		MGlobal::displayInfo("maya state is kLibraryApp; drawing skipped");
		return true;
	}
	if (pUserData == nullptr)
	{
		return true;
	}
	
	return false;
}


NAMESPACE_BASE_END
