#pragma once

#include <vector>
#include <string>

namespace yunuGI
{
	struct BoneInfo
	{
		std::wstring name;
		int index;
		int parentIndex;
		std::vector<BoneInfo> child;
	};

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

		BoneInfo boneInfo;
	};


}
