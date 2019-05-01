#include <gtest/gtest.h>

#include "baseMaya/pch.h"
#include "baseMaya/attribute.h"
#include "baseMaya/triangleMeshUtil.h"
#include "baseMaya/pluginRegister.h"
#include "baseMaya/usdVariantSelect.h"

#include "baseMaya/mayaMeshExtractor.h"
#include "baseMaya/usdUtil.h"
#include "baseMaya/util.h"

#include <maya/MLibrary.h>
#include <maya/MGlobal.h>
#include <maya/MFileIO.h>

int googleTest_main(int argc, char **argv)
{
	STDOUT_INFO("/*----------------------------- google test */");
	std::string testCase;
	
	//testCase += "scatter.*:";
	
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
	
	STDOUT_INFO("/*----------------------------- baseMaya play ground */");
	{
	}
	STDOUT_INFO("/*----------------------------- baseMaya play ground end */\n");
	return 0;
}















