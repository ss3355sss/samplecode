#include "maya/taneMPxShapeGeometryOverride.h"

#include "base/hackedQueue.h"
#include "base/points.h"

#include "lib/baseMaya/util.h"

#include "maya/messageMPxData.h"
#include "maya/pointsMPxData.h"
#include "maya/sourceMPxData.h"
#include "maya/sourcesMPxData.h"

#include <thrust/system/omp/execution_policy.h>

NAMESPACE_BASE_BEGIN
///------------------------------------------------- UserData
struct UserData : public MUserData
{
public:
	MDAGDrawOverrideInfo        drawOverrideInfo;
	std::shared_ptr<Points>     pPoints;
	short                       colorMode;
	RGBColor                    color;                  //using solid color mode
	std::map<int, RGBColor>     colorMap;               //using source color mode
	float                       transparency;

public:
	UserData() : MUserData(true) /* let Maya clean up */
	{
	}
	virtual ~UserData()
	{
	}
	
};
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------- TaneMPxShapeComponentConverter
MHWRender::MPxComponentConverter* TaneMPxShapeComponentConverter::Creator()
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	return new TaneMPxShapeComponentConverter;
}
TaneMPxShapeComponentConverter::TaneMPxShapeComponentConverter()
	: MHWRender::MPxComponentConverter()
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
}
TaneMPxShapeComponentConverter::~TaneMPxShapeComponentConverter()
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
}
void TaneMPxShapeComponentConverter::initialize(const MHWRender::MRenderItem &renderItem)
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	fComponentObject = fComponent.create(MFn::kMeshVertComponent);
	
	UserData* selectionData = dynamic_cast<UserData*>(renderItem.customData());
	if(selectionData)
	{
		std::shared_ptr<Points> data    = selectionData->pPoints;
		int npoints = data->size();
		fVertices.resize(npoints);
		for (int idx = 0; idx < npoints; ++idx)
		{
			fVertices[idx] = idx;
		}
	}
}
void TaneMPxShapeComponentConverter::addIntersection(MHWRender::MIntersection &intersection)
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	// Convert the intersection index, which represent the primitive position in the index buffer, to the correct vertex component
	const int rawIdx = intersection.index();
	int       idx    = 0;
	if(rawIdx >= 0 && rawIdx < (int) fVertices.size())
	{
		idx = fVertices[rawIdx];
	}
	fComponent.addElement(idx);
}
MObject TaneMPxShapeComponentConverter::component()
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	return fComponentObject;
}
MSelectionMask TaneMPxShapeComponentConverter::selectionMask() const
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	MSelectionMask retVal;
	retVal.setMask(MSelectionMask::kSelectMeshVerts);
	retVal.addMask(MSelectionMask::kSelectPointsForGravity);
	return retVal;
}
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------- TaneMPxSurfaceShapeGeometryOverride
///----------------------------- MHWRender RenderItemNames
MString TaneMPxShapeGeometryOverride::DormantPointsRenderItem(RegisteredRenderItems->DormantPointsRenderItem);
MString TaneMPxShapeGeometryOverride::ActivePointsRenderItem(RegisteredRenderItems->ActivePointsRenderItem);
MString TaneMPxShapeGeometryOverride::DeletedPointsRenderItem(RegisteredRenderItems->DeletedPointsRenderItem);

///------------------------------------------------------------ static functions

MHWRender::MPxGeometryOverride* TaneMPxShapeGeometryOverride::Creator(const MObject &obj)
{
	return new TaneMPxShapeGeometryOverride(obj);
}

///------------------------------------------------------------ constructor / destructor
TaneMPxShapeGeometryOverride::TaneMPxShapeGeometryOverride(const MObject &obj)
	: MHWRender::MPxGeometryOverride(obj),
	  pShape_(nullptr),
	  pSourceMgr_(nullptr),
	  pPoints_(nullptr)
{
	MStatus stat;
	MFnDependencyNode node(obj, &stat);
	if(stat)
	{
		pShape_ = dynamic_cast<TaneMPxShape*>(node.userNode());
	}
}
TaneMPxShapeGeometryOverride::~TaneMPxShapeGeometryOverride()
{
	pShape_     = nullptr;
	pSourceMgr_ = nullptr;
	pPoints_    = nullptr;
}

