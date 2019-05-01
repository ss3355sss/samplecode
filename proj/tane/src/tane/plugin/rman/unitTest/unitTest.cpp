#include <gtest/gtest.h>
#include <ri.h>

#include "base/pch.h"
#include "base/logger.h"
void foo(const std::string rib_filename, const int frame)
{
	//RiBegin(const_cast<char*>(rib_filename.c_str()));
	{
		
		RiFormat(960, 540, 1);
		RiDisplay("testRender", "it", "rgba", RI_NULL);
		
		float fov = 20;
		RiProjection("perspective", "fov", &fov, RI_NULL);
		
		RiFrameBegin(frame);
		{
			//-----------------------------------------------------------------------------------------Render Setting
			int incremental = 1;
			int minsamples = 0;
			int maxsamples = 128;
			float darkfalloff = 0.025f;
			int maxPathLength = 10;
			int allowCaustics = 0;
			RiHider("raytrace",
			        "int incremental", &incremental,
			        "int minsamples", &minsamples,
			        "int maxsamples", &maxsamples,
			        "float darkfalloff", &darkfalloff, RI_NULL);
			RiIntegrator("PxrPathTracer", "PxrPathTracer",
			             "int maxPathLength", &maxPathLength,
			             "int allowCaustics", &allowCaustics, RI_NULL);
			RiPixelVariance(0.05f);
			
			//-----------------------------------------------------------------------------------------Camera Configuration
			RiTranslate(0.0f, -2.0f, 30.0f);
			RiRotate(-30.0f, 1.0f, 0.0f, 0.0f);
			RiScale(1.0f, 1.0f, -1.0f);
			
			RiArchiveRecord( (char*)"structure", (char*)"rifcontrol insert begin -rif RLFInjector -rifend", RI_NULL );
			
			RiWorldBegin();
			{
				float light_exposure = 5.0f;
				//-----------------------------------------------------------------------------------------Light Configuration
				RiAttributeBegin();
				{
					RiRotate(0.0f, 0.0f, 1.0f, 0.0f);
					RiRotate(-50.0f, 1.0f, 0.0f, 0.0f);
					RiTranslate(0.0f, 0.0f, 8.0f);
					RiScale(15.0f, 1.0f, 50.0f);
					
					RiRotate(180.0f, 0.0f, 1.0f, 0.0f);
					RiLight("PxrRectLight", "default_light", "float exposure", &light_exposure ,RI_NULL);
					
				}
				RiAttributeEnd();
				
				//-----------------------------------------------------------------------------------------Geometry Configuration
				std::string dso_path	= "/opt/Zelos/renderman/ZEnvDSO";
				std::string cpt			= "-cpt /dexter/Cache_DATA/RND/dohyeon/testRfMproj/tmp/renderman/shader_test_v03/rib/cache";
				std::string cnm			= " -cnm /ZEnvGroup1";
				std::string cfr			= " -cfr 1";
				
				const char* arg1 = dso_path.c_str();
				const char* arg2 = (cpt + cnm + cfr).c_str();
				
				RtString args[] = {const_cast<RtString>(arg1), const_cast<RtString>(arg2)};
				
				//RtString args[] = { dso_path.c_str(), (cpt + cnm + cfr).c_str()};
				
				//RtString args[] = {"/opt/Zelos/renderman/ZEnvDSO", "-cpt /dexter/Cache_DATA/RND/dohyeon/testRfMproj/tmp/renderman/shader_test_v03/rib/cache -cnm ZEnvGroup1 -cfr 1 -tds 0", RI_NULL};
				RtBound bound = {-1000, 1000, -1000, 1000, -1000, 1000};
				RiProcedural((RtPointer)args, bound, RiProcDynamicLoad, NULL);
				//------------------------------------------------------------------Ground
				RiAttributeBegin();
				{
					RiScale(50.0f, 1.0f, 50.0f);
					RtPoint points[4][3] = { -0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f};
					RiPolygon(4, RI_P, (RtPointer) points, RI_NULL);
				}
				RiAttributeEnd();
			}
			RiWorldEnd();
		}
		RiFrameEnd();
	}
	//RiEnd();
	
}

