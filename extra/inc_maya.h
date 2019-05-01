#pragma once

#include <maya/MLibrary.h>
#include <maya/MIOStream.h>
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MTime.h>
#include <maya/MPlane.h>
#include <maya/MTimer.h>
#include <maya/MItDag.h>
#include <maya/MImage.h>
#include <maya/MPoint.h>
#include <maya/MColor.h>
#include <maya/MGlobal.h>
#include <maya/MStatus.h>
#include <maya/MFnMesh.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
//#include <maya/M3dView.h>
#include <maya/MSyntax.h>
#include <maya/MCursor.h>
#include <maya/MPxData.h>
#include <maya/MFnField.h>
#include <maya/MDagPath.h>
#include <maya/MFnFluid.h>
#include <maya/MArgList.h>
#include <maya/MAnimUtil.h>

#include <maya/MItDependencyGraph.h>
#include <maya/MFnExpression.h>
#include <maya/MMaterial.h>
#include <maya/MUserData.h>
#include <maya/MDistance.h>
#include <maya/MFnCamera.h>
#include <maya/MIOStream.h>
#include <maya/MDrawData.h>
#include <maya/MPlugArray.h>
#include <maya/MItCurveCV.h>
#include <maya/MFnDagNode.h>
#include <maya/MToolsInfo.h>
#include <maya/MUiMessage.h>
#include <maya/MPxCommand.h>
#include <maya/MPxContext.h>
#include <maya/MPxSelectionContext.h>
#include <maya/MDGMessage.h>
#include <maya/MHWGeometry.h>
#include <maya/MDGModifier.h>
#include <maya/MQuaternion.h>
#include <maya/MDataHandle.h>
#include <maya/MFnMeshData.h>
#include <maya/MFloatArray.h>
#include <maya/MItMeshEdge.h>
#include <maya/MPointArray.h>
#include <maya/MItGeometry.h>
#include <maya/MColorArray.h>
#include <maya/MRenderUtil.h>
#include <maya/MFileObject.h>
#include <maya/MDagModifier.h>
#include <maya/MAnimControl.h>
#include <maya/MFloatMatrix.h>
#include <maya/MNodeMessage.h>
#include <maya/MThreadUtils.h>
#include <maya/MComputation.h>
#include <maya/MMatrixArray.h>
#include <maya/MArgDatabase.h>
#include <maya/MFnTransform.h>
#include <maya/MDrawRequest.h>
#include <maya/MBoundingBox.h>
#include <maya/MDrawRequest.h>
#include <maya/MDrawContext.h>
#include <maya/MDagMessage.h>
#include <maya/MEventMessage.h>
#include <maya/MStateManager.h>
#include <maya/MDrawRegistry.h>
#include <maya/MFnStringData.h>
#include <maya/MModelMessage.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnMatrixData.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItSurfaceCV.h>
#include <maya/MFnPluginData.h>
#include <maya/MFeedbackLine.h>
#include <maya/MShaderManager.h>
#include <maya/MRampAttribute.h>
#include <maya/MPxLocatorNode.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MSelectionList.h>
#include <maya/MFnNumericData.h>
#include <maya/MEulerRotation.h>
#include <maya/MPxToolCommand.h>
#include <maya/MSelectionMask.h>
#include <maya/MAttributeSpec.h>
#include <maya/MAttributeIndex.h>
#include <maya/MPxGeometryData.h>
#include <maya/MTextureManager.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MProgressWindow.h>
#include <maya/MPxDeformerNode.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MPxSurfaceShape.h>
#include <maya/MPxComponentShape.h>
#include <maya/MMeshIntersector.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MItSelectionList.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MGLFunctionTable.h>
#include <maya/MHardwareRenderer.h>
#include <maya/MPxManipContainer.h>
#include <maya/MFnCircleSweepManip.h>
#include <maya/MFnDirectionManip.h>
#include <maya/MFnDiscManip.h>
#include <maya/MFnDistanceManip.h>
#include <maya/MFnFreePointTriadManip.h>
#include <maya/MFnStateManip.h>
#include <maya/MFnToggleManip.h>
#include <maya/MFnRotateManip.h>
#include <maya/MFnScaleManip.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MPxSurfaceShapeUI.h>
#include <maya/MFnParticleSystem.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MPxContextCommand.h>
#include <maya/MFnFloatArrayData.h>
#include <maya/MFnStringArrayData.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MViewport2Renderer.h>
#include <maya/MPxSubSceneOverride.h>
#include <maya/MAttributeSpecArray.h>
#include <maya/MPxGeometryOverride.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MPxGeometryIterator.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnComponentListData.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MTextureEditorDrawInfo.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MPxTransform.h>
#include <maya/MPxTransformationMatrix.h>
#include <maya/MObjectHandle.h>
#include <maya/MSceneMessage.h>