///------------------------------------------------------------ virtual functions
MHWRender::DrawAPI TaneMPxShapeGeometryOverride::supportedDrawAPIs() const
{
	return (MHWRender::kOpenGL | MHWRender::kOpenGLCoreProfile);
}

void TaneMPxShapeGeometryOverride::updateDG()
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	
	if(!pShape_)
	{
		LOGGER_ERROR("Failed to get node");
		return;
	}
	
	///* get display mode */
	activeIDs_.clear();
	
	pSourceMgr_ = pShape_->outSources();
	pPoints_    = pShape_->outPoints();
	
	if(pShape_->hasActiveComponents())
	{
		MObjectArray activeComponents = pShape_->activeComponents();
		if(activeComponents.length())
		{
			MFnSingleIndexedComponent fnComponent(activeComponents[0]);
			if(fnComponent.elementCount())
			{
				if(fnComponent.componentType() == MFn::kMeshVertComponent)
				{
					fnComponent.getElements(activeIDs_);
				}
			}
		}
	}
}
void TaneMPxShapeGeometryOverride::updateRenderItems(const MDagPath &objPath, MHWRender::MRenderItemList &list)
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	
	if(!pShape_)
	{
		LOGGER_ERROR("Failed to get node");
		return;
	}
	if(!pSourceMgr_)
	{
		LOGGER_ERROR("Failed to get sources");
		return;
	}
	if(!pPoints_ || !(pPoints_->size() > 0))
	{
		LOGGER_ERROR("Failed to get valid points");
		return;
	}
	
	
	MHWRender::MRenderer* theRendererPtr = MHWRender::MRenderer::theRenderer();
	if(!theRendererPtr)
	{
		LOGGER_ERROR("Failed to get theRenderer");
		return;
	}
	const MHWRender::MShaderManager* shaderMgrPtr = theRendererPtr->getShaderManager();
	if(!shaderMgrPtr)
	{
		LOGGER_ERROR("Failed to get shaderManager");
		return;
	}
	///---------------------------dormant poin
	updateDormantPointsRenderItem(objPath, list, shaderMgrPtr);
	
	///---------------------------active point
	updateActivePointsRenderItem(objPath, list, shaderMgrPtr);
}
void TaneMPxShapeGeometryOverride::populateGeometry(const MHWRender::MGeometryRequirements &requirements, const MHWRender::MRenderItemList &renderItems, MHWRender::MGeometry &data)
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	
	if(!pShape_)
	{
		LOGGER_ERROR("Failed to get node");
		return;
	}
	if(!pSourceMgr_)
	{
		LOGGER_ERROR("Failed to get sources");
		return;
	}
	if(!pPoints_ || !(pPoints_->size() > 0))
	{
		LOGGER_ERROR("Failed to get points");
		return;
	}
	
	/////--------------------------------------------------------------------------------------------------------------------Update indexing data for all appropriate render items
	int numItems = renderItems.length();
	for (int i = 0; i < numItems; i++)
	{
		const MHWRender::MRenderItem* item = renderItems.itemAt(i);
		if(!item)
		{
			continue;
		}
		if(item->name() == DormantPointsRenderItem)
		{
			populateDormantPointsRenderItem_(requirements, item, data);
		}
		if(item->name() == ActivePointsRenderItem)
		{
			populateActivePointsRenderItem_(requirements, item, data);
		}
	}
}
void TaneMPxShapeGeometryOverride::cleanUp()
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	
	pPoints_    = nullptr;
	pSourceMgr_ = nullptr;
}
bool TaneMPxShapeGeometryOverride::hasUIDrawables() const
{
	return true;
}
void TaneMPxShapeGeometryOverride::addUIDrawables(const MDagPath &objPath, MHWRender::MUIDrawManager &drawManager, const MHWRender::MFrameContext &frameContext)
{
	DECL_LOGGER(false);
	if(!pPoints_)
	{
		LOGGER_ERROR("Failed to get pPoints");
		return;
	}

	MStatus stat;
	MFnDagNode shapeFn(objPath.node(&stat));

	MPlug   displayPointInfoPlug(objPath.node(&stat), shapeFn.attribute("displayPointInfo"));
	bool displayPointInfo = displayPointInfoPlug.asBool();
	
	MPlug localAxisScalePlug(objPath.node(&stat), shapeFn.attribute("localAxisScale"));
	float localAxisScale = localAxisScalePlug.asBool();

	MPlug displayLocalAxisPlug(objPath.node(&stat), shapeFn.attribute("displayLocalAxis"));
	bool displayLocalAxis = displayLocalAxisPlug.asBool();

	MMatrix wm;
	stat = GetWorldMtx(objPath.node(), &wm);
	

	//if(drawAll)
	//{
		drawManager.beginDrawable();
		{
			for(int elemIndex = 0; elemIndex < pPoints_->size(); ++elemIndex)
			{
				MPoint pos = MPoint((*pPoints_)[elemIndex][0], (*pPoints_)[elemIndex][1], (*pPoints_)[elemIndex][2]);
				if(displayPointInfo)
				{
					addPointInfoUIDrawables(elemIndex, pos * wm, drawManager, frameContext);
				}
				if(displayLocalAxis)
				{
					addLocalAxisUIDrawables(localAxisScale, elemIndex, pos, drawManager, frameContext);
				}
			}
		}
		drawManager.endDrawable();
		pShape_->signalDirtyToViewport();
	//}
	//else
	//{
	//	if(nodePtr_->hasActiveComponents())
	//	{
	//		drawManager.beginDrawable();
	//		{
	//			MObjectArray activeComponent = nodePtr_->activeComponents();
	//			unsigned int len             = activeComponent.length();
	//			for (int     i               = 0; i < len; ++i)
	//			{
	//				MObject                   comp      = activeComponent[i];
	//				MFnSingleIndexedComponent compFn(comp);
	//				int                       elemCount = compFn.elementCount();
	//
	//				float    sum[3] = {0.0f, 0.0f, 0.0f};
	//				for (int idx    = 0; idx < elemCount; idx++)
	//				{
	//					int elemIndex = compFn.element(idx);
	//
	//					MPoint pos = MPoint((*pPoints_)[elemIndex][0], (*pPoints_)[elemIndex][1], (*pPoints_)[elemIndex][2]);
	//					//pos = pos * wm;
	//
	//					if(displayPointInfo)
	//					{
	//						addPointInfoUIDrawables(elemIndex, pos * wm, drawManager, frameContext);
	//					}
	//					if(displayLocalAxis)
	//					{
	//						addLocalAxisUIDrawables(elemIndex, pos, drawManager, frameContext);
	//					}
	//				}
	//			}
	//		}
	//		drawManager.endDrawable();
	//		//nodePtr_->signalDirtyToViewport();
	//	}
	//}
}
void TaneMPxShapeGeometryOverride::updateSelectionGranularity(const MDagPath &path, MHWRender::MSelectionContext &selectionContext)
{
	MHWRender::DisplayStatus displayStatus = MHWRender::MGeometryUtilities::displayStatus(path);
	if(displayStatus == MHWRender::kHilite)
	{
		MSelectionMask globalComponentMask;
		if(MGlobal::selectionMode() == MGlobal::kSelectComponentMode)
		{
			globalComponentMask = MGlobal::componentSelectionMask();
		}
		else
		{
			globalComponentMask = MGlobal::objectSelectionMask();
		}
		
		MSelectionMask supportedComponents(MSelectionMask::kSelectMeshVerts);
		supportedComponents.addMask(MSelectionMask::kSelectPointsForGravity);
		
		if(globalComponentMask.intersects(supportedComponents))
		{
			selectionContext.setSelectionLevel(MHWRender::MSelectionContext::kComponent);
		}
	}
	else if(pointSnappingActive())
	{
		selectionContext.setSelectionLevel(MHWRender::MSelectionContext::kComponent);
	}
	
}
///------------------------------------------------------------ public helper functions
///------------------------------------------------------------ private helper functions
void TaneMPxShapeGeometryOverride::addPointInfoUIDrawables(const int elemIndex, const MPoint pos, MHWRender::MUIDrawManager &drawManager, const MHWRender::MFrameContext &frameContext)
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	//static MColor textColor(0.0f, 1.0f, 0.0f, 1.0f); // Text color
	//drawManager.setColor(textColor);
	//drawManager.setFontSize(MHWRender::MUIDrawManager::kDefaultFontSize);
	//
	//const int32_t&  pid 		= pPoints_->data<int32_t>(sementic::attribute::pid)[elemIndex];
	//const int32_t& 	sid 		= pPoints_->data<int32_t>(sementic::attribute::sid)[elemIndex];
	//
	//const int32_t& 	renderable 	= pPoints_->data<int32_t>(sementic::attribute::renderable)[elemIndex];
	//
	//const Point3f&	point 		= (*pPoints_)[elemIndex];
	//const Vec3f&	orientation = pPoints_->data<Vec3f>(sementic::attribute::eulerAngle)[elemIndex];
	//const Vec3f&	scale 	  	= pPoints_->data<Vec3f>(sementic::attribute::scale)[elemIndex];
	//
	//const int32_t& 	timeOffset 	= pPoints_->data<int32_t>(sementic::attribute::timeoffset)[elemIndex];
	//
	//const float& 	u 			= pPoints_->data<float>(sementic::attribute::uCoord)[elemIndex];
	//const float& 	v 			= pPoints_->data<float>(sementic::attribute::vCoord)[elemIndex];
	////const Radius &radius = pointsPtr_->data<Radius>(RADIUS_ATTRNAME)[elemIndex];
	//
	//
	//std::string msg;
	//double      x, y;
	//frameContext.worldToViewport(pos, x, y);
	//
	//int lineNum = 0;
	//
	////msg = (boost::format("radius = %5.2f") % radius).str();
	////drawManager.text2d(MPoint(x, y + lineNum++ * MHWRender::MUIDrawManager::kDefaultFontSize), msg.c_str(), MHWRender::MUIDrawManager::kCenter);
	////msg.clear();
	//
	//msg = (boost::format("uv = (%5.2f, %5.2f)") % u % v).str();
	//drawManager.text2d(MPoint(x, y + lineNum++ * MHWRender::MUIDrawManager::kDefaultFontSize), msg.c_str(), MHWRender::MUIDrawManager::kLeft);
	//msg.clear();
	//
	//msg = (boost::format("timeOffset = %d") % timeOffset).str();
	//drawManager.text2d(MPoint(x, y + lineNum++ * MHWRender::MUIDrawManager::kDefaultFontSize), msg.c_str(), MHWRender::MUIDrawManager::kLeft);
	//msg.clear();
	//
	//msg = (boost::format("scale = (%5.2f, %5.2f, %5.2f)") % scale[0] % scale[1] % scale[2]).str();
	//drawManager.text2d(MPoint(x, y + lineNum++ * MHWRender::MUIDrawManager::kDefaultFontSize), msg.c_str(), MHWRender::MUIDrawManager::kLeft);
	//msg.clear();
	//
	//msg = (boost::format("orientation = (%5.2f, %5.2f, %5.2f)") % orientation[0] % orientation[1] % orientation[2]).str();
	//drawManager.text2d(MPoint(x, y + lineNum++ * MHWRender::MUIDrawManager::kDefaultFontSize), msg.c_str(), MHWRender::MUIDrawManager::kLeft);
	//msg.clear();
	//
	//msg = (boost::format("position = (%5.2f, %5.2f, %5.2f)") % (*pPoints_)[elemIndex][0] % (*pPoints_)[elemIndex][1] % (*pPoints_)[elemIndex][2]).str();
	//drawManager.text2d(MPoint(x, y + lineNum++ * MHWRender::MUIDrawManager::kDefaultFontSize), msg.c_str(), MHWRender::MUIDrawManager::kLeft);
	//msg.clear();
	//
	//msg = (boost::format("renderable = %s") % (renderable ? "true" : "false")).str();
	//drawManager.text2d(MPoint(x, y + lineNum++ * MHWRender::MUIDrawManager::kDefaultFontSize), msg.c_str(), MHWRender::MUIDrawManager::kLeft);
	//msg.clear();
	//
	//msg = (boost::format("sid = %d") % sid).str();
	//drawManager.text2d(MPoint(x, y + lineNum++ * MHWRender::MUIDrawManager::kDefaultFontSize), msg.c_str(), MHWRender::MUIDrawManager::kLeft);
	//msg.clear();
	//
	//msg = (boost::format("pid = %d") % pid).str();
	//drawManager.text2d(MPoint(x, y + lineNum++ * MHWRender::MUIDrawManager::kDefaultFontSize), msg.c_str(), MHWRender::MUIDrawManager::kLeft);
	//msg.clear();
	
	
	
}
void TaneMPxShapeGeometryOverride::addLocalAxisUIDrawables(const double s, const int elemIndex, const MPoint pos, MHWRender::MUIDrawManager &drawManager, const MHWRender::MFrameContext &frameContext)

