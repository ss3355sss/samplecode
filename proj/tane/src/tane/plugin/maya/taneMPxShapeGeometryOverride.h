#pragma once
#include "maya/pch.h"

#include "base/instancingItem.h"
#include "base/messageQueue.h"

#include "maya/messageMPxData.h"
#include "maya/sourcesMPxData.h"
#include "maya/pointsMPxData.h"

#include "maya/taneMPxShape.h"

NAMESPACE_BASE_BEGIN
///------------------------------------------------- TaneMPxSurfaceShapeComponentConverter
class TaneMPxShapeComponentConverter : public MHWRender::MPxComponentConverter
{
private:
private:
	MFnSingleIndexedComponent fComponent;
	MObject                   fComponentObject;
	std::vector<int>          fVertices;
public:
	static MHWRender::MPxComponentConverter* Creator();
public:
	TaneMPxShapeComponentConverter();
	virtual ~TaneMPxShapeComponentConverter();
public:
	virtual void initialize(const MHWRender::MRenderItem &renderItem);
	virtual void addIntersection(MHWRender::MIntersection &intersection);
	virtual MObject component();
	virtual MSelectionMask selectionMask() const;
};
NAMESPACE_BASE_END

NAMESPACE_BASE_BEGIN
///------------------------------------------------- TaneMPxSurfaceShapeGeometryOverride
class TaneMPxShapeGeometryOverride : public MHWRender::MPxGeometryOverride
{
private:
	TaneMPxShape*                   pShape_;
	std::shared_ptr<SourceManager>  pSourceMgr_;
	std::shared_ptr<Points>         pPoints_;
	MIntArray                       activeIDs_;
	
public:
	static MString DormantPointsRenderItem;
	static MString ActivePointsRenderItem;
	static MString DeletedPointsRenderItem;
public:
	static MHWRender::MPxGeometryOverride* Creator(const MObject &obj);
private:
	TaneMPxShapeGeometryOverride(const MObject &obj);
public:
	virtual ~TaneMPxShapeGeometryOverride();
	
	virtual MHWRender::DrawAPI supportedDrawAPIs() const;
	
	virtual void updateDG();
	virtual void updateRenderItems(const MDagPath &objPath, MHWRender::MRenderItemList &list);
	virtual void populateGeometry(const MHWRender::MGeometryRequirements &requirements, const MHWRender::MRenderItemList &renderItems, MHWRender::MGeometry &data);
	virtual void cleanUp();
	virtual bool hasUIDrawables() const;
	virtual void addUIDrawables(const MDagPath &objPath, MHWRender::MUIDrawManager &drawManager, const MHWRender::MFrameContext &frameContext);
	virtual void updateSelectionGranularity(const MDagPath &path, MHWRender::MSelectionContext &selectionContext);

private:
	void addPointInfoUIDrawables(const int elemIndex, const MPoint pos, MHWRender::MUIDrawManager &drawManager, const MHWRender::MFrameContext &frameContext);
	void addLocalAxisUIDrawables(const double s, const int elemIndex, const MPoint pos, MHWRender::MUIDrawManager &drawManager, const MHWRender::MFrameContext &frameContext);
	
	void updateDormantPointsRenderItem(const MDagPath &objPath, MHWRender::MRenderItemList &list, const MHWRender::MShaderManager* shaderMgrPtr);
	void updateActivePointsRenderItem(const MDagPath &objPath, MHWRender::MRenderItemList &list, const MHWRender::MShaderManager* shaderMgrPtr);
	
	void populateDormantPointsRenderItem_(const MHWRender::MGeometryRequirements &requirements, const MHWRender::MRenderItem* item, MHWRender::MGeometry &data);
	void populateActivePointsRenderItem_(const MHWRender::MGeometryRequirements &requirements, const MHWRender::MRenderItem* item, MHWRender::MGeometry &data);
	
	bool enableActiveComponentDisplay(const MDagPath &path) const;
private:    //back up
};
NAMESPACE_BASE_END