#if MAYA_API_VERSION >= 201602
#include <maya/MSelectionContext.h>
#include <maya/MPxComponentConverter.h>
#endif




//#include <maya/M3dView.h>
//#include <maya/MAddRemoveAttrEdit.h>
//#include <maya/MAngle.h>
//#include <maya/MAnimControl.h>
//#include <maya/MAnimCurveChange.h>
//#include <maya/MAnimCurveClipboard.h>
//#include <maya/MAnimCurveClipboardItemArray.h>
//#include <maya/MAnimCurveClipboardItem.h>
//#include <maya/MAnimMessage.h>

//#include <maya/MAnimUtil.h>
//#include <maya/MApiVersion.h>
//#include <maya/MArgDatabase.h>
//#include <maya/MArgList.h>
//#include <maya/MArgParser.h>
//#include <maya/MArrayDataBuilder.h>
//#include <maya/MArrayDataHandle.h>
//#include <maya/MAtomic.h>
//#include <maya/MAttributeIndex.h>
//#include <maya/MAttributePatternArray.h>
//#include <maya/MAttributePattern.h>
//#include <maya/MAttributeSpecArray.h>
//#include <maya/MAttributeSpec.h>
//#include <maya/MBoundingBox.h>
//#include <maya/MCacheFormatDescription.h>
//#include <maya/MCallbackIdArray.h>
//#include <maya/MCameraSetMessage.h>
//#include <maya/MColorArray.h>
//#include <maya/MColor.h>
//#include <maya/MColorManagementUtilities.h>
//#include <maya/MCommandMessage.h>
//#include <maya/MCommandResult.h>
//#include <maya/MCommonRenderSettingsData.h>
//#include <maya/MCommonSystemUtils.h>
//#include <maya/MComponentDataIndexing.h>
//#include <maya/MComputation.h>
//#include <maya/MConditionMessage.h                           >
//#include <maya/MConnectDisconnectAttrEdit.h                  >
//#include <maya/MContainerMessage.h                           >
//#include <maya/MCursor.h                                     >
//#include <maya/MCurveAttribute.h                             >
//#include <maya/MCustomEvaluatorClusterNode.h                 >
//#include <maya/MD3D9Renderer.h                               >
//#include <maya/MDagMessage.h                                 >
//#include <maya/MDagModifier.h                                >
//#include <maya/MDagPathArray.h                               >
//#include <maya/MDagPath.h                                    >
//#include <maya/MDataBlock.h                                  >
//#include <maya/MDataHandle.h                                 >
//#include <maya/MDeviceChannel.h                              >
//#include <maya/MDeviceState.h                                >
//#include <maya/MDGContext.h                                  >
//#include <maya/MDGMessage.h                                  >
//#include <maya/MDGModifier.h                                 >
//#include <maya/MDistance.h                                   >
//#include <maya/MDoubleArray.h                                >
//#include <maya/MDrawContext.h                                >
//#include <maya/MDrawData.h                                   >
//#include <maya/MDrawInfo.h                                   >
//#include <maya/MDrawProcedureBase.h                          >
//#include <maya/MDrawRegistry.h                               >
//#include <maya/MDrawRequest.h                                >
//#include <maya/MDrawRequestQueue.h                           >
//#include <maya/MDrawTraversal.h                              >
//#include <maya/MDynamicsUtil.h                               >
//#include <maya/MDynSweptLine.h                               >
//#include <maya/MDynSweptTriangle.h                           >
//#include <maya/MEdit.h                                       >
//#include <maya/MEulerRotation.h                              >
//#include <maya/MEvaluationGraph.h                            >
//#include <maya/MEvaluationManager.h                          >
//#include <maya/MEvaluationNode.h                             >
//#include <maya/MEvaluationNodeIterator.h                     >
//#include <maya/MEvent.h                                      >
//#include <maya/MEventMessage.h                               >
//#include <maya/MExternalContentInfoTable.h                   >
//#include <maya/MExternalContentLocationTable.h               >
//#include <maya/MExternalDropCallback.h                       >
//#include <maya/MExternalDropData.h                           >
//#include <maya/MFcurveEdit.h                                 >
//#include <maya/MFeedbackLine.h                               >
//#include <maya/MFileIO.h                                     >
//#include <maya/MFileObject.h                                 >
//#include <maya/MFloatArray.h                                 >
//#include <maya/MFloatMatrix.h                                >
//#include <maya/MFloatPointArray.h                            >
//#include <maya/MFloatPoint.h                                 >
//#include <maya/MFloatVectorArray.h                           >
//#include <maya/MFloatVector.h                                >
//#include <maya/MFnAirField.h                                 >
//#include <maya/MFnAmbientLight.h                             >
//#include <maya/MFnAnimCurve.h                                >
//#include <maya/MFnAnisotropyShader.h                         >
//#include <maya/MFnAreaLight.h                                >
//#include <maya/MFnArrayAttrsData.h                           >
//#include <maya/MFnAssembly.h                                 >
//#include <maya/MFnAttribute.h                                >
//#include <maya/MFnBase.h                                     >
//#include <maya/MFnBlendShapeDeformer.h                       >
//#include <maya/MFnBlinnShader.h                              >
//#include <maya/MFnCamera.h                                   >
//#include <maya/MFnCameraSet.h                                >
//#include <maya/MFnCharacter.h                                >
//#include <maya/MFnCircleSweepManip.h                         >
//#include <maya/MFnClip.h                                     >
//#include <maya/MFnComponent.h                                >
//#include <maya/MFnComponentListData.h                        >
//#include <maya/MFnCompoundAttribute.h                        >
//#include <maya/MFnContainerNode.h                            >
//#include <maya/MFnCurveSegmentManip.h                        >
//#include <maya/MFnDagNode.h                                  >
//#include <maya/MFnData.h                                     >
//#include <maya/MFnDependencyNode.h                           >
//#include <maya/MFnDirectionalLight.h                         >
//#include <maya/MFnDirectionManip.h                           >
//#include <maya/MFnDiscManip.h                                >
//#include <maya/MFnDistanceManip.h                            >
//#include <maya/MFnDoubleArrayData.h                          >
//#include <maya/MFnDoubleIndexedComponent.h                   >
//#include <maya/MFnDragField.h                                >
//#include <maya/MFnDynSweptGeometryData.h                     >
//#include <maya/MFnEnumAttribute.h                            >
//#include <maya/MFnExpression.h                               >
//#include <maya/MFnField.h                                    >
//#include <maya/MFnFloatArrayData.h                           >
//#include <maya/MFnFluid.h                                    >
//#include <maya/MFnFreePointTriadManip.h                      >
//#include <maya/MFnGenericAttribute.h                         >
//#include <maya/MFnGeometryData.h                             >
//#include <maya/MFnGeometryFilter.h                           >
//#include <maya/MFnGravityField.h                             >
//#include <maya/MFn.h                                         >
//#include <maya/MFnHikEffector.h                              >
//#include <maya/MFnIkEffector.h                               >
//#include <maya/MFnIkHandle.h                                 >
//#include <maya/MFnIkJoint.h                                  >
//#include <maya/MFnIkSolver.h                                 >
//#include <maya/MFnImageSource.h                              >
//#include <maya/MFnInstancer.h                                >
//#include <maya/MFnIntArrayData.h                             >
//#include <maya/MFnKeyframeDeltaAddRemove.h                   >
//#include <maya/MFnKeyframeDeltaBlockAddRemove.h              >
//#include <maya/MFnKeyframeDeltaBreakdown.h                   >
//#include <maya/MFnKeyframeDelta.h                            >
//#include <maya/MFnKeyframeDeltaInfType.h                     >
//#include <maya/MFnKeyframeDeltaMove.h                        >
//#include <maya/MFnKeyframeDeltaScale.h                       >
//#include <maya/MFnKeyframeDeltaTangent.h                     >
//#include <maya/MFnKeyframeDeltaWeighted.h                    >
//#include <maya/MFnLambertShader.h                            >
//#include <maya/MFnLatticeData.h                              >
//#include <maya/MFnLatticeDeformer.h                          >
//#include <maya/MFnLattice.h                                  >
//#include <maya/MFnLayeredShader.h                            >
//#include <maya/MFnLightDataAttribute.h                       >
//#include <maya/MFnLight.h                                    >
//#include <maya/MFnManip3D.h                                  >
//#include <maya/MFnMatrixArrayData.h                          >
//#include <maya/MFnMatrixAttribute.h                          >
//#include <maya/MFnMatrixData.h                               >
//#include <maya/MFnMeshData.h                                 >
//#include <maya/MFnMesh.h                                     >
//#include <maya/MFnMessageAttribute.h                         >
//#include <maya/MFnMotionPath.h                               >
//#include <maya/MFnNewtonField.h                              >
//#include <maya/MFnNIdData.h                                  >
//#include <maya/MFnNObjectData.h                              >
//#include <maya/MFnNonAmbientLight.h                          >
//#include <maya/MFnNonExtendedLight.h                         >
//#include <maya/MFnNumericAttribute.h                         >
//#include <maya/MFnNumericData.h                              >
//#include <maya/MFnNurbsCurveData.h                           >
//#include <maya/MFnNurbsCurve.h                               >
//#include <maya/MFnNurbsSurfaceData.h                         >
//#include <maya/MFnNurbsSurface.h                             >
//#include <maya/MFnParticleSystem.h                           >
//#include <maya/MFnPartition.h                                >
//#include <maya/MFnPfxGeometry.h                              >
//#include <maya/MFnPhongEShader.h                             >
//#include <maya/MFnPhongShader.h                              >
//#include <maya/MFnPluginData.h                               >
//#include <maya/MFnPlugin.h                                   >
//#include <maya/MFnPointArrayData.h                           >
//#include <maya/MFnPointLight.h                               >
//#include <maya/MFnPointOnCurveManip.h                        >
//#include <maya/MFnPointOnSurfaceManip.h                      >
//#include <maya/MFnRadialField.h                              >
//#include <maya/MFnReference.h                                >
//#include <maya/MFnReflectShader.h                            >
//#include <maya/MFnRenderLayer.h                              >
//#include <maya/MFnRenderPass.h                               >
//#include <maya/MFnRotateManip.h                              >
//#include <maya/MFnScaleManip.h                               >
//#include <maya/MFnSet.h                                      >
//#include <maya/MFnSingleIndexedComponent.h                   >
//#include <maya/MFnSkinCluster.h                              >
//#include <maya/MFnSphereData.h                               >
//#include <maya/MFnSpotLight.h                                >
//#include <maya/MFnStateManip.h                               >
//#include <maya/MFnStringArrayData.h                          >
//#include <maya/MFnStringData.h                               >
//#include <maya/MFnSubdData.h                                 >
//#include <maya/MFnSubd.h                                     >
//#include <maya/MFnSubdNames.h                                >
//#include <maya/MFnToggleManip.h                              >
//#include <maya/MFnToolContext.h                              >
//#include <maya/MFnTransform.h                                >
//#include <maya/MFnTripleIndexedComponent.h                   >
//#include <maya/MFnTurbulenceField.h                          >
//#include <maya/MFnTypedAttribute.h                           >
//#include <maya/MFnUInt64ArrayData.h                          >
//#include <maya/MFnUint64SingleIndexedComponent.h             >
//#include <maya/MFnUniformField.h                             >
//#include <maya/MFnUnitAttribute.h                            >
//#include <maya/MFnVectorArrayData.h                          >
//#include <maya/MFnVolumeAxisField.h                          >
//#include <maya/MFnVolumeLight.h                              >
//#include <maya/MFnVortexField.h                              >
//#include <maya/MFnWeightGeometryFilter.h                     >
//#include <maya/MFnWireDeformer.h                             >
//#include <maya/MFragmentManager.h                            >
//#include <maya/MFrameContext.h                               >
//#include <maya/MFStream.h                                    >
//#include <maya/MGeometryData.h                               >
//#include <maya/MGeometryExtractor.h                          >
//#include <maya/MGeometry.h                                   >
//#include <maya/MGeometryList.h                               >
//#include <maya/MGeometryManager.h                            >
//#include <maya/MGeometryPrimitive.h                          >
//#include <maya/MGeometryRequirements.h                       >
//#include <maya/MGLdefinitions.h                              >
//#include <maya/MGLFunctionTable.h                            >
//#include <maya/MGL.h                                         >
//#include <maya/MGlobal.h                                     >
//#include <maya/MGPUDeformerRegistry.h                        >
//#include <maya/MGraphEditorInfo.h                            >
//#include <maya/MGraphNodeIterator.h                          >
//#include <maya/MHairSystem.h                                 >
//#include <maya/MHardwareRenderer.h                           >
//#include <maya/MHWGeometry.h                                 >
//#include <maya/MHWGeometryUtilities.h                        >
//#include <maya/MHwrCallback.h                                >
//#include <maya/MHWShaderSwatchGenerator.h                    >
//#include <maya/MHwTextureManager.h                           >
//#include <maya/MIffFile.h                                    >
//#include <maya/MIffTag.h                                     >
//#include <maya/MIkHandleGroup.h                              >
//#include <maya/MIkSystem.h                                   >
//#include <maya/MImageFileInfo.h                              >
//#include <maya/MImage.h                                      >
//#include <maya/MInt64Array.h                                 >
//#include <maya/MIntArray.h                                   >
//#include <maya/MIOStreamFwd.h                                >
//#include <maya/MIOStream.h                                   >
//#include <maya/MItCurveCV.h                                  >
//#include <maya/MItDag.h                                      >
//#include <maya/MItDependencyGraph.h                          >
//#include <maya/MItDependencyNodes.h                          >
//#include <maya/MItEdits.h                                    >
//#include <maya/MIteratorType.h                               >
//#include <maya/MItGeometry.h                                 >
//#include <maya/MItInstancer.h                                >
//#include <maya/MItKeyframe.h                                 >
//#include <maya/MItMeshEdge.h                                 >
//#include <maya/MItMeshFaceVertex.h                           >
//#include <maya/MItMeshPolygon.h                              >
//#include <maya/MItMeshVertex.h                               >
//#include <maya/MItSelectionList.h                            >
//#include <maya/MItSubdEdge.h                                 >
//#include <maya/MItSubdFace.h                                 >
//#include <maya/MItSubdVertex.h                               >
//#include <maya/MItSurfaceCV.h                                >
//#include <maya/MLibrary.h                                    >
//#include <maya/MLightLinks.h                                 >
//#include <maya/MLockMessage.h                                >
//#include <maya/MManipData.h                                  >
//#include <maya/MMaterialArray.h                              >
//#include <maya/MMaterial.h                                   >
//#include <maya/MMatrixArray.h                                >
//#include <maya/MMatrix.h                                     >
//#include <maya/MMeshIntersector.h                            >
//#include <maya/MMeshSmoothOptions.h                          >
//#include <maya/MMessage.h                                    >
//#include <maya/MModelMessage.h                               >
//#include <maya/MMutexLock.h                                  >
//#include <maya/MNamespace.h                                  >
//#include <maya/MNativeWindowHdl.h                            >
//#include <maya/MnCloth.h                                     >
//#include <maya/MnObject.h                                    >
//#include <maya/MNodeClass.h                                  >
//#include <maya/MNodeMessage.h                                >
//#include <maya/MnParticle.h                                  >
//#include <maya/MnRigid.h                                     >
//#include <maya/MnSolver.h                                    >
//#include <maya/MNurbsIntersector.h                           >
//#include <maya/MObjectArray.h                                >
//#include <maya/MObject.h                                     >
//#include <maya/MObjectHandle.h                               >
//#include <maya/MObjectListFilter.h                           >
//#include <maya/MObjectSetMessage.h                           >
//#include <maya/mocapserial.h                                 >
//#include <maya/mocapserver.h                                 >
//#include <maya/mocaptcp.h                                    >
//#include <maya/MOpenCLAutoPtr.h                              >
//#include <maya/MOpenCLBuffer.h                               >
//#include <maya/MOpenCLInfo.h                                 >
//#include <maya/MPaintMessage.h                               >
//#include <maya/MParentingEdit.h                              >
//#include <maya/MPlane.h                                      >
//#include <maya/MPlugArray.h                                  >
//#include <maya/MPlug.h                                       >
//#include <maya/MPointArray.h                                 >
//#include <maya/MPoint.h                                      >
//#include <maya/MPolyMessage.h                                >
//#include <maya/MProfiler.h                                   >
//#include <maya/MProgressWindow.h                             >
//#include <maya/MPx3dModelView.h                              >
//#include <maya/MPxAnimCurveInterpolator.h                    >
//#include <maya/MPxAssembly.h                                 >
//#include <maya/MPxAttributePatternFactory.h                  >
//#include <maya/MPxBakeEngine.h                               >
//#include <maya/MPxBlendShape.h                               >
//#include <maya/MPxCacheFormat.h                              >
//#include <maya/MPxCameraSet.h                                >
//#include <maya/MPxClientDeviceNode.h                         >
//#include <maya/MPxCommand.h                                  >
//#include <maya/MPxComponentConverter.h                       >
//#include <maya/MPxComponentShape.h                           >
//#include <maya/MPxConstraintCommand.h                        >
//#include <maya/MPxConstraint.h                               >
//#include <maya/MPxContextCommand.h                           >
//#include <maya/MPxContext.h                                  >
//#include <maya/MPxControlCommand.h                           >
//#include <maya/MPxCustomEvaluator.h                          >
//#include <maya/MPxData.h                                     >
//#include <maya/MPxDeformerNode.h                             >
//#include <maya/MPxDragAndDropBehavior.h                      >
//#include <maya/MPxDrawOverride.h                             >
//#include <maya/MPxEditData.h                                 >
//#include <maya/MPxEmitterNode.h                              >
//#include <maya/MPxFieldNode.h                                >
//#include <maya/MPxFileResolver.h                             >
//#include <maya/MPxFileTranslator.h                           >
//#include <maya/MPxFluidEmitterNode.h                         >
//#include <maya/MPxGeometryData.h                             >
//#include <maya/MPxGeometryFilter.h                           >
//#include <maya/MPxGeometryIterator.h                         >
//#include <maya/MPxGeometryOverride.h                         >
//#include <maya/MPxGlBuffer.h                                 >
//#include <maya/MPxGPUDeformer.h                              >
//#include <maya/MPxHardwareShader.h                           >
//#include <maya/MPxHwShaderNode.h                             >
//#include <maya/MPxIkSolver.h                                 >
//#include <maya/MPxIkSolverNode.h                             >
//#include <maya/MPxImageFile.h                                >
//#include <maya/MPxImagePlane.h                               >
//#include <maya/MPxIndexBufferMutator.h                       >
//#include <maya/MPxLocatorNode.h                              >
//#include <maya/MPxManipContainer.h                           >
//#include <maya/MPxManipulatorNode.h                          >
//#include <maya/MPxMaterialInformation.h                      >
//#include <maya/MPxMayaAsciiFilter.h                          >
//#include <maya/MPxMidiInputDevice.h                          >
//#include <maya/MPxModelEditorCommand.h                       >
//#include <maya/MPxMotionPathNode.h                           >
//#include <maya/MPxMultiPolyTweakUVCommand.h                  >
//#include <maya/MPxNode.h                                     >
//#include <maya/MPxObjectSet.h                                >
//#include <maya/MPxParticleAttributeMapperNode.h              >
//#include <maya/MPxPolyTrg.h                                  >
//#include <maya/MPxPolyTweakUVCommand.h                       >
//#include <maya/MPxPolyTweakUVInteractiveCommand.h            >
//#include <maya/MPxPrimitiveGenerator.h                       >
//#include <maya/MPxRenderer.h                                 >
//#include <maya/MPxRenderPassImpl.h                           >
//#include <maya/MPxRepresentation.h                           >
//#include <maya/MPxSelectionContext.h                         >
//#include <maya/MPxShaderOverride.h                           >
//#include <maya/MPxShadingNodeOverride.h                      >
//#include <maya/MPxSkinCluster.h                              >
//#include <maya/MPxSpringNode.h                               >
//#include <maya/MPxSubSceneOverride.h                         >
//#include <maya/MPxSurfaceShadingNodeOverride.h               >
//#include <maya/MPxSurfaceShape.h                             >
//#include <maya/MPxSurfaceShapeUI.h                           >
//#include <maya/MPxTexContext.h                               >
//#include <maya/MPxThreadedDeviceNode.h                       >
//#include <maya/MPxToolCommand.h                              >
//#include <maya/MPxTransformationMatrix.h                     >
//#include <maya/MPxTransform.h                                >
//#include <maya/MPxUIControl.h                                >
//#include <maya/MPxUITableControl.h                           >
//#include <maya/MPxVertexBufferGenerator.h                    >
//#include <maya/MPxVertexBufferMutator.h                      >
//#include <maya/MQtUtil.h                                     >
//#include <maya/MQuaternion.h                                 >
//#include <maya/MRampAttribute.h                              >
//#include <maya/MRandom.h                                     >
//#include <maya/MRenderCallback.h                             >
//#include <maya/MRenderData.h                                 >
//#include <maya/MRenderingInfo.h                              >
//#include <maya/MRenderLineArray.h                            >
//#include <maya/MRenderLine.h                                 >
//#include <maya/MRenderParameters.h                           >
//#include <maya/MRenderPassDef.h                              >
//#include <maya/MRenderPassRegistry.h                         >
//#include <maya/MRenderProfile.h                              >
//#include <maya/MRenderSetup.h                                >
//#include <maya/MRenderSetupPrivate.h                         >
//#include <maya/MRenderShadowData.h                           >
//#include <maya/MRenderTarget.h                               >
//#include <maya/MRenderTargetManager.h                        >
//#include <maya/MRenderUtil.h                                 >
//#include <maya/MRenderUtilities.h                            >
//#include <maya/MRenderView.h                                 >
//#include <maya/MRichSelection.h                              >
//#include <maya/MSceneMessage.h                               >
//#include <maya/MScriptUtil.h                                 >
//#include <maya/MSelectInfo.h                                 >
//#include <maya/MSelectionContext.h                           >
//#include <maya/MSelectionList.h                              >
//#include <maya/MSelectionMask.h                              >
//#include <maya/MSetAttrEdit.h                                >
//#include <maya/MShaderManager.h                              >
//#include <maya/MSimple.h                                     >
//#include <maya/MSpinLock.h                                   >
//#include <maya/MStateManager.h                               >
//#include <maya/MStatus.h                                     >
//#include <maya/MStreamUtils.h                                >
//#include <maya/MStringArray.h                                >
//#include <maya/MString.h                                     >
//#include <maya/MStringResource.h                             >
//#include <maya/MStringResourceId.h                           >
//#include <maya/MSwatchRenderBase.h                           >
//#include <maya/MSwatchRenderRegister.h                       >
//#include <maya/MSyntax.h                                     >
//#include <maya/MTemplateCommand.h                            >
//#include <maya/MTemplateManipulator.h                        >
//#include <maya/MTesselationParams.h                          >
//#include <maya/MTextureEditorDrawInfo.h                      >
//#include <maya/MTextureManager.h                             >
//#include <maya/MThreadAsync.h                                >
//#include <maya/MThreadPool.h                                 >
//#include <maya/MThreadUtils.h                                >
//#include <maya/MTimeArray.h                                  >
//#include <maya/MTime.h                                       >
//#include <maya/MTimer.h                                      >
//#include <maya/MTimerMessage.h                               >
//#include <maya/MToolsInfo.h                                  >
//#include <maya/MTransformationMatrix.h                       >
//#include <maya/MTrimBoundaryArray.h                          >
//#include <maya/MTypeId.h                                     >
//#include <maya/MTypes.h                                      >
//#include <maya/MUIDrawManager.h                              >
//#include <maya/MUiMessage.h                                  >
//#include <maya/MUint64Array.h                                >
//#include <maya/MUintArray.h                                  >
//#include <maya/MUniformParameter.h                           >
//#include <maya/MUniformParameterList.h                       >
//#include <maya/MURI.h                                        >
//#include <maya/MUserData.h                                   >
//#include <maya/MUserEventMessage.h                           >
//#include <maya/MUuid.h                                       >
//#include <maya/MVaryingParameter.h                           >
//#include <maya/MVaryingParameterList.h                       >
//#include <maya/MVectorArray.h                                >
//#include <maya/MVector.h                                     >
//#include <maya/MViewport2Renderer.h                          >
//#include <maya/MViewportRenderer.h                           >
//#include <maya/MWeight.h                                     >