{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	static MColor xAxisColor(1.0f, 0.0f, 0.0f, 1.0f); /// x axis color
	static MColor yAxisColor(0.0f, 1.0f, 0.0f, 1.0f); /// y axis color
	static MColor zAxisColor(0.0f, 0.0f, 1.0f, 1.0f); /// z axis color
	
	drawManager.setLineWidth(3.0f);
	const int& renderable = pPoints_->data<int>(sementic::attribute::renderable)[elemIndex];
	const Mat3x3f& localAxis = pPoints_->data<Mat3x3f>(sementic::attribute::localAxis)[elemIndex];
	
	const Vec3f& localAxisX = localAxis.col(0);
	const Vec3f& localAxisY = localAxis.col(1);
	const Vec3f& localAxisZ = localAxis.col(2);
	
	
	drawManager.setColor(xAxisColor);
	drawManager.line(pos, pos + s * MPoint(localAxisX[0], localAxisX[1], localAxisX[2]));
	
	drawManager.setColor(yAxisColor);
	drawManager.line(pos, pos + s * MPoint(localAxisY[0], localAxisY[1], localAxisY[2]));
	
	drawManager.setColor(zAxisColor);
	drawManager.line(pos, pos + s * MPoint(localAxisZ[0], localAxisZ[1], localAxisZ[2]));
}

