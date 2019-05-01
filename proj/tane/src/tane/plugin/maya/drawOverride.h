#pragma once
#include "maya/pch.h"

#include "base/frame.h"

#include "base/vector.h"
#include "base/matrix.h"

#include "base/bbox.h"

#include "base/renderItem.h"
#include "baseGl/renderTask.h"
#include "baseGl/taskManager.h"

#include "lib/baseMaya/util.h"

NAMESPACE_BASE_BEGIN

class DrawOverride //: boost::noncopyable
{
public:
	struct TransformMtx
	{
		Mat4x4f viewportMtx         = Mat4x4f::Identity();
		Mat3x3f normalMtx           = Mat3x3f::Identity();
		
		Mat4x4f modelViewMtx        = Mat4x4f::Identity();
		Mat4x4f projectionMtx       = Mat4x4f::Identity();
		Mat4x4f modelViewProjMtx    = Mat4x4f::Identity();
		
		Mat4x4f	xformMtx            = Mat4x4f::Identity();
	};
	
	struct Parameter
	{
		bool        doubleSided         = true;
		short       displayMode         = mode::display::none;

		RGBColor   color                = RGBColor(0.0f, 0.0f, 0.0f);
		float      transparency         = 1.0f;
		RGBColor   wireColor            = RGBColor(0.0f, 0.0f, 0.0f);
		
		short       animCycle           = mode::animCycle::none;
		Time        time                = Time();
		
		bool        drawBound           = false;
		
		bool        enableIcr           = false;
		bool        enableLOD           = false;
		double      minDistance         = 50;
		double      maxDistance         = 100.0;
		
		bool        noLighting          = false;
		bool        noWire              = false;
		bool        overrideMask        = false;
	};

public:
	typedef MDagPathHashMap<const std::shared_ptr<DrawOverride>>    MapT;
	typedef typename MapT::iterator                                 iterator;
private:
	/* ------------------------------------------------------------ decl singleton */
	static MapT kDrawOverrideMap;

public:
	/* ------------------------------------------------------------ static functions for singleton */
	static const std::shared_ptr<DrawOverride> GetDrawOverride(const MDagPath& objPath);
	static void Register(const MDagPath& objPath, const std::shared_ptr<DrawOverride>& pDerived);
	static void Deregister(const MDagPath& objPath);
	static iterator Begin();
	static iterator End();
public:
	/* ------------------------------------------------------------ MUserData */
	struct UserData;
	
public:
	/* ------------------------------------------------------------ MPxDrawOverride DrawCallback Wrap */
	static void DrawCallback(const MHWRender::MDrawContext& context, const MUserData* pData);
	static bool SkipDraw(const MHWRender::MDrawContext& context, const MUserData* pUserData);
	
private:
	const MHWRender::MBlendState* 	 pBlendState_           = nullptr;
	MDAGDrawOverrideInfo 	         drawOverrideInfo_      = MDAGDrawOverrideInfo();
	
	TransformMtx                     transformMtx_          = TransformMtx();
	Parameter                        param_                 = Parameter();

	bool                             isSelected_            = false;
	bool                             isPassShaderOverrided_ = false;
	bool                             isTransparent_         = false;

	BBox3f                           bound_                 = BBox3f();

	bool                             drawShaded_            = false;
	bool                             drawWire_              = false;
	bool                             drawTexture_           = false;

protected:
	/* ------------------------------------------------------------ constructor / destructor*/
	DrawOverride();
	virtual ~DrawOverride();
	
protected:
	/*==============================================================================*/
	/*Needed to be impl in Derived Class                                            */
	/*==============================================================================*/
	virtual MStatus prepareForDrawImpl_(const MDagPath& objpath,                       // shape node dagPath
	                                    const MDagPath& cameraPath,                    // actvie camera dag path
	                                    const MHWRender::MFrameContext& frameContext   // frame context
	) = 0;

	/*==============================================================================*/
	/*Needed to be impl in Derived Class											*/
	/*==============================================================================*/
	virtual void drawImpl_(const MHWRender::MDrawContext& 		context,         // draw context
						  const MHWRender::MShaderInstance* 	pOverridedShader // pass shader instance
	) = 0;
public:

	/* ------------------------------------------------------------ helpers*/
	MUserData* prepareForDraw(const MDagPath& objPath, const MDagPath& cameraPath, const MHWRender::MFrameContext& frameContext, MUserData* pOldUserData);
	const TransformMtx& transformMtx() const;
	const Parameter& parameter() const;
	
	const bool isSelected() const;
	const bool isPassShaderOverrided() const;
	const bool isTransparent() const;
	
	const BBox3f& bound() const;
	const bool drawShaded() const;
	const bool drawWire() const;
	const bool drawTexture() const;
	
private:
	/* ------------------------------------------------------------ private helpers*/
	MStatus getRenderParam_(const MDagPath& objPath);
	MStatus getWorldMtx_   (const MDagPath& objPath);
	
	MStatus setBound_(const MDagPath& objPath);
	
	MStatus setGlState_(const MHWRender::MDrawContext& context);
	MStatus unsetGlState_(const MHWRender::MDrawContext& context);
	MStatus setDisplayState_(const MHWRender::MDrawContext& context);
	MStatus unsetDisplayState_(const MHWRender::MDrawContext& context);
	MStatus setTransformMtx_(const MHWRender::MDrawContext& context);
	MStatus unsetTransformMtx_(const MHWRender::MDrawContext& context);
	MStatus setLighting_(const MHWRender::MDrawContext& context);
	MStatus unsetLighting_(const MHWRender::MDrawContext& context);

	void bindContext_(const MHWRender::MDrawContext* pContext, const MHWRender::MShaderInstance* pOverridedShader);
	void unbindContext_(const MHWRender::MDrawContext* pContext, const MHWRender::MShaderInstance* pOverridedShader);
	
	void drawBound_(const MHWRender::MDrawContext& context, const MHWRender::MShaderInstance* pOverridedShader);
};
NAMESPACE_BASE_END