#pragma once

#include <vector>
#include <string>

namespace yunuGI
{
	struct BoneInfo
	{
		int index = -1;
	};

	struct MaterialData
	{
		std::wstring materialName;
		std::wstring albedoMap;
		std::wstring normalMap;
		std::wstring armMap;
		std::wstring emissionMap;
		std::wstring opacityMap;
	};

	struct FBXData
	{
		std::vector<FBXData*> child;

		std::wstring nodeName;
		std::wstring meshName;

		yunuGI::Vector3 pos;
		yunuGI::Vector3 scale;
		yunuGI::Vector4 quat;

		std::vector<MaterialData> materialVec;

		BoneInfo boneInfo;

		bool hasAnimation;
	};


}