void TaneMPxShapeGeometryOverride::updateDormantPointsRenderItem(const MDagPath &objPath, MHWRender::MRenderItemList &list, const MHWRender::MShaderManager* shaderMgrPtr)
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	MStatus stat;
	
	MHWRender::MRenderItem* dormantPointsRenderItemPtr = nullptr;
	int index = list.indexOf(DormantPointsRenderItem);

	/* Create RenderItem */
	if(index < 0)
	{
		dormantPointsRenderItemPtr = MHWRender::MRenderItem::Create(DormantPointsRenderItem, MHWRender::MRenderItem::DecorationItem, MHWRender::MGeometry::kPoints);
		dormantPointsRenderItemPtr->setDrawMode(MHWRender::MGeometry::DrawMode::kAll);
		
		MSelectionMask vertexAndGravity(MSelectionMask::kSelectMeshVerts);
		vertexAndGravity.addMask(MSelectionMask::kSelectPointsForGravity);
		dormantPointsRenderItemPtr->setSelectionMask(vertexAndGravity);
		
		dormantPointsRenderItemPtr->depthPriority(MHWRender::MRenderItem::sDormantPointDepthPriority);
		
		list.append(dormantPointsRenderItemPtr);
		
		MHWRender::MShaderInstance* shaderPtr = nullptr;
		
		shaderPtr = shaderMgrPtr->getStockShader(MHWRender::MShaderManager::k3dCPVFatPointShader);
		if(shaderPtr)
		{
			// assign shader
			dormantPointsRenderItemPtr->setShader(shaderPtr);
			
			// once assigned, no need to hold on to shader instance
			shaderMgrPtr->releaseShader(shaderPtr);
		}
		
	}
	else
	{
		dormantPointsRenderItemPtr = list.itemAt(index);
	}
	
	/* Update RenderItem */
	if(dormantPointsRenderItemPtr)
	{
		UserData* userData = dynamic_cast<UserData*>(dormantPointsRenderItemPtr->customData());
		if(!userData)
		{
			// create the custom data
			userData = new UserData();
			dormantPointsRenderItemPtr->setCustomData(userData);
		}
		
		
		MFnDagNode shapeFn(objPath.node());
		/// update the custom data
		/* set points pointer for component converter  */
		{
			userData->pPoints = pPoints_;
		}
		/* set drawOverrideInfo  */
		{
			userData->drawOverrideInfo = objPath.getDrawOverrideInfo();
		}
		/* set dormant point size */
		{
			MHWRender::MShaderInstance* shaderInstancePtr = dormantPointsRenderItemPtr->getShader();
			MPlug         pointSizePlug(objPath.node(&stat), shapeFn.attribute("pointSize"));
			const float   pointSize                       = pointSizePlug.asInt();
			const float   pointSizeArray[2]               = {static_cast<float>(pointSize), static_cast<float>(pointSize)};
			const MString pointSizeParameterName          = "pointSize";
			shaderInstancePtr->setParameter(pointSizeParameterName, pointSizeArray);
		}
		
		/* set point color */
		{
			MPlug colorModePlug(objPath.node(&stat), shapeFn.attribute("colorMode"));
			short colorMode = colorModePlug.asShort();
			userData->colorMode = colorMode;
			std::map<int, RGBColor> colorMap;
			switch (colorMode)
			{
				case mode::color::solid:
				{
					MPlug colorPlug(objPath.node(&stat), shapeFn.attribute("color"));
					const float r = colorPlug.child(0).asFloat();
					const float g = colorPlug.child(1).asFloat();
					const float b = colorPlug.child(2).asFloat();
					
					userData->color = RGBColor(r, g, b);
				}
				break;
				case mode::color::foreach:
				{
					for(SourceManager::iterator iter = pSourceMgr_->begin(); iter != pSourceMgr_->end(); ++iter)
					{
						const int      elemIndex = iter->first;
						const MDagPath srcPath   = iter->second->objPath();

						MFnDagNode srcFn(srcPath.node());
						MPlug colorPlug(srcPath.node(), srcFn.attribute("color"));
						const float r = colorPlug.child(0).asFloat();
						const float g = colorPlug.child(1).asFloat();
						const float b = colorPlug.child(2).asFloat();
						colorMap[elemIndex] = RGBColor(r, g, b);
					}
				}
				break;
				case mode::color::random:
				{
					/* not supported */
				}
				break;
			}
			userData->colorMap = colorMap;
		}
		/* set transparency for deleted points color */
		{
			MPlug transparencyPlug(objPath.node(), shapeFn.attribute("transparency"));
			userData->transparency = transparencyPlug.asFloat();
		}
		
		/// set renderItem enable
		MPlug displayModePlug(objPath.node(&stat), TaneMPxShape::displayModeObj);
		short displayMode = displayModePlug.asShort();
		if(pPoints_->size() == 0 || displayMode == mode::display::none)
		{
			dormantPointsRenderItemPtr->enable(false);
		}
		else
		{
			dormantPointsRenderItemPtr->enable(true);
		}
	}
}
void TaneMPxShapeGeometryOverride::updateActivePointsRenderItem(const MDagPath &objPath, MHWRender::MRenderItemList &list, const MHWRender::MShaderManager* shaderMgrPtr)
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	MHWRender::MRenderItem* activePointsRenderItemPtr = nullptr;
	int index = list.indexOf(ActivePointsRenderItem);
	if(index < 0)
	{
		activePointsRenderItemPtr = MHWRender::MRenderItem::Create(ActivePointsRenderItem, MHWRender::MRenderItem::DecorationItem, MHWRender::MGeometry::kPoints);
		activePointsRenderItemPtr->setDrawMode(MHWRender::MGeometry::kAll);
		
		/// Set depth priority to be active point. This should offset it to be visible above items with "dormant point" priority.
		activePointsRenderItemPtr->depthPriority(MHWRender::MRenderItem::sActivePointDepthPriority);
		list.append(activePointsRenderItemPtr);
		
		MHWRender::MShaderInstance* shaderPtr = shaderMgrPtr->getStockShader(MHWRender::MShaderManager::k3dFatPointShader, nullptr, nullptr);
		if(shaderPtr)
		{
			// Set the point size parameter. Make it slightly larger for active vertices
			static const float pointSize = 5.0f;
			
			// set active point size
			const float   pointSizeArray[2]      = {pointSize, pointSize};
			const MString pointSizeParameterName = "pointSize";
			shaderPtr->setParameter(pointSizeParameterName, pointSizeArray);
			
			// set active color
			static const float theColor[]         = {1.0f, 1.0f, 0.0f, 1.0f};
			const MString      colorParameterName = "solidColor";
			shaderPtr->setParameter(colorParameterName, theColor);
			
			activePointsRenderItemPtr->setShader(shaderPtr, NULL);
			/// once assigned, no need to hold on to shader instance
			shaderMgrPtr->releaseShader(shaderPtr);
		}
	}
	else
	{
		activePointsRenderItemPtr = list.itemAt(index);
	}
	
	if(activePointsRenderItemPtr)
	{
		const bool enable = (activeIDs_.length() > 0 && enableActiveComponentDisplay(objPath));
		activePointsRenderItemPtr->enable(enable);
	}
}