int test_render2(const std::string rib_filename, const int n_frame)
{
	RiBegin(const_cast<char*>(rib_filename.c_str()));
	{
		//-------------------------------------------------- image optionss
		//RiDisplay(...)
		//RiFormat(...);
		
		//-------------------------------------------------- camera options (static intrinsic camera)
		//RiClipping(...)
		//RiDepthOfField(...);
		//RiProjection(...);
		
		//-------------------------------------------------- camera postion and orientation (static extrinsic camera)
		//RiRotate(...)
		//RiTranslate(...);
		
		//-------------------------------------------------- light postion and orientation (static light)
		//...
		for(int frame = 1; frame < n_frame; ++frame)
		{
			//RiFrameBegin(frame);
			{
				//-------------------------------------------------- frame-dependent option section
				//...
				
				//-------------------------------------------------- camera options (dynamic intrinsic camera)
				//RiClipping(...)
				//RiDepthOfField(...);
				//RiProjection(...);
				
				//-------------------------------------------------- camera postion and orientation (dynmaic extrinsic camera)
				//RiRotate(...)
				//RiTranslate(...);
				
				//-------------------------------------------------- light postion and orientation (dynamic light)
				//...
				
				//RiWorldBegin();
				{
					//-------------------------------------------------- scene description section
					//...
					
					//-------------------------------------------------- begin a distinct object
					//RiAttributeBegin();
					{
						//RiColor();
						//RiSurface();
						//RiTransformBegin();
						{
							//-------------------------------------------------- object positioning in world space
							//RiTranslate(...);
							//RiRotate(...);
							
							//RiSphere(...)
						}
						//RiTransformEnd();
					}
					//RiAttributeEnd();
					//...
				}
				//RiWorldEnd();
			}
			//RiFrameEnd();
		}
	}
	RiEnd();
}

void bakeRib(const std::string filename, const float startFrame, const float endFrame, const float stepSize)
{
	RiBegin(const_cast<char*>(filename.c_str()));
	{
		//-------------------------------------------------- image optionss
		//RiDisplay(...)
		//RiFormat(...);
		RiFormat(960, 540, 1);
		RiDisplay("testRender", "it", "rgba", RI_NULL);
		
		//-------------------------------------------------- camera options (static intrinsic camera)
		//RiClipping(...)
		//RiDepthOfField(...);
		//RiProjection(...);
		float fov = 20;
		RiProjection("perspective", "fov", &fov, RI_NULL);
		
		//-------------------------------------------------- camera postion and orientation (static extrinsic camera)
		//RiRotate(...)
		//RiTranslate(...);
		
		//-------------------------------------------------- light postion and orientation (static light)
		//...
		for(float frame = startFrame; frame <= endFrame; frame += stepSize)
		{
			//int tmp = frame;
			RiFrameBegin(frame);
			{
				//-------------------------------------------------- frame-dependent option section
				//...
				//int incremental = 1;
				//int minsamples = 0;
				//int maxsamples = 128;
				//float darkfalloff = 0.025f;
				//int maxPathLength = 10;
				//int allowCaustics = 0;
				//RiHider("raytrace",
				//        "int incremental", &incremental,
				//        "int minsamples", &minsamples,
				//        "int maxsamples", &maxsamples,
				//        "float darkfalloff", &darkfalloff, RI_NULL);
				//RiIntegrator("PxrPathTracer", "PxrPathTracer",
				//             "int maxPathLength", &maxPathLength,
				//             "int allowCaustics", &allowCaustics, RI_NULL);
				//RiPixelVariance(0.05f);
				//
				////-----------------------------------------------------------------------------------------Camera Configuration
				//RiTranslate(0.0f, -2.0f, 30.0f);
				//RiRotate(-30.0f, 1.0f, 0.0f, 0.0f);
				//RiScale(1.0f, 1.0f, -1.0f);
				//
				//		//-------------------------------------------------- light postion and orientation (dynamic light)
				//
				//RiWorldBegin();
				//		{
				//			//-------------------------------------------------- scene description section
				//			float light_exposure = 5.0f;
				//			//-----------------------------------------------------------------------------------------Light Configuration
				//			RiAttributeBegin();
				//			{
				//				RiRotate(0.0f, 0.0f, 1.0f, 0.0f);
				//				RiRotate(-50.0f, 1.0f, 0.0f, 0.0f);
				//				RiTranslate(0.0f, 0.0f, 8.0f);
				//				RiScale(15.0f, 1.0f, 50.0f);
				//
				//				RiRotate(180.0f, 0.0f, 1.0f, 0.0f);
				//				RiLight("PxrRectLight", "default_light", "float exposure", &light_exposure ,RI_NULL);
				//
				//			}
				//			RiAttributeEnd();
				//
				//
				//			//-------------------------------------------------- begin a distinct object
				//			//RiAttributeBegin();
				//			{
				//				//RiColor();
				//				//RiSurface();
				//				//RiTransformBegin();
				//				{
				//					//-------------------------------------------------- object positioning in world space
				//					//RiTranslate(...);
				//					//RiRotate(...);
				//
				//					//RiSphere(...)
				//				}
				//				//RiTransformEnd();
				//			}
				//			//RiAttributeEnd();
				//			//...
				//		}
				//RiWorldEnd();
			}
			RiFrameEnd();
		}
	}
	RiEnd();
}