//#include <maya/MLibrary.h>
//#include <maya/MIOStream.h>
//#include <maya/MGlobal.h>
//
//#include <maya/MPlug.h>
//#include <maya/MTime.h>
//#include <maya/MPlane.h>
//#include <maya/MTimer.h>
//#include <maya/MItDag.h>
//#include <maya/MImage.h>
//#include <maya/MPoint.h>
//#include <maya/MColor.h>
//#include <maya/MGlobal.h>
//#include <maya/MStatus.h>
//#include <maya/MFnMesh.h>
//#include <maya/MVector.h>
//#include <maya/MMatrix.h>
////#include <maya/M3dView.h>
//#include <maya/MSyntax.h>
//#include <maya/MCursor.h>
//#include <maya/MPxData.h>
//#include <maya/MFnField.h>
//#include <maya/MDagPath.h>
//#include <maya/MFnFluid.h>
//#include <maya/MArgList.h>


//#include <maya/MMaterial.h>
//#include <maya/MUserData.h>
//#include <maya/MDistance.h>
//#include <maya/MFnCamera.h>
//#include <maya/MIOStream.h>
//#include <maya/MDrawData.h>
//#include <maya/MPlugArray.h>
//#include <maya/MItCurveCV.h>
//#include <maya/MFnDagNode.h>
//#include <maya/MToolsInfo.h>
//#include <maya/MUiMessage.h>
//#include <maya/MPxCommand.h>
//#include <maya/MPxContext.h>
//#include <maya/MPxSelectionContext.h>
//#include <maya/MDGMessage.h>
//#include <maya/MHWGeometry.h>
//#include <maya/MDGModifier.h>
//#include <maya/MQuaternion.h>
//#include <maya/MDataHandle.h>
//#include <maya/MFnMeshData.h>
//#include <maya/MFloatArray.h>
//#include <maya/MItMeshEdge.h>
//#include <maya/MPointArray.h>
//#include <maya/MItGeometry.h>
//#include <maya/MColorArray.h>
//#include <maya/MRenderUtil.h>
//#include <maya/MFileObject.h>
//#include <maya/MDagModifier.h>
//#include <maya/MAnimControl.h>
//#include <maya/MFloatMatrix.h>
//#include <maya/MNodeMessage.h>
//#include <maya/MThreadUtils.h>
//#include <maya/MComputation.h>
//#include <maya/MMatrixArray.h>
//#include <maya/MArgDatabase.h>
//#include <maya/MFnTransform.h>
//#include <maya/MDrawRequest.h>
//#include <maya/MBoundingBox.h>
//#include <maya/MDrawRequest.h>
//#include <maya/MDrawContext.h>
//#include <maya/MEventMessage.h>
//#include <maya/MStateManager.h>
//#include <maya/MDrawRegistry.h>
//#include <maya/MFnStringData.h>
//#include <maya/MModelMessage.h>
//#include <maya/MFnNurbsCurve.h>
//#include <maya/MDagPathArray.h>
//#include <maya/MFnMatrixData.h>
//#include <maya/MItMeshVertex.h>
//#include <maya/MItSurfaceCV.h>
//#include <maya/MFnPluginData.h>
//#include <maya/MFeedbackLine.h>
//#include <maya/MShaderManager.h>
//#include <maya/MRampAttribute.h>
//#include <maya/MPxLocatorNode.h>
//#include <maya/MItMeshPolygon.h>
//#include <maya/MSelectionList.h>
//#include <maya/MFnNumericData.h>
//#include <maya/MEulerRotation.h>
//#include <maya/MPxToolCommand.h>
//#include <maya/MSelectionMask.h>
//#include <maya/MAttributeSpec.h>
//#include <maya/MAttributeIndex.h>
//#include <maya/MPxGeometryData.h>
//#include <maya/MTextureManager.h>
//#include <maya/MPxDrawOverride.h>
//#include <maya/MProgressWindow.h>
//#include <maya/MPxDeformerNode.h>
//#include <maya/MFnIntArrayData.h>
//#include <maya/MPxSurfaceShape.h>
//#include <maya/MPxComponentShape.h>
//#include <maya/MMeshIntersector.h>
//#include <maya/MFnUnitAttribute.h>
//#include <maya/MItSelectionList.h>
//#include <maya/MItDependencyNodes.h>
//#include <maya/MFloatPointArray.h>
//#include <maya/MFnEnumAttribute.h>
//#include <maya/MGLFunctionTable.h>
//#include <maya/MHardwareRenderer.h>
//#include <maya/MPxManipContainer.h>
//#include <maya/MFnCircleSweepManip.h>
//#include <maya/MFnDirectionManip.h>
//#include <maya/MFnDiscManip.h>
//#include <maya/MFnDistanceManip.h>
//#include <maya/MFnFreePointTriadManip.h>
//#include <maya/MFnStateManip.h>
//#include <maya/MFnToggleManip.h>
//#include <maya/MFnRotateManip.h>
//#include <maya/MFnScaleManip.h>
//#include <maya/MFnPointArrayData.h>
//#include <maya/MFnNurbsCurveData.h>
//#include <maya/MPxSurfaceShapeUI.h>
//#include <maya/MFnParticleSystem.h>
//#include <maya/MFloatVectorArray.h>
//#include <maya/MFnTypedAttribute.h>
//#include <maya/MArrayDataBuilder.h>
//#include <maya/MPxContextCommand.h>
//#include <maya/MFnFloatArrayData.h>
//#include <maya/MFnStringArrayData.h>
//#include <maya/MFnMatrixAttribute.h>
//#include <maya/MFnVectorArrayData.h>
//#include <maya/MFnDoubleArrayData.h>
//#include <maya/MViewport2Renderer.h>
//#include <maya/MPxSubSceneOverride.h>
//#include <maya/MAttributeSpecArray.h>
//#include <maya/MPxGeometryOverride.h>
//#include <maya/MFnMessageAttribute.h>
//#include <maya/MFnNumericAttribute.h>
//#include <maya/MPxGeometryIterator.h>
//#include <maya/MFnCompoundAttribute.h>
//#include <maya/MFnComponentListData.h>
////#include <maya/MHWGeometryUtilities.h>
//#include <maya/MTransformationMatrix.h>
////#include <maya/MTextureEditorDrawInfo.h>
//#include <maya/MFnSingleIndexedComponent.h>
//#include <maya/MPxTransform.h>
//#include <maya/MPxTransformationMatrix.h>
//#include <maya/MObjectHandle.h>
//
//#if MAYA_API_VERSION >= 201602
//#include <maya/MSelectionContext.h>
//#include <maya/MPxComponentConverter.h>
//#endif