void TaneMPxShapeGeometryOverride::populateDormantPointsRenderItem_(const MHWRender::MGeometryRequirements &requirements, const MHWRender::MRenderItem* item, MHWRender::MGeometry &data)
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	const size_t npoints = pPoints_->size();
	
	LOGGER_DEBUG("npoints = %u", npoints);
	
	MHWRender::MVertexBuffer* positionBuffer = nullptr;
	float                   * positions      = nullptr;
	MHWRender::MVertexBuffer* cpvBuffer      = nullptr;
	float                   * cpv            = nullptr;
	MHWRender::MIndexBuffer * indicesBuffer  = nullptr;
	unsigned int            * indices        = nullptr;
	
	const MHWRender::MVertexBufferDescriptorList& descList    = requirements.vertexRequirements();
	int numVertexReqs = descList.length();
	MHWRender::MVertexBufferDescriptor desc;
	
	/* Create Buffer */
	for (int reqNum = 0; reqNum < numVertexReqs; reqNum++)
	{
		if(!descList.getDescriptor(reqNum, desc))
		{
			continue;
		}
		switch (desc.semantic())
		{
			case MHWRender::MGeometry::kPosition:
				if(!positionBuffer)
				{
					positionBuffer = data.createVertexBuffer(desc);
					if(positionBuffer)
					{
						positions = (float*) positionBuffer->acquire(npoints, true /*writeOnly - we don't need the current buffer values*/);
					}
	                
					indicesBuffer = data.createIndexBuffer(MHWRender::MGeometry::kUnsignedInt32);
					if(indicesBuffer)
					{
						indices = (unsigned int*) indicesBuffer->acquire(npoints, true /*writeOnly - we don't need the current buffer values*/);
					}
	    
				}
				break;
			case MHWRender::MGeometry::kColor:
				if(!cpvBuffer)
				{
					cpvBuffer = data.createVertexBuffer(desc);
					if(cpvBuffer)
					{
						cpv = (float*) cpvBuffer->acquire(npoints, true /*writeOnly - we don't need the current buffer values*/);
					}
				}
				break;
			default:
				break;
		}
	
	}
	
	/* Update Buffer */
	if(positions && cpv)
	{
		UserData *userData = dynamic_cast<UserData *>(item->customData());
		Point3fArray& points = pPoints_->get();
		IntArray& renderables = pPoints_->get<int>(sementic::attribute::renderable);
		IntArray& sourceIDs = pPoints_->get<int>(sementic::attribute::sid);
		
		/////set indices
		//thrust::sequence(thrust::omp::par, indices, indices + npoints);
		
		/* set cpv by color mode, set cpv by renderables attribute */
		int nsources = pSourceMgr_->size();
		LOGGER_IN("set point buffer");
		thrust::for_each(thrust::omp::par,
		                 thrust::counting_iterator<size_t>(0),
		                 thrust::counting_iterator<size_t>(static_cast<size_t>(npoints)),
		                 [&](const size_t i)
		                 {
			                 /* set indices */
		                 	 indices[i] = i;
			
			                 /* set position */
			                 positions[i * 3 + 0] = (*pPoints_)[i][0];  //r
			                 positions[i * 3 + 1] = (*pPoints_)[i][1];  //g
			                 positions[i * 3 + 2] = (*pPoints_)[i][2];  //b
			                 

			                 /* set color */
			                 RGBColor color(0.0f, 0.0f, 0.0f);
			                 switch (userData->colorMode)
			                 {
				                 case mode::color::solid:
					                 color = userData->color;
					                 break;
				                 case mode::color::foreach:
					                 if (nsources > 0)
						                 color = userData->colorMap[sourceIDs[i]];
					                 break;
				                 case mode::color::random:    /* not supported */
					                 color = userData->color;
					                 break;
			                 }
			                 cpv[i * 4 + 0] = color[0];  //r
			                 cpv[i * 4 + 1] = color[1];  //g
			                 cpv[i * 4 + 2] = color[2];  //b
			                 cpv[i * 4 + 3] = renderables[i] ? 1.0 : 0.125f;
			
		                 });
		LOGGER_OUT();
	}
	if(positions && cpv)
	{
		positionBuffer->commit(positions);
		cpvBuffer->commit(cpv);
	
		indicesBuffer->commit(indices);
		item->associateWithIndexBuffer(indicesBuffer);
	}
}
void TaneMPxShapeGeometryOverride::populateActivePointsRenderItem_(const MHWRender::MGeometryRequirements &requirements, const MHWRender::MRenderItem* item, MHWRender::MGeometry &data)
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	MHWRender::MIndexBuffer* indexBuffer = data.createIndexBuffer(MHWRender::MGeometry::kUnsignedInt32);
	if(indexBuffer)
	{
		unsigned int activePointsCount = activeIDs_.length();
		unsigned int* buffer = (unsigned int*) indexBuffer->acquire(activePointsCount, true /*writeOnly - we don't need the current buffer values*/);
		if(buffer)
		{
			LOGGER_IN("set active point buffer");
			bool useOpenMP = true;
#pragma omp parallel for if( useOpenMP )
			for(int i = 0; i < activePointsCount; i++)
			{
				buffer[i] = activeIDs_[i];
			}
			LOGGER_OUT();
			
			indexBuffer->commit(buffer);
			item->associateWithIndexBuffer(indexBuffer);
		}
	}
}

bool TaneMPxShapeGeometryOverride::enableActiveComponentDisplay(const MDagPath &path) const
{
	DECL_LOGGER(logging::maya->TaneMPxShapeGeometryOverride);
	bool                     enable        = true;
	MHWRender::DisplayStatus displayStatus = MHWRender::MGeometryUtilities::displayStatus(path);
	if((displayStatus != MHWRender::kHilite) && (displayStatus != MHWRender::kActiveComponent))
	{
		enable = false;
	}
	else
	{
		if(path.isTemplated())
			enable = false;
	}
	
	return enable;
}

NAMESPACE_BASE_END