//void testFree(void* p)
//{
//}
//void bakeRib(const std::string rib_filename)
//{
//	USING_NAMESPACE_BASE;
//	RiBegin(const_cast<char*>(rib_filename.c_str()));
//	{
//		RiFormat(960, 540, 1);
//		RiDisplay("testRender", "it", "rgba", RI_NULL);
//
//		float fov = 20;
//		RiProjection("perspective", "fov", &fov, RI_NULL);
//
//		//-----------------------------------------------------------------------------------------Render Setting
//		int incremental = 1;
//		int minsamples = 0;
//		int maxsamples = 128;
//		float darkfalloff = 0.025f;
//		int maxPathLength = 10;
//		int allowCaustics = 0;
//		RiHider("raytrace",
//		        "int incremental", &incremental,
//		        "int minsamples", &minsamples,
//		        "int maxsamples", &maxsamples,
//		        "float darkfalloff", &darkfalloff, RI_NULL);
//		RiIntegrator("PxrPathTracer", "PxrPathTracer",
//		             "int maxPathLength", &maxPathLength,
//		             "int allowCaustics", &allowCaustics, RI_NULL);
//		RiPixelVariance(0.05f);
//
//		///-----------------------------------------------------------------------------------------Camera Configuration
//		//RiTranslate(0.0f, -2.0f, 75.0f);
//		//RiRotate(-30.0f, 1.0f, 0.0f, 0.0f);
//		//RiScale(1.0f, 1.0f, -1.0f);
//		RtMatrix xformMtx = {
//			{0.671721, -0.606374, -0.425561, -7.76132e-11},
//			{-1.80714e-08, 0.574457, -0.818534, 5.35005e-10},
//			{-0.740805, -0.549826, -0.385875, 2.67718e-10},
//			{2.58942e-06, -2.09406e-06, 117.287, 1}
//		};
//		RiConcatTransform(xformMtx);
//
//
//		RiArchiveRecord( (char*)"structure", (char*)"rifcontrol insert begin -rif RLFInjector -rifend", RI_NULL );
//
//		RiWorldBegin();
//		{
//			float light_exposure = 5.0f;
//			///-----------------------------------------------------------------------------------------Light Configuration
//			RiAttributeBegin();
//			{
//				RiRotate(0.0f, 0.0f, 1.0f, 0.0f);
//				RiRotate(-50.0f, 1.0f, 0.0f, 0.0f);
//				RiTranslate(0.0f, 0.0f, 20.0f);
//				RiScale(15.0f, 1.0f, 50.0f);
//				RiRotate(180.0f, 0.0f, 1.0f, 0.0f);
//				RiLight("PxrRectLight", "default_light", "float exposure", &light_exposure ,RI_NULL);
//
//			}
//			RiAttributeEnd();
//
//			//-----------------------------------------------------------------------------------------Geometry Configuration
//			RiAttributeBegin();
//			{
//				//RiAttributeWrapper("procedural", "immediatesubdivide", 1);
//
//				int         value       = 2;
//				std::string attrParam   = "procedural";
//				std::string attrName    = "int immediatesubdivide";
//				RiAttribute(const_cast<char*>(attrParam.c_str()),	const_cast<char*>(attrName.c_str()), &value, RI_NULL);
//
//
//				Point3f lower(-100.0f, -100.0f, -100.0f);
//				Point3f upper(+100.0f, +100.0f, +100.0f);
//
//				RtBound bound = {
//					lower[0], lower[0], lower[1],
//					upper[1], upper[2], upper[2]
//				};
//
//				std::string usdFile         = "/home/dohyeon.yang/Desktop/usdTest/TN_Tane1.usda";
//				double      frame           = 1.0;
//				double      fps             = 24.0;
//				double      shutterOpen     = 0.0;  //no motion blur
//				double      shutterClose    = 0.0;  //no motion blur
//
//				//double      shutterOpen     = 0.0;  //if motion blur, value is not 0.0
//				//double      shutterClose    = 0.0;  //if motion blur, value is not 0.0
//
//				std::string opt;
//				opt += "--filename "        + usdFile + " ";
//				opt += "--frame "           + std::to_string(frame) + " ";
//				opt += "--fps "             + std::to_string(fps) + " ";
//				opt += "--shutterOpen "     + std::to_string(shutterOpen) + " ";
//				opt += "--shutterClose "    + std::to_string(shutterClose) + " ";
//
//
//				std::string buildPath;
//				buildPath   = "/dexter/Cache_DATA/RND/dohyeon/build/Tane2/2017_21.7/renderman";
//				std::string dsoName;
//				dsoName     = "TaneRiProcedural.so";
//
//				std::string dsoFile = buildPath + "/" + dsoName;
//
//				const char  * args[] = {
//					dsoFile.c_str(),
//					opt.c_str()
//				};
//
//				RiProcedural(args, bound, RiProcDynamicLoad, testFree);
//
//
//			}
//			RiAttributeEnd();
//
//			///------------------------------------------------------------------Ground
//			RiAttributeBegin();
//			{
//				RiScale(50.0f, 1.0f, 50.0f);
//				RtPoint points[4][3] = { -0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f};
//				RiPolygon(4, RI_P, (RtPointer) points, RI_NULL);
//			}
//			RiAttributeEnd();
//		}
//		RiWorldEnd();
//	}
//	RiEnd();
//
//}


