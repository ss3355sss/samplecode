#include <gtest/gtest.h>
#include "base/pch.h"

#include "base/array.h"
#include "base/attributes.h"
#include "base/bbox.h"
#include "base/box.h"
#include "base/bvh.h"
#include "base/digest.h"
#include "base/fileUtil.h"
#include "base/frame.h"
#include "base/hackedQueue.h"
#include "base/instancingItem.h"
#include "base/intersectionQueryEngine.h"
#include "base/logger.h"
#include "base/mapUtil.h"
#include "base/mathUtil.h"
#include "base/matrix.h"
#include "base/matrixStack.h"
#include "base/messageQueue.h"
#include "base/nearestNeighborQueryEngine.h"
#include "base/plane.h"
#include "base/points.h"
#include "base/quat.h"
#include "base/query.h"
#include "base/ray.h"
#include "base/renderItem.h"
#include "base/scatter.h"
#include "base/size.h"
#include "base/staticData.h"
#include "base/surface.h"
#include "base/texture.h"
#include "base/transform.h"
#include "base/transformMtx.h"
#include "base/triangle.h"
#include "base/uniforms.h"
#include "base/vector.h"


int googleTest_main(int argc, char **argv)
{
	STDOUT_INFO("/*----------------------------- google test */");
	std::string testCase;
	
	
	int ret = -1;
	::testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(filter) = testCase.c_str();
	{
		ret = RUN_ALL_TESTS();
		
		if (!ret)
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
	//GL_TEXTURE_1D_ARRAY;

	//bool kernelTest = true;
	//if(kernelTest )
	//	int ret = kernel_main(argc, argv);
	bool googleTest = true;
	if(googleTest)
		int ret = googleTest_main(argc, argv);

	STDOUT_INFO("/*----------------------------- play ground */");
	{
	}
	STDOUT_INFO("/*----------------------------- play ground end */\n");
	return 0;
}















