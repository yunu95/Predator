#pragma once

#include <vector>
#include <string>

struct MaterialData
{
	std::wstring materialName;
	std::wstring albedoMap;
	std::wstring normalMap;
	std::wstring armMap;
	std::wstring emissionMap;
};

struct FBXData
{
	std::wstring meshName;

	std::vector<MaterialData> materialVec;
};