int googleTest_main(int argc, char** argv)
{
	STDOUT_INFO("/*----------------------------- google test */");
	std::string testCase;
	/* AlembicRiProcedural2 */
	testCase += "AlembicRiProcedural2.*:";
	/* UsdRiProcedural */
	//testCase += "UsdRiProcedural.*:";
	/* TaneRiProcedural */
	//testCase += "TaneRiProcedural.*:";
	
	int ret = -1;
	::testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(filter) = testCase.c_str();
	{
		ret = RUN_ALL_TESTS();
		
		if(!ret)
			cout << "-------------test is success -------------" << endl;
		else
			cout << "-------------test is failed  -------------" << endl;
	}
	STDOUT_INFO("/*----------------------------- google test end */");
	
	return ret;
}


int main(int argc, char** argv)
{
	USING_NAMESPACE_BASE;
	
	bool googleTest = true;
	if(googleTest)
		int ret = googleTest_main(argc, argv);
	
	STDOUT_INFO("/*----------------------------- play ground */");
	{
	
	}
	STDOUT_INFO("/*----------------------------- play ground end */\n");
	return 0;
	
	
	//DECL_LOGGER(true);
	//const std::string renderCmd = "/netapp/backstage/pub/apps/renderman2/applications/linux/RenderManProServer-21.7/bin/prman";
	//const std::string filename = "test.rib";
	//float startFrame    = 1.0f;
	//float endFrame      = 2.0f;
	//float stepSize      = 1.0f;
	//LOGGER_IN("bakeRib");
	////bakeRib(filename);
	//LOGGER_OUT();
	//
	////system((renderCmd + " " + filename).c_str());
	//return EXIT_SUCCESS;
}


