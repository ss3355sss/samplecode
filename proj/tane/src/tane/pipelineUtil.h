#pragma once
/* ----------------------------------------------- include */
#include "tane/pch.h"

#include "base/fileUtil.h"

NAMESPACE_BASE_BEGIN
inline const std::string GetAbcAssetName(const std::string filePath)
{
    namespace fs = boost::filesystem;
    if(!IsExists(filePath))
    {
        STDOUT_ERROR("%s is not exists", filePath.c_str());
        return std::string();
    }

    if(!IsFile(filePath))
    {
        STDOUT_ERROR("%s is not file", filePath.c_str());
        return std::string();
    }

    std::string assetName = GetBaseName(filePath);
    if(assetName.find("_model_") != std::string::npos)
    {
        assetName = assetName.substr(0, filePath.find("_model_"));
    }

    return assetName;
}

inline const std::string GetAssetPayloadRelativePath(const std::string assetName, const std::string from, const std::string to)
{
    const std::string relative = GetRelativePath(from, to);
    const std::string pathToAssetPayload = relative + "/" + assetName + "/" + assetName + ".payload.usd";
    return pathToAssetPayload;
}

inline const std::string GetAssetPayloadAbsolutePath(const std::string assetName, const std::string baseDir)
{
    const std::string pathToAssetPayload = baseDir + "/" + assetName + "/" + assetName + ".payload.usd";
	if(!IsExists(pathToAssetPayload))
	{
		STDOUT_WARRNING("pathToAssetPayload is not exists, \"%s\"", pathToAssetPayload.c_str());
	}
	if(!IsFile(pathToAssetPayload))
	{
		STDOUT_WARRNING("pathToAssetPayload is not a file, \"%s\"", pathToAssetPayload.c_str());
	}
	return pathToAssetPayload;
}

inline void SetAssetModelAPI(const pxr::UsdPrim& prim, const std::string& name, const std::string& identifier)
{
	if(!IsExists(identifier))
	{
		STDOUT_WARRNING("identifier is not exists, \"%s\"", identifier.c_str());
	}
	if(!IsFile(identifier))
	{
		STDOUT_WARRNING("identifier is not afile, \"%s\"", identifier.c_str());
	}
	
    pxr::UsdModelAPI modelAPI(prim);
        modelAPI.SetKind(pxr::KindTokens->component);
        modelAPI.SetAssetName(name);
        modelAPI.SetAssetIdentifier(pxr::SdfAssetPath(identifier));
}
inline void SetAssetPayload(const pxr::UsdPrim& prim, const std::string& payload)
{
    prim.SetPayload(pxr::SdfPayload(payload));
}



NAMESPACE_BASE_END
