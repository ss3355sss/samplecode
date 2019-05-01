#include <gtest/gtest.h>

#include "base/triangleMesh.h"

#include "baseGl/glBuffer.h"
#include "baseGl/glShader.h"
#include "baseGl/glutil.h"
#include "baseGl/instancingTask.h"
#include "baseGl/pch.h"
#include "baseGl/query.h"
#include "baseGl/renderTask.h"
#include "baseGl/taskManager.h"

#include "framework.h"

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